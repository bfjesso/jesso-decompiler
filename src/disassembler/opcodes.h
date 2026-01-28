#pragma once
#include "disassemblyStructs.h"
#include "registers.h"
#include "prefixes.h"
#include "mnemonics.h"

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
	Eb, Ed, Ev, Ew, Ep, Ey,
	Gb, Gd, Gv, Gz, Gw, Gy,
	By,
	M, Mb, Mw, Md, Mv, Mp, Ma, Mq, Mt, Mps, Mpd, Mdq, My, Mx,
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
	Vps, Vpd, Vx, Vss, Vsd, Vdq, Vqq, Vy, Vq,
	Wps, Wpd, Wss, Wsd, Wdq, Wqq, Wd, Wx, Wq,
	Hps, Hpd, Hx, Hss, Hsd, Hdq, Hqq, Hq,
	Lx,

	EVEXvvvv,

	A_BYTE, // added for the F3 0F 1E FA (ENDBR) instruction. IDK what FA is supposed to be so leaving this here
	NO_OPERAND_CODE
};

enum OpcodeSuperscript
{
	i64,  // invalid in 64-bit mode
	o64,  // only available in 64-bit mode
	d64,  // operand size defaults to 64-bit size
	f64,  // operand size forced to 64-bit size
	f256, // operand size forced to 256-bit size

	NO_SUPERSCRIPT
};

struct Opcode
{
	enum Mnemonic mnemonic;
	char extensionGroup; // -1 if the opcode is not an extended one. 0 = Group 1; 1 = Group 1A; from there this number corresponds to the actual group number
	enum OperandCode operands[4];
	enum OpcodeSuperscript opcodeSuperscript;
};

unsigned char handleOpcode(unsigned char** bytesPtr, unsigned char* maxBytesAddr, char* hasGotModRMRef, unsigned char* modRMByteRef, struct DisassemblerOptions* disassemblerOptions, struct LegacyPrefixes* legPrefixes, struct REXPrefix* rexPrefix, struct VEXPrefix* vexPrefix, struct EVEXPrefix* evexPrefix, struct Opcode* result);
