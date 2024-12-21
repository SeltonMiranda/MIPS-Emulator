#include "CPU.hpp"

#include <limits>

namespace Emulator {

CPU::CPU() {
    this->mem_size = std::numeric_limits<uint32_t>::max(); 
}



}