#pragma once

#include "CPU.hpp"
#include "Tokenizer.hpp"

namespace Emulator {

class Engine {
public:
  Tokenizer& tokenizer;
  CPU& cpu;

  auto assembleSysCall(u8* program, const Token& token, u64& address) -> void;
  auto assembleInstruction(u8* program, const Token& token, u64& address) -> void;
  auto assembleLiteral(u8* program, const Token& token, u64& address) -> void;

  Engine() = default;
  Engine(Tokenizer& tokenizer, CPU& cpu) : tokenizer{tokenizer}, cpu{cpu} {};

  auto preComputeProgramLength() -> u64;
  auto run(const std::span<u8>& code) -> void;
  auto assembler(const std::string& file) -> std::tuple<u8*, size_t>;
  auto setCPUstartAddress() -> void;
  auto printContentFromAllRegisters() -> void;
  auto setContentToAllRegisters() -> void;
};

}