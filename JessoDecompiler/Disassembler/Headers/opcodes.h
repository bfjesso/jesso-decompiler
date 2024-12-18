#pragma once

// The information in this file comes from the
//	Intel 64 and IA-32 architectures software developer's manual

enum Mnemonic
{
	AAA, AAD, AAM, AAS, ADC, ADD, AND, ARPL,
	BOUND,
	CALL_FAR, CALL_NEAR, CDQ, CLC, CLD, CLI, CMC, CMP, CMPS, CWDE,
	DAA, DAS, DEC, DIV,
	ENTER,
	F2XM1, FABS, FADD, FCHS, FCOM, FCOMP, FCOS, FDECSTP, FDIV, FDIVR, FINCSTP, FLD, FLD1, FLDCW, FLDENV, FLDL2E, FLDL2T, FLDLG2, FLDLN2, FLDPI, FLDZ, FMUL, FNOP, FPATAN, FPREM1, FPREM, FPTAN, FRNDINT, FSCALE, FSIN, FSINCOS, FSQRT, FST, FSTCW, FSTENV, FSTP, FSUB, FSUBR, FTST, FXAM, FXCH, FXTRACT, FYL2X, FYL2XP1,
	HLT,
	IDIV, IMUL, IN, INC, INS, INT, INT1, INT3, INTO, IRET,
	JA, JB, JBE, JG, JL, JLE, JMP_FAR, JMP_NEAR, JMP_SHORT, JNB, JNL, JNO, JNP, JNS, JNZ, JO, JP, JRCXZ, JS, JZ,
	LAHF, LDS, LEA, LEAVE, LES, LODS, LOOP, LOOPNZ, LOOPZ, 
	MOV, MOVS, MOVSXD, MUL,
	NEG, NOP, NOT,
	OR, OUT, OUTS,
	POP, POPAD, POP_DS, POP_ES, POP_SS, POPF, PUSH, PUSHAD, PUSH_CS, PUSH_DS, PUSH_ES, PUSH_SS, PUSHF,
	RCL, RCR, RET_FAR, RET_NEAR, ROL, ROR,
	SAHF, SAR, SBB, SCAS, SHL, SHR, STC, STD, STI, STOS, SUB,
	TEST,
	WAIT,
	XCHG, XLAT, XOR,

	EXTENDED_OPCODE,
	NO_MNEMONIC
};

// Appendix A: A.2
// "Operands are identified by a two-character code of the form Zz. The first character, an uppercase letter, specifies
//	the addressing method; the second character, a lowercase letter, specifies the type of operand."
enum OperandCode
{
	ONE,
	AL_CODE, CL_CODE, DX_CODE,
	rAX, rCX, rDX, rBX, rSP, rBP, rSI, rDI,
	rAX_r8, rCX_r9, rDX_r10, rBX_r11, rSP_r12, rBP_r13, rSI_r14, rDI_r15,
	AL_R8B, CL_R9B, DL_R10B, BL_R11B, AH_R12B, CH_R13B, DH_R14B, BH_R15B,
	ST0_CODE, ST1_CODE, ST2_CODE, ST3_CODE, ST4_CODE, ST5_CODE, ST6_CODE, ST7_CODE,
	Eb, Ev, Ew, Ep,
	Gb, Gv, Gz, Gw,
	M, Mw, Md, Mp, Ma,
	Ib, Iv, Iz, Iw,
	Yb, Yv, Yz,
	Xb, Xv, Xz,
	Jb, Jz,
	Ob, Ov,
	Sw,
	Ap,

	NO_OPERAND_CODE
};

enum OpcodeSuperscript
{
	i64, // invalid in 64-bit mode
	o64, // only available in 64-bit mode
	d64, // operand size defaults to 64-bit size
	f64, // operand size forced to 64-bit size

	NO_SUPERSCRIPT
};

struct Opcode
{
	unsigned char mnemonic;
	char extensionGroup; // -1 if the opcode is not an extended one. 0 = Group 1; 1 = Group 1A; from there this number corresponds to the actual group number
	unsigned char operands[3];
	unsigned char opcodeSuperscript;
};

extern const char* mnemonicStrs[];