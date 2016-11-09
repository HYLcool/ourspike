#include <cstdio>

class Instruction{
	unsigned instruct;       //31 to 0

	unsigned char opcode;
	unsigned char rd;
	unsigned char rs1;
	unsigned char rs2;
	unsigned char rs3;
	unsigned char funct3;
	unsigned char funct7;
	unsigned char funct2;
	unsigned char shamt;

	unsigned imm;          //todo

public:
	Instruction();
	unsigned fromto(int high,int low); 
	void Decode();
	unsigned char getOpcode();
	unsigned char getRd();
	unsigned char getRs1();
	unsigned char getRs2();
	unsigned char getRs3();
	unsigned char getFunct2();
	unsigned char getFunct3();
	unsigned char getFunct7();
	unsigned char getShamt();
	unsigned getImm();
	unsigned getInstr();

	void setInstruct(unsigned inst);
};