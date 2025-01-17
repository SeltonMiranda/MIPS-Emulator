#include "Tokenizer.hpp"

namespace Emulator {

Tokenizer::Tokenizer(std::ifstream &&file) : file{std::move(file)} {}

Tokenizer::Tokenizer(const std::string &file) {
  this->file.open(file);
  if (!this->file.is_open()) {
    throw std::runtime_error{"Couldn't open file\n"};
  }
}


auto Tokenizer::isLabel(const std::string& label) -> bool {
  return label.back() == ':';
}

auto Tokenizer::parse() -> std::vector<Token> {
  std::string line;
  std::vector<Token> tokens;
  u64 address = 0;

  while (std::getline(this->file, line)) {
    VecString symbols;

    if (line.empty() || line[0] == '#') {
      continue;
    }
    
    boost::trim(line);
    boost::algorithm::to_lower(line);
    boost::split(symbols, line, boost::is_any_of(", "), boost::token_compress_on);

    if (this->isLabel(symbols[0])) {
      symbols[0].erase(symbols[0].begin() + symbols[0].size() - 1); // remove ':'
      tokens.push_back({Type::LABEL, symbols[0], address});
    } else if (symbols[0] == "ebreak") {
      tokens.push_back({Type::SYS_CALL, symbols[0], address});
    } else {
      Token instructionToken;
      instructionToken.tokenType = Type::INSTRUCTION;
      instructionToken.value = symbols[0];
      
      std::vector<std::string> args;

      for (size_t i = 1; i < 4; i++) {
        args.push_back(symbols[i]);
      }


      
    }

    address += 4;
  }
  return tokens;
}

auto validateArguments(const std::vector<std::string>& args) -> bool {
  u8 shouldHave;


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

} // namespace Emulator