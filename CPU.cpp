#include "CPU.hpp"

#include <bitset>
#include <limits>

namespace Emulator {

CPU::CPU() {
  this->mem_size = std::numeric_limits<uint32_t>::max();
  this->pc = 0;
  this->registers.fill(0);
  this->ebreak = false;
}

auto CPU::hasHalted() -> bool { return this->ebreak; }

auto CPU::loadProgram(const std::vector<std::uint8_t> &program) -> void {
  if (program.size() > this->mem_size) {
    std::cout << std::format("Program is too large to fit in memory\n");
    return;
  }

  this->mem.resize(program.size());
  this->writeMemoryBlock(0, program);
}

auto CPU::readMemory(uint32_t address) -> std::uint8_t {
  if (address > this->mem_size) {
    std::cout << std::format("Out of bounds for memory\n");
    return 0;
  }

  return this->mem[address];
}

auto CPU::readMemoryBlock(std::uint32_t address, std::uint32_t size)
    -> std::vector<std::uint8_t> {
  uint32_t end{address + size - 1};
  if (end > this->mem_size) {
    std::cout << std::format("Out of bounds for memory\n");
    return std::vector<std::uint8_t>(0);
  }

  std::vector<std::uint8_t> Block(begin(this->mem) + address,
                                  begin(this->mem) + end + 1);
  return Block;
}

auto CPU::writeMemory(std::uint32_t address, std::uint8_t value) -> void {
  if (address > this->mem_size) {
    std::cout << std::format("Out of bounds\n");
    return;
  }

  if (address > this->mem.size())
    this->mem.resize(address + 1);

  this->mem[address] = value;
}

auto CPU::writeMemoryBlock(std::uint32_t address,
                           const std::vector<std::uint8_t> &value) -> void {
  uint32_t end{address + static_cast<std::uint32_t>(value.size()) - 1};
  if (end > this->mem_size) {
    std::cout << std::format("Out of bounds\n");
    return;
  }

  if (end >= this->mem.size())
    this->mem.resize(end + 1);

  for (const auto &content : value) {
    this->mem[address] = content;
    address++;
  }
}

auto CPU::writeRegister(std::uint32_t index, std::uint32_t value) -> void {
  if (index == 0)
    return;

  this->registers[index] = value;
}

auto CPU::readRegister(std::uint32_t index) -> std::uint32_t {
  return this->registers[index];
}

auto CPU::nextInstruction() -> void {
  auto value{this->readMemoryBlock(this->pc, 4)};

  std::uint32_t instruction{
      std::uint32_t((value[0] << 0)) | std::uint32_t((value[1] << 8)) |
      std::uint32_t((value[2] << 16)) | std::uint32_t((value[3] << 24))};
  this->execute(instruction);
}

auto CPU::parseR(std::uint32_t instruction) -> Instruction {
  Instruction i;
  i.rd = (instruction >> 11) & 0x1F;
  i.rt = (instruction >> 16) & 0x1F;
  i.rs = (instruction >> 21) & 0x1F;
  i.shamt = (instruction >> 6) & 0x1F;
  i.funct = (instruction >> 0) & 0x3F;

  return i;
}

auto CPU::executeR(Instruction i) -> void {
  std::uint32_t rsContent{this->readRegister(i.rs)};
  std::uint32_t rtContent{this->readRegister(i.rt)};
  std::uint32_t valueToWrite;

  switch (i.funct) {

  case 0x00: // sll
    valueToWrite = rtContent << i.shamt;
    break;

  case 0x02: // srl
    valueToWrite = rtContent >> i.shamt;
    break;

  case 0x08: // jr
    this->pc = rsContent;
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

  case 0x0D: // ebreak
    this->ebreak = true;
    break;
  }

  this->writeRegister(i.rd, valueToWrite);
  this->pc += 4;
}

auto CPU::execute(std::uint32_t instruction) -> void {
  uint32_t opcode{(instruction >> 26) & 0x3F};

  switch (opcode) {
  case 0x0: {
    Instruction i{this->parseR(instruction)};
    this->executeR(i);
  } break;

  default:
    std::cout << std::bitset<32>(instruction) << " ";
    std::cout << std::format("Not implemented yet\n");
    break;
  }
}

} // namespace Emulator