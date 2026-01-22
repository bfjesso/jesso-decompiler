#include "mnemonics.h"

extern const char* mnemonicStrs[] =
{
	"ERROR",

	"MOV", "MOVUPS", "MOVUPD", "MOVSS", "MOVSD", "MOVSX", "MOVD", "MOVAPS", "MOVAPD", "MOVLPS", "MOVLPD", "MOVSLDUP", "MOVDDUP", "MOVHPS", "MOVHPD", "MOVSHDUP", "MOVNTPS", "MOVNTPD", "MOVMSKPS", "MOVMSKPD", "MOVS", "MOVSXD", "MOVZX", "MOVQ", "MOVDQA", "MOVDQU", "MOVNTI", "MOVQ2DQ", "MOVDQ2Q", "PMOVMSKB", "MOVNTQ", "MOVNTDQ", "MASKMOVQ", "MASKMOVDQU",
	"LEA",
	"ADD", "ADDPS", "ADDPD", "ADDSS", "ADDSD", "HADDPD", "HADDPS", "XADD", "PADDQ",
	"SUB", "SUBPS", "SUBPD", "SUBSS", "SUBSD", "HSUBPD", "HSUBPS",
	"AND", "ANDPS", "ANDPD", "ANDNPS", "ANDNPD", "PAND", "PANDN",
	"OR", "ORPS", "ORPD",
	"XOR", "XORPD", "XORPS",  "PXOR",
	"SHL", "SHLD",
	"SHR", "SHRD",
	"IMUL", "MULPS", "MULPD", "MULSS", "MULSD",
	"IDIV", "DIVPS", "DIVPD", "DIVSS", "DIVSD",
	"CVTPS2PD", "CVTSS2SD",  "CVTPI2PD", "CVTSI2SD", "CVTDQ2PD",
	"CVTPD2PS", "CVTSD2SS",  "CVTPI2PS", "CVTSI2SS", "CVTDQ2PS",

	"CVTTPS2PI", "CVTTPD2PI", "CVTTSS2SI", "CVTTSD2SI", "CVTPS2PI", "CVTPD2PI", "CVTSS2SI", "CVTSD2SI", "CVTPS2DQ", "CVTTPS2DQ", "CVTTPD2DQ", "CVTPD2DQ",
	"CMP", "COMISS", "COMISD",

	"PMULLW",

	"JA SHORT", "JB SHORT", "JBE SHORT", "JG SHORT", "JL SHORT", "JLE SHORT", "JNB SHORT", "JGE SHORT", "JNO SHORT", "JNP SHORT", "JNS SHORT", "JNZ SHORT", "JO SHORT", "JP SHORT", "JS SHORT", "JZ SHORT", "JMP SHORT", "JMP FAR", "JMP NEAR",

	"CMOVO", "CMOVNO", "CMOVB", "CMOVNB", "CMOVZ", "CMOVNZ", "CMOVBE", "CMOVA", "CMOVS", "CMOVNS", "CMOVP", "CMOVNP", "CMOVL", "CMOVGE", "CMOVLE", "CMOVG",

	"SQRTPS", "SQRTPD", "SQRTSS", "SQRTSD", "RSQRTPS", "RSQRTSS",

	"MINPS", "MINPD", "MINSS", "MINSD",
	"MAXPS", "MAXPD", "MAXSS", "MAXSD",

	"ADDSUBPD", "ADDSUBPS",

	"PUNPCKLBW", "PUNPCKLWD", "PUNPCKLDQ", "PACKSSWB", "PACKUSWB", "PUNPCKHBW", "PUNPCKHWD", "PUNPCKHDQ", "PACKSSDW", "PUNPCKLQDQ", "PUNPCKHQDQ",
	"PCMPGTB", "PCMPGTW", "PCMPGTD",

	"PSHUFW", "PSHUFD", "PSHUFHW", "PSHUFLW",

	"PCMPEQB", "PCMPEQW", "PCMPEQD",

	"CMPPS", "CMPPD", "CMPSS", "CMPSD",

	"SHUFPS", "SHUFPD",

	"PSRLW", "PSRLD", "PSRLQ", "PSRLDQ", "PSLLDQ",

	"PSUBUSB", "PSUBUSW",
	"PMINUB",
	"PADDUSB", "PADDUSW",
	"PMAXUB",
	"PAVGB", "PSRAW", "PSRAD", "PAVGW", "PMULHUW", "PMULHW",
	"PSUBSB", "PSUBSW", "PMINSW", "POR", "PADDSB", "PADDSW", "PMAXSW",
	"PSLLW", "PSLLD", "PSLLQ", "PMULUDQ", "PMADDWD", "PSADBW",
	"PSUBB", "PSUBW", "PSUBD", "PSUBQ", "PADDB", "PADDW", "PADDD",

	"FXSAVE", "FXRSTOR", "LDMXCSR", "STMXCSR", "XSAVE", "XRSTOR", "XSAVEOPT", "CLFLUSH",
	"LFENCE", "MFENCE", "SFENCE",
	"RDFSBASE", "RDGSBASE", "WRFSBASE", "WRGSBASE",
	"RDRAND", "RDSEED", "RDPID",

	"AAA", "AAD", "AAM", "AAS", "ADC", "ARPL",
	"BOUND", "BSF", "BSR", "BSWAP", "BT", "BTC", "BTR", "BTS", "BNDLDX", "BNDMOV", "BNDCL", "BNDCU", "BNDSTX", "BNDMK", "BNDCN",
	"CALL FAR", "CALL NEAR", "CDQ", "CLAC", "CLC", "CLD", "CLI", "CLTS", "CMC", "CMPS", "CMPXCHG", "CPUID", "CBW", "CWDE", "CDQE",
	"DAA", "DAS", "DEC", "DIV",
	"EMMS", "ENCLS", "ENCLU", "ENDBR", "ENTER",
	"F2XM1", "FABS", "FADD", "FADDP", "FBLD", "FBSTP", "FCHS", "FCLEX", "FCMOVB", "FCMOVBE", "FCMOVE", "FCMOVNB", "FCMOVNBE", "FCMOVNE", "FCMOVNU", "FCMOVU", "FCOM", "FCOMI", "FCOMIP", "FCOMP", "FCOMPP", "FCOS", "FDECSTP", "FDIV", "FDIVP", "FDIVR", "FDIVRP", "FFREE", "FIADD", "FICOM", "FICOMP", "FIDIV", "FIDIVR", "FILD", "FIMUL", "FINCSTP", "FINIT", "FIST", "FISTP", "FISTTP", "FISUB", "FISUBR", "FLD", "FLD1", "FLDCW", "FLDENV", "FLDL2E", "FLDL2T", "FLDLG2", "FLDLN2", "FLDPI", "FLDZ", "FMUL", "FMULP", "FNOP", "FPATAN", "FPREM", "FPREM1", "FPTAN", "FRNDINT", "FRSTOR", "FSAVE", "FSCALE", "FSIN", "FSINCOS", "FSQRT", "FST", "FSTCW", "FSTENV", "FSTP", "FSTSW", "FSUB", "FSUBP", "FSUBR", "FSUBRP", "FTST", "FUCOM", "FUCOMI", "FUCOMIP", "FUCOMP", "FUCOMPP", "FXAM", "FXCH", "FXTRACT", "FYL2X", "FYL2XP1",
	"GETSEC",
	"HLT",
	"IN", "INC", "INS", "INT", "INT1", "INT3", "INTO", "INVD", "IRET",
	"JRCXZ", "JMPE",
	"LAHF", "LAR", "LDDQU", "LDS", "LEAVE", "LES", "LFS", "LGS", "LODS", "LOOP", "LOOPNZ", "LOOPZ", "LSL", "LSS", "LZCNT",
	"MONITOR", "MUL", "MWAIT",
	"NEG", "NOP", "NOT",
	"OUT", "OUTS",
	"POP", "POPAD", "POP DS", "POP ES", "POP FS", "POP GS", "POP SS", "POPF", "PREFETCHW", "PUSH", "PUSHAD", "PUSH CS", "PUSH DS", "PUSH ES", "PUSH FS", "PUSH GS", "PUSH SS", "PUSHF", "POPCNT", "PINSRW", "PEXTRW",
	"RCL", "RCR", "RDMSR", "RDPMC", "RDTSC", "RET FAR", "RET NEAR", "ROL", "ROR", "RSM", "RCPPS", "RCPSS",
	"SAHF", "SAR", "SBB", "SCAS", "SETA", "SETB", "SETBE", "SETG", "SETL", "SETLE", "SETNB", "SETNL", "SETNO", "SETNP", "SETNS", "SETNZ", "SETO", "SETP", "SETS", "SETZ", "STAC", "STC", "STD", "STI", "STOS", "SYSCALL", "SYSENTER", "SYSEXIT", "SYSRET",
	"TEST", "TZCNT",
	"UCOMISD", "UCOMISS", "UNPCKHPD", "UNPCKHPS", "UNPCKLPD", "UNPCKLPS",
	"VMCALL", "VMFUNC", "VMLAUNCH", "VMRESUME", "VMXOFF", "VMREAD", "VMWRITE",
	"WAIT", "WBINVD", "WRMSR",
	"XABORT", "XBEGIN", "XCHG", "XEND", "XGETBV", "XLAT", "XSETBV", "XTEST"
};

unsigned char isOpcodeMov(enum Mnemonic opcode)
{
	if (opcode >= MOV && opcode <= MASKMOVDQU)
	{
		return 1;
	}

	return 0;
}

unsigned char isOpcodeAdd(enum Mnemonic opcode)
{
	if (opcode >= ADD && opcode <= PADDQ)
	{
		return 1;
	}

	return 0;
}

unsigned char isOpcodeSub(enum Mnemonic opcode)
{
	if (opcode >= SUB && opcode <= HSUBPS)
	{
		return 1;
	}

	return 0;
}

unsigned char isOpcodeAnd(enum Mnemonic opcode)
{
	if (opcode >= AND && opcode <= PANDN)
	{
		return 1;
	}

	return 0;
}

unsigned char isOpcodeOr(enum Mnemonic opcode)
{
	if (opcode >= OR && opcode <= ORPD)
	{
		return 1;
	}

	return 0;
}

unsigned char isOpcodeXor(enum Mnemonic opcode)
{
	if (opcode >= XOR && opcode <= PXOR)
	{
		return 1;
	}

	return 0;
}

unsigned char isOpcodeShl(enum Mnemonic opcode)
{
	if (opcode >= SHL && opcode <= SHLD)
	{
		return 1;
	}

	return 0;
}

unsigned char isOpcodeShr(enum Mnemonic opcode)
{
	if (opcode >= SHR && opcode <= SHRD)
	{
		return 1;
	}

	return 0;
}

unsigned char isOpcodeMul(enum Mnemonic opcode)
{
	if (opcode >= IMUL && opcode <= MULSD)
	{
		return 1;
	}

	return 0;
}

unsigned char isOpcodeDiv(enum Mnemonic opcode)
{
	if (opcode >= IDIV && opcode <= DIVSD)
	{
		return 1;
	}

	return 0;
}

unsigned char isOpcodeCvtToDbl(enum Mnemonic opcode)
{
	if (opcode >= CVTPS2PD && opcode <= CVTDQ2PD)
	{
		return 1;
	}

	return 0;
}

unsigned char isOpcodeCvtToFlt(enum Mnemonic opcode)
{
	if (opcode >= CVTPD2PS && opcode <= CVTDQ2PS)
	{
		return 1;
	}

	return 0;
}

unsigned char isOpcodeCall(enum Mnemonic opcode)
{
	if (opcode == CALL_FAR || opcode == CALL_NEAR || opcode == JMP_FAR || opcode == JMP_NEAR)
	{
		return 1;
	}

	return 0;
}

unsigned char isOpcodeReturn(enum Mnemonic opcode)
{
	if (opcode == RET_NEAR || opcode == RET_FAR)
	{
		return 1;
	}

	return 0;
}

unsigned char isOpcodeJcc(enum Mnemonic opcode)
{
	if (opcode >= JA_SHORT && opcode <= JZ_SHORT)
	{
		return 1;
	}

	return 0;
}

unsigned char isOpcodeCMOVcc(enum Mnemonic opcode)
{
	if (opcode >= CMOVO && opcode <= CMOVG)
	{
		return 1;
	}

	return 0;
}