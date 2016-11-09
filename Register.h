#define ZERO 0
#define RA 1
#define S0 8
#define FP 8
#define S1 9
#define S2 18
#define S3 19
#define S4 20
#define S5 21
#define S6 22
#define S7 23
#define S8 24
#define S9 25
#define S10 26
#define S11 27
#define SP 2
#define TP 4
#define A0 10
#define A1 11
#define A2 12
#define A3 13
#define A4 14
#define A5 15
#define A6 16
#define A7 17
#define T0 5
#define T1 6
#define T2 7
#define T3 28
#define T4 29
#define T5 30
#define T6 31
#define GP 3

class Register{
	long long unsigned IntRegister[32];
	float DoubleRegister[32];
	unsigned PC;
	float fcsr;
public:
	Register();
	long long unsigned getIntReg(int num);
	bool setIntReg(int num, long long unsigned value);

	float getFloatReg(int num);
	bool setFloatReg(int num,float value);

	unsigned getPC();
	bool setPC(unsigned pc);

	float getFcsr();
	bool setFcsr(float);

};