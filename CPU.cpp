#include "CPU.hpp"

#include <bitset>

namespace Emulator {

CPU::CPU() {
  this->max_size = std::numeric_limits<uint32_t>::max();
  this->mem = new u8[this->max_size];
  this->pc = 0;
  this->registers.fill(0);
  this->registers[29] = this->max_size;
  this->halt = false;
}

auto CPU::hasHalted() -> bool { 
  return this->halt;
}

auto CPU::loadProgram(const std::span<u8> program) -> void {
  if (size(program) > this->max_size - 1) {
    std::cout << std::format("Program is too large to fit in memory\n");
    return;
  }

  this->writeMemoryBlock(0, program);
}

auto CPU::readMemory(u64 address) -> u8 {
  if (address > this->max_size) {
    std::cout << std::format("ERROR! Address {} is bigger than 32 bits addres space\n", address);
    return 0;
  }

  return this->mem[address];
}

auto CPU::readMemoryBlock(u64 address, u32 size) -> u8* {
  u64 end = address + size - 1;
  if (end > this->max_size - 1) {
    std::cout << std::format("ERROR! Address {} is bigger than 32 bits addres space\n", end);
    return nullptr;
  }

  u8* block = new u8[size];
  std::memcpy(block, this->mem + address, size);

  return block;
}

auto CPU::writeMemory(u64 address, u8 value) -> void {
  if (address > this->max_size - 1) {
    throw std::runtime_error(
      std::format("ERROR! Address {} is bigger than 32 bits addres space\n", address)
    );
  }
  this->mem[address] = value;
}

auto CPU::writeMemoryBlock(u64 address, const std::span<u8> value) -> void {
  u64 end = static_cast<u32>(address) + static_cast<u32>(value.size()) - 1;
  if (end > this->max_size - 1) {
    throw std::runtime_error(
      std::format("ERROR! Address {} is bigger than 32 bits addres space\n", end)
    );
  }

  for (const auto &content : value) {
    this->mem[address] = content;
    address++;
  }
}

auto CPU::writeRegister(u32 index, s32 value) -> void {
  if (index == 0)
    return;

  this->registers[index] = value;
}

auto CPU::readRegister(u32 index) -> u32 {
  return this->registers[index];
}

auto CPU::nextInstruction() -> void {
  auto value{this->readMemoryBlock(this->pc, 4)};

  u32 instruction{u32((value[0] <<  0)) | u32((value[1] <<  8)) |
                  u32((value[2] << 16)) | u32((value[3] << 24))};
  this->execute(instruction);
}

auto CPU::parseImm(u32 instruction) -> Instruction {
  Instruction i;
  i.opcode  =  (instruction >> 26) & 0x3F;
  i.rt      =  (instruction >> 16) & 0x1F;
  i.rs      =  (instruction >> 21) & 0x1F;
  i.imm     =  (instruction >>  0) & 0xFFFF;
  return i;
}

auto CPU::parseR(u32 instruction) -> Instruction {
  Instruction i;
  i.rd = (instruction >> 11) & 0x1F;
  i.rt = (instruction >> 16) & 0x1F;
  i.rs = (instruction >> 21) & 0x1F;
  i.shamt = (instruction >> 6) & 0x1F;
  i.funct = (instruction >> 0) & 0x3F;
  return i;
}

auto CPU::parseJ(u32 instruction) -> Instruction {
  Instruction i;
  i.opcode = (instruction >> 26) & 0x3F;
  i.address = (instruction & 0x3FFFFFF);
  return i;
}

constexpr inline auto CPU::immExt(s16 imm) -> s32 {
  return static_cast<s32>(imm);
}

constexpr inline auto CPU::zeroExt(s16 imm) -> u32 {
  return static_cast<u32>(std::abs(imm));
}

auto CPU::executeJ(Instruction i) -> void {
  switch (i.opcode) {
    case 0x02:
      this->pc = u32(i.address);
      break;

    case 0x03:
      this->registers[31] = this->pc + 8;
      this->pc = u32(i.address);
      break;
  }
}

auto CPU::executeImm(Instruction i) -> void {
  u32 rsContent = this->readRegister(i.rs);
  u32 rtContent = this->readRegister(i.rt);
  u32 valueToWrite;
  u8 valueToStore[4];

  switch (i.opcode) {
    case 0x04: // beq
      if (rsContent == rtContent) {
        s32 offset = this->immExt(i.imm) << 2 ;
        this->pc = u32(s32(this->pc) + offset) - 4; 
      }
      break;
    
    case 0x05: // bne 
      if (rsContent != rtContent) {
        s32 offset = this->immExt(i.imm) << 2 ;
        this->pc = u32(s32(this->pc) + offset) - 4;
      }
      break;

    case 0x06: // blt
      if (rtContent < rsContent) {
        s32 offset = this->immExt(i.imm) << 2 ;
        this->pc = u32(s32(this->pc) + offset) - 4;
      }
      break;

    case 0x07: // bge
      if (rtContent >= rsContent) {
        s32 offset = this->immExt(i.imm) << 2 ;
        this->pc = u32(s32(this->pc) + offset) - 4;
      }
      break;

    case 0x08: // addi
      valueToWrite = rsContent + this->immExt(i.imm);
      this->writeRegister(i.rt, valueToWrite);
      break;

    case 0x0C: // andi
      valueToWrite = rsContent & this->zeroExt(i.imm);
      this->writeRegister(i.rt, valueToWrite);
      break;

    case 0x0D: // ori
      valueToWrite = rsContent | this->zeroExt(i.imm);
      this->writeRegister(i.rt, valueToWrite);
      break;

    case 0x23: // lw
      valueToWrite = this->mem[rsContent + this->immExt(i.imm)];
      this->writeRegister(i.rt, valueToWrite);
      break;

    case 0x2b:
      for (u8 i = 0; i < 4; i++) {
        valueToStore[i] = static_cast<u8>((rsContent >> i * 8) & 0xFF);
      }
      this->writeMemoryBlock(
        rtContent + this->immExt(i.imm),
        std::span<u8>(valueToStore, 4)
      );
      break;

  }
  this->pc += 4;
}

auto CPU::executeR(Instruction i) -> void {
  u32 rsContent{this->readRegister(i.rs)};
  u32 rtContent{this->readRegister(i.rt)};
  u32 valueToWrite = 0;

  switch (i.funct) {

  case 0x00: // sll
    valueToWrite = rtContent << i.shamt;
    break;

  case 0x02: // srl
    valueToWrite = rtContent >> i.shamt;
    break;

  case 0x08: // jr
    this->pc = rsContent - 4;
    return;
    break;

  case 0x20: // add
    valueToWrite = rsContent + rtContent;
    break;

  case 0x22: // sub
    valueToWrite = rsContent - rtContent;
    break;

  case 0x24: // and
    valueToWrite = rsContent & rtContent;
    break;

  case 0x25: // or
    valueToWrite = rsContent | rtContent;
    break;

  case 0x27: // nor
    valueToWrite = ~(rsContent | rtContent);
    break;

  case 0x2A: // slt
    valueToWrite =  (rsContent < rtContent) ? 1 : 0;
    break;

  case 0x0D: // ebreak
    this->halt = true;
    break;
  }
  this->writeRegister(i.rd, valueToWrite);
  this->pc += 4;
}

auto CPU::execute(u32 instruction) -> void {
  u32 opcode = (instruction >> 26) & 0x3F;
  Instruction i;
  switch (opcode) {
  case 0x00:
    i = this->parseR(instruction);
    this->executeR(i);
    break;

  case 0x04: // beq
  case 0x05: // bne
  case 0x06: // blt (Pseudo)
  case 0x07: // bge (Pseudo)
  case 0x08: // addi
  case 0x0C: // andi
  case 0x0D: // ori
  case 0x23:
  case 0x2b:
    i = this->parseImm(instruction);
    this->executeImm(i);
    break;

  case 0x02: // j
  case 0x03: // jal
    i = this->parseJ(instruction);
    this->executeJ(i);
    break;

  default:
    std::cout << std::bitset<32>(instruction) << " ";
    std::cout << std::format("Not implemented yet\n");
    break;
  }
}

auto CPU::dumpMemory(size_t size) -> void {
  u64 address = 0;
  for (size_t i = 0; i < 100; i += 4) {
     u32 instruction{u32((this->mem[i] <<  0)) | u32((this->mem[i + 1] <<  8)) |
                  u32((this->mem[i + 2] << 16)) | u32((this->mem[i + 3] << 24))};
    std::cout << std::format("address {} -> ", address);
    std::cout << std::showbase << std::hex << instruction << '\n';
    address += 4;
  }
}

} // namespace Emulator