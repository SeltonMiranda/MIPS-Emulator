#include "CPU.hpp"

#include <bitset>
#include <limits>

namespace Emulator {

CPU::CPU() {
  this->mem_size = std::numeric_limits<uint32_t>::max();
  this->pc = 0;
  this->registers.fill(0);
}

auto CPU::loadProgram(const std::vector<std::uint8_t> &program) -> void {
  if (program.size() > this->mem_size) {
    std::cout << std::format("Program is too large to fit in memory\n");
    return;
  }

  this->mem.resize(program.size());
  this->writeMemoryBlock(0, program);
}

auto CPU::readMemory(uint32_t address) -> std::uint8_t {
  if (address > this->mem_size) {
    std::cout << std::format("Out of bounds for memory\n");
    return 0;
  }

  return this->mem[address];
}

auto CPU::readMemoryBlock(std::uint32_t address, std::uint32_t size)
    -> std::vector<std::uint8_t> {
  uint32_t end{address + size - 1};
  if (end > this->mem_size) {
    std::cout << std::format("Out of bounds for memory\n");
    return std::vector<std::uint8_t>(0);
  }

  std::vector<std::uint8_t> Block(this->mem.begin() + address,
                                  this->mem.begin() + end + 1);
  return Block;
}

auto CPU::writeMemory(std::uint32_t address, std::uint8_t value) -> void {
  if (address > this->mem_size) {
    std::cout << std::format("Out of bounds\n");
    return;
  }

  if (address > this->mem.size())
    this->mem.resize(address + 1);

  this->mem[address] = value;
}

auto CPU::writeMemoryBlock(std::uint32_t address,
                           const std::vector<std::uint8_t> &value) -> void {
  uint32_t end{address + static_cast<std::uint32_t>(value.size()) - 1};
  if (end > this->mem_size) {
    std::cout << std::format("Out of bounds\n");
    return;
  }

  if (end >= this->mem.size())
    this->mem.resize(end + 1);

  for (const auto &content : value) {
    this->mem[address] = content;
    address++;
  }
}

auto CPU::writeRegister(int index, std::uint32_t value) -> void {
  if (index == 0)
    return;

  this->registers[index] = value;
}

auto CPU::readRegister(int index) -> std::uint32_t {
  return this->registers[index];
}

auto CPU::nextInstruction() -> void {
  auto value{this->readMemoryBlock(this->pc, 4)};
  for (const auto &content : value) {
    std::cout << std::bitset<8>(content) << " ";
    std::cout << std::format("address {}\n", this->pc);
    this->pc++;
  }
}

} // namespace Emulator