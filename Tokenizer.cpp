#include "Tokenizer.hpp"
#include "debugHelper.hpp"

namespace Emulator {

static constexpr u64 WORD_SIZE = 4;

static const std::unordered_map<std::string_view, u8> 
mnemonicArgsSizeMap = {
  {"addi", 3},
  {"andi", 3},
  {"ori" , 3},
  {"add" , 3},
  {"sub" , 3},
  {"and" , 3},
  {"or"  , 3},
  {"nor" , 3},
  {"sll" , 3},
  {"srl" , 3},
  {"slt" , 3},
  {"jr"  , 1},
  {"beq" , 3},
  {"bne" , 3},
  {"j"   , 1},
  {"jal" , 1},
  {"sw"  , 3},
  {"lw"  , 3},
  {"slti", 3},
  {"lbu" , 3},
  {"sb"  , 3},
  {"mul" , 3},
  // Pseudo instructions
  {"blt" , 3},
  {"bge" , 3},
  {"move", 2},
  {"li"  , 2},
  {"la"  , 2},
};

static const std::unordered_map<std::string_view, u64> 
RegisterNames = {
    {"$zero",  0},
    {"$at" ,  1},
    {"$v0" ,  2},
    {"$v1" ,  3},
    {"$a0" ,  4},
    {"$a1" ,  5},
    {"$a2" ,  6},
    {"$a3" ,  7},
    {"$t0" ,  8},
    {"$t1" ,  9},
    {"$t2" , 10},
    {"$t3" , 11},
    {"$t4" , 12},
    {"$t5" , 13},
    {"$t6" , 14},
    {"$t7" , 15},
    {"$s0" , 16},
    {"$s1" , 17},
    {"$s2" , 18},
    {"$s3" , 19},
    {"$s4" , 20},
    {"$s5" , 21},
    {"$s6" , 22},
    {"$s7" , 23},
    {"$t8" , 24},
    {"$t9" , 25},
    {"$k0" , 26},
    {"$k1" , 27},
    {"$gp" , 28},
    {"$sp" , 29},
    {"$fp" , 30},
    {"$ra" , 31},
};

auto Tokenizer::isSysCall(const std::string& call) -> bool {
  return call == "syscall";
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

  // boost::split creates an empty token when string ends with a delimiter, in that case, it should be removed 
  std::erase_if(args, [](const std::string& str) { return str.empty(); });

  if (!validateArgumentsSize(symbols[0], args)) {
    throw std::invalid_argument(
      std::format("ERROR! Not enough arguments in instruction {} size {}\n", instructionToken.value, args.size())
    );
  }

  this->tokens.push_back(instructionToken);
  _args[address] = args;
}

auto Tokenizer::parseDataSection(std::string& line, u64& address) -> void {
  VecString symbols;
  boost::split(symbols, line, boost::is_any_of(": "), boost::token_compress_on);

  if (symbols.size() < 3) {
    throw std::runtime_error(
      std::format("Invalid literal declaration: {}", line)
    );
  }

  if (symbols.at(1) == ".word") {

    Token literalToken = {
      .tokenType = Type::LITERAL,
      .address = address,
      .value = symbols.at(0),
      .directive = Directive::WORD,
    };

    u8 numberOfLiterals = 0;
    for (size_t i = 2; i < symbols.size(); i++) {
      literalToken.args.push_back(std::stoi(symbols[i]));
      numberOfLiterals++;
    }
    this->tokens.push_back(literalToken);
    this->labelsToAddress[literalToken.value] = address;

    address += WORD_SIZE * numberOfLiterals;

  } else if (symbols.at(1) == ".space") {

    Token literalToken = {
      .tokenType = Type::LITERAL,
      .address = address,
      .value = symbols.at(0),
      .directive = Directive::SPACE,
    };

    literalToken.args.push_back(std::stoi(symbols[2]));
    this->tokens.push_back(literalToken);
    this->labelsToAddress[literalToken.value] = address;

    address += literalToken.args.front();

  } else if (symbols.at(1) == ".asciiz") {

    std::string strValue;
    for (u32 i = 2; i < symbols.size(); i++) {
      strValue.append(symbols.at(i));
      strValue.append(" ");
    }
    std::erase_if(strValue, [](char c) { return c == '"'; });

    Token literalToken = {
      .tokenType = Type::LITERAL,
      .address = address,
      .value =  symbols.at(0),
      .directive = Directive::ASCIIZ,
    };

    for (char c: strValue) {
      literalToken.args.push_back(static_cast<u8>(c));
    }
    literalToken.args.push_back(0); // Null terminate string ('\0')

    this->tokens.push_back(literalToken);
    this->labelsToAddress[literalToken.value] = address;

    address += literalToken.args.size();

  } else {
    throw std::runtime_error{
                              std::format("ERROR! Directive {} doesn't exist\n", symbols.at(1))
                            };
  }
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
  while (std::getline(_file, line)) {
    VecString symbols;
    boost::trim(line);
    if (line.empty() || line.starts_with('#')) {
      continue;
    } else {
      this->removeInlineComments(line);
    }

    if (line == ".data") {
      section = ".data";
      continue;
    } else if (line == ".text") {
      section = ".text";
      this->textStartAddress = address;
      continue;
    }

    if (section == ".data") {

      this->parseDataSection(line, address);
      
    } else if (section == ".text") {

      boost::split(symbols, line, boost::is_any_of(", ()"), boost::token_compress_on);
      std::string tag = symbols[0];

      if (this->isLabel(tag)) {
        this->parseLabel(tag, address);
      } else if (this->isSysCall(tag)) {
        this->parseSysCall(tag, address);
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

auto Tokenizer::removeInlineComments(std::string& line) -> void {
  size_t commentPos = line.find('#');
  if (commentPos != std::string::npos) {
    line = line.substr(0, commentPos);
  }
  line.erase(line.find_last_not_of(" \t") + 1);
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

auto Tokenizer::validateArgumentsSize(const std::string& mnemonic, const VecString& args) -> bool {
  auto it = mnemonicArgsSizeMap.find(mnemonic);
  if (it == mnemonicArgsSizeMap.end()) {
    throw std::invalid_argument(
      std::format("ERROR! mnemonic not found {}\n", mnemonic)
    );
  }
  return args.size() == it->second;
}

auto Tokenizer::parseRegister(const char *arg) -> u64 {
  if (RegisterNames.contains(std::string(arg))) {
    return RegisterNames.at(arg);
  }

  if (arg[0] != '$') {
    throw std::runtime_error{std::format("ERROR! Register doesn't exist {} \n", arg)};
  }

  if (!std::isdigit(arg[1])) {
    throw std::runtime_error{std::format("ERROR! Register doesn't exist {} \n", arg)};
  }

  return std::atoi(arg + 1);
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