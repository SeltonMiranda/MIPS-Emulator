#ifndef __CPU__
#define __CPU__

#include <iostream>
#include <vector>

namespace Emulator {

struct Instruction {
    std::string label;
    std::string opcode;
    uint32_t rd;
    uint32_t rs;
    uint32_t rt;
    uint32_t immediate;
    uint32_t address;
};

class CPU {
    private:
        uint32_t registers[32];
        uint32_t mem_size;
        std::vector<uint8_t> mem;
        uint32_t pc;

    public:
        // todo;
        CPU();
        ~CPU() = default;
        auto loadProgram(const std::vector<uint8_t>& program) -> void;
        auto execute(uint32_t instruction) -> void;
        auto run() -> void;
        auto readRegister(int index) -> uint32_t;
        auto writeRegister(int index, uint32_t value) -> void;
        auto readMemory(uint32_t address) -> uint32_t;
        auto readMemoryBlock(uint32_t address) -> uint32_t;
        auto writeMemory(uint32_t address, uint32_t value) -> void;
        auto writeMemoryBlock(uint32_t address, uint32_t value) -> void;
};

}


#endif // __CPU__