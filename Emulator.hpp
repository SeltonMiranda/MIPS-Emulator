#pragma once

#include "CPU.hpp"
#include "Tokenizer.hpp"

namespace Emulator {
class Emulator {
private:
  Tokenizer *tokenizer;
  CPU *cpu;

  auto assembleInstruction(std::vector<uint8_t> &program, ResolvedToken *token)
      -> void;
  auto assemble(const std::vector<ResolvedToken *> &tokens)
      -> std::vector<uint8_t>;

public:
  Emulator() = default;
  Emulator(Tokenizer *tokenizer, CPU *cpu) : tokenizer{tokenizer}, cpu{cpu} {};
  ~Emulator();

  auto run(const std::vector<uint8_t> &code) -> void;
  auto assembler() -> std::vector<std::uint8_t>;

  // Debug purposes
  auto printBinaryProgram(const std::vector<std::uint8_t> &program) -> void;
};
} // namespace Emulator