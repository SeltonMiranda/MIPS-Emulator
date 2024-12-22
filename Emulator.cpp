#include "Emulator.hpp"

#include <bitset>

namespace Emulator {

Emulator::~Emulator() {
  if (this->cpu != nullptr)
    delete this->cpu;

  if (this->tokenizer != nullptr)
    delete this->tokenizer;
}

auto Emulator::assembleInstruction(std::vector<uint8_t> &program,
                                   ResolvedToken *token) -> void {
  std::uint32_t bin{0};

  static const std::unordered_map<std::string, uint8_t> functMap{
      {"add", 0x20}, {"sub", 0x22},  {"and", 0x24}, {"or", 0x25},
      {"xor", 0x26}, {"nor", 0x27},  {"sll", 0x00}, {"srl", 0x02},
      {"sra", 0x03}, {"mult", 0x18}, {"div", 0x1A}, {"ebreak", 0x0D},
  };

  auto it{functMap.find(token->value)};
  uint8_t funct{it->second};

  if (token->value == "ebreak") { // an ebreak is a R-type instruction with all
                                  // fields filled with zeroes except funct
    bin |= (0 << 26);
    bin |= (0 & 0x1F) << 21;
    bin |= (0 & 0x1F) << 16;
    bin |= (0 & 0x1F) << 11;
    bin |= (0 & 0x1F) << 6;
    bin |= (funct & 0x3F);
  } else { // Process others instructions
    uint8_t rd{static_cast<uint8_t>(token->args[0])}; // rd
    uint8_t rs{static_cast<uint8_t>(token->args[1])}; // rs
    uint8_t rt{static_cast<uint8_t>(token->args[2])}; // rt
    uint8_t shamt =
        (token->args.size() > 3) ? static_cast<uint8_t>(token->args[3]) : 0;

    // Builds the 32 bits binary
    bin |= (0 << 26);           // opcode (Only has R-format instrution for now)
    bin |= (rs & 0x1F) << 21;   // rs
    bin |= (rt & 0x1F) << 16;   // rt
    bin |= (rd & 0x1F) << 11;   // rd
    bin |= (shamt & 0x1F) << 6; // shamt
    bin |= (funct & 0x3F);      // funct
  }

  // inserts it to program code
  program.push_back(static_cast<uint8_t>(bin & 0xFF));
  program.push_back(static_cast<uint8_t>((bin >> 8) & 0xFF));
  program.push_back(static_cast<uint8_t>((bin >> 16) & 0xFF));
  program.push_back(static_cast<uint8_t>((bin >> 24) & 0xFF));
}

auto Emulator::assemble(const std::vector<ResolvedToken *> &tokens)
    -> std::vector<std::uint8_t> {
  std::vector<std::uint8_t> program;
  std::uint64_t address{0};

  for (size_t i = 0; i < tokens.size(); i++) {
    if (tokens[i]->type == Type::INSTRUCTION) {
      this->assembleInstruction(program, tokens[i]);
      address += 4; // it'll be used for others tokens
    } else {
      std::cout << std::format("Unknown token type\n");
    }
  }

  return program;
}

auto Emulator::assembler() -> std::vector<std::uint8_t> {
  std::vector<Token *> tokens{this->tokenizer->parse()};
  this->tokenizer->resolveTokens(tokens);
  return this->assemble(this->tokenizer->getTokens());
}

auto Emulator::printBinaryProgram(const std::vector<std::uint8_t> &program)
    -> void {
  for (const auto &code : program) {
    std::cout << std::bitset<8>(code) << '\n';
  }
}

auto Emulator::run(const std::vector<uint8_t> &code) -> void {
  this->cpu->loadProgram(code);
  uint32_t address{0};

  this->cpu->nextInstruction();
  return;
}

} // namespace Emulator