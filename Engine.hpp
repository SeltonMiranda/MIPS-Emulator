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

  // Assembles an MIPS asm file
  auto assembler(const std::string& file) -> std::tuple<u8*, size_t>;

  // Self-explanatory
  auto assembleSysCall(u8* program, const Token& token, u64& address) -> void;

  // Self-explanatory
  auto assembleInstruction(u8* program, const Token& token, u64& address) -> void;

  // Self-explanatory
  auto assembleLiteral(u8* program, const Token& token, u64& address) -> void;
  
  // Self-explanatory
  auto assemblePseudoInstruction(u8* program, const Token& token, u32& bin) -> void;

  // Self-explanatory
  auto assembleR(u8* program, const Token& token, u32& bin) -> void;

  // Self-explanatory
  auto assembleI(u8* program, const Token& token, u32& bin, u64& address) -> void;

  // Self-explanatory
  auto assembleJ(u8* program, const Token& token, u32& bin) -> void;

  // Helper function that handles the assemble
  auto assemble(u8* program) -> void;

  // Self-explanatory
  auto isRInstruction(const std::string& mnemonic) -> bool;

  // Self-explanatory
  auto isIInstruction(const std::string& mnemonic) -> bool;

  // Self-explanatory
  auto isJInstruction(const std::string& mnemonic) -> bool;

  // Self-explanatory
  auto isPseudoInstruction(const std::string& mnemonic) -> bool;

  // Inserts a binary 32-bits code into the program
  auto insertCode(u8* program, u32 bin, u64& address) -> void;

  // Computes the program's length before assemble it
  auto preComputeProgramLength() -> u64;

  // Runs the asm program
  auto run(const std::span<u8>& code) -> void;

  // Sets the start of main function
  auto setCPUstartAddress() -> void;

  // Debug purposes
  auto printContentFromAllRegisters() -> void;
  auto setContentToAllRegisters() -> void;
};

}