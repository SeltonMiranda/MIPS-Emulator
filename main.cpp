
#include "Emulator.hpp"

auto main() -> int {
  std::string file = "example.txt";
  auto tokenizer{new Emulator::Tokenizer{file}};
  auto cpu{new Emulator::CPU()};
  auto emulator{new Emulator::Emulator(tokenizer, cpu)};

  std::vector<std::uint8_t> code{emulator->assembler()};
  emulator->printBinaryProgram(code);
  // emulator->run(code);

  return 0;
}