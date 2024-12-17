
#include "Tokenizer.hpp"

auto main() -> int {
    std::ifstream file{"example.txt"};
    Emulator::Tokenizer tok{std::move(file)};
    std::vector<std::string> tokens{tok.parse()};
    tok.printTokens(tokens);
    return 0;
}