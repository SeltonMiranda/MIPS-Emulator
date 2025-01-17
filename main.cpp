
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
    std::cout << std::format("size of code {}\n", size);
    for (size_t i = 0; i < size; i++) {
      std::cout << std::format("code[{}]: {}\n", i, code[i]);
    }
    //if (code != nullptr) {
    //  for (size_t i = 0; i < size; i++) {
    //    std::cout << std::format("address {} : {}\n", i, code[i]);
    //  }
    //  //engine.run(std::span<u8>(code, size));
    //} else {
    //  throw std::runtime_error("Couldn't assemble program\n");
    //}
  } catch (const std::exception& e) {
    std::cout << e.what();
  }

  return 0;
}