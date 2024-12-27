#include "Engine.hpp"

#include <bitset>
#include <random>
#include <set>

namespace Emulator {

static const std::unordered_map<std::string, u8> functMap{
    {"add", 0x20}, {"sub", 0x22},  {"and", 0x24}, {"or", 0x25},
    {"nor", 0x27},  {"sll", 0x00}, {"srl", 0x02}, {"slt", 0x2A},
};

static const std::unordered_map<std::string, u8> opcodeMap{
    {"addi", 0x08},
    {"andi", 0x0C},
    {"ori" , 0x0D},
    {"beq" , 0x04},
};

Engine::~Engine() {
  if (this->cpu != nullptr)
    delete this->cpu;

  if (this->tokenizer != nullptr)
    delete this->tokenizer;
}

auto Engine::assembleInstruction(VecU8 &program, ResolvedToken *token, u64 address) -> void {
  u32 bin = 0;

  if (functMap.contains(token->value)) {
    u8 funct{functMap.at(token->value)};
    u8 rs, rt, rd;
    u8 shamt = 0;

    rd = static_cast<u8>(token->args.at(0));
    if (token->value == "sll" || token->value == "srl") {
      rt = static_cast<u8>(token->args.at(1));
      shamt = static_cast<u8>(token->args.at(2));
    } else {
      rs = static_cast<u8>(token->args.at(1));
      rt = static_cast<u8>(token->args.at(2));
    }

    // Builds the 32 bits binary
    bin |= (rs    & 0x1F) << 21;    // rs
    bin |= (rt    & 0x1F) << 16;    // rt
    bin |= (rd    & 0x1F) << 11;    // rd
    bin |= (shamt & 0x1F) << 6;     // shamt
    bin |= (funct & 0x3F) << 0;     // funct
  } else if (opcodeMap.contains(token->value)) {
    u8 opcode, rt, rs;
    s16 imm;

    opcode = opcodeMap.at(token->value);
    rt = static_cast<u8>(token->args.at(0));
    rs = static_cast<u8>(token->args.at(1));
    imm = static_cast<s16>(token->args.at(2));

    bin |= (opcode &   0x3F) << 26;
    bin |= (rs     &   0x1F) << 21;
    bin |= (rt     &   0x1F) << 16;
    if (token->value == "beq") {
      s32 offset = (imm - address) >> 2;
      bin |= (offset & 0xFFFF);
    } else {
      bin |= (imm    & 0xFFFF);
    }

  } else {
    std::string err{std::format("Mnemonic {} in line {} not found\n", token->value, token->line)};
    throw std::runtime_error{err};
  }

  // inserts it to program code (big-endian)
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