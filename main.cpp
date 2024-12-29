
#include "Engine.hpp"

auto main(int argc, char *argv[]) -> int {
  if (argc != 2) {
    std::cout << std::format("Not enough arguments\n");
    std::cout << std::format("Usage : {} <file.asm>\n", argv[0]);
    return 0;
  }

  std::string text = std::string(argv[1]);
  if (!text.contains(".asm")) {
    std::cout << std::format("You must use only .asm files\n");
    return 0;
  }

  try {
    auto tokenizer{new Emulator::Tokenizer{text}};
    auto cpu{new Emulator::CPU()};
    auto emulator{new Emulator::Engine(tokenizer, cpu)};
    VecU8 code{emulator->assembler()};
    emulator->run(code);
    delete emulator;
  } catch (std::exception &e) {
    std::cout << e.what();
  }
  return 0;
}