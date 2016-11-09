// field.h
// store some constants like opcode, funct3 and funct7

// opcodes
#define LOAD 0x03 // with funct3				// done
#define LOAD_FP 0x07 							// done
#define MISC_MEM 0x0F // can be ignored
#define OP_IMM 0x13 							// done
#define AUIPC 0x17 // no funct3 				// done
#define OP_IMM_32 0x1B							// done

#define STORE 0x23								// done
#define STORE_FP 0x27							// done
#define AMO 0x2F // can be ignored
#define OP 0x33									// done
#define LUI 0x37 // no funct 					// done
#define OP_32 0x3B								// done

#define MADD 0x43 // with funct2				// done
#define MSUB 0x47 // with funct2				// done
#define NMSUB 0x4B // with funct2				// done
#define NMADD 0x4F // with funct2				// done
#define OP_FP 0x53 // with funct7				// done

#define BRANCH 0x63 // with funct3				// done
#define JALR 0x67 // with funct3				// done
#define JAL 0x6F // no funct 					// done
#define SYSTEM 0x73 // with funct7 + rs2

// system
#define SCALL 0x000

// funct3
// load
#define LB 0
#define LH 1
#define LW 2
#define LBU 4
#define LHU 5
// 64
#define LWU 6
#define LD 3

// store
#define SB 0
#define SH 1
#define SW 2
// 64
#define SD 3

// op-imm
#define ADDI 0
#define SLTI 2
#define SLTIU 3
#define XORI 4
#define ORI 6
#define ANDI 7
// with funct7, same as 64
#define SLLI 1
#define OP_IMM_7_5 5

// op-imm-32
#define ADDIW 0
// with funct7
#define SLLIW 1
#define OP_IMM_32_7_5 5

// op
#define OP_7_0 0
#define OP_7_1 1
#define OP_7_2 2
#define OP_7_3 3
#define OP_7_4 4
#define OP_7_5 5
#define OP_7_6 6
#define OP_7_7 7

// op-32
#define OP_32_7_0 0
#define SLLW 1
#define DIVW 4
#define OP_32_7_5 5
#define REMW 6
#define REMUW 7

// branch
#define BEQ 0
#define BNE 1
#define BLT 4
#define BGE 5
#define BLTU 6
#define BGEU 7

// jalr
#define JALR3 0

// float point
// load-fp
#define FLW 2
#define FLD 3 // D set, can be ignored

// store-fp
#define FSW 2
#define FSD 3

// op-fp
// 10
#define FSGNJ_S 0
#define FSGNJN_S 1
#define FSGNJX_S 2
// 11
#define FSGNJ_D 0
#define FSGNJN_D 1
#define FSGNJX_D 2
// 14
#define FMIN_S 0
#define FMAX_S 1
// 15
#define FMIN_D 0
#define FMAX_D 1
// 50
#define FEQ_S 2
#define FLT_S 1
#define FLE_S 0
// 51
#define FEQ_D 2
#define FLT_D 1
#define FLE_D 0

// op-fp-7-3
// 32b
#define FMV_X_S 0
#define FMV_S_X 0
#define FCLASS_S 1
// 64b, ignored
#define FMV_X_D 0
#define FMV_D_X 0
#define FCLASS_D 1



// funct2
// 32
#define FMADD_S 0
#define FMSUB_S 0
#define FNMSUB_S 0
#define FNMADD_S 0
// 64
#define FMADD_D 1
#define FMSUB_D 1
#define FNMSUB_D 1
#define FNMADD_D 1



// funct7
// op-imm-5
#define SRLI 0x00
#define SRAI_32 0x20
#define SRAI_64 0x10

// op-imm-32-5
#define SRLIW 0x00
#define SRAIW 0x20

// op 0-7
// 0
#define ADD 0x00
#define SUB 0x20
#define MUL 0x01
// 1
#define SLL 0x00
#define MULH 0x01
// 2
#define SLT 0x00
#define MULHSU 0x01
// 3
#define SLTU 0x00
#define MULHU 0x01
// 4
#define XOR 0x00
#define DIV 0x01
// 5
#define SRL 0x00
#define SRA 0x20
#define DIVU 0x01
// 6
#define OR 0x00
#define REM 0x01
// 7
#define AND 0x00
#define REMU 0x01

// op-32 0 and 5
// 0
#define ADDW 0x00
#define SUBW 0x20
#define MULW 0x01
// 5
#define SRLW 0x00
#define SRAW 0x20
#define DIVUW 0x01

// float point
// op-fp
// 32
#define FADD_S 0x00
#define FSUB_S 0x04
#define FMUL_S 0x08
#define FDIV_S 0x0C
#define OP_FP_s2_2C 0x2C // with rs2
#define OP_FP_3_10 0x10 // with funct3
#define OP_FP_3_14 0x14 // with funct3
#define OP_FP_3_50 0x50 // with funct3
#define OP_FP_s2_60 0x60 // with rs2
#define OP_FP_s2_70 0x70 // with rs2
#define OP_FP_s2_68 0x68 // with rs2
#define OP_FP_s2_78 0x78 // with rs2
// 64, ignored
#define FADD_D 0x01
#define FSUB_D 0x05
#define FMUL_D 0x09
#define FDIV_D 0x0D
#define OP_FP_s2_2D 0x2D // with rs2
#define OP_FP_3_11 0x11 // with funct3
#define OP_FP_3_15 0x15 // with funct3
#define OP_FP_3_51 0x51 // with funct3
#define OP_FP_s2_20 0x20 // with rs2
#define OP_FP_s2_21 0x21 // with rs2
#define OP_FP_s2_71 0x71 // with rs2
#define OP_FP_s2_61 0x61 // with rs2
#define OP_FP_s2_69 0x69 // with rs2
#define OP_FP_s2_79 0x79 // with rs2




// rs2
// op-fp
// 2C
#define FSQRT_S 0x00
// 2D
#define FSART_D 0x00

// 60
// 32b
#define FCVT_W_S 0x00
#define FCVT_WU_S 0x01
// 64b
#define FCVT_L_S 0x02
#define FCVT_LU_S 0x03

// 61
// 32b
#define FCVT_W_D 0x00
#define FCVT_WU_D 0x01
// 64b
#define FCVT_L_D 0x02
#define FCVT_LU_D 0x03

// 20
#define FCVT_S_D 0x01
// 21
#define FCVT_D_S 0x00

// 70
#define OP_FP_70_3_00 0x00 // with funct3
// 71
#define OP_FP_71_3_00 0x00 // with funct3

// 68
// 32b
#define FCVT_S_W 0x00
#define FCVT_S_WU 0x01
// 64b
#define FCVT_S_L 0x02
#define FCVT_S_LU 0x03

// 69
// 32b
#define FCVT_D_W 0x00
#define FCVT_D_WU 0x01
// 64b
#define FCVT_D_L 0x02
#define FCVT_D_LU 0x03

// 78
#define OP_FP_78_3_00 0x00 // with funct3
// 79
#define OP_FP_79_3_00 0x00 // with funct3

