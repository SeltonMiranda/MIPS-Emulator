#include "Engine.hpp"

#include <bitset>
#include <random>
#include <set>

namespace Emulator {

static const std::unordered_map<std::string_view, u8> functMap {
    {"add", 0x20}, {"sub", 0x22},  {"and", 0x24}, {"or", 0x25},
    {"nor", 0x27},  {"sll", 0x00}, {"srl", 0x02}, {"slt", 0x2A},
    {"jr" , 0x08},
};

static const std::unordered_map<std::string_view, u8> opcodeMap {
    {"addi", 0x08},
    {"andi", 0x0C},
    {"ori" , 0x0D},
    {"beq" , 0x04},
    {"bne" , 0x05},
    {"blt" , 0x06}, // Pseudo
    {"bge" , 0x07}, // Pseudo
    {"lw"  , 0x23},
    {"sw"  , 0x2b},
};

static const std::unordered_map<std::string_view, u8> jumpMap {
  {"j"  , 0x02},
  {"jal", 0x03},
};


auto Engine::assembleInstruction(u8* program, const Token& token, u64& address) -> void {
  u32 bin = 0;
  if (functMap.contains(token.value)) {
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
  } else if (opcodeMap.contains(token.value)) {
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
      case 0x0C: // andi
      case 0x0D: // ori
      case 0x23: // lw
      case 0x2b: // sw
        bin |= (imm & 0xFFFF);
      break;
    }
  } else if (jumpMap.contains(token.value)) {
    u8 opcode;
    u32 address;
    opcode = jumpMap.at(token.value);
    address = static_cast<u32>(token.args.at(0));

    bin |= (opcode & 0x3F) << 26;
    bin |= (address & 0x3FFFFFF);
  } else {
    std::string err{std::format("Mnemonic {} not found\n", token.value)};
    throw std::runtime_error(err);
  } 

  for (size_t i = 0; i < 4; i++) {
      program[address + i] = static_cast<u8>((bin >> i * 8) & 0xFF);
  }
  address += 4;
}

auto Engine::assembleSysCall(u8* program, const Token& token, u64& address)
    -> void {
  u32 bin = 0;
  // An ebreak is a R-type instruction with all
  // fields filled with zeroes except funct
  if (token.value == "ebreak") {
    bin |= (0x0D & 0x3F); // funct for ebreak is 0x0D
  } else {
    throw std::runtime_error{std::format("Mnemonic {} not found\n", token.value)};
  }

  // inserts it to program code
  for (size_t i = 0; i < 4; i++) {
    program[address + i] = static_cast<u8>((bin >> i * 8) & 0xFF);
  }

  address += 4;
}

auto Engine::assembleLiteral(u8* program, const Token& token, u64& address) -> void {
  if (token.directive == Directive::WORD) {
    for (u8 i = 0; i < token.args.size(); i++) {
      u32 bin = static_cast<u32>(token.args[i]);
      for (u8 i = 0; i < 4; i++) {
        program[address + i] = static_cast<u8>((bin >> i * 8) & 0xFF);
      }
      address += 4;
    }
  } else if (token.directive == Directive::SPACE) {
    address += token.args.front();
  } else {
    throw std::invalid_argument("Not implemented yet\n");
  }
}

auto Engine::assembler(const std::string& file) -> std::tuple<u8*, size_t> {
  this->tokenizer.parse(file);

  u8* program;
  u64 length = 0;
  u64 address = 0;

  for (const auto& token : this->tokenizer.tokens) {
    if (token.tokenType == Type::INSTRUCTION || token.tokenType == Type::SYS_CALL) {
      length += 4;
    } else if (token.tokenType == Type::LITERAL) {
      if (token.directive == Directive::WORD) {
        length += 4 * token.args.size();
      } else if (token.directive == Directive::SPACE) {
        length += token.args.front();
      }
    } else {
      throw std::runtime_error("Invalid token type");
    }
  }
  
  program = new u8[length];
  
  for (const auto& token : this->tokenizer.tokens) {
    if (token.tokenType == Type::INSTRUCTION) {
      this->assembleInstruction(program, token, address);
    } else if (token.tokenType == Type::SYS_CALL) {
      this->assembleSysCall(program, token, address);
    } else if (token.tokenType == Type::LITERAL) {
      this->assembleLiteral(program, token, address);
    } else {
      throw std::runtime_error(
        std::format("Invalid token {}\n", token.value)
      );
    }
  }

  //debug
  //for (u64 i = 0; i < length; i++) {
  //  std::cout << std::format("bin[{}] ", i) << std::bitset<8>(program[i]) << '\n';
  //}

  return {program, length};
}

auto Engine::setCPUstartAddress() -> void {
  cpu.pc = tokenizer.textStartAddress;
}

auto Engine::run(const std::span<u8>& code) -> void {
  this->cpu.loadProgram(code);
  this->setCPUstartAddress();

  this->printContentFromAllRegisters(); // Debug

  while (!this->cpu.hasHalted())
    this->cpu.nextInstruction();

  std::cout << "--------------------------------------\n"; // Debug
  this->printContentFromAllRegisters();                    // Debug
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