
#include "Engine.hpp"

auto main() -> int {
    std::string file = "example.txt";
  try {
      auto tokenizer{new Emulator::Tokenizer{file}};
    auto cpu{new Emulator::CPU()};
    auto emulator{new Emulator::Engine(tokenizer, cpu)};
    VecU8 code{emulator->assembler()};
    emulator->run(code);
    delete emulator;
  } catch (std::exception &e) {
    std::cout << std::format("invalid\n");
  }
  return 0;
}