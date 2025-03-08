# MIPS Emulator

This project is a work-in-progress emulator for the MIPS instruction set. It doesn't mean to have any purpose, I'm only doing this for fun and trying new ways of programming

## Status
**Still in development**

## Requirements
- linux
- g++ compiler
- Boost c++ library (check if your PATH `/usr/include/boost` is right for your system)
- Make

## How to install
1. Git clone the repo
```bash
git clone https://github.com/seu_usuario/MIPS-Emulator.git
cd MIPS-Emulator
```

2. Install dependencies 
for debian based:
```bash
sudo apt install libboost-all-dev
```
for red-hat based:
```bash
sudo dnf install boost-devel
```

3. Compile the project
```bash
make
```
## How to use
Create a file with `.asm` extesion with your program, then run the following command
```bash
./emulator <file.asm>
```

## Goal
The main objective is to implement all instructions and system calls as defined in the [MIPS Instruction Set](https://www.dsi.unive.it/~gasparetto/materials/MIPS_Instruction_Set.pdf).

## Features
- Emulates basic MIPS instructions.

# MIPS Instruction Set

This document describes the main MIPS instructions currently supported. The instructions are divided into three main types: **R-types**, **I-types**, and **J-types**, along with some **Pseudo** instructions.

## Instruction Types

### 1. **R-type Instructions**

R-type instructions operate on registers and do not use an immediate (constant value) as an argument. They are mainly used for arithmetic, logical operations, and control flow.

| Instruction | Description                             |
|-------------|-----------------------------------------|
| `add`       | Adds two registers.                     |
| `sub`       | Subtracts the second register from the first. |
| `or`        | Performs a logical OR between two registers. |
| `and`       | Performs a logical AND between two registers. |
| `nor`       | Performs a logical NOR between two registers. |
| `sll`       | Performs a logical shift left.          |
| `srl`       | Performs a logical shift right.         |
| `slt`       | Sets the destination to 1 if the first register is less than the second. |
| `jr`        | Jumps to the address contained in a register. |

### 2. **I-type Instructions**

I-type instructions use an immediate value (constant) as an argument, along with registers.

| Instruction | Description                             |
|-------------|-----------------------------------------|
| `addi`      | Adds an immediate value to a register.  |
| `slti`      | Sets the destination to 1 if the value of the register is less than the immediate. |
| `andi`      | Performs a logical AND with an immediate value. |
| `ori`       | Performs a logical OR with an immediate value. |
| `beq`       | Branches if the two registers are equal. |
| `bne`       | Branches if the two registers are not equal. |
| `lw`        | Loads a value from memory into a register. |
| `sw`        | Stores the value of a register into memory. |

### 3. **J-type Instructions**

J-type instructions are used to perform direct jumps in the code.

| Instruction | Description                             |
|-------------|-----------------------------------------|
| `j`         | Performs an unconditional jump to the specified address. |
| `jal`       | Performs an unconditional jump to the specified address, saving the return address in the `$ra` register. |

### 4. **Pseudo-instructions**

Pseudo-instructions are not part of the original MIPS specification but are commonly used for convenience, as they are translated into real instructions.

| Instruction | Description                             |
|-------------|-----------------------------------------|
| `bge`       | Branches if the first register is greater than or equal to the second. |
| `blt`       | Branches if the first register is less than the second. |
| `move`      | Moves the value from one register to another. (Equivalent to `add $d, $s, $0`) |
| `li`        | Loads an immediate value into a register. |
| `la`        | Loads the address of a label into a register. |

## Notes

- **R-type** instructions are the most common in MIPS and deal with direct operations between registers.
- **I-type** instructions are used to handle operations involving constant values and interactions with memory.
- **J-type** instructions are used to perform jumps in programs.
- **Pseudo-instructions** provide a more convenient way to write code.

# Supported SysCalls

This document lists the supported MIPS SysCalls and provides examples of how to use them.
To further details, read [MIPS Instruction Set](https://www.dsi.unive.it/~gasparetto/materials/MIPS_Instruction_Set.pdf) to
know what code is which syscall and their arguments.

## Available SysCalls

| SysCall      | Code  | Description                          |
|--------------|-------|--------------------------------------|
| `print_int`  | 1     | Prints an integer to the console.    |
| `print_string`| 4    | Prints a null-terminated string.     |
| `read_int`   | 5     | Reads an integer from the input.     |
| `read_string`| 8     | Reads a string from the input.       |
| `exit`       | 10    | Terminates the program.              |
| `print_char` | 11    | Prints a single character.           |
| `read_char`  | 12    | Reads a single character from input. |
| `exit2`      | 17    | Another way to exit (alternative exit). |

## Example Usage

### Print Integer

To print an integer (`42` in this example), you use the `print_int` syscall:

```assembly
addi $v0, $zero, 1   # Set $v0 to 1, which is the syscall code for print_int
addi $a0, $zero, 42  # Set $a0 to 42, which is the integer to print
syscall              # Make the syscall
```

## Sections
The emulator is divided into **two main sections**: `.data` and `.text`

### `.data` Section 
is used to store literal data, such as constants and strings

### Available Directives:

- **`.space`** -> Reserves a specific number of bytes in memory
```assembly
emptySpace: .space 8 # Reserves 8 bytes
```

- **`.word`** -> Reserves space for 32-bit integer values (4 bytes per value)
```assembly
array: .word 7, 42, 20, 420 # Creates an array with these values
const: .word 42             # Defines a 32-bit value
```

- **`.asciiz`** -> Reseves space for a null-terminated string (the string **must** be between `"` )
```assembly
string: .asciiz "To be or not to be, that is the question"
```

### `.text` Section 
Contains the program code and is where instructions are executed
```assembly
.text
main:
  addi $a0, zero, 10
  addi $a1, zero, 20
  addi $sp, $sp, -4
  sw $sp, a1, 0
  ...
```

### Comments
Comments starts with "#" and can be written on a separate line or inline.
```assembly
  # the answer to life the universe and everything
  addi $a0, zero, 42 # inline comment
```

## Contributing
Contributions are welcome! Feel free to open issues or submit pull requests.