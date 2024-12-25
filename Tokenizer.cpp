#include "Tokenizer.hpp"

namespace Emulator {

Tokenizer::Tokenizer(std::ifstream &&file) : file{std::move(file)} {}

Tokenizer::Tokenizer(const std::string &file) {
  this->file.open(file);
  if (!this->file.is_open()) {
    throw std::runtime_error{"Couldn't open file\n"};
  }
}

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

Token::Token(Type type, size_t line, const std::string &value)
    : type{type}, line{line}, value{value} {}

ResolvedToken::ResolvedToken(Type type, size_t line, const std::string &value,
                             u64 address)
    : type{type}, line{line}, value{value}, address{address} {}

auto Tokenizer::parseLine(std::vector<Token *> &tokens, std::string &line,
                          size_t lineNumber) -> void {
  VecString symbols;
  boost::trim(line);
  boost::split(symbols, line, boost::is_any_of(", "), boost::token_compress_on);
  std::erase_if(symbols, [](const std::string &s) { return s.empty(); });

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
}

auto Tokenizer::parse() -> std::vector<Token *> {
  std::string line;
  std::vector<Token *> tokens;
  size_t i{0};

  while (std::getline(
      this->file,
      line)) { // The program for a while only has R-type instructions
    this->parseLine(tokens, line, i);
    i++;
  }

  return tokens;
}

auto Tokenizer::parseRegister(const std::string &arg) -> u64 {
  if ((0x30 <= arg[0] && arg[0] <= 0x39) || arg[0] == '-') {
    return static_cast<u64>(std::stoi(arg));
  }

  auto it = std::find_if(begin(arg), end(arg), ::isdigit);
  if (it == end(arg)) {
    throw std::invalid_argument("Invalid register format: " + arg);
  }

  return static_cast<u64>(std::stoull(
      std::string(it, end(arg)))); // Yes, I need to do this bullshit
}

auto Tokenizer::translateArgs(const VecString &args) -> VecU64 {
  if (size(args) < 3)
    throw std::runtime_error("ERROR! Instruction arguments aren't enough\n");

  for (const auto &arg : args) {
    std::cout << arg << "\n";
  }
  VecU64 translatedArgs;
  for (const auto &arg : args) {
    translatedArgs.push_back(this->parseRegister(arg));
  }

  return translatedArgs;
}

auto Tokenizer::resolveTokens(const std::vector<Token *> &tokens) -> void {
  u64 address{0};

  for (size_t i = 0; i < size(tokens); i++) {
    switch (tokens[i]->type) {
    case Type::ARG:
      std::cout << "This token could not resolve\n";
      break;

    case Type::INSTRUCTION: {
      ResolvedToken *_resolvedToken{new ResolvedToken{
          tokens[i]->type, tokens[i]->line, tokens[i]->value, address}};

      // insert arguments into a vector to facilitate it
      i++;
      VecString args;
      while (i < size(tokens) && tokens[i]->type == Type::ARG) {
        std::cout << "arg " << tokens[i]->value << "\n";
        args.push_back(tokens[i]->value);
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