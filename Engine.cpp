#include "Engine.hpp"

#include <bitset>
#include <random>
#include <set>

namespace Emulator {

static constexpr u64 WORD_SIZE = 4;

static constexpr std::string_view MOVE = "move";
static constexpr std::string_view LI = "li";
static constexpr std::string_view LA = "la";

static const std::unordered_map<std::string_view, u8> functMap = {
    {"add", 0x20},
    {"sub", 0x22}, 
    {"and", 0x24}, 
    {"or" , 0x25},
    {"nor", 0x27}, 
    {"sll", 0x00}, 
    {"srl", 0x02}, 
    {"slt", 0x2A},
    {"jr" , 0x08},
    {"mul", 0x01}, // Pseudo
};

static const std::unordered_map<std::string_view, u8> opcodeMap = {
    {"addi", 0x08},
    {"slti", 0x0A},
    {"andi", 0x0C},
    {"ori" , 0x0D},
    {"beq" , 0x04},
    {"bne" , 0x05},
    {"blt" , 0x06}, // Pseudo
    {"bge" , 0x07}, // Pseudo
    {"lw"  , 0x23},
    {"lbu" , 0x24},
    {"sb"  , 0x28},
    {"sw"  , 0x2b},
};

static const std::unordered_map<std::string_view, u8> jumpMap = {
  {"j"  , 0x02},
  {"jal", 0x03},
};

auto Engine::insertCode(u8* program, u32 bin, u64& address) -> void {
  for (size_t i = 0; i < WORD_SIZE; i++) {
      program[address + i] = static_cast<u8>((bin >> i * 8) & 0xFF);
  }
  address += WORD_SIZE;
}

auto Engine::isPseudoInstruction(const std::string& mnemonic) -> bool {
  return mnemonic == MOVE || mnemonic == LI || mnemonic == LA;
}

auto Engine::isRInstruction(const std::string& mnemonic) -> bool {
  return functMap.contains(mnemonic);
}

auto Engine::isIInstruction(const std::string& mnemonic) -> bool {
  return opcodeMap.contains(mnemonic);
}

auto Engine::isJInstruction(const std::string& mnemonic) -> bool {
  return jumpMap.contains(mnemonic);
}

auto Engine::assemblePseudoInstruction(u8* program, const Token& token, u32& bin) -> void {

  // "move" instruction turns into: "add $dest, $src, zero" to cpu
  if (token.value == MOVE) {
    u8 rd = static_cast<u8>(token.args.at(0)); // Destiny
    u8 rs = static_cast<u8>(token.args.at(1)); // Source
    u8 rt = 0; // zero

    bin |= (rs    & 0x1F) << 21; // rs
    bin |= (rt    & 0x1F) << 16; // rt ($zero)
    bin |= (rd    & 0x1F) << 11; // rd
    bin |= (0     & 0x1F) << 6;  // shamt = 0
    bin |= (0x20  & 0x3F) << 0;  // funct = add (0x20)
  } 
  // "li" instruction turns into: "addi $dst, $src, zero" to cpu
  // Note that this implementation only works for 16 bits immediate
  // 32 bits immediate will eventually deal with it
  else if (token.value == LI) {
    u8 rt = static_cast<u8>(token.args.at(0)); //Destiny
    s32 imm = static_cast<s32>(token.args.at(1)); // immediate

    bin |= (0x08 & 0x3F) << 26; // opcode 
    bin |= (0x00 & 0x1F) << 21; // rs (zero)
    bin |= (rt   & 0x1F) << 16; // rt 
    bin |= (imm  & 0xFFFF) << 0; // immediate
  }
  // "la" instruction tuns into: "addi $dst, zero, label" to cpu 
  // Note that this implementation only works for 16 bits address space (64Kb)
  else if (token.value == LA) {
    u8 rd = static_cast<u8>(token.args.at(0));
    u16 address_label = static_cast<u16>(token.args.at(1));

    bin |= (0x08 & 0x3F) << 26; // opcode addi
    bin |= (0    & 0x1F) << 21; // rs = $zero
    bin |= (rd   & 0x1F) << 16; // rt
    bin |= (address_label & 0xFFFF);
  } else {
    throw std::runtime_error{std::format("ERROR! Pseudo instruction {} doesn't exist\n", token.value)};
  }
}

auto Engine::assembleR(u8* program, const Token& token, u32& bin) -> void {
  u8 rs, rt, rd, shamt, funct;
  rs = rt = 0;
  shamt = 0;
 
  funct = functMap.at(token.value);
  rd = static_cast<u8>(token.args.at(0));

  switch (funct) {
    case 0x00: // sll
    case 0x02: // srl
      rt = static_cast<u8>(token.args.at(1));
      shamt = static_cast<u8>(token.args.at(2));
      break;

    case 0x01: // mul
    case 0x20: // add
    case 0x22: // sub
    case 0x24: // and
    case 0x25: // or
    case 0x27: // nor 
    case 0x2A: // slt
      rs = static_cast<u8>(token.args.at(1));
      rt = static_cast<u8>(token.args.at(2));
      break;

    case 0x08: // jr
      rs = static_cast<u8>(token.args.at(0));
      break;
  }

  bin |= (rs    & 0x1F) << 21;    // rs
  bin |= (rt    & 0x1F) << 16;    // rt
  bin |= (rd    & 0x1F) << 11;    // rd
  bin |= (shamt & 0x1F) << 6;     // shamt
  bin |= (funct & 0x3F) << 0;     // funct
}

auto Engine::assembleI(u8* porgram, const Token& token, u32& bin, u64& address) -> void {
  u8 opcode, rt, rs;
  s16 imm;
    
  rt = static_cast<u8>(token.args.at(0));
  rs = static_cast<u8>(token.args.at(1));
  imm = static_cast<s16>(token.args.at(2));
  opcode = opcodeMap.at(token.value);

  bin |= (opcode &   0x3F) << 26;
  bin |= (rs     &   0x1F) << 21;
  bin |= (rt     &   0x1F) << 16;
  switch (opcode) {
    case 0x04: // beq
    case 0x05: // bne
    case 0x06: // blt
    case 0x07: // bge
    {
      s32 offset = (imm - address) >> 2;
      bin |= (offset & 0xFFFF);
    }
    break;

    case 0x08: // addi
    case 0x0A: // slti
    case 0x0C: // andi
    case 0x0D: // ori
    case 0x23: // lw
    case 0x24: // lbu
    case 0x28: // sb
    case 0x2b: // sw
      bin |= (imm & 0xFFFF);
    break;
  }
}

auto Engine::assembleJ(u8* porgram, const Token& token, u32& bin) -> void {
  u8 opcode;
  u32 address;
  opcode = jumpMap.at(token.value);
  address = static_cast<u32>(token.args.at(0));

  bin |= (opcode & 0x3F) << 26;
  bin |= (address & 0x3FFFFFF);
}

auto Engine::assembleInstruction(u8* program, const Token& token, u64& address) -> void {
  u32 bin = 0;
  if (this->isPseudoInstruction(token.value))
    this->assemblePseudoInstruction(program, token, bin);

  else if (this->isRInstruction(token.value)) 
    this->assembleR(program, token, bin);

  else if (this->isIInstruction(token.value))
    this->assembleI(program, token, bin, address);

  else if (this->isJInstruction(token.value))
    this->assembleJ(program, token, bin);

  else 
    throw std::runtime_error{std::format("ERROR! Mnemonic {} not found\n", token.value)};

  this->insertCode(program, bin, address);
}

auto Engine::assembleSysCall(u8* program, const Token& token, u64& address)
    -> void {
  u32 bin = 0;
  if (token.value == "syscall") {
    bin |= (0x0C & 0x3F); // funct for an syscall is 0x0C
  } else {
    throw std::runtime_error{std::format("Mnemonic {} doesn't exist\n", token.value)};
  }

  this->insertCode(program, bin, address);
}

auto Engine::assembleLiteral(u8* program, const Token& token, u64& address) -> void {
  if (token.directive == Directive::WORD) {

    for (u8 i = 0; i < token.args.size(); i++) {
      u32 bin = static_cast<u32>(token.args[i]);
      this->insertCode(program, bin, address);
    }

  } else if (token.directive == Directive::SPACE) {

    address += token.args.front();

  } else if (token.directive == Directive::ASCIIZ) {

    for (u32 i = 0; i < token.args.size(); i++) {
      program[address] = static_cast<u8>(token.args.at(i));
      address++;
    }

  } else {
    throw std::invalid_argument("Not implemented yet\n");
  }
}

auto Engine::preComputeProgramLength() -> u64 {
  u64 length = 0;

  for (const auto& token : this->tokenizer.tokens) {
    if (token.tokenType == Type::LITERAL) {

      if (token.directive == Directive::WORD)
        length += WORD_SIZE * token.args.size();
      else if (token.directive == Directive::SPACE)
        length += token.args.front();
      else if (token.directive == Directive::ASCIIZ)
        length += token.args.size();
      else 
        throw std::runtime_error(std::format("Unknown directive\n"));

    } else if (token.tokenType == Type::INSTRUCTION || token.tokenType == Type::SYS_CALL) {

      length += WORD_SIZE;

    }
  }
  return length;
}

auto Engine::assemble(u8* program) -> void {
  u64 address = 0;

  for (const auto& token : this->tokenizer.tokens) {
    if (token.tokenType == Type::INSTRUCTION)
      this->assembleInstruction(program, token, address);

    else if (token.tokenType == Type::SYS_CALL)
      this->assembleSysCall(program, token, address);

    else if (token.tokenType == Type::LITERAL)
      this->assembleLiteral(program, token, address);

    else 
      throw std::runtime_error(
        std::format("Invalid token {}\n", token.value)
      ); 
  }
}

auto Engine::assembler(const std::string& file) -> std::tuple<u8*, size_t> {

  this->tokenizer.parse(file);
  u64 length = this->preComputeProgramLength();
  u8* program = new u8[length];
  this->assemble(program); 

  return {program, length};
}

auto Engine::setCPUstartAddress() -> void {
  cpu.pc = tokenizer.textStartAddress;
}

auto Engine::run(const std::span<u8>& code) -> void {
  this->cpu.loadProgram(code);
  this->setCPUstartAddress();

  //this->printContentFromAllRegisters(); // Debug

  while (!this->cpu.hasHalted())
    this->cpu.nextInstruction();

  //std::cout << "--------------------------------------\n"; // Debug
  //this->printContentFromAllRegisters();                    // Debug
}

auto Engine::printContentFromAllRegisters() -> void {
  for (uint32_t i = 0; i < 32; i++) {
    std::cout << std::format("Content from register {} = {}\n", i,
                             static_cast<s32>(this->cpu.readRegister(i)));
  }
}

auto Engine::setContentToAllRegisters() -> void {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(1, 100);

  for (uint32_t i = 0; i < 32; i++) {
    this->cpu.writeRegister(i, static_cast<uint32_t>(dist(gen)));
  }
}

}