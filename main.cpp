
#include "Tokenizer.hpp"

auto main() -> int {
    std::ifstream file{"example.txt"};
    Emulator::Tokenizer tok{std::move(file)};
    std::vector<Emulator::Token*> tokens{tok.parse()};
    return 0;
}