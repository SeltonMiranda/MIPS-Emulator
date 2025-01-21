#include "Tokenizer.hpp"
#include "debugHelper.hpp"

namespace Emulator {

auto Tokenizer::isSysCall(const std::string& call) -> bool {
  return call == "ebreak";
}

auto Tokenizer::isLabel(const std::string& label) -> bool {
  return label.back() == ':';
}

auto Tokenizer::parseLabel(std::string& symbol, u64 address) -> void {
  symbol.erase(symbol.begin() + symbol.size() - 1); // remove ':'
  this->labelsToAddress[symbol] = address;
}

auto Tokenizer::parseSysCall(std::string& symbol, u64 address) -> void {
  Token SysCallToken;
  SysCallToken.tokenType = Type::SYS_CALL;
  SysCallToken.value = symbol;
  SysCallToken.address = address;
  this->tokens.push_back(SysCallToken);
}

auto Tokenizer::parseInstruction(VecString& symbols, u64 address, std::unordered_map<u64, VecString>& _args) -> void {
  Token instructionToken;
  instructionToken.tokenType = Type::INSTRUCTION;
  instructionToken.value = symbols[0];
  instructionToken.address = address;
  
  std::vector<std::string> args;
  for (size_t i = 1; i < size(symbols); i++) {
    args.push_back(symbols[i]);
  }

  if (!validateArgumentsSize(symbols[0], args)) {
    throw std::invalid_argument(
      std::format("ERROR! Not enough arguments")
    );
  }

  this->tokens.push_back(instructionToken);

  _args[address] = args;
}

auto Tokenizer::parseDataSection(std::string& line, u64& address) -> void {
  VecString symbols;
  boost::split(symbols, line, boost::is_any_of(": "), boost::token_compress_on);

  if (symbols.size() < 3 || symbols[1] != ".word") {
    throw std::runtime_error(
      std::format("Invalid literal declaration: {}", line)
    );
  }

  Token literalToken;
  literalToken.directive = Directive::WORD;
  literalToken.tokenType = Type::LITERAL;
  literalToken.address = address;
  literalToken.value = symbols[0];

  u8 numLiteral = 0;
  for (size_t i = 2; i < symbols.size(); i++) {
    u32 arg = std::stoi(symbols[i]);
    literalToken.args.push_back(std::stoi(symbols[i]));
    numLiteral++;
  }

  this->tokens.push_back(literalToken);
  this->labelsToAddress[literalToken.value] = address;
  address += 4 * numLiteral;
}


auto Tokenizer::parse(const std::string& file) -> void {
  std::ifstream _file{file};

  if (!_file.is_open()) {
    throw std::runtime_error(
      std::format("Couldn't open file {}", file)
    );
  }

  std::string line;
  std::string section;
  u64 address = 0;
  std::unordered_map<u64, VecString> _args;
  int i = 0;

  while (std::getline(_file, line)) {
    VecString symbols;
    boost::trim(line);
    boost::algorithm::to_lower(line);
    
    if (line.empty() || line.starts_with('#')) {
      continue;
    }

    if (line == ".data") {
      section = ".data";
      continue;
    } else if (line == ".text") {
      section = ".text";
      this->textStartAddress = address;
      continue;
    }

    // debug
    std::cout << std::format("line {} : {}\n", i++, line);

    if (section == ".data") {
      this->parseDataSection(line, address);
    } else if (section == ".text") {
      boost::split(symbols, line, boost::is_any_of(", "), boost::token_compress_on);

      if (this->isLabel(symbols[0])) {
        this->parseLabel(symbols[0], address);
      } else if (this->isSysCall(symbols[0])) {
        this->parseSysCall(symbols[0], address);
        address += 4;
      } else {
        this->parseInstruction(symbols, address, _args);
        address += 4;
      }

    } else {
      throw std::runtime_error("ERROR! Directive not found\n");
    }
  }

  for (auto& token : this->tokens) {  
    if (token.tokenType == Type::INSTRUCTION) {
      token.args = this->parseArgs(_args[token.address]);
    }
  }
  
  _file.close();
}

auto Tokenizer::isNumber(const char& c) -> bool {
  return (0x30 <= c && c <= 0x39) || c == '-';
}

auto Tokenizer::parseArgs(const VecString& args) -> VecU64 {
  VecU64 parsedArgs;
  for (const auto& arg: args) {
    if (this->labelsToAddress.contains(arg)) {
      parsedArgs.push_back(this->labelsToAddress[arg]);
    } else if (this->isNumber(arg.front())) {
      parsedArgs.push_back(static_cast<u64>(std::stoi(arg)));
    } else {
      parsedArgs.push_back(this->parseRegister(arg.data()));
    }
  }

  return parsedArgs;
}

static const std::unordered_map<std::string_view, u8> mnemonicArgsSizeMap = {
  {"addi", 3},
  {"andi", 3},
  {"ori", 3},
  {"add", 3},
  {"sub", 3},
  {"and", 3},
  {"or", 3},
  {"nor", 3},
  {"sll", 3},
  {"srl", 3},
  {"slt", 3},
  {"jr", 1},
  {"beq" , 3},
  {"bne" , 3},
  {"blt" , 3},
  {"bge" , 3},
  {"j"  , 1},
  {"jal", 1},
  {"sw", 3},
  {"lw", 3},
};

auto Tokenizer::validateArgumentsSize(const std::string& mnemonic, const VecString& args) -> bool {
  auto it = mnemonicArgsSizeMap.find(mnemonic);
  if (it == mnemonicArgsSizeMap.end()) {
    throw std::invalid_argument(
      std::format("ERROR! mnemonic not found {}\n", mnemonic)
    );
  }
  return args.size() == it->second;
}

static const std::unordered_map<std::string_view, u64> fixedRegisters = {
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

  if (1 < strlen(arg) && strlen(arg) <= 2) {
    number = std::atoi(arg + 1);
  } else {
    throw std::invalid_argument(std::format("ERROR! No such a register {}\n", arg));
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
    throw std::invalid_argument(std::format("ERROR! No such a register {}\n", arg));
  }
}

auto Tokenizer::printTokens() -> void {
      for (const auto& token : tokens) {
    std::string type{""};
    switch (token.tokenType) {
      case Emulator::Type::LITERAL:
        type = "LITERAL";
      break;
      case Emulator::Type::INSTRUCTION:
        type = "INSTRUCTION";
      break;
      case Emulator::Type::SYS_CALL:
        type = "SYS_CALL";
      break;
      case Emulator::Type::LABEL:
        type = "LABEL";
      break;
    }
    std::cout << std::format("token\n address {}, type {}, value ``{}``, args\n", token.address, type, token.value);
    for (const auto& arg : token.args) {
      std::cout << std::format("\t arg ``{}``\n", arg);
    }
    std::cout << "----------------------------------------------------\n";
  }
}

} // namespace Emulator