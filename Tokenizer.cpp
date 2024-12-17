#include "Tokenizer.hpp"

namespace Emulator {

Tokenizer::Tokenizer(std::ifstream&& program) : program{std::move(program)}, current_line{1} {}

Tokenizer::Tokenizer(const std::string& program) : current_line{1} {
    this->program.open(program);
    if (!this->program.is_open()) {
        throw std::runtime_error{"Couldn't open file\n"};
    }
}

Tokenizer::~Tokenizer() {
    if (this->program.is_open()) {
    this->program.close();
    }
}

auto Tokenizer::parse() -> std::vector<std::string> {
    std::vector<std::string> symbols;
    std::string line;
    while (std::getline(this->program, line)) {
        std::vector<std::string> line_symbols;
        boost::trim(line);
        boost::split(
                    line_symbols,
                    line,
                    boost::is_any_of(", "),
                    boost::token_compress_on
                );
        symbols.insert(symbols.end(), line_symbols.begin(), line_symbols.end());
    }   
    return symbols;          
}


auto Tokenizer::printTokens(const std::vector<std::string>& symbols) -> void {
    for (const auto& symbol : symbols) {
        std::cout << std::format("\033[33mtoken\033[0m {}\n", symbol);
    }
}

}