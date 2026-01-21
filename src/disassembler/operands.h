#pragma once
#include "prefixes.h"

// Appendix A: A.2
// "Operands are identified by a two-character code of the form Zz. The first character, an uppercase letter, specifies
//	the addressing method; the second character, a lowercase letter, specifies the type of operand."
enum OperandCode
{
	ONE,
	AL_CODE, CL_CODE, AX_CODE, DX_CODE,
	rAX, rCX, rDX, rBX, rSP, rBP, rSI, rDI,
	rAX_r8, rCX_r9, rDX_r10, rBX_r11, rSP_r12, rBP_r13, rSI_r14, rDI_r15,
	AL_R8B, CL_R9B, DL_R10B, BL_R11B, AH_R12B, CH_R13B, DH_R14B, BH_R15B,
	ST0_CODE, ST1_CODE, ST2_CODE, ST3_CODE, ST4_CODE, ST5_CODE, ST6_CODE, ST7_CODE,
	Eb, Ev, Ew, Ep, Ey,
	Gb, Gd, Gv, Gz, Gw, Gy,
	M, Mw, Md, Mp, Ma, Mq, Mt, Mps, Mpd, My, Mx,
	Ib, Iv, Iz, Iw,
	Yb, Yv, Yz,
	Xb, Xv, Xz,
	Jb, Jz,
	Nq,
	Ob, Ov,
	Rd, Ry, Rv,
	Cd,
	Dd,
	Sw,
	Ap,
	Pd, Ppi, Pq,
	Qd, Qpi, Qq,
	Ups, Upd, Udq, Uq, Ux,
	Vps, Vpd, Vx, Vss, Vsd, Vdq, Vy, Vq,
	Wps, Wpd, Wss, Wsd, Wdq, Wx, Wq,
	Hps, Hpd, Hx, Hss, Hsd, Hdq, Hq,

	A_BYTE, // added for the F3 0F 1E FA (ENDBR) instruction. IDK what FA is supposed to be so leaving this here
	NO_OPERAND_CODE
};

enum OperandType
{
	NO_OPERAND,
	SEGMENT,
	REGISTER,
	MEM_ADDRESS,
	IMMEDIATE
};

struct MemoryAddress
{
	unsigned char ptrSize;
	enum Segment segment;
	unsigned short constSegment;

	enum Register reg;
	unsigned char scale; // if SIB byte
	enum Register regDisplacement;
	long long constDisplacement;
};

struct Operand
{
	union
	{
		enum Segment segment;
		enum Register reg;
		struct MemoryAddress memoryAddress;
		long long immediate;
	};

	enum OperandType type;
};

unsigned char handleOperands(unsigned char** bytesPtr, unsigned char* maxBytesAddr, unsigned char* startBytePtr, char hasGotModRM, unsigned char* modRMByteRef, unsigned char is64BitMode, struct Opcode* opcode, struct LegacyPrefixes* legPrefixes, struct REXPrefix* rexPrefix, struct VEXPrefix* vexPrefix, struct Operand* result);

unsigned char isOperandStackArgument(struct Operand* operand);

unsigned char isOperandLocalVariable(struct Operand* operand);

unsigned char getLastOperand(struct DisassembledInstruction* instruction);

unsigned char areOperandsEqual(struct Operand* op1, struct Operand* op2);

unsigned char operandToValue(struct DisassembledInstruction* instructions, unsigned long long* addresses, int startInstructionIndex, struct Operand* operand, unsigned long long* result);
