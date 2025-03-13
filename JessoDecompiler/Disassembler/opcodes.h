#pragma once

// The information in this file comes from the
//	Intel 64 and IA-32 architectures software developer's manual

enum Mnemonic
{
	MOVUPS, MOVUPD, MOVSS, MOVSD, MOVSX, MOV, 
	LEA, 
	ADDPS, ADDPD, ADDSS, ADDSD, ADD, ALIGNMENT, 
	SUB, AND, OR, XOR, SHL, SHR, 
	
	CVTPS2PD, CVTPD2PS, CVTSS2SD, CVTSD2SS,
	COMISS, COMISD,

	IMUL, IDIV,
	
	AAA, AAD, AAM, AAS, ADC, ARPL,
	BOUND,
	CALL_FAR, CALL_NEAR, CDQ, CLC, CLD, CLI, CLTS, CMC, CMP, CMPS, CMPXCHG, CPUID, CWDE,
	DAA, DAS, DEC, DIV,
	ENDBR, ENTER,
	F2XM1, FABS, FADD, FADDP, FBLD, FBSTP, FCHS, FCLEX, FCMOVB, FCMOVBE, FCMOVE, FCMOVNB, FCMOVNBE, FCMOVNE, FCMOVNU, FCMOVU, FCOM, FCOMI, FCOMIP, FCOMP, FCOMPP, FCOS, FDECSTP, FDIV, FDIVP, FDIVR, FDIVRP, FFREE, FIADD, FICOM, FICOMP, FIDIV, FIDIVR, FILD, FIMUL, FINCSTP, FINIT, FIST, FISTP, FISTTP, FISUB, FISUBR, FLD, FLD1, FLDCW, FLDENV, FLDL2E, FLDL2T, FLDLG2, FLDLN2, FLDPI, FLDZ, FMUL, FMULP, FNOP, FPATAN, FPREM, FPREM1, FPTAN, FRNDINT, FRSTOR, FSAVE, FSCALE, FSIN, FSINCOS, FSQRT, FST, FSTCW, FSTENV, FSTP, FSTSW, FSUB, FSUBP, FSUBR, FSUBRP, FTST, FUCOM, FUCOMI, FUCOMIP, FUCOMP, FUCOMPP, FXAM, FXCH, FXTRACT, FYL2X, FYL2XP1, 
	HLT,
	IN, INC, INS, INT, INT1, INT3, INTO, IRET,
	JA_SHORT, JB_SHORT, JBE_SHORT, JG_SHORT, JL_SHORT, JLE_SHORT, JNB_SHORT, JGE_SHORT, JNO_SHORT, JNP_SHORT, JNS_SHORT, JNZ_SHORT, JO_SHORT, JP_SHORT, JS_SHORT, JZ_SHORT, JMP_SHORT, JMP_FAR, JMP_NEAR, JRCXZ,
	LAHF, LAR, LDS, LEAVE, LES, LODS, LOOP, LOOPNZ, LOOPZ, LSL, 
	MOVS, MOVSXD, MUL, MOVZX,
	NEG, NOP, NOT,
	OUT, OUTS,
	POP, POPAD, POP_DS, POP_ES, POP_SS, POPF, PUSH, PUSHAD, PUSH_CS, PUSH_DS, PUSH_ES, PUSH_SS, PUSHF,
	RCL, RCR, RET_FAR, RET_NEAR, ROL, ROR,
	SAHF, SAR, SBB, SCAS, SETA, SETB, SETBE, SETG, SETL, SETLE, SETNB, SETNL, SETNO, SETNP, SETNS, SETNZ, SETO, SETP, SETS, SETZ, STC, STD, STI, STOS, SYSCALL, SYSRET, 
	TEST,
	WAIT,
	XCHG, XLAT,

	EXTENDED_OPCODE,
	NO_MNEMONIC
};

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
	Eb, Ev, Ew, Ep,
	Gb, Gv, Gz, Gw,
	M, Mw, Md, Mp, Ma, Mq, Mt,
	Ib, Iv, Iz, Iw,
	Yb, Yv, Yz,
	Xb, Xv, Xz,
	Jb, Jz,
	Ob, Ov,
	Sw,
	Ap,
	Vps, Vpd, Vx, Vss, Vsd, 
	Wps, Wpd, Wss, Wsd,
	Hps, Hpd, Hx, Hss, Hsd,
	
	ALIGN,
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
	unsigned char mnemonic;
	char extensionGroup; // -1 if the opcode is not an extended one. 0 = Group 1; 1 = Group 1A; from there this number corresponds to the actual group number
	unsigned char operands[3];
	unsigned char opcodeSuperscript;
};

extern const char* mnemonicStrs[];
