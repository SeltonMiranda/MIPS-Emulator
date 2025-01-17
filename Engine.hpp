// TODO
// Branch instructions
// Jump instructions
// Store and Load instructions


#pragma once

#include "CPU.hpp"
#include "Tokenizer.hpp"

namespace Emulator {

class Engine {
private:
  Tokenizer tokenizer;
  CPU cpu;

  auto assembleSysCall(u8* program, const Token& token, u64 address) -> void;
  auto assembleInstruction(u8* program, const Token& token, u64 address) -> void;

public:
  Engine() = default;
  Engine(Tokenizer tokenizer, CPU cpu) : tokenizer{tokenizer}, cpu{cpu} {};

  auto run(const std::span<u8> code) -> void;
  auto assembler(const std::string& file) -> std::tuple<u8*, size_t>;

  // Debug purposes
  auto printBinaryProgram(const VecU8 &program) -> void;
  auto printTokens() -> void;
  auto printContentFromAllRegisters() -> void;
  auto setContentToAllRegisters() -> void;
  auto printMap() -> void;
};

}