#pragma once

#include "CPU.hpp"
#include "Tokenizer.hpp"

namespace Emulator {

class Engine {
private:
  Tokenizer *tokenizer;
  CPU *cpu;

  auto assembleSysCall(VecU8 &program, ResolvedToken *token, u64 address)
      -> void;
  auto assembleInstruction(VecU8 &program, ResolvedToken *token, u64 address)
      -> void;
  auto assemble(const std::vector<ResolvedToken *> &tokens) -> VecU8;

public:
  Engine() = default;
  Engine(Tokenizer *tokenizer, CPU *cpu) : tokenizer{tokenizer}, cpu{cpu} {};
  ~Engine();

  auto run(const VecU8 &code) -> void;
  auto assembler() -> VecU8;

  // Debug purposes
  auto printBinaryProgram(const VecU8 &program) -> void;
  auto printTokens() -> void;
  auto printContentFromAllRegisters() -> void;
  auto setContentToAllRegisters() -> void;
};

} // namespace Emulator