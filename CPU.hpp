#ifndef __CPU__
#define __CPU__

#include <array>
#include <iostream>
#include <vector>

namespace Emulator {

class CPU {
private:
  std::array<std::uint32_t, 32> registers;
  std::uint32_t mem_size;
  std::vector<uint8_t> mem;
  uint32_t pc;
  bool ebreak;

  struct Instruction { // intermediate
    std::uint32_t rd;
    std::uint32_t rs;
    std::uint32_t rt;
    std::uint32_t imm;
    std::uint32_t shamt;
    std::uint32_t funct;
    std::uint32_t address;
  };

public:
  // todo;
  CPU();
  ~CPU() = default;

  auto nextInstruction() -> void;

  auto hasHalted() -> bool;

  // Self-explanatory
  auto loadProgram(const std::vector<uint8_t> &program) -> void;

  auto parseR(std::uint32_t instruction) -> Instruction;

  auto executeR(Instruction i) -> void;

  // Executes an instruction
  auto execute(uint32_t instruction) -> void;

  // Reads the value from a register
  auto readRegister(std::uint32_t index) -> uint32_t;

  // Writes value into a register
  auto writeRegister(std::uint32_t index, uint32_t value) -> void;

  // Reads 1 byte from memory
  auto readMemory(uint32_t address) -> std::uint8_t;

  // Reads "size" bytes from memory
  auto readMemoryBlock(std::uint32_t address, std::uint32_t size)
      -> std::vector<std::uint8_t>;

  // Writes 1 byte to memory
  auto writeMemory(std::uint32_t address, std::uint8_t value) -> void;

  // Writes "len(value)" bytes to memory
  auto writeMemoryBlock(std::uint32_t address,
                        const std::vector<std::uint8_t> &value) -> void;
};

} // namespace Emulator

#endif // __CPU__