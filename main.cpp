#include "Engine.hpp"

auto main(int argc, char *argv[]) -> int {
  if (argc != 2) {
    std::cout << std::format("Not enough arguments\n");
    std::cout << std::format("Usage : {} <file.asm>\n", argv[0]);
    return 0;
  }

  std::string program = std::string(argv[1]);
  if (!program.contains(".asm")) {
    std::cout << std::format("You must use only .asm files\n");
    return 0;
  }

  Emulator::Tokenizer tokenizer;
  Emulator::CPU cpu;
  Emulator::Engine engine(tokenizer, cpu);

  try {
    auto [code, size] = engine.assembler(program);
    engine.run(std::span<u8>(code, size));
    delete[] code;
  } catch (const std::exception& e) {
    std::cout << e.what();
  }

  return 0;
}