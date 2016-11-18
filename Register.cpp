#include<iostream>
#include "Register.h"
using namespace std;


Register::Register(){
	for(int i=0;i<=31;i++){
		IntRegister[i]=0;
		DoubleRegister[i]=0.0;
	}
}
long long unsigned Register::getIntReg(int num){
	if (num>=32||num<0){
		return -1;
	}
	return IntRegister[num];

}
bool Register::setIntReg(int num,long long unsigned value){
	if (num>=32||num<0){
		return 0;
	}
	IntRegister[num]=value;
	IntRegister[0]=0;
	return 1;
}

double Register::getDoubleReg(int num){
	if (num>=32||num<0){
		return -1;
	}
	return DoubleRegister[num];

}
bool Register::setDoubleReg(int num,double value){
	if (num>=32||num<0){
		return 0;
	}
	DoubleRegister[num]=value;
	return 1;
}

unsigned Register::getPC() {
	return PC;
}

bool Register::setPC(unsigned pc) {
	PC = pc;
	return true;
}

double Register::getFcsr() {
	return fcsr;
}

bool Register::setFcsr(double v) {
	fcsr = v;
	return true;
}



/*int main(){
	Register a=Register();
	a.setIntReg(1,233);
	cout<<"hehe"<<endl;
	cout<<a.getIntReg(1)<<endl;
	cout<<sizeof(a.getIntReg(1))<<endl;

	a.setDoubleReg(1,11.11);
	cout<<a.getDoubleReg(1)<<endl;
	return 0;
}*/

