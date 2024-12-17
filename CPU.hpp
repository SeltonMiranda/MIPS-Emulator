#ifndef __CPU__
#define __CPU__

#include <iostream>
#include <memory>

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
        std::unique_ptr<uint32_t[]> mem;
        uint32_t pc;

    public:
        // todo;
        CPU(size_t mem_size = 1024);
        auto loadProgram() -> void;
        auto execute() -> void;
        auto run() -> void;
        auto readRegister(int index) -> uint32_t;
        auto writeRegister(int index, uint32_t value) -> void;
        auto readMemory(uint32_t address) -> uint32_t;
        auto writeMemomry(uint32_t address, uint32_t value) -> void;
};

}


#endif // __CPU__