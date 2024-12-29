#include "Engine.hpp"

#include <bitset>
#include <random>
#include <set>

namespace Emulator {

static const std::unordered_map<std::string, u8> functMap {
    {"add", 0x20}, {"sub", 0x22},  {"and", 0x24}, {"or", 0x25},
    {"nor", 0x27},  {"sll", 0x00}, {"srl", 0x02}, {"slt", 0x2A},
    {"jr" , 0x08}
};

static const std::unordered_map<std::string, u8> opcodeMap {
    {"addi", 0x08},
    {"andi", 0x0C},
    {"ori" , 0x0D},
    {"beq" , 0x04},
    {"bne" , 0x05},
    {"blt" , 0x06}, // Pseudo
    {"bge" , 0x07}, // Pseudo
};

static const std::unordered_map<std::string, u8> jumpMap {
  {"j"  , 0x02},
  {"jal", 0x03},
};

Engine::~Engine() {
  if (this->cpu != nullptr)
    delete this->cpu;

  if (this->tokenizer != nullptr)
    delete this->tokenizer;
}

auto Engine::assembleInstruction(VecU8& program, ResolvedToken* token, u64 address) -> void {
  u32 bin = 0;
  if (functMap.contains(token->value)) {
    u8 rs, rt, rd, shamt, funct;

    shamt = 0;
    funct = functMap.at(token->value);
    rd = static_cast<u8>(token->args.at(0));

    switch (funct) {
      case 0x00: // sll
      case 0x02: // srl
        rt = static_cast<u8>(token->args.at(1));
        shamt = static_cast<u8>(token->args.at(2));
        break;

      case 0x20: // add
      case 0x22: // sub
      case 0x24: // and
      case 0x25: // or
      case 0x27: // nor 
      case 0x2A: // slt
        rs = static_cast<u8>(token->args.at(1));
        rt = static_cast<u8>(token->args.at(2));
        break;

      case 0x08: // jr
        rs = static_cast<u8>(token->args.at(0));
        break;
    }

    bin |= (rs    & 0x1F) << 21;    // rs
    bin |= (rt    & 0x1F) << 16;    // rt
    bin |= (rd    & 0x1F) << 11;    // rd
    bin |= (shamt & 0x1F) << 6;     // shamt
    bin |= (funct & 0x3F) << 0;     // funct
  } else if (opcodeMap.contains(token->value)) {
    u8 opcode, rt, rs;
    s16 imm;
    
    rt = static_cast<u8>(token->args.at(0));
    rs = static_cast<u8>(token->args.at(1));
    imm = static_cast<s16>(token->args.at(2));
    opcode = opcodeMap.at(token->value);

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
        bin |= (imm & 0xFFFF);
      break;
    }
  } else if (jumpMap.contains(token->value)) {
    u8 opcode;
    u32 address;

    opcode = jumpMap.at(token->value);
    address = static_cast<u32>(token->args.at(0));

    bin |= (opcode & 0x3F) << 26;
    bin |= (address & 0x3FFFFFF);
  } else {
    std::string err{std::format("Mnemonic {} in line {} not found\n", token->value, token->line)};
    throw std::runtime_error{err};
  }

  for (size_t i = 0; i < 4; i++) {
      program[address + i] = static_cast<u8>((bin >> i * 8) & 0xFF);
  }
}

auto Engine::assembleSysCall(VecU8 &program, ResolvedToken *token, u64 address)
    -> void {
  u32 bin{0};
  // An ebreak is a R-type instruction with all
  // fields filled with zeroes except funct
  if (token->value == "ebreak") {
    bin |= (0x0D & 0x3F); // funct for ebreak is 0x0D
  }

  // inserts it to program code
  for (size_t i = 0; i < 4; i++) {
    program[address + i] = static_cast<u8>((bin >> i * 8) & 0xFF);
  }
}

auto Engine::assemble(const std::vector<ResolvedToken*>& tokens) -> VecU8 {
  VecU8 program;
  u64 length{0};
  u64 address{0};

  // Pre-calculate program length to avoid realloc operations in vector
  for (size_t i = 0; i < size(tokens); i++) {
    if (tokens.at(i)->type == Type::INSTRUCTION || tokens.at(i)->type == Type::SYS_CALL) {
      length += 4;
    } else {
      length += tokens.at(i)->value.size();
    }
  }
  program.resize(length);

  for (size_t i = 0; i < size(tokens); i++) {
    if (tokens[i]->type == Type::INSTRUCTION) {
      this->assembleInstruction(program, tokens[i], address);
      address += 4;
    } else if (tokens[i]->type == Type::SYS_CALL) {
      this->assembleSysCall(program, tokens[i], address);
      address += 4;
    } else {
      std::cout << std::format("Unknown token type\n");
    }
  }
  return program;
}

auto Engine::assembler() -> VecU8 {
  std::vector<Token*> tokens{this->tokenizer->parse()};
  this->tokenizer->resolveTokens(tokens);

  for (size_t i = 0; i < size(tokens); i++) {
    if (tokens.at(i) != nullptr)
      delete tokens.at(i);
  }

  return this->assemble(this->tokenizer->getTokens());
}

auto Engine::run(const VecU8 &code) -> void {
  this->cpu->loadProgram(code);

  this->printContentFromAllRegisters(); // Debug

  while (!this->cpu->hasHalted())
    this->cpu->nextInstruction();

  std::cout << "--------------------------------------\n"; // Debug
  this->printContentFromAllRegisters();                    // Debug
}

// Debug
auto Engine::printBinaryProgram(const std::vector<std::uint8_t> &program)
    -> void {
  for (const auto &code : program) {
    std::cout << std::bitset<8>(code) << '\n';
  }
}

auto Engine::printContentFromAllRegisters() -> void {
  for (uint32_t i = 0; i < 32; i++) {
    std::cout << std::format("Content from register {} = {}\n", i,
                             this->cpu->readRegister(i));
  }
}

auto Engine::setContentToAllRegisters() -> void {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(1, 100);

  for (uint32_t i = 0; i < 32; i++) {
    this->cpu->writeRegister(i, static_cast<uint32_t>(dist(gen)));
  }
}

auto Engine::printTokens() -> void { this->tokenizer->printTokensResolved(); }

auto Engine::printMap() -> void {
  this->tokenizer->printMap();
}

}