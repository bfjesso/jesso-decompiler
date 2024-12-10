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
	HLT,
	IDIV, IMUL, IN, INC, INS, INT, INT1, INT3, INTO, IRET,
	JA, JB, JBE, JG, JL, JLE, JMP, JMP_FAR, JMP_NEAR, JMP_SHORT, JNB, JNL, JNO, JNP, JNS, JNZ, JO, JP, JRCXZ, JS, JZ,
	LAHF, LDS, LEA, LEAVE, LES, LODS, LOOP, LOOPNZ, LOOPZ, 
	MOV, MOVS, MOVSXD, MUL,
	NEG, NOP, NOT,
	OR, OUT, OUTS,
	POP, POPAD, POP_DS, POP_ES, POP_SS, POPFD, PUSH, PUSHAD, PUSH_CS, PUSH_DS, PUSH_ES, PUSH_SS, PUSHFD,
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
	Eb, Ev, Ew,
	Gb, Gv, Gz, Gw,
	M, Mp, Ma,
	Ib, Iv, Iz, Iw,
	Yb, Yv, Yz,
	Xb, Xv, Xz,
	Jb, Jz,
	Ob, Ov,
	Sw,
	Ap,
	Fv,

	NO_OPERAND_CODE
};

struct Opcode
{
	const enum Mnemonic mnemonic;
	const char extensionGroup; // -1 if the opcode is not an extended one. 0 = Group 1; 1 = Group 1A; from there this number corresponds to the actual group number
	const enum OperandCode operand1;
	const enum OperandCode operand2;
	const enum OperandCode operand3;
};