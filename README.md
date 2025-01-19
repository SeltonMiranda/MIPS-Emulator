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
./emulator <yourprogramnam.asm>
```

## Goal
The main objective is to implement all instructions and system calls as defined in the [MIPS Instruction Set](https://www.dsi.unive.it/~gasparetto/materials/MIPS_Instruction_Set.pdf).

## Features
- Emulates basic MIPS instructions.

## Contributing
Contributions are welcome! Feel free to open issues or submit pull requests.