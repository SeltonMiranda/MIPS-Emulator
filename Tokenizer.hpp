// TODO
// Remember to deallocate memory for std::vector<Token*>

#ifndef _TOKENIZER_
#define _TOKENIZER_

#include "Config.hpp"

namespace Emulator {

enum class Type { INSTRUCTION, ARG, SYS_CALL };

struct Token // (Unresolved token)
{
  Type type;         // Type of the token
  size_t line;       // line that token was in it
  std::string value; // his value (may be an instruction, arg, label, literal)

  Token(Type type, size_t line, const std::string &value);
};

struct ResolvedToken {
  Type type;         // Type of the token (INSTRUCTION, LABEL, LITERAL)
  size_t line;       // Line where it was found
  std::string value; // mnemonic or label or literal
  u64 address;       // address
  VecU64 args;       // registers (rd, rs, rt)

  ResolvedToken(Type type, size_t line, const std::string &value, u64 address);
};

class Tokenizer {
private:
  std::ifstream file;                          // input file stream
  std::vector<ResolvedToken *> resolvedTokens; // self-explanatory

  // Parses line
  auto parseLine(std::vector<Token *> &tokens, std::string &line,
                 size_t lineNumber) -> void;

  // Parses a register
  auto parseRegister(const std::string &arg) -> u64;

  // Translantes an register to uint64_t
  auto translateArgs(const std::vector<std::string> &args) -> VecU64;

public:
  Tokenizer(std::ifstream &&file);
  Tokenizer(const std::string &file);
  ~Tokenizer();

  auto getTokens() -> const std::vector<ResolvedToken *> &;

  // Parses a file, returns an array of (unresolved) tokens
  auto parse() -> std::vector<Token *>;

  // Generates resolved tokens, this tokens are the ones that will actually be
  // used
  auto resolveTokens(const std::vector<Token *> &tokens) -> void; // todo

  // Debug purposes
  auto printTokens() -> void;
  auto printTokensResolved() -> void;
};

} // namespace Emulator

#endif // _TOKENIZER_