#include <iostream>
#include "Instruction.h"
using namespace std;

Instruction::Instruction(){
	instruct=0;
}

unsigned Instruction::fromto(int high,int low){
	return (instruct<<(31-high))>>(31-high+low);
} 

void Instruction::Decode(){
	opcode=fromto(6,0);
	rd=fromto(11,7);
	rs1=fromto(19,15);
	rs2=fromto(24,20);
	funct3=fromto(14,12);
	funct7=fromto(31,25);
	funct2=fromto(26,25);
	rs3=fromto(31,27);
	shamt=rs2;
	imm=fromto(31,20);
	//cout<<instruct<<endl;
	//cout<<opcode<<rd<<endl;
}

unsigned char Instruction::getOpcode(){
	return opcode;
}

unsigned char Instruction::getRd(){
	return rd;
}

unsigned char Instruction::getRs1(){
	return rs1;
}

unsigned char Instruction::getRs2(){
	return rs2;
}

unsigned char Instruction::getRs3(){
	return rs3;
}

unsigned char Instruction::getFunct2(){
	return funct2;
}

unsigned char Instruction::getFunct3(){
	return funct3;
}

unsigned char Instruction::getFunct7(){
	return funct7;
}

unsigned char Instruction::getShamt() {
	return shamt;
}

unsigned Instruction::getImm(){
	return imm;
}

unsigned Instruction::getInstr() {
	return instruct;
}

void Instruction::setInstruct(unsigned inst) {
	// printf("%x\n", inst);
	// char *p = (char*)&inst;
	// char tmp;
	// tmp = *(p+3);
	// *(p+3) = *p;
	// *p = tmp;

	// tmp = *(p+1);
	// *(p+1) = *(p+2);
	// *(p+2) = tmp;

	instruct = inst;
	// printf("%x\n", inst);
}


/*int main(){
	unsigned test=0xbd8fc158;
	Instruction a=Instruction(test);
	a.Decode();
	cout<<"opcode "<<int(a.getOpcode())<<endl;
	cout<<"rs1 "<<int(a.getRs1())<<endl;
	cout<<"rs2 "<<int(a.getRs2())<<endl;
	cout<<"rd "<<int(a.getRd())<<endl;
	cout<<"funct3 "<<int(a.getFunct3())<<endl;
	cout<<"funct7 "<<int(a.getFunct7())<<endl;

	return 0;
}*/