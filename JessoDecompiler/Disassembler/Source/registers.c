#include "../Headers/registers.h"

const char* segmentStrs[] =
{
	"CS",
	"SS",
	"DS",
	"ES",
	"FS",
	"GS"
};

const char* registerStrs[] =
{
	"AL", "CL", "DL", "BL", "AH", "CH", "DH", "BH",
	"R8B", "R9B", "R10B", "R11B", "R12B", "R13B", "R14B", "R15B",
	"AX", "CX", "DX", "BX", "SP", "BP", "SI", "DI", "IP",
	"EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI", "EIP",
	"RAX", "RCX", "RDX", "RBX", "RSP", "RBP", "RSI", "RDI", "RIP",
	"R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15",
	"ST(0)", "ST(1)", "ST(2)", "ST(3)", "ST(4)", "ST(5)", "ST(6)", "ST(7)"
};