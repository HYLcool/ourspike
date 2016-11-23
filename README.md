# ourspike

- A simulator for RISC-V program wriiten by Huang Yilun, Ye Gen and Xing Xinyu

## QuickStart

- First, you should install the RISC-V toolchains. You can download and install it from https://github.com/riscv/riscv-tools
- Second, get our source code from Github: https://github.com/HYLcool/ourspike
- Third, you just need to `make` it, and run the risc-v program (which is actually a ELF file) compiled with riscv-toolchain.
```
$ make
$ ./ourspike a.out
```
- About the usage of this program:
```
./ourspike [-options] <filename>

options:
	-u	print this usage message.
	-d	debugging mode. In this mode, you can give a '?' to get the debug help message
	-ic	print the total instruction amount after finishing running a program
	-trace	print the executing trace of a program, cover the -d option
```
- Finally, you get what you want~

## About Program

- We have several modules, and the simulator consists of them. Let's have a look at them.

### main.cpp/h (by HYL)

This module parses the running options and filename, also initializes the necessary data structure Machine and do some cleaning work after running the program.

### machine.cpp/h (by HYL)

This module imitates the behaviors that a real machine will do with the executable program. What it does are:

- Define the size of data used in the data transforms between memory and registers, system call numbers
- Including the processes running an instruction, such as Fetch, Decode, Execute, Write Register, Write Memory.
- Initialize all the data structures needed when running a program, such as Memory, Register File, Stack(ignored because it's implemented as memory in risc-v), Instruction.

### loader.cpp/h (by YG)

This module actually includes two modules: Loadelf and Memory. Loadelf is a module to load necessary imformation from ELF files into memory; Memory imitates the real memory.

### Stack.cpp/h (by XXY)

This module imitates the real stack. However, in risc-v instruction set, there is no "push" and "pop", and these 2 instructions is implemented as "ld" and "sd" with memory.

### Register.cpp/h (by XXY)

This modeule imitates the real register file and define the register names (such as SP and A0). The register file includes Int register file and Float register file. Also, don't forget PC.

### Instruction.cpp/h (by XXY)

This module get a binary code of an instruction, and parse it into different parts, such as opcode, funct7, imm and so on. The parsing process is written in the Decode method.

### filed.h (by HYL)

This module doesn't do any actions. It just defines a parsing tree for binary codes of instructions for understanding what this instruction is quickly and clearly. Also, it helps a lot in making our source code more readable.
