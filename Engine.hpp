#pragma once

#include "CPU.hpp"
#include "Tokenizer.hpp"

namespace Emulator {

class Engine {
public:
  Tokenizer& tokenizer;
  CPU& cpu;

  Engine() = default;
  Engine(Tokenizer& tokenizer, CPU& cpu) : tokenizer{tokenizer}, cpu{cpu} {};

  auto assembler(const std::string& file) -> std::tuple<u8*, size_t>;

  auto assembleSysCall(u8* program, const Token& token, u64& address) -> void;
  auto assembleInstruction(u8* program, const Token& token, u64& address) -> void;
  auto assembleLiteral(u8* program, const Token& token, u64& address) -> void;
  auto assemblePseudoInstruction(u8* program, const Token& token, u32& bin) -> void;
  auto assembleR(u8* program, const Token& token, u32& bin) -> void;
  auto assembleI(u8* program, const Token& token, u32& bin, u64& address) -> void;
  auto assembleJ(u8* program, const Token& token, u32& bin) -> void;
  auto assemble(u8* program) -> void;

  auto isRInstruction(const std::string& mnemonic) -> bool;
  auto isIInstruction(const std::string& mnemonic) -> bool;
  auto isJInstruction(const std::string& mnemonic) -> bool;
  auto isPseudoInstruction(const std::string& mnemonic) -> bool;

  auto insertCode(u8* program, u32 bin, u64& address) -> void;
  auto preComputeProgramLength() -> u64;
  auto run(const std::span<u8>& code) -> void;
  auto setCPUstartAddress() -> void;
  auto printContentFromAllRegisters() -> void;
  auto setContentToAllRegisters() -> void;
};

}