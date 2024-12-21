#include "Tokenizer.hpp"

namespace Emulator {

Tokenizer::Tokenizer(std::ifstream&& file) : file{std::move(file)}, current_line{1} {}

Tokenizer::Tokenizer(const std::string& file) : current_line{1} {
    this->file.open(file);
    if (!this->file.is_open()) {
        throw std::runtime_error{"Couldn't open file\n"};
    }
}

Tokenizer::~Tokenizer() {
    if (this->file.is_open()) {
    this->file.close();
    }
}

Token::Token(Type type, size_t line, const std::string& value)
: type{type}, line{line}, value{value} {}

ResolvedToken::ResolvedToken(Type type, size_t line, const std::string& value, std::uint64_t address)
: type{type}, line{line}, value{value}, address{address} {}

auto Tokenizer::parseLine(std::vector<Token*>& tokens, std::string& line, size_t lineNumber) -> void {
    std::vector<std::string> symbols;
    boost::trim(line);
    boost::split(
                symbols,
                line,
                boost::is_any_of(", "),
                boost::token_compress_on
            );
    
    // Creates token for instruction mnemonic
    tokens.emplace_back(new Token(
        Type::INSTRUCTION,
        lineNumber,
        symbols[0]
    ));


    // Creates tokens to intructions arguments
    for (size_t i = 1; i < symbols.size(); i++) {
        tokens.emplace_back(new Token(
            Type::ARG,
            lineNumber,
            symbols[i]
        ));
    }
}

auto Tokenizer::parse() -> std::vector<Token*> {
    std::string line;
    std::vector<Token*> tokens;
    size_t i{0};
    while (std::getline(this->file, line)) { // The program for a while only has R-type instructions
        this->parseLine(tokens, line, i);
        i++;
    }   

    return tokens;
}

}