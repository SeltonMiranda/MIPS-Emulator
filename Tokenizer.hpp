#ifndef _TOKENIZER_
#define _TOKENIZER_

#include "Config.hpp"

namespace Emulator {

enum class Type { INSTRUCTION, SYS_CALL, LABEL, LITERAL };

enum class Directive { WORD, SPACE, ASCIIZ };

struct Token {
  Type tokenType;
  u64 address;
  VecU64 args;
  std::string value;
  Directive directive;
};

class Tokenizer {
public:
  std::vector<Token> tokens;
  std::unordered_map<std::string, u64> labelsToAddress;
  u64 textStartAddress;

  // Parses the register's number
  auto parseRegister(const char *arg) -> u64;

  // Parses the label 
  auto parseLabel(std::string& symbol, u64 address) -> void;

  // Parses the syscall
  auto parseSysCall(std::string& symbol, u64 address) -> void;

  // Parses an Instruction
  // Parameter _args stores the address from a label (wether the instruction has any as argument)
  auto parseInstruction(VecString& symbols, u64 address, std::unordered_map<u64, VecString>& _args) -> void;

  // Parses the data section
  auto parseDataSection(std::string& line, u64& address) -> void;

  // Self-explanatory
  auto removeInlineComments(std::string& line) -> void;
  
  // Self-explanatory
  auto isNumber(const char& c) -> bool;

  // Self-explanatory
  auto isSysCall(const std::string& call) -> bool;

  // Self-explanatory
  auto isLabel(const std::string& label) -> bool; 

  // Parses arguments from an instruction
  auto parseArgs(const VecString& args) -> VecU64;

  // Verifies if total of arguments matches the respectively instruction
  auto validateArgumentsSize(const std::string& mnemonic, const VecString& args) -> bool;

  // Parses the asm file
  auto parse(const std::string& file) -> void;

  // Debug purposes
  auto printTokens() -> void;
};

} // namespace Emulator

#endif // _TOKENIZER_