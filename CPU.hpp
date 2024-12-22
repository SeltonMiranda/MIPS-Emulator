#ifndef __CPU__
#define __CPU__

#include <iostream>
#include <vector>
#include <array>

namespace Emulator {

class CPU {
    private:
        std::array<std::uint32_t, 32> registers;
        std::uint32_t mem_size;
        std::vector<uint8_t> mem;
        uint32_t pc;

    public:
        // todo;
        CPU();
        ~CPU() = default;

        // Self-explanatory
        auto loadProgram(const std::vector<uint8_t>& program) -> void;

        
        auto execute(uint32_t instruction) -> void;

        // Reads the value from a register
        auto readRegister(int index) -> uint32_t;

        // Writes value into a register
        auto writeRegister(int index, uint32_t value) -> void;

        // Reads 1 byte from memory
        auto readMemory(uint32_t address) -> std::uint8_t;

        // Reads "size" bytes from memory
        auto readMemoryBlock(std::uint32_t address, std::uint32_t size) -> std::vector<std::uint8_t>;

        // Writes 1 byte to memory
        auto writeMemory(std::uint32_t address, std::uint8_t value) -> void;

        // Writes "len(value)" bytes to memory
        auto writeMemoryBlock(std::uint32_t address, std::vector<std::uint8_t> value) -> void;
};

}


#endif // __CPU__