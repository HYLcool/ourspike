// machine.cpp
// implement the methods in class Machine

#include "machine.h"

using namespace std;

Machine::Machine(bool instructionCount, bool debug, bool tr) {
	instr = new Instruction();
	reg = new Register();
	stack = new Stack();
	mem = new Memory();

	loader = new Loadelf(mem);

	wb = wm = exeI = exeF = proFinished = false;
	reg -> setIntReg(SP, VIRMEM_OFFSET + STACK_POINTER);
	reg -> setIntReg(FP, VIRMEM_OFFSET + STACK_POINTER);

	resi = 0;
	resf = 0.0;
	rd = 0;

	ic = instructionCount;
	db = debug;
	trace = tr;
	if (trace)
		db = false;

	stopPC = 0;
	inJump = false;

	if (ic) {
		count = 0;
		ldC = 0;
		stC = 0;
		opC = 0;
		brC = 0;
		sysC = 0;
	}
}

Machine::~Machine() {
	delete loader;
	delete instr;
	delete reg;
	delete stack;
	delete mem;
}

void Machine::setFileName(string fn) {
	filename = fn;
}

void Machine::Run() {
	// parser the elf file
	loader -> Load_file(filename);

	// set the original pc
	reg -> setPC(VIRMEM_OFFSET);

	while (1) {
		// instruction count
		if (ic)
			count++;

		// instruction fetch
		Fetch();

		// instruction decode
		instr -> Decode();
		// printf("0x%08x\n", instr -> getInstr());

		// execute
		Execute();
		if (proFinished) // program finished?
			break;

		// write back to register or memory
		if (wb)
			WriteBack();
		if (wm)
			WriteMemory();
	}

	PrintMessage();
}

void Machine::PrintMessage() {
	printf("\n********************\n");

	if (ic) {
		printf("Total Instruction Count : %12d %3.2f%%\n", count, 100.0);
		printf("Load ins. Count:          %12d %3.2f%%\n", ldC, (float)ldC / count * 100.0);
		printf("Store ins. Count:         %12d %3.2f%%\n", stC, (float)stC / count * 100.0);
		printf("Arithmetical ins. Count:  %12d %3.2f%%\n", opC, (float)opC / count * 100.0);
		printf("Branch ins. Count:        %12d %3.2f%%\n", brC, (float)brC / count * 100.0);
		printf("System call Count:        %12d %3.2f%%\n", sysC, (float)sysC / count * 100.0);
	}

	printf("********************\n");
}

int Machine::SExtension(int ori) {
	return ((ori << 20) >> 20);
}

unsigned Machine::UExtension(int ori) {
	return unsigned(ori);
}

int Machine::LLU2INT(long long unsigned ori) {
	return int(unsigned(ori));
}

unsigned Machine::LLU2UNS(long long unsigned ori) {
	return unsigned(ori);
}

long long unsigned Machine::INT2LLU(int ori) {
	return (long long unsigned)((long long)ori);
}

long long unsigned Machine::UNS2LLU(unsigned ori) {
	return (long long unsigned)ori;
}

void Machine::Fetch() {
	unsigned instrBuff;
	unsigned pc = reg -> getPC();
	mem -> readMem(pc, (char*)&instrBuff, INSTR_SIZE);
	if (trace)
		printf("PC: 0x%08x Instr: 0x%08x\n", pc, instrBuff);

	if (db && (!inJump || stopPC == pc)) {
		if (inJump)
			inJump = false;
		bool isCir = true;
		
		char *buf = new char[80];
		int num;
		printf("\nPC: 0x%08x Instr: 0x%08x\n", pc, instrBuff);
		while (isCir) {
			printf("> ");
			// fflush(stdout);
			scanf("%s", buf);
			switch (*buf) {
				case 's': // single-step
					isCir = false;
					break;
				case 'l': // run until the end
					db = false;
					isCir = false;
					break;
				case 'j': // jump to an instruction
					scanf("%x", &num);
					inJump = true;
					stopPC = num;
					isCir = false;
					break;
				case 'i': // integer registers
					scanf("%d", &num);
					printf("Integer register %d is 0x%016llx\n", num, reg -> getIntReg(num));
					break;
				case 'f': {// floating registers
					scanf("%d", &num);
					double x = reg -> getDoubleReg(num);
					printf("Floating register %d is 0x%016llx\n", num, *(long long unsigned*)&x);
					break;
				}
				case 'm': { // memory
					scanf("%x", &num);
					int add = num / 4;
					add *= 4; // align
					char * content = new char[5];
					mem -> readMem(add, content, 4);
					content[4] = '\0';
					for (int i = 0; i < 4; i++) {
						printf("0x%08x: %u ", add + i, unsigned((unsigned char)content[i]));
					}
					printf("\n");
					delete content;
					break;
				}
				case '?':
				default:
					printf("Debug commands:\n");
					printf("\ts\texecute one instruction\n");
					printf("\tl\texecute until the program ends\n");
					printf("\tj <address>\texecute until reaching the instruction <address>\n");
					printf("\ti <number>\tprint the content of integer register <number>\n");
					printf("\tf <number>\tprint the content of float register <number>\n");
					printf("\tm <address>\tprint the content of memory at <address>\n");
					printf("\t?\tprint this help message\n");
					break;
			}
		}
		delete buf;


		// printf("PC: 0x%08x Instr: 0x%08x\n", pc, instrBuff);
		// printf("ZERO: 0x%08x RA: 0x%08x SP: 0x%08x GP: 0x%08x ", unsigned(reg -> getIntReg(ZERO)), unsigned(reg -> getIntReg(RA)), unsigned(reg -> getIntReg(SP)), unsigned(reg -> getIntReg(GP)));
		// printf("TP: 0x%08x T0: 0x%08x T1: 0x%08x T2: 0x%08x\n", unsigned(reg -> getIntReg(TP)), unsigned(reg -> getIntReg(T0)), unsigned(reg -> getIntReg(T1)), unsigned(reg -> getIntReg(T2)));
		// printf("S0/FP: 0x%08x S1: 0x%08x A0: 0x%08x A1: 0x%08x ", unsigned(reg -> getIntReg(S0)), unsigned(reg -> getIntReg(S1)), unsigned(reg -> getIntReg(A0)), unsigned(reg -> getIntReg(A1)));
		// printf("A2: 0x%08x A3: 0x%08x A4: 0x%08x A5: 0x%08x\n", unsigned(reg -> getIntReg(A2)), unsigned(reg -> getIntReg(A3)), unsigned(reg -> getIntReg(A4)), unsigned(reg -> getIntReg(A5)));
		// printf("A6: 0x%08x A7: 0x%08x S2: 0x%08x S3: 0x%08x\n", unsigned(reg -> getIntReg(A6)), unsigned(reg -> getIntReg(A7)), unsigned(reg -> getIntReg(S2)), unsigned(reg -> getIntReg(S3)));
		// printf("S4: 0x%08x S5: 0x%08x S6: 0x%08x S7: 0x%08x\n", unsigned(reg -> getIntReg(S4)), unsigned(reg -> getIntReg(S5)), unsigned(reg -> getIntReg(S6)), unsigned(reg -> getIntReg(S7)));
		// printf("S8: 0x%08x S9: 0x%08x S10: 0x%08x S11: 0x%08x ", unsigned(reg -> getIntReg(S8)), unsigned(reg -> getIntReg(S9)), unsigned(reg -> getIntReg(S10)), unsigned(reg -> getIntReg(S11)));
		// printf("T3: 0x%08x T4: 0x%08x T5: 0x%08x T6: 0x%08x\n", unsigned(reg -> getIntReg(T3)), unsigned(reg -> getIntReg(T4)), unsigned(reg -> getIntReg(T5)), unsigned(reg -> getIntReg(T6)));
		// // double a = reg -> getDoubleReg(15);
		// // printf("FA5: %f 0x%lld\n", a, *(long long *)&a);
		// printf("\n");
	}

	instr -> setInstruct(instrBuff);
	reg -> setPC(pc + 4U);
}

void Machine::WriteBack() {
	if (exeI) {
		reg -> setIntReg(rd, resi);
	} else if (exeF) {
		reg -> setDoubleReg(rd, resf);
		// printf("%d\n", rd);
	}
}

void Machine::WriteMemory() {
	mem -> writeMem(memadd, (char*)&data, memsize);
}

void Machine::Execute() {
	wb = wm = exeI = exeF = false;
	unsigned char opcode = instr -> getOpcode();
	unsigned char funct3 = instr -> getFunct3();
	unsigned char funct2 = instr -> getFunct2();
	unsigned char funct7 = instr -> getFunct7();

	unsigned imm = instr -> getImm();
	rd = instr -> getRd();
	unsigned char rs1 = instr -> getRs1();
	unsigned char rs2 = instr -> getRs2();
	unsigned char rs3 = instr -> getRs3();
	unsigned char shamt = instr -> getShamt();

	resi = 0;
	resf = 0.0;

	switch (opcode) {
		case LOAD: {
			ldC ++;
			int s1 = LLU2INT(reg -> getIntReg(rs1));
			int simm = SExtension(int(imm));
			int add = s1 + simm;
			exeI = wb = true;
			switch (funct3) {
				case LB: {
					char byte;
					mem -> readMem(add, &byte, BYTE_SIZE);
					resi = INT2LLU(int(byte));
					break;
				}
				case LH: {
					short half;
					mem -> readMem(add, (char *)&half, HALF_SIZE);
					resi = INT2LLU(int(half));
					break;
				}
				case LW: {
					int word;
					mem -> readMem(add, (char *)&word, WORD_SIZE);
					resi = INT2LLU(word);
					break;
				}
				case LBU: {
					unsigned char byte;
					mem -> readMem(add, (char *)&byte, BYTE_SIZE);
					resi = UNS2LLU((unsigned)byte);
					break;
				}
				case LHU: {
					unsigned short half;
					mem -> readMem(add, (char *)&half, HALF_SIZE);
					resi = UNS2LLU((unsigned)half);
					break;
				}
				case LD: {
					long long unsigned dw;
					// printf("0x%08x\n", add);
					mem -> readMem(add, (char *)&dw, DOUBLE_WORD_SIZE);
					resi = dw;
					// printf("0x%08x\n", int(resi));
					break;
				}
				case LWU: {
					unsigned word;
					mem -> readMem(add, (char *)&word, WORD_SIZE);
					resi = UNS2LLU(word);
					break;
				}
				default:
					cerr << "Unknown funct3 in LOAD : " << int(funct3) << endl;
					exit(1);
			}
			break;
		}
		case LOAD_FP: {
			ldC ++;
			int s1 = LLU2INT(reg -> getIntReg(rs1));
			int simm = SExtension(int(imm));
			int add = s1 + simm;
			exeF = wb = true;
			switch (funct3) {
				case FLW: {
					float f;
					mem -> readMem(add, (char *)&f, FLOAT_SIZE);
					resf = double(f);
					// printf("FLW : %lf\n", resf);
					break;
				}
				case FLD:
					double d;
					mem -> readMem(add, (char *)&d, DOUBLE_SIZE);
					resf = d;
					// printf("PC: %08x FLW: %lf\n", reg -> getPC() - 4, resf);
					break;
				default:
					cerr << "Unknown funct3 in LOAD_FP : " << int(funct3) << endl;
					exit(1);
			}
			break;
		}
		case MISC_MEM: // can be ignored
			cout << "The MISC_MEM type instruction has been ignored!" << endl;
			break;
		case OP_IMM: {
			opC ++;
			long long unsigned s1lu = reg -> getIntReg(rs1);
			long long s1l = (long long)s1lu;
			int s1 = LLU2INT(s1lu);
			unsigned s1u = LLU2UNS(s1lu);
			long long simm = (long long)SExtension(int(imm));
			exeI = wb = true;
			switch (funct3) {
				case ADDI: {
					resi = s1l + simm;
					break;
				}
				case SLTI: {
					if (s1l < simm)
						resi = 1LLU;
					else
						resi = 0LLU;
					break;
				}
				case SLTIU: {
					// implement pseudo-op SEQZ
					if (s1lu < (long long unsigned)simm)
						resi = 1LLU;
					else 
						resi = 0LLU;
					break;
				}
				case XORI: {
					// implement pseudo-op NOT
					resi = s1l ^ simm;
					break;
				}
				case ORI: {
					resi = s1l | simm;
					break;
				}
				case ANDI: {
					resi = s1l & simm;
					break;
				}
				// need funct7
				case SLLI:
					resi = (s1lu << (imm & 0x3F));
					break;
				case OP_IMM_7_5:
					if ((funct7 >> 1) == SRLI) { // 64b SRLI
						resi = (s1lu >> (imm & 0x3F));
					} else if ((funct7 >> 1) == SRAI_64) { // 64b SRAI
						resi = ((s1l) >> (imm & 0x3F));
					}
					break;
				default:
					cerr << "Unknown funct3 in OP_IMM : " << int(funct3) << endl;
					exit(1);
			}
			break;
		}
		case AUIPC: {
			opC ++;
			int pc = int(reg -> getPC()) - 4;
			int offset = int(((((imm << 5) | rs1) << 3) | funct3) << 12);
			resi = UNS2LLU(pc + offset);
			// printf("resi = 0x%08x offset = 0x%08x\n", unsigned(resi), offset);
			exeI = wb = true;
			break;
		}
		case OP_IMM_32: {
			opC ++;
			long long unsigned s1l = reg -> getIntReg(rs1);
			int s1 = LLU2INT(s1l);
			unsigned s1u = LLU2UNS(s1l);
			exeI = wb = true;
			switch (funct3) {
				case ADDIW: {
					int simm = SExtension(imm);
					resi = INT2LLU(s1 + simm);
					// printf("simm : %d s1 : %d resi : %d\n", simm, s1, int(resi));
					break;
				}
				case SLLIW:
					resi = INT2LLU(s1 << shamt);
					break;
				case OP_IMM_32_7_5:
					switch (funct7) {
						case SRLIW:
							resi = UNS2LLU(s1u >> shamt);
							break;
						case SRAIW:
							resi = INT2LLU(s1 >> shamt);
							break;
						default:
							cerr << "Unknown funct7 in OP_IMM_32 : " << int(funct7) << endl;
							exit(1);
					}
					break;
				default:
					cerr << "Unknown funct3 in OP_IMM_32 : " << int(funct3) << endl;
					exit(1);
			}
			break;
		}
		case STORE: {
			stC ++;
			data = reg -> getIntReg(rs2);
			int simm = SExtension(int((imm & 0xFE0) | rd));
			int s1 = LLU2INT(reg -> getIntReg(rs1));
			memadd = s1 + simm;
			// printf("%d %d %d\n", simm, s1, memadd);
			wm = true;
			switch (funct3) {
				case SB:
					memsize = BYTE_SIZE;
					break;
				case SH:
					memsize = HALF_SIZE;
					break;
				case SW:
					memsize = WORD_SIZE;
					break;
				case SD:
					memsize = DOUBLE_WORD_SIZE;
					break;
				default:
					cerr << "Unknown funct3 in STORE : " << int(funct3) << endl;
					exit(1);
			}
			break;
		}
		case STORE_FP: {
			stC ++;
			int simm = SExtension((imm & 0xFE0) | rd);
			int s1 = LLU2INT(reg -> getIntReg(rs1));
			memadd = s1 + simm;
			double dd = reg -> getDoubleReg(rs2);
			float d = float(dd);
			data = (*(long long unsigned*)&dd);
			wm = true;
			switch (funct3) {
				case FSW:
					data = (long long unsigned)(*(unsigned*)&d);
					memsize = FLOAT_SIZE;
					break;
				case FSD:
					memsize = DOUBLE_SIZE;
					// printf("FSD: %lld\n", data);
					break;
				default:
					cerr << "Unknown funct3 in STORE_FP : " << funct3 << endl;
			}
			break;
		}
		case AMO: // A set, can be ignored
			cout << "AMO has been ignored." << endl;
			break;
		case OP: {
			opC ++;
			long long unsigned s1lu = reg -> getIntReg(rs1);
			long long s1l = (long long)s1lu;
			int s1 = LLU2INT(s1lu);
			unsigned s1u = LLU2UNS(s1lu);
			long long unsigned s2lu = reg -> getIntReg(rs2);
			long long s2l = (long long)s2lu;
			int s2 = LLU2INT(s2lu);
			unsigned s2u = LLU2UNS(s2lu);
			exeI = wb = true;
			switch (funct3) {
				case OP_7_0:
					switch (funct7) {
						case ADD:
							resi = s1l + s2l;
							break;
						case SUB:
							resi = s1l - s2l;
							break;
						case MUL:
							// store the low 32 bits
							resi = s1l * s2l;
							break;
						default:
							cerr << "Unknown funct7 in OP_7_0 : " << funct7 << endl;
							exit(1);
					}
					break;
				case OP_7_1:
					switch (funct7) {
						case SLL:
							resi = (s1l << (s2 & 0x3F));
							break;
						case MULH:
							// store the high 32 bits
							resi = ((s1l * s2l) >> 32) & 0xFFFFFFFF;
							break;
						default:
							cerr << "Unknown funct7 in OP_7_1 : " << funct7 << endl;
							exit(1);
					}
					break;
				case OP_7_2:
					switch (funct7) {
						case SLT:
							if (s1l < s2l)
								resi = 1LLU;
							else
								resi = 0LLU;
							break;
						case MULHSU: // signed * unsigned
							// store the high 32 bits
							resi = UNS2LLU(unsigned(((s1l * s2l) >> 32) & 0xFFFFFFFF));
							break;
						default:
							cerr << "Unknown funct7 in OP_7_2 : " << funct7 << endl;
							exit(1);
					}
					break;
				case OP_7_3:
					switch (funct7) {
						case SLTU:
							if (s1lu < s2lu)
								resi = 1LLU;
							else 
								resi = 0LLU;
							break;
						case MULHU: // unsigned * unsigned
							// store the high 32 bits
							resi = UNS2LLU(unsigned(((s1lu * s2lu) >> 32) & 0xFFFFFFFF));
							break;
						default:
							cerr << "Unknown funct7 in OP_7_3 : " << funct7 << endl;
							exit(1);
					}
					break;
				case OP_7_4:
					switch (funct7) {
						case XOR:
							resi = s1l ^ s2l;
							break;
						case DIV: // store the quotient
							resi = s1l / s2l;
							break;
						default:
							cerr << "Unknown funct7 in OP_7_4 : " << funct7 << endl;
							exit(1);
					}
					break;
				case OP_7_5:
					switch (funct7) {
						case SRL:
							resi = (s1lu >> (s2 & 0x3F));
							break;
						case SRA:
							resi = (s1l >> (s2 & 0x3F));
							break;
						case DIVU: // store the quotient
							resi = s1lu / s2lu;
							break;
						default:
							cerr << "Unknown funct7 in OP_7_5 : " << funct7 << endl;
							exit(1);
					}
					break;
				case OP_7_6:
					switch (funct7) {
						case OR:
							resi = s1l | s2l;
							break;
						case REM: // store the remainder
							resi = s1l % s2l;
							break;
						default:
							cerr << "Unknown funct7 in OP_7_6 : " << funct7 << endl;
							exit(1);
					}
					break;
				case OP_7_7:
					switch (funct7) {
						case AND:
							resi = s1l & s2l;
							break;
						case REMU: // store the remainder
							resi = s1lu % s2lu;
							break;
						default:
							cerr << "Unknown funct7 in OP_7_7 : " << funct7 << endl;
							exit(1);
					}
					break;
				default:
					cerr << "Unknown funct3 in OP : " << int(funct3) << endl;
					exit(1);
			}
			break;
		}
		case LUI: {
			opC ++;
			resi = INT2LLU(int(((((imm << 5) | rs1) << 3) | funct3) << 12));
			// printf("In LUI, resi : 0x%08x\n", unsigned(resi));
			exeI = wb = true;
			break;
		}
		case OP_32: {
			opC ++;
			long long unsigned s1l = reg -> getIntReg(rs1);
			int s1 = LLU2INT(s1l);
			unsigned s1u = LLU2UNS(s1l);
			long long unsigned s2l = reg -> getIntReg(rs2);
			int s2 = LLU2INT(s2l);
			unsigned s2u = LLU2UNS(s2l);
			exeI = wb = true;
			switch (funct3) {
				case OP_32_7_0:
					switch (funct7) {
						case ADDW:
							resi = INT2LLU(s1 + s2);
							break;
						case SUBW:
							resi = INT2LLU(s1 - s2);
							break;
						case MULW: // only for RV64
							resi = INT2LLU(s1 * s2);
							break;
						default:
							cerr << "Unknown funct7 in OP_32_7_0 : " << funct7 << endl;
							exit(1);
					}
					break;
				case SLLW:
					resi = INT2LLU(s1 << (s2 & 0x1F));
					break;
				case DIVW: // only for RV64
					resi = INT2LLU(s1 / s2);
					break;
				case OP_32_7_5:
					switch (funct7) {
						case SRLW:
							resi = UNS2LLU(s1u >> (s2 & 0x1F));
							break;
						case SRAW:
							resi = INT2LLU(s1 >> (s2 & 0x1F));
							break;
						case DIVUW: // only for RV64
							resi = INT2LLU(int(s1u / s2u));
							break;
						default:
							cerr << "Unknown funct7 in OP_32_7_5 : " << funct7 << endl;
							exit(1);
					}
					break;
				case REMW: // only for RV64
					resi = INT2LLU(s1 % s2);
					break;
				case REMUW: // only for RV64
					resi = INT2LLU(int(s1u % s2u));
					break;
				default:
					cerr << "Unknown funct3 in OP_32 : " << funct3 << endl;
					exit(1);
			}
			break;
		}

		case MADD: { // r1 * r2 + r3
			opC ++;
			double s1d = reg -> getDoubleReg(rs1);
			double s2d = reg -> getDoubleReg(rs2);
			double s3d = reg -> getDoubleReg(rs3);
			float s1 = float(s1d);
			float s2 = float(s2d);
			float s3 = float(s3d);
			exeF = wb = true;
			switch (funct2) {
				case FMADD_S:
					resf = double(s1 * s2 + s3);
					break;
				case FMADD_D:
					resf = s1d * s2d + s3d;
					break;
				default:
					cerr << "Unknown funct2 in MADD : " << funct2 << endl;
					exit(1);
			}
			break;
		}
		case MSUB: { // r1 * r2 - r3
			opC ++;
			double s1d = reg -> getDoubleReg(rs1);
			double s2d = reg -> getDoubleReg(rs2);
			double s3d = reg -> getDoubleReg(rs3);
			float s1 = float(s1d);
			float s2 = float(s2d);
			float s3 = float(s3d);
			exeF = wb = true;
			switch (funct2) {
				case FMSUB_S:
					resf = double(s1 * s2 - s3);
					break;
				case FMSUB_D:
					resf = s1d * s2d - s3d;
					break;
				default:
					cerr << "Unknown funct2 in MSUB : " << funct2 << endl;
					exit(1);
			}
			break;
		}
		case NMSUB: { // - (r1 * r2 - r3)
			opC ++;
			double s1d = reg -> getDoubleReg(rs1);
			double s2d = reg -> getDoubleReg(rs2);
			double s3d = reg -> getDoubleReg(rs3);
			float s1 = float(s1d);
			float s2 = float(s2d);
			float s3 = float(s3d);
			exeF = wb = true;
			switch (funct2) {
				case FNMSUB_S:
					resf = double(- (s1 * s2 - s3));
					break;
				case FNMSUB_D: // ignored
					resf = - (s1d * s2d - s3d);
					break;
				default:
					cerr << "Unknown funct2 in NMSUB : " << funct2 << endl;
					exit(1);
			}
			break;
		}
		case NMADD: { // - (r1 * r2 + r3)
			opC ++;
			double s1d = reg -> getDoubleReg(rs1);
			double s2d = reg -> getDoubleReg(rs2);
			double s3d = reg -> getDoubleReg(rs3);
			float s1 = float(s1d);
			float s2 = float(s2d);
			float s3 = float(s3d);
			exeF = wb = true;
			switch (funct2) {
				case FNMADD_S:
					resf = double(- (s1 * s2 + s3));
					break;
				case FNMADD_D: // ignored
					resf = - (s1d * s2d + s3d);
					break;
				default:
					cerr << "Unknown funct2 in NMADD : " << funct2 << endl;
					exit(1);
			}
			break;
		}
		case OP_FP: {
			opC ++;
			double s1d = reg -> getDoubleReg(rs1);
			float s1f = float(s1d);
			long long unsigned s1lu = reg -> getIntReg(rs1);
			long long s1l = (long long)(s1lu);
			int s1i = LLU2INT(s1lu);
			unsigned s1u = LLU2UNS(s1lu);
			float s2f;
			wb = true;
			switch (funct7) {
				case FADD_S:
					s2f = float(reg -> getDoubleReg(rs2));
					resf = double(s1f + s2f);
					exeF = true;
					break;
				case FADD_D: {
					double s2d = reg -> getDoubleReg(rs2);
					resf = s1d + s2d;
					exeF = true;
					break;
				}
				case FSUB_S:
					s2f = float(reg -> getDoubleReg(rs2));
					resf = double(s1f - s2f);
					exeF = true;
					break;
				case FSUB_D: {
					double s2d = reg -> getDoubleReg(rs2);
					resf = s1d - s2d;
					exeF = true;
					break;
				}
				case FMUL_S:
					s2f = float(reg -> getDoubleReg(rs2));
					resf = double(s1f * s2f);
					exeF = true;
					break;
				case FMUL_D: {
					double s2d = reg -> getDoubleReg(rs2);
					resf = s1d * s2d;
					exeF = true;
					break;
				}
				case FDIV_S:
					s2f = float(reg -> getDoubleReg(rs2));
					resf = double(s1f / s2f);
					exeF = true;
					break;
				case FDIV_D: {
					double s2d = reg -> getDoubleReg(rs2);
					resf = s1d / s2d;
					exeF = true;
					break;
				}
				// with funct3
				case OP_FP_3_10: {
					s2f = float(reg -> getDoubleReg(rs2));
					float as1 = fabs(s1f);
					unsigned sign1 = (*(unsigned*)&s1f) >> 31;
					unsigned sign2 = (*(unsigned*)&s2f) >> 31;
					exeF = true;
					switch (funct3) {
						case FSGNJ_S:
							if (sign2)
								resf = double(- as1);
							else
								resf = double(as1);
							break;
						case FSGNJN_S:
							if (sign2)
								resf = double(as1);
							else
								resf = double(- as1);
							break;
						case FSGNJX_S:
							if ((sign1 ^ sign2))
								resf = double(- as1);
							else
								resf = double(as1);
							break;
						default:
							cerr << "Unknown funct3 in OP_FP_3_10 : " << unsigned(funct3) << endl;
							exit(1);
					}
					break;
				}
				case OP_FP_3_11: {
					double s2 = reg -> getDoubleReg(rs2);
					double as1 = fabs(s1d);
					long long unsigned sign1 = (*(long long unsigned*)&s1d) >> 63;
					long long unsigned sign2 = (*(long long unsigned*)&s2) >> 63;
					exeF = true;
					switch (funct3) {
						case FSGNJ_D:
							if (sign2)
								resf = - as1;
							else
								resf = as1;
							break;
						case FSGNJN_D:
							if (sign2)
								resf = as1;
							else
								resf = - as1;
							break;
						case FSGNJX_D:
							if (sign1 ^ sign2)
								resf = - as1;
							else
								resf = as1;
							break;
						default:
							cerr << "Unknown funct3 in OP_FP_3_11 : " << unsigned(funct3) << endl;
							exit(1);
					}
					break;
				}
				case OP_FP_3_14: {
					s2f = float(reg -> getDoubleReg(rs2));
					exeF = true;
					switch (funct3) {
						case FMIN_S:
							resf = double((s1f < s2f) ? s1f : s2f);
							break;
						case FMAX_S:
							resf = double((s1f > s2f) ? s1f : s2f);
							break;
						default:
							cerr << "Unknown funct3 in OP_FP_3_14 : " << funct3 << endl;
							exit(1);
					}
					break;
				}
				case OP_FP_3_50:
					exeI = true;
					s2f = float(reg -> getDoubleReg(rs2));
					switch (funct3) {
						case FEQ_S:
							resi = (s1f == s2f) ? 1LLU : 0LLU;
							break;
						case FLT_S:
							resi = (s1f < s2f) ? 1LLU : 0LLU;
							break;
						case FLE_S:
							resi = (s1f <= s2f) ? 1LLU : 0LLU;
							break;
						default:
							cerr << "Unknown funct3 in OP_FP_3_50 : " << funct3 << endl;
							exit(1);
					}
					break;
				case OP_FP_3_51: {
					exeI = true;
					double s2 = reg -> getDoubleReg(rs2);
					switch (funct3) {
						case FEQ_D:
							resi = (s1d == s2) ? 1LLU : 0LLU;
							break;
						case FLT_D:
							resi = (s1d < s2) ? 1LLU : 0LLU;
							break;
						case FLE_S:
							resi = (s1d <= s2) ? 1LLU : 0LLU;
							break;
						default:
							cerr << "Unknown funct3 in OP_FP_3_51 : " << funct3 << endl;
							exit(1);
					}
					break;
				}
				// with rs2
				case OP_FP_s2_2C:
					exeF = true;
					switch (rs2) {
						case FSQRT_S:
							resf = sqrt(s1d);
							break;
						default:
							cerr << "Unknown rs2 in OP_FP_s2_2C : " << rs2 << endl;
							exit(1);
					}
					break;
				case OP_FP_s2_60:
					exeI = true;
					switch (rs2) {
						case FCVT_W_S:
							resi = INT2LLU(int(s1f));
							// printf("FCVT_W_S : %lld\n", resi);
							break;
						case FCVT_WU_S:
							resi = UNS2LLU(unsigned(s1f));
							break;
						case FCVT_L_S:
							resi = (long long unsigned)((long long)(s1f));
							break;
						case FCVT_LU_S:
							resi = (long long unsigned)s1f;
							break;
						default:
							cerr << "Unknown rs2 in OP_FP_s2_60 : " << rs2 << endl;
							exit(1);
					}
					break;
				case OP_FP_s2_61:
					exeI = true;
					switch (rs2) {
						case FCVT_W_D:
							resi = INT2LLU(int(s1d));
							// printf("FCVT_W_d : %lld\n", resi);
							break;
						case FCVT_WU_D:
							resi = UNS2LLU(unsigned(s1d));
							break;
						case FCVT_L_D:
							resi = (long long unsigned)((long long)(s1d));
							break;
						case FCVT_LU_D:
							resi = (long long unsigned)s1d;
							break;
						default:
							cerr << "Unknown rs2 in OP_FP_s2_61 : " << rs2 << endl;
							exit(1);
					}
					break;
				case OP_FP_s2_70:
					switch (rs2) {
						case OP_FP_70_3_00:
							switch (funct3) {
								case FMV_X_S:
									resi = INT2LLU(*(int*)(&s1f));
									exeI = true;
									break;
								case FCLASS_S: {
									unsigned binary = unsigned(*(int*)(&s1f));
									unsigned s = (binary >> 30);
									unsigned exp = (binary >> 23) & 0xFF;
									unsigned f = binary & 0x7FFFFF;
									resi = 0;
									if (binary == 0xFF800000) // -infty
										resi |= 0x1;
									if (exp != 0 && exp != 255) { // neg normal
										if (s == 1)
											resi |= 0x2;
										else if (s == 0)
											resi |= 0x40;
									}
									if (exp == 0) { // neg submornal
										if (s == 1)
											resi |= 0x4;
										else if (s == 0)
											resi |= 0x20;
									}
									if (binary == 0x80000000) // -0
										resi |= 0x8;
									if (binary == 0) // 0
										resi |= 0x10;
									if (binary == 0x7F800000) // infty
										resi |= 0x80;
									if (exp == 255 && f != 0) { // NaN
										if ((f >> 22) == 0)
											resi |= 0x100;
										else if ((f >> 22) == 1)
											resi |= 0x200;
									}
									exeI = true;
									break;
								}
								default:
									cerr << "Unknown funct3 in OP_FP_70_3_00 : " << funct3 << endl;
									exit(1);
							}
							break;
						default:
							cerr << "Unknown rs2 in OP_FP_s2_70 : " << rs2 << endl;
							exit(1);
					}
					break;
				case OP_FP_s2_68:
					exeF = true;
					switch (rs2) {
						case FCVT_S_W:
							resf = double(s1i);
							// printf("FCVT_S_W : %lf\n", resf);
							break;
						case FCVT_S_WU:
							resf = double(s1u);
							break;
						case FCVT_S_L:
							resf = double(s1l);
							break;
						case FCVT_S_LU:
							resf = double(s1lu);
							break;
						default:
							cerr << "Unknown rs2 in OP_FP_s2_68 : " << rs2 << endl;
							exit(1);
					}
					break;
				case OP_FP_s2_69: {
					exeF = true;
					switch (rs2) {
						case FCVT_D_W:
							resf = double(s1i);
							break;
						case FCVT_D_WU:
							resf = double(s1u);
							break;
						case FCVT_D_L:
							resf = double(s1l);
							break;
						case FCVT_D_LU:
							resf = double(s1lu);
							break;
						default:
							cerr << "Unknown rs2 in OP_FP_s2_69 : " << rs2 << endl;
							exit(1);
					}
					break;
				}
				case OP_FP_s2_78:
					switch (rs2) {
						case OP_FP_78_3_00:
							switch (funct3) {
								case FMV_S_X:
									resf = double(*(float*)(&s1i));
									exeF = true;
									break;
								default:
									cerr << "Unknown funct3 in OP_FP_78_3_00 : " << funct3 << endl;
									exit(1);
							}
							break;
						default:
							cerr << "Unknown rs2 in OP_FP_s2_78 : " << rs2 << endl;
							exit(1);
					}
					break;
				case OP_FP_s2_79:
					switch (rs2) {
						case OP_FP_79_3_00:
							switch (funct3) {
								case FMV_D_X:
									resf = *(double*)(&s1l);
									exeF = true;
									break;
								default:
									cerr << "Unknown funct3 in OP_FP_79_3_00 : " << funct3 << endl;
									exit(1);
							}
							break;
						default:
							cerr << "Unknown rs2 in OP_FP_s2_78 : " << rs2 << endl;
							exit(1);
					}
					break;
				case OP_FP_s2_20: {
					switch (rs2) {
						case FCVT_S_D:
							resf = double(s1f);
							exeF = true;
							break;
						default:
							cerr << "Unknown rs2 in OP_FP_s2_20 : " << int(rs2) << endl;
							exit(1);
					}
					break;
				}
				case OP_FP_s2_21:
					switch (rs2) {
						case FCVT_D_S:
							resf = s1d;
							exeF = true;
							break;
						default:
							cerr << "Unknown rs2 in OP_FP_s2_21 : " << int(rs2) << endl;
							exit(1);
					}
					break;
				case OP_FP_s2_71:
					switch (rs2) {
						case FMV_X_D:
							resi = *(long long unsigned*)&s1d;
							// printf("%lld\n", resi);
							exeI = true;
							break;
						default:
							cerr << "Unknown rs2 in OP_FP_s2_71 : " << int(rs2) << endl;
							exit(1);
					}
					break;
				default:
					cerr << "Unknown funct7 in OP_FP : " << int(funct7) << endl;
					exit(1);
			}
			break;
		}

		case BRANCH: {
			brC ++;
			long long unsigned s1lu = reg -> getIntReg(rs1);
			long long s1l = (long long)s1lu;
			int s1 = LLU2INT(s1lu);
			unsigned s1u = LLU2UNS(s1lu);
			long long unsigned s2lu = reg -> getIntReg(rs2);
			long long s2l = (long long)s2lu;
			int s2 = LLU2INT(s2lu);
			unsigned s2u = LLU2UNS(s2lu);
			int offset = (SExtension(imm & 0x800) << 1U);
			unsigned rimm = unsigned(rd);// 
			offset |= ((rimm & 0x1) << 11U);
			offset |= (((imm >> 5U) & 0x3F) << 5U);
			offset |= (rimm & 0x1E);
			offset &= unsigned(-2);
			// printf("0x%08x\n", offset);
			unsigned pc = reg -> getPC() - 4U;
			unsigned target = unsigned(offset + pc);
			switch (funct3) {
				case BEQ:
					if (s1l == s2l)
						reg -> setPC(target);
					break;
				case BNE:
					if (s1l != s2l)
						reg -> setPC(target);
					break;
				case BLT:
					if (s1l < s2l)
						reg -> setPC(target);
					break;
				case BGE:
					if (s1l >= s2l)
						reg -> setPC(target);
					break;
				case BLTU:
					if (s1lu < s2lu)
						reg -> setPC(target);
					break;
				case BGEU:
					if (s1lu >= s2lu)
						reg -> setPC(target);
					break;
				default:
					cerr << "Unknown funct3 in BRANCH : " << int(funct3) << endl;
					break;
			}
			break;
		}
		case JALR: {
			brC ++;
			int s1 = LLU2INT(reg -> getIntReg(rs1));
			int simm = SExtension(imm);
			unsigned pc = reg -> getPC() - 4U;
			// printf("%d %d %d\n", rs1, s1, simm);
			// printf("In JALR: 0x%08x %u\n", pc, unsigned(s1 + simm));
			reg -> setPC((unsigned(s1 + simm)) & unsigned(-2));
			resi = UNS2LLU(pc + 4U);
			exeI = wb = true;
			break;
		}
		case JAL: {
			brC ++;
			// jump and link
			// printf("HAHA\n");
			// printf("0x%08x 0x%08x\n", reg -> getPC(), instr -> getInstr());
			unsigned s1u = unsigned(rs1);
			unsigned char funct3 = instr -> getFunct3();
			int offset = SExtension(imm & 0x800) << 9U;
			offset |= (((s1u << 3) | funct3) << 12U);
			offset |= ((imm & 0x1) << 11U);
			offset |= (imm & 0x7FE);
			offset &= unsigned(-2);

			unsigned pc = reg -> getPC() - 4U;
			resi = UNS2LLU(pc + 4U);
			// printf("ra : 0x%08x, regNum: %d\n", unsigned(resi), int(rd));
			// printf("jump address: 0x%08x\n", offset);
			reg -> setPC(unsigned(int(pc) + offset));
			exeI = wb = true;
			break;
		}
		case SYSTEM: {
			sysC ++;
			unsigned token = (funct7 << 5) | rs2;
			switch (token) {
				case SCALL: {
					long long unsigned syscall = reg -> getIntReg(A7);
					switch (syscall) {
						case SYS_EXIT:
							proFinished = true;
							break;
						case SYS_WRITE: {
							int c1=int(reg->getIntReg(A0));
							
							unsigned c2=(unsigned)(reg->getIntReg(A1));
							
							unsigned c3=unsigned(reg->getIntReg(A2));
							char *newc2=new char[c3];
							mem->readMem(c2, newc2, c3);
							int temz = write(c1,(void*)newc2,c3);
							reg->setIntReg(A0 ,INT2LLU(temz));
							// printf("new c2 is : %s", newc2);
							delete newc2;
							
							// printf("c1=%d c2=%d c3=%d temz=%d\n",c1,c2,(int)c3, temz);
							break;
						}
						case SYS_READ:{
							int c1=int(reg->getIntReg(A0));
							
							unsigned c2=(unsigned)(reg->getIntReg(A1));
							
							unsigned c3=unsigned(reg->getIntReg(A2));

							char *newc2=new char[c3];

							int temz=read(c1,(void *)newc2,c3);
							reg->setIntReg(A0 ,INT2LLU(temz));
							mem->writeMem(c2,newc2,c3);
							delete newc2;
							break;

						}
						case SYS_GET_TIME_OF_DAY:{
							unsigned c1=(unsigned)(reg->getIntReg(A0));
							unsigned c2=(unsigned)(reg->getIntReg(A1));

							struct  timeval    tv;

        					struct  timezone   tz;

        					int temz=gettimeofday(&tv,&tz);

        					mem->writeMem(c1,(char *)&tv,sizeof(tv));
        					mem->writeMem(c2,(char *)&tz,sizeof(tz));
        					reg->setIntReg(A0 ,INT2LLU(temz));
        					break;

						}
						case SYS_BRK:{
							long long unsigned c1=(long long unsigned)(reg->getIntReg(A0));
							reg->setIntReg(A0 ,c1);
							break;
						}
						case SYS_FSTAT:{
							int c1=int(reg->getIntReg(A0));
							
							unsigned c2=(unsigned)(reg->getIntReg(A1));

							struct stat newc2;
							int temz=fstat(c1 ,&newc2);
							mem->writeMem(c2,(char*)&newc2,sizeof(newc2));
							reg->setIntReg(A0 ,INT2LLU(temz));
							break;

						}
						case SYS_LSEEK: {
							int c1=int(reg->getIntReg(A0));
							int c2=(int)(reg->getIntReg(A1));
							int c3=int(reg->getIntReg(A2));

							int temz = lseek(c1, c2, c3);

							reg -> setIntReg(A0, INT2LLU(temz));

							break;
						}
						case SYS_CLOSE: {
							// int c1 = int(reg -> getIntReg(A0));

							// int temz = close(c1);

							// reg -> setIntReg(A0, INT2LLU(temz));
							break;
						}
						default:
							cerr << "Unknown syscall : " << syscall << endl;
							// exit(1);
							break;
					}
					break;
				}
				default:
					cerr << "Unknown SYSTEM token : " << token << endl;
					exit(1);
			}
			break;
		}

		default:
			cerr << "Unknown instruction opcode : " << int(opcode) << endl;
			exit(1);
	}
}