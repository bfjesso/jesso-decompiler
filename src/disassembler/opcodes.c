#include "opcodes.h"
#include "registers.h"

extern const char* mnemonicStrs[] =
{
	"MOVUPS", "MOVUPD", "MOVSS", "MOVSD", "MOVSX", "MOVD", "VMOVD", "MOV", 
	"LEA",
	"ADDPS", "ADDPD", "ADDSS", "ADDSD", "ADD", "ALIGNMENT",
	"SUB", "AND", "OR", "XOR", "SHL", "SHR", 

	"CVTPS2PD", "CVTPD2PS", "CVTSS2SD", "CVTSD2SS",
	"COMISS", "COMISD",
	
	"IMUL", "IDIV",

	"AAA", "AAD", "AAM", "AAS", "ADC", "ARPL",
	"BOUND",
	"CALL FAR", "CALL NEAR", "CDQ", "CLC", "CLD", "CLI", "CLTS", "CMC", "CMP", "CMPS", "CMPXCHG", "CPUID", "CWDE",
	"DAA", "DAS", "DEC", "DIV",
	"ENDBR", "ENTER",
	"F2XM1", "FABS", "FADD", "FADDP", "FBLD", "FBSTP", "FCHS", "FCLEX", "FCMOVB", "FCMOVBE", "FCMOVE", "FCMOVNB", "FCMOVNBE", "FCMOVNE", "FCMOVNU", "FCMOVU", "FCOM", "FCOMI", "FCOMIP", "FCOMP", "FCOMPP", "FCOS", "FDECSTP", "FDIV", "FDIVP", "FDIVR", "FDIVRP", "FFREE", "FIADD", "FICOM", "FICOMP", "FIDIV", "FIDIVR", "FILD", "FIMUL", "FINCSTP", "FINIT", "FIST", "FISTP", "FISTTP", "FISUB", "FISUBR", "FLD", "FLD1", "FLDCW", "FLDENV", "FLDL2E", "FLDL2T", "FLDLG2", "FLDLN2", "FLDPI", "FLDZ", "FMUL", "FMULP", "FNOP", "FPATAN", "FPREM", "FPREM1", "FPTAN", "FRNDINT", "FRSTOR", "FSAVE", "FSCALE", "FSIN", "FSINCOS", "FSQRT", "FST", "FSTCW", "FSTENV", "FSTP", "FSTSW", "FSUB", "FSUBP", "FSUBR", "FSUBRP", "FTST", "FUCOM", "FUCOMI", "FUCOMIP", "FUCOMP", "FUCOMPP", "FXAM", "FXCH", "FXTRACT", "FYL2X", "FYL2XP1", 
	"HLT",
	"IN", "INC", "INS", "INT", "INT1", "INT3", "INTO", "IRET",
	"JA SHORT", "JB SHORT", "JBE SHORT", "JG SHORT", "JL SHORT", "JLE SHORT", "JNB SHORT", "JGE SHORT", "JNO SHORT", "JNP SHORT", "JNS SHORT", "JNZ SHORT", "JO SHORT", "JP SHORT", "JS SHORT", "JZ SHORT", "JMP SHORT", "JMP FAR", "JMP NEAR", "JRCXZ",
	"LAHF", "LAR", "LDS", "LEAVE", "LES", "LODS", "LOOP", "LOOPNZ", "LOOPZ", "LSL", 
	"MOVS", "MOVSXD", "MUL", "MOVZX",
	"NEG", "NOP", "NOT",
	"OUT", "OUTS",
	"POP", "POPAD", "POP DS", "POP ES", "POP SS", "POPF", "PUSH", "PUSHAD", "PUSH CS", "PUSH DS", "PUSH ES", "PUSH SS", "PUSHF",
	"RCL", "RCR", "RET FAR", "RET NEAR", "ROL", "ROR",
	"SAHF", "SAR", "SBB", "SCAS", "SETA", "SETB", "SETBE", "SETG", "SETL", "SETLE", "SETNB", "SETNL", "SETNO", "SETNP", "SETNS", "SETNZ", "SETO", "SETP", "SETS", "SETZ", "STC", "STD", "STI", "STOS", "SYSCALL", "SYSRET", 
	"TEST",
	"WAIT",
	"XCHG", "XLAT",
};

unsigned char doesOpcodeModifyRegister(unsigned char opcode, unsigned char reg, unsigned char* overwrites)
{
	if (compareRegisters(reg, AX))
	{
		switch (opcode)
		{
		case IDIV:
			return 1;
		}
	}

	return 0;
}