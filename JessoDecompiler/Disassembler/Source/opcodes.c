#include "../Headers/opcodes.h"

const char* mnemonicStrs[] =
{
	"AAA", "AAD", "AAM", "AAS", "ADC", "ADD", "AND", "ARPL",
	"BOUND",
	"CALL FAR", "CALL NEAR", "CDQ", "CLC", "CLD", "CLI", "CMC", "CMP", "CMPS", "CWDE",
	"DAA", "DAS", "DEC", "DIV",
	"ENTER",
	"HLT",
	"IDIV", "IMUL", "IN", "INC", "INS", "INT", "INT1", "INT3", "INTO", "IRET",
	"JA SHORT", "JB SHORT", "JBE SHORT", "JG SHORT", "JL SHORT", "JLE SHORT", "JMP FAR", "JMP NEAR", "JMP SHORT", "JNB SHORT", "JNL SHORT", "JNO SHORT", "JNP SHORT", "JNS SHORT", "JNZ SHORT", "JO SHORT", "JP SHORT", "JRCXZ", "JS SHORT", "JZ SHORT",
	"LAHF", "LDS", "LEA", "LEAVE", "LES", "LODS", "LOOP", "LOOPNZ", "LOOPZ",
	"MOV", "MOVS", "MOVSXD", "MUL",
	"NEG", "NOP", "NOT",
	"OR", "OUT", "OUTS",
	"POP", "POPAD", "POP DS", "POP ES", "POP SS", "POPF", "PUSH", "PUSHAD", "PUSH CS", "PUSH DS", "PUSH ES", "PUSH SS", "PUSHF",
	"RCL", "RCR", "RET FAR", "RET NEAR", "ROL", "ROR",
	"SAHF", "SAR", "SBB", "SCAS", "SHL", "SHR", "STC", "STD", "STI", "STOS", "SUB",
	"TEST",
	"WAIT",
	"XCHG", "XLAT", "XOR"
};