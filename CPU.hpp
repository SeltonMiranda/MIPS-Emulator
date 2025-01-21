#ifndef __CPU__
#define __CPU__

#include "Config.hpp"

namespace Emulator {

class CPU {
public:
  struct Instruction { 
    u64 address;
    u32 opcode;
    u32 rd;
    u32 rs;
    u32 rt;
    u32 shamt;
    u32 funct;
    s16 imm;
  };

  std::array<u32, 32> registers;
  u8* mem;
  u32 max_size;
  u32 pc;
  bool halt;

  CPU();
  ~CPU() = default;

  // Reads next instruction and execute it
  auto nextInstruction() -> void;

  // Checks if program finished
  auto hasHalted() -> bool;

  // Extends immediate with signal
  constexpr inline auto immExt(s16 imm) -> s32;

  // Extends immediate without signal
  constexpr inline auto zeroExt(s16 imm) -> u32;

  // Self-explanatory
  auto loadProgram(const std::span<u8> program) -> void;

  auto parseJ(u32 instruction) -> Instruction;
 
  // Parses I-type instructions
  auto parseImm(u32 instruction) -> Instruction;

  // Parses R-type instructions
  auto parseR(u32 instruction) -> Instruction;

  auto executeJ(Instruction i) -> void;

  // Executes an I-type instruction
  auto executeImm(Instruction i) -> void;

  // Executes an R-type instruction
  auto executeR(Instruction i) -> void;

  // Executes an instruction
  auto execute(u32 instruction) -> void;

  // Reads the value from a register
  auto readRegister(u32 index) -> u32;

  // Writes value into a register
  auto writeRegister(u32 index, s32 value) -> void;

  // Reads 1 byte from memory
  auto readMemory(u64 address) -> u8;

  // Reads "size" bytes from memory
  auto readMemoryBlock(u64 address, u32 size) -> u8*;

  // Writes 1 byte to memory
  auto writeMemory(u64 address, u8 value) -> void;

  // Writes "len(value)" bytes to memory
  auto writeMemoryBlock(u64 address, const std::span<u8> value) -> void;

  auto dumpMemory(size_t size) -> void;
};

}

#endif // __CPU__