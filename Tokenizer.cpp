#include "Tokenizer.hpp"

namespace Emulator {

Tokenizer::Tokenizer(std::ifstream &&file) : file{std::move(file)} {}

Tokenizer::Tokenizer(const std::string &file) {
  this->file.open(file);
  if (!this->file.is_open()) {
    throw std::runtime_error{"Couldn't open file\n"};
  }
}

Token::Token(Type type, size_t line, const std::string &value)
    : type{type}, line{line}, value{value} {}

ResolvedToken::ResolvedToken(Type type, size_t line, const std::string &value,
                             u64 address)
    : type{type}, line{line}, value{value}, address{address} {}

Tokenizer::~Tokenizer() {
  if (this->file.is_open()) {
    this->file.close();
  }

  for (const auto &token : this->resolvedTokens) {
    delete token;
  }
}

auto Tokenizer::getTokens() -> const std::vector<ResolvedToken *> & {
  return this->resolvedTokens;
}

auto Tokenizer::parse() -> std::vector<Token *> {
  std::string line;
  std::vector<Token *> tokens;
  size_t lineNumber{0};

  while (std::getline(this->file, line)) {
    VecString symbols; // Yes, I have to use vector because boost::split
    boost::trim(line);
    boost::split(symbols, line, boost::is_any_of(", "),
                 boost::token_compress_on);
    std::erase_if(symbols, [](const std::string &s) {
      return s.empty();
    }); // Remove empty strings

    // TODO: parse literals before instructions

    if (symbols[0] == "ebreak") {
      tokens.emplace_back(new Token{Type::SYS_CALL, lineNumber, symbols[0]});
    } else {
      // Creates token for instruction mnemonic
      tokens.emplace_back(new Token{Type::INSTRUCTION, lineNumber, symbols[0]});
      // Creates tokens to intructions arguments
      for (size_t i = 1; i < symbols.size(); i++) {
        tokens.emplace_back(new Token{Type::ARG, lineNumber, symbols[i]});
      }
    }
    lineNumber++;
  }
  return tokens;
}

static const std::unordered_map<std::string, u64> fixedRegisters = {
    {"zero", 0},
    {"sp", 29},
    {"ra", 31},
};

auto Tokenizer::parseRegister(const char *arg) -> u64 {
  auto value{fixedRegisters.find(arg)}; // Checks if it's a named register
  if (value != fixedRegisters.end()) {
    return value->second;
  }

  char prefix{arg[0]};
  u64 number;
  try {
    number = std::atoi(arg + 1);
  } catch (std::exception &e) {
    throw std::invalid_argument("No such register " + std::string(arg));
  }

  switch (prefix) {
  case 't':
    return (number < 8) ? number + 8 : number + 16;
  case 's':
    return number + 16;
  case 'a':
    return number + 4;
  case '$':
    return number;
  default:
    throw std::invalid_argument("No such register: " + std::string(arg));
  }
}

auto Tokenizer::translate(const std::string &arg) -> u64 {
  // First, we get the immediate, if it has
  if ((0x30 <= arg[0] && arg[0] <= 0x39) || arg[0] == '-') {
    return static_cast<u64>(std::stoi(arg));
  }

  return this->parseRegister(arg.data());
}

auto Tokenizer::translateArgs(const std::array<std::string, 3> &args)
    -> std::array<u64, 3> {
  if (size(args) < 3)
    throw std::invalid_argument(
        std::format("ERROR! Not enough arguments in line"));

  std::array<u64, 3> translatedArgs;
  size_t i{0};
  for (const auto &arg : args) {
    // TODO : Check if argument is a label, if it is then do nothing
    translatedArgs.at(i) = this->translate(arg);
    i++;
  }

  return translatedArgs;
}

auto Tokenizer::resolveTokens(const std::vector<Token *> &tokens) -> void {
  u64 address{0};

  for (size_t i = 0; i < size(tokens); i++) {
    switch (tokens[i]->type) {
    case Type::ARG:
      throw std::invalid_argument("Token type ARG mustn't resolve\n");
      break;

    case Type::INSTRUCTION: {
      ResolvedToken *_resolvedToken{new ResolvedToken{
          tokens[i]->type, tokens[i]->line, tokens[i]->value, address}};

      // insert arguments into a vector to facilitate it
      i++;
      std::array<std::string, 3> args;
      size_t j{0};
      while (i < size(tokens) && tokens[i]->type == Type::ARG) {
        std::cout << "arg " << tokens[i]->value << "\n";
        args.at(j) = tokens[i]->value;
        j++;
        i++;
      }
      i--;

      // Process the arguments
      _resolvedToken->args = this->translateArgs(args);
      this->resolvedTokens.push_back(_resolvedToken);
      address += 4;
      break;
    }

    case Type::SYS_CALL:
      this->resolvedTokens.emplace_back(new ResolvedToken{
          tokens[i]->type, tokens[i]->line, tokens[i]->value, address});

      address += 4;
      break;
    }
  }
}

auto Tokenizer::printTokensResolved() -> void {
  for (const auto &token : this->resolvedTokens) {
    std::string type;
    switch (token->type) {
    case Type::INSTRUCTION:
      type = "INSTRUCTION";
      break;

    case Type::ARG:
      type = "ARG";
      break;

    case Type::SYS_CALL:
      type = "SYS_CALL";
      break;
    }

    std::cout << std::format("Token value {}, line {}, type {}, address {}\n",
                             token->value, token->line, type, token->address);
    if (token->type == Type::INSTRUCTION) {
      size_t i{0};
      for (const auto &arg : token->args) {
        std::cout << std::format("\targ {} ; {}\n", i, arg);
        i++;
      }
    }
  }
}

} // namespace Emulator