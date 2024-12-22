#include "CPU.hpp"

#include <limits>

namespace Emulator {

CPU::CPU() {
    this->mem_size = std::numeric_limits<uint32_t>::max(); 
    this->pc = 0;
    this->registers.fill(0);
}

auto CPU::loadProgram(const std::vector<std::uint8_t>& program) -> void {
    this->mem = program;
}

auto CPU::readMemory(uint32_t address) -> std::uint8_t {
    if (address > this->mem_size) {
        std::cout << std::format("Out of bounds for memory\n");
        return 0;
    }

    return this->mem[address];
} 

auto CPU::readMemoryBlock(std::uint32_t address, std::uint32_t size) -> std::vector<std::uint8_t> {
    uint32_t end{address + size - 1};
    if (end > this->mem_size) {
        std::cout << std::format("Out of bounds for memory\n");
        return std::vector<std::uint8_t>(0);
    }

    std::vector<std::uint8_t> Block(this->mem.begin() + address, this->mem.begin() + end + 1);
    return Block;
}

auto CPU::writeMemory(std::uint32_t address, std::uint8_t value) -> void {
    if (address > this->mem_size) {
        std::cout << std::format("Out of bounds\n");
        return;
    }

    this->mem[address] = value;
}

auto CPU::writeMemoryBlock(std::uint32_t address, std::vector<std::uint8_t> value) -> void {
    uint32_t end{address + static_cast<std::uint32_t>(value.size()) - 1};
    if (end > this->mem_size) {
        std::cout << std::format("Out of bounds\n");
        return;
    }

    for (const auto& content : value) {
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

}