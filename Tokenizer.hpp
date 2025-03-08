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

  auto parseLabel(std::string& symbol, u64 address) -> void;
  auto parseSysCall(std::string& symbol, u64 address) -> void;
  auto parseInstruction(VecString& symbols, u64 address, std::unordered_map<u64, VecString>& _args) -> void;
  auto parseDataSection(std::string& line, u64& address) -> void;

  auto removeInlineComments(std::string& line) -> void;
  auto isNumber(const char& c) -> bool;
  auto isSysCall(const std::string& call) -> bool;
  auto isLabel(const std::string& label) -> bool; 
  auto parseArgs(const VecString& args) -> VecU64;
  auto validateArgumentsSize(const std::string& mnemonic, const VecString& args) -> bool;
  auto parse(const std::string& file) -> void;

  auto printTokens() -> void;
};

} // namespace Emulator

#endif // _TOKENIZER_