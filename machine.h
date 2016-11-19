// machine.h
// define the machine class
// which runs the program that has been stored in the memory

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <string>
#include <unistd.h>
#include <sys/time.h>

#include "Instruction.h"
#include "Register.h"
#include "Stack.h"
#include "field.h"
#include "loader.h"

// size of data
#define INSTR_SIZE 4
#define BYTE_SIZE 1
#define HALF_SIZE 2
#define WORD_SIZE 4
#define DOUBLE_WORD_SIZE 8
#define FLOAT_SIZE 4
#define DOUBLE_SIZE 8

// syscall
#define SYS_EXIT 93
#define SYS_READ 63
#define SYS_WRITE 64
#define SYS_GET_TIME_OF_DAY 169
#define SYS_BRK 214
#define SYS_FSTAT 80

#define STACK_POINTER 0x30000

using namespace std;

class Machine {
public:
	Machine(bool, bool);
	~Machine();

	void Run();
	void Fetch();
	void Execute();
	void WriteBack();
	void WriteMemory();

	void setFileName(string );

private:
	Instruction * instr;
	Register * reg;
	Stack * stack;
	Memory * mem;

	Loadelf * loader;

	// file
	string filename;

	// record whether we need to write into register or write into memory
	bool wb;
	bool wm;
	bool exeI;
	bool exeF;
	bool proFinished;

	// some extra functions
	// instruction count
	bool ic;
	int count;
	// debug
	bool db;

	// result of ALU
	long long unsigned resi;
	double resf;
	unsigned char rd;

	int memsize;
	int memadd;
	long long unsigned data;

	int SExtension(int);
	unsigned UExtension(int);
	int LLU2INT(long long unsigned);
	unsigned LLU2UNS(long long unsigned);
	long long unsigned INT2LLU(int);
	long long unsigned UNS2LLU(unsigned);

	void PrintMessage();
};