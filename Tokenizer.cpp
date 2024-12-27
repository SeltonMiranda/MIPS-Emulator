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

auto Tokenizer::getLabelsMap() -> const std::unordered_map<std::string, u64>& {
  return this->labelsToAddress;
}

auto Tokenizer::getTokens() -> const std::vector<ResolvedToken *> & {
  return this->resolvedTokens;
}

auto Tokenizer::isLabel(const std::string& label) -> bool {
  return !label.empty() && label.back() == ':';
}

auto Tokenizer::parse() -> std::vector<Token*> {
  std::string line;
  std::vector<Token *> tokens;
  size_t lineNumber{0};

  while (std::getline(this->file, line)) {
    VecString symbols; // Yes, I have to use vector because boost::split
    boost::trim(line);
    boost::split(symbols, line, boost::is_any_of(", "), boost::token_compress_on);
    std::erase_if(symbols, [](const std::string &s) { return s.empty(); }); // Remove empty strings

    // TODO: parse literals before instructions
    if (this->isLabel(symbols[0])) {
      symbols[0].erase(symbols[0].begin() + symbols[0].size() - 1); // remove ':'
      tokens.emplace_back(new Token{Type::LABEL, lineNumber, symbols[0]});
    } else if (symbols[0] == "ebreak") {
      tokens.emplace_back(new Token{Type::SYS_CALL, lineNumber, symbols[0]});
    } else {
      if (!symbols.empty()) {
        // Creates token for instruction mnemonic
        tokens.emplace_back(new Token{Type::INSTRUCTION, lineNumber, symbols[0]});
        // Creates tokens to intructions arguments
        for (size_t i = 1; i < symbols.size(); i++) {
          tokens.emplace_back(new Token{Type::ARG, lineNumber, symbols[i]});
        }
      }
    }
    lineNumber++;
  }
  return tokens;
}

static const std::unordered_map<std::string, u64> fixedRegisters = {
    {"zero", 0},
    {"v0", 2},
    {"v1", 3},
    {"sp", 29},
    {"ra", 31},
};

auto Tokenizer::parseRegister(const char *arg) -> u64 {
  
  if (fixedRegisters.contains(std::string(arg))) { // Checks if it's a named register
    return fixedRegisters.at(arg);
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
    throw std::invalid_argument("No such register: " + std::string(arg) + "\n");
  }
}

auto Tokenizer::translate(const std::string &arg) -> u64 {
  if (this->labelsToAddress.contains(arg)) {
    return this->labelsToAddress[arg];
  }

  // First, we get the immediate, if it has
  if ((0x30 <= arg.front() && arg.front() <= 0x39) || arg.front() == '-') {
    return static_cast<u64>(std::stoi(arg));
  }

  return this->parseRegister(arg.data());
}

auto Tokenizer::translateArgs(const VecString& args) -> VecU64 {
  if (size(args) < 1 || size(args) >= 4) {
    throw std::invalid_argument(
        std::format("ERROR! Not enough arguments in line"));
  }

  VecU64 translatedArgs;
  for (const auto &arg : args) {
    translatedArgs.push_back(this->translate(arg));
  }

  return translatedArgs;
}

auto Tokenizer::resolveTokens(const std::vector<Token *> &tokens) -> void {
  u64 address = 0;
  std::unordered_map<u64, VecString> arguments;

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
      VecString args;
      while (i < size(tokens) && tokens[i]->type == Type::ARG) {
        if (!tokens[i]->value.empty()) {
          args.push_back(tokens[i]->value);
        }
        i++;
      }
      i--;

      arguments[address] = args;
      this->resolvedTokens.push_back(_resolvedToken);
      address += 4;
      break;
    }

    case Type::SYS_CALL:
      this->resolvedTokens.emplace_back(new ResolvedToken{
          tokens[i]->type, tokens[i]->line, tokens[i]->value, address});

      address += 4;
      break;

    case Type::LABEL:
      this->labelsToAddress[tokens[i]->value] = address;
      break;
    }
  }

  for (const auto& token : this->resolvedTokens) {
    if (token->type == Type::INSTRUCTION) {
      token->args = this->translateArgs(arguments[token->address]);
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

    case Type::LABEL:
      type = "label";
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

auto Tokenizer::printTokenss(const std::vector<Token*>& tokens) -> void {
    for (const auto &token : tokens) {
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

    case Type::LABEL:
      type = "label";
      break;
    }



    std::cout << std::format("Token value {}, line {}, type {}\n",
                             token->value, token->line, type);
  }
}

auto Tokenizer::printMap() -> void {
  for (const auto& [key, value] : this->labelsToAddress) {
    std::cout << std::format("key {} value {}\n", key, value);
  }
}

} // namespace Emulator