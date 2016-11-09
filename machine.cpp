// machine.cpp
// implement the methods in class Machine

#include "machine.h"

using namespace std;

Machine::Machine(bool instructionCount) {
	instr = new Instruction();
	reg = new Register();
	stack = new Stack();
	mem = new Memory();

	loader = new Loadelf(mem);

	wb = wm = exeI = exeF = proFinished = false;
	reg -> setIntReg(SP, MAX_SIZE + VIRMEM_OFFSET - 1);
	reg -> setIntReg(FP, MAX_SIZE + VIRMEM_OFFSET - 1);

	resi = 0;
	resf = 0.0;
	rd = 0;

	ic = instructionCount;

	if (ic)
		count = 0;
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

		// printf("ha\n");

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

	if (ic)
		printf("Instruction Count : %d\n", count);

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
	// printf("ra: 0x%08x sp: 0x%08x S4: 0x%08x ", unsigned(reg -> getIntReg(RA)), unsigned(reg -> getIntReg(SP)), unsigned(reg -> getIntReg(S4)));
	mem -> readMem(pc, (char*)&instrBuff, INSTR_SIZE);
	printf("PC: 0x%08x Instr: 0x%08x\n", pc, instrBuff);
	instr -> setInstruct(instrBuff);
	reg -> setPC(pc + 4U);
}

void Machine::WriteBack() {
	if (exeI)
		reg -> setIntReg(rd, resi);
	else if (exeF)
		reg -> setFloatReg(rd, resf);
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
			int s1 = LLU2INT(reg -> getIntReg(rs1));
			int simm = SExtension(int(imm));
			int add = s1 + simm;
			exeF = wb = true;
			switch (funct3) {
				case FLW:
					float f;
					mem -> readMem(add, (char *)&f, FLOAT_SIZE);
					resf = f;
					break;
				case FLD: // D set, can be ignored
					cout << "D extension instruction FLD has been ignored!" << endl;
					break;
				default:
					cerr << "Unknown funct3 in LOAD_FP : " << int(funct3) << endl;
					exit(1);
			}
		}
		break;
		case MISC_MEM: // can be ignored
			cout << "The MISC_MEM type instruction has been ignored!" << endl;
			break;
		case OP_IMM: {
			long long unsigned s1l = reg -> getIntReg(rs1);
			int s1 = LLU2INT(s1l);
			unsigned s1u = LLU2UNS(s1l);
			int simm = SExtension(int(imm));
			exeI = wb = true;
			switch (funct3) {
				case ADDI: {
					resi = INT2LLU(s1 + simm);
					break;
				}
				case SLTI: {
					if (s1 < simm)
						resi = 1LLU;
					else
						resi = 0LLU;
					break;
				}
				case SLTIU: {
					// implement pseudo-op SEQZ
					if (s1u < unsigned(simm))
						resi = 1LLU;
					else 
						resi = 0LLU;
					break;
				}
				case XORI: {
					// implement pseudo-op NOT
					resi = INT2LLU(s1 ^ simm);
					break;
				}
				case ORI: {
					resi = INT2LLU(s1 | simm);
					break;
				}
				case ANDI: {
					resi = INT2LLU(s1 & simm);
					break;
				}
				// need funct7
				case SLLI:
					if (funct7 == 0x00) {
						unsigned char shamt = imm & 0x1F;
						resi = UNS2LLU((s1u << shamt));
					} else {
						unsigned char shamt = imm & 0x3F;
						resi = (s1l << shamt);
					}
					break;
				case OP_IMM_7_5:
					if (funct7 == SRLI) { // 32b SRLI
						unsigned char shamt = imm & 0x1F;
						resi = UNS2LLU((s1u >> shamt));
					} else if ((funct7 >> 1) == SRLI) { // 64b SRLI
						unsigned char shamt = imm & 0x3F;
						resi = (s1l >> shamt);
					} else if (funct7 == SRAI_32) { // 32b SRAI
						unsigned char shamt = imm & 0x1F;
						resi = INT2LLU((s1 >> shamt));
					} else if ((funct7 >> 1) == SRAI_64) { // 64b SRAI
						unsigned char shamt = imm & 0x3F;
						resi = (s1l >> shamt);
					}
					break;
				default:
					cerr << "Unknown funct3 in OP_IMM : " << int(funct3) << endl;
					exit(1);
			}
			break;
		}
		case AUIPC: {
			int pc = int(reg -> getPC()) - 4;
			int offset = int(((((imm << 5) | rs1) << 3) | funct3) << 12);
			resi = INT2LLU(pc + offset);
			// printf("%d\n", unsigned(resi));
			// printf("0x%08x 0x%08x\n", unsigned(resi), offset);
			exeI = wb = true;
			break;
		}
		case OP_IMM_32: {
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
			data = reg -> getIntReg(rs2);
			int simm = SExtension((imm & 0xFE0) | rd);
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
			int simm = SExtension((imm & 0xFE0) | rd);
			int s1 = LLU2INT(reg -> getIntReg(rs1));
			memadd = s1 + simm;
			float d = reg -> getFloatReg(rs2);
			data = INT2LLU(*(int*)&d);
			wb = true;
			switch (funct3) {
				case FSW:
					memsize = FLOAT_SIZE;
					break;
				case FSD: // D extension, can be ignored
					cout << "The RV64D instruction FSD has been ignored." << endl;
					break;
			}
			break;
		}
		case AMO: // A set, can be ignored
			cout << "AMO has been ignored." << endl;
			break;
		case OP: {
			long long unsigned s1l = reg -> getIntReg(rs1);
			int s1 = LLU2INT(s1l);
			unsigned s1u = LLU2UNS(s1l);
			long long unsigned s2l = reg -> getIntReg(rs2);
			int s2 = LLU2INT(s2l);
			unsigned s2u = LLU2UNS(s2l);
			exeI = wb = true;
			switch (funct3) {
				case OP_7_0:
					switch (funct7) {
						case ADD:
							resi = INT2LLU(s1 + s2);
							break;
						case SUB:
							resi = INT2LLU(s1 - s2);
							break;
						case MUL:
							// store the low 32 bits
							resi = INT2LLU(int((s1l * s2l) & 0xFFFFFFFF));
							break;
					}
					break;
				case OP_7_1:
					switch (funct7) {
						case SLL:
							resi = INT2LLU((s1 << (s2 & 0x3F)));
							break;
						case MULH:
							// store the high 32 bits
							resi = INT2LLU(int(((s1l * s2l) >> 32) & 0xFFFFFFFF));
							break;
					}
					break;
				case OP_7_2:
					switch (funct7) {
						case SLT:
							if (s1 < s2)
								resi = 1LLU;
							else
								resi = 0LLU;
							break;
						case MULHSU: // signed * unsigned
							// store the high 32 bits
							resi = UNS2LLU(unsigned(((s1l * s2l) >> 32) & 0xFFFFFFFF));
							break;
					}
					break;
				case OP_7_3:
					switch (funct7) {
						case SLTU:
							if (s1u < s2u)
								resi = 1LLU;
							else 
								resi = 0LLU;
							break;
						case MULHU: // unsigned * unsigned
							// store the high 32 bits
							resi = UNS2LLU(unsigned(((s1l * s2l) >> 32) & 0xFFFFFFFF));
							break;
					}
					break;
				case OP_7_4:
					switch (funct7) {
						case XOR:
							resi = INT2LLU(s1 ^ s2);
							break;
						case DIV: // store the quotient
							resi = INT2LLU(s1 / s2);
							break;
					}
					break;
				case OP_7_5:
					switch (funct7) {
						case SRL:
							resi = INT2LLU((s1 >> (s2 & 0x3F)));
							break;
						case SRA:
							resi = UNS2LLU((s1u >> (s2 & 0x3F)));
							break;
						case DIVU: // store the quotient
							resi = UNS2LLU(s1u / s2u);
							break;
					}
					break;
				case OP_7_6:
					switch (funct7) {
						case OR:
							resi = INT2LLU(s1 | s2);
							break;
						case REM: // store the remainder
							resi = INT2LLU(s1 % s2);
							break;
					}
					break;
				case OP_7_7:
					switch (funct7) {
						case AND:
							resi = INT2LLU(s1 & s2);
							break;
						case REMU: // store the remainder
							resi = UNS2LLU(s1u % s2u);
							break;
					}
					break;
				default:
					cerr << "Unknown funct3 in OP : " << int(funct3) << endl;
					exit(1);
			}
			break;
		}
		case LUI: {
			resi = INT2LLU(int(((((imm << 5) | rs1) << 3) | funct3) << 12));
			// printf("In LUI, resi : 0x%08x\n", unsigned(resi));
			exeI = wb = true;
			break;
		}
		case OP_32: {
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
					}
					break;
				case SLLW:
					resi = INT2LLU(s1 << (s2u & 0x1F));
					break;
				case DIVW: // only for RV64
					resi = INT2LLU(s1 / s2);
					break;
				case OP_32_7_5:
					switch (funct7) {
						case SRLW:
							resi = INT2LLU(int(s1u >> (s2u & 0x1F)));
							break;
						case SRAW:
							resi = INT2LLU(int(s1u >> (s2u & 0x1F)));
							break;
						case DIVUW: // only for RV64
							resi = INT2LLU(int(s1u / s2u));
							break;
					}
					break;
				case REMW: // only for RV64
					resi = INT2LLU(s1 % s2);
					break;
				case REMUW: // only for RV64
					resi = INT2LLU(int(s1u % s2u));
					break;
			}
			break;
		}

		case MADD: { // r1 * r2 + r3
			float s1 = reg -> getFloatReg(rs1);
			float s2 = reg -> getFloatReg(rs2);
			float s3 = reg -> getFloatReg(rs3);
			exeF = wb = true;
			switch (funct2) {
				case FMADD_S:
					resf = s1 * s2 + s3;
					break;
				case FMADD_D: // ignored
					cout << "The RV64D instructions have been ignored." << endl;
					break;
			}
			break;
		}
		case MSUB: { // r1 * r2 - r3
			float s1 = reg -> getFloatReg(rs1);
			float s2 = reg -> getFloatReg(rs2);
			float s3 = reg -> getFloatReg(rs3);
			exeF = wb = true;
			switch (funct2) {
				case FMSUB_S:
					resf = s1 * s2 - s3;
					break;
				case FMSUB_D: // ignored
					cout << "The RV64D instructions have been ignored." << endl;
					break;
			}
			break;
		}
		case NMSUB: { // - (r1 * r2 - r3)
			float s1 = reg -> getFloatReg(rs1);
			float s2 = reg -> getFloatReg(rs2);
			float s3 = reg -> getFloatReg(rs3);
			exeF = wb = true;
			switch (funct2) {
				case FNMSUB_S:
					resf = - (s1 * s2 - s3);
					break;
				case FNMSUB_D: // ignored
					cout << "The RV64D instructions have been ignored." << endl;
					break;
			}
			break;
		}
		case NMADD: { // - (r1 * r2 + r3)
			float s1 = reg -> getFloatReg(rs1);
			float s2 = reg -> getFloatReg(rs2);
			float s3 = reg -> getFloatReg(rs3);
			exeF = wb = true;
			switch (funct2) {
				case FNMADD_S:
					resf = - (s1 * s2 + s3);
					break;
				case FNMADD_D: // ignored
					cout << "The RV64D instructions have been ignored." << endl;
					break;
			}
			break;
		}
		case OP_FP: {
			float s1f = reg -> getFloatReg(rs1);
			long long unsigned s1lu = reg -> getIntReg(rs1);
			long long s1l = (long long)(s1lu);
			int s1i = LLU2INT(s1lu);
			unsigned s1u = LLU2UNS(s1lu);
			float s2;
			wb = true;
			switch (funct7) {
				case FADD_S:
					s2 = reg -> getFloatReg(rs2);
					resf = s1f + s2;
					exeF = true;
					break;
				case FSUB_S:
					s2 = reg -> getFloatReg(rs2);
					resf = s1f - s2;
					exeF = true;
					break;
				case FMUL_S:
					s2 = reg -> getFloatReg(rs2);
					resf = s1f * s2;
					exeF = true;
					break;
				case FDIV_S:
					s2 = reg -> getFloatReg(rs2);
					resf = s1f / s2;
					exeF = true;
					break;
				// with funct3
				case OP_FP_3_10: {
					s2 = reg -> getFloatReg(rs2);
					float as1 = fabs(s1f);
					exeF = true;
					switch (funct3) {
						case FSGNJ_S:
							if (s2 < 0)
								resf = - as1;
							else
								resf = as1;
							break;
						case FSGNJN_S:
							if (s2 < 0)
								resf = as1;
							else
								resf = - as1;
							break;
						case FSGNJX_S:
							if (s1f < 0 && s2 < 0 || s1f > 0 && s2 > 0)
								resf = - as1;
							else
								resf = as1;
							break;
					}
					break;
				}
				case OP_FP_3_14: {
					s2 = reg -> getFloatReg(rs2);
					exeF = true;
					switch (funct3) {
						case FMIN_S:
							resf = (s1f < s2) ? s1f : s2;
							break;
						case FMAX_S:
							resf = (s1f > s2) ? s1f : s2;
							break;
					}
					break;
				}
				case OP_FP_3_50:
					exeI = true;
					s2 = reg -> getFloatReg(rs2);
					switch (funct3) {
						case FEQ_S:
							resi = (s1f == s2) ? 1 : 0;
							break;
						case FLT_S:
							resi = (s1f < s2) ? 1 : 0;
							break;
						case FLE_S:
							resi = (s1f <= s2) ? 1 : 0;
							break;
					}
					break;
				// with rs2
				case OP_FP_s2_2C:
					exeF = true;
					switch (rs2) {
						case FSQRT_S:
							resf = sqrt(s1f);
							break;
					}
					break;
				case OP_FP_s2_60:
					exeF = true;
					switch (rs2) {
						case FCVT_W_S:
							resf = float(s1i);
							break;
						case FCVT_WU_S:
							resf = float(s1u);
							break;
						case FCVT_L_S:
							resf = float(s1l);
							break;
						case FCVT_LU_S:
							resf = float(s1lu);
							break;
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
							}
							break;
					}
					break;
				case OP_FP_s2_68:
					exeI = true;
					switch (rs2) {
						case FCVT_S_W:
							resi = INT2LLU(int(s1f));
							break;
						case FCVT_S_WU:
							resi = UNS2LLU(unsigned(s1f));
							break;
						case FCVT_S_L:
							resi = (long long unsigned)((long long)(s1f));
							break;
						case FCVT_S_LU:
							resi = (long long unsigned)(s1f);
							break;
					}
					break;
				case OP_FP_s2_78:
					switch (rs2) {
						case OP_FP_78_3_00:
							switch (funct3) {
								case FMV_S_X:
									resf = *(float*)(&s1i);
									exeF = true;
									break;
							}
							break;
					}
					break;
			}
			break;
		}

		case BRANCH: {
			long long unsigned s1l = reg -> getIntReg(rs1);
			int s1 = LLU2INT(s1l);
			unsigned s1u = LLU2UNS(s1l);
			long long unsigned s2l = reg -> getIntReg(rs2);
			int s2 = LLU2INT(s2l);
			unsigned s2u = LLU2UNS(s2l);
			int offset = SExtension(imm & 0x800);
			unsigned rimm = unsigned(rd);
			offset |= ((rimm & 0x1) << 11U);
			offset |= (((imm >> 5U) & 0x3F) << 5U);
			offset |= (rimm & 0x1E);
			offset &= unsigned(-2);
			unsigned pc = reg -> getPC() - 4U;
			unsigned target = unsigned(offset + pc);
			switch (funct3) {
				case BEQ:
					if (s1 == s2)
						reg -> setPC(target);
					break;
				case BNE:
					if (s1 != s2)
						reg -> setPC(target);
					break;
				case BLT:
					if (s1 < s2)
						reg -> setPC(target);
					break;
				case BGE:
					if (s1 >= s2)
						reg -> setPC(target);
					break;
				case BLTU:
					if (s1u < s2u)
						reg -> setPC(target);
					break;
				case BGEU:
					if (s1u >= s2u)
						reg -> setPC(target);
					break;
				default:
					cerr << "Unknown funct3 in BRANCH : " << int(funct3) << endl;
					break;
			}
			break;
		}
		case JALR: {
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
			unsigned token = (funct7 << 5) | rs2;
			switch (token) {
				case SCALL: {
					long long unsigned syscall = reg -> getIntReg(A7);
					switch (syscall) {
						case SYS_EXIT:
							proFinished = true;
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