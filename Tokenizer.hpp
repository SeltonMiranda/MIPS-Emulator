// TODO
// Remember to deallocate memory for std::vector<Token*>

#ifndef _TOKENIZER_
#define _TOKENIZER_

#include "Config.hpp"

namespace Emulator {

enum class Type { INSTRUCTION, SYS_CALL, LABEL };

struct Token {
  Type tokenType;
  std::string value;
  u64 address;
  std::vector<u64> args;
};

class Tokenizer {
public:
  std::ifstream file;
  std::vector<Token> tokens;

  // Translates the argument
  auto translate(const std::string &arg) -> u64;

  // Parses the register's number
  auto parseRegister(const char *arg) -> u64;

  // Translantes arguments into an array of u64
  auto translateArgs(const VecString& args) -> VecU64;


  auto isLabel(const std::string& label) -> bool; 

  auto validateArguments(const std::vector<std::string>& args) -> bool;

  Tokenizer(std::ifstream &&file);
  Tokenizer(const std::string &file);

  // Parses a file, returns an array of (unresolved) tokens
  auto parse() -> std::vector<Token>;

};

} // namespace Emulator

#endif // _TOKENIZER_