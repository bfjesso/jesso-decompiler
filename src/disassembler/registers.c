#include "registers.h"

extern const char* segmentStrs[] =
{
	"CS",
	"SS",
	"DS",
	"ES",
	"FS",
	"GS"
};

extern const char* registerStrs[] =
{
	"AL", "CL", "DL", "BL", "AH", "CH", "DH", "BH",
	"R8B", "R9B", "R10B", "R11B", "R12B", "R13B", "R14B", "R15B",
	"AX", "CX", "DX", "BX", "SP", "BP", "SI", "DI", "IP",
	"R8W", "R9W", "R10W", "R11W", "R12W", "R13W", "R14W", "R15W",
	"EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI", "EIP",
	"R8D", "R9D", "R10D", "R11D", "R12D", "R13D", "R14D", "R15D",
	"RAX", "RCX", "RDX", "RBX", "RSP", "RBP", "RSI", "RDI", "RIP",
	"R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15",
	"ST(0)", "ST(1)", "ST(2)", "ST(3)", "ST(4)", "ST(5)", "ST(6)", "ST(7)",
	"MM0", "MM1", "MM2", "MM3", "MM4", "MM5", "MM6", "MM7",
	"XMM0", "XMM1", "XMM2", "XMM3", "XMM4", "XMM5", "XMM6", "XMM7",
	"YMM0", "YMM1", "YMM2", "YMM3", "YMM4", "YMM5", "YMM6", "YMM7",
};

unsigned char compareRegisters(enum Register reg1, enum Register reg2)
{
	if (reg1 == AL || reg1 == AH || reg1 == AX || reg1 == EAX || reg1 == RAX)
	{
		return (reg2 == AL || reg2 == AH || reg2 == AX || reg2 == EAX || reg2 == RAX);
	}
	else if (reg1 == CL || reg1 == CH || reg1 == CX || reg1 == ECX || reg1 == RCX)
	{
		return (reg2 == CL || reg2 == CH || reg2 == CX || reg2 == ECX || reg2 == RCX);
	}
	else if (reg1 == DL || reg1 == DH || reg1 == DX || reg1 == EDX || reg1 == RDX)
	{
		return (reg2 == DL || reg2 == DH || reg2 == DX || reg2 == EDX || reg2 == RDX);
	}
	else if (reg1 == BL || reg1 == BH || reg1 == BX || reg1 == EBX || reg1 == RBX)
	{
		return (reg2 == BL || reg2 == BH || reg2 == BX || reg2 == EBX || reg2 == RBX);
	}
	else if (reg1 == SP || reg1 == ESP || reg1 == RSP)
	{
		return (reg2 == SP || reg2 == ESP || reg2 == RSP);
	}
	else if (reg1 == BP || reg1 == EBP || reg1 == RBP)
	{
		return (reg2 == BP || reg2 == EBP || reg2 == RBP);
	}
	else if (reg1 == SI || reg1 == ESI || reg1 == RSI)
	{
		return (reg2 == SI || reg2 == ESI || reg2 == RSI);
	}
	else if (reg1 == IP || reg1 == EIP || reg1 == RIP)
	{
		return (reg2 == IP || reg2 == EIP || reg2 == RIP);
	}
	else if (reg1 == DI || reg1 == EDI || reg1 == RDI)
	{
		return (reg2 == DI || reg2 == EDI || reg2 == RDI);
	}
	else if (reg1 == R8B || reg1 == R8W || reg1 == R8D || reg1 == R8) 
	{
		return (reg2 == R8B || reg2 == R8W || reg2 == R8D || reg2 == R8);
	}
	else if (reg1 == R9B || reg1 == R9W || reg1 == R9D || reg1 == R9)
	{
		return (reg2 == R9B || reg2 == R9W || reg2 == R9D || reg2 == R9);
	}
	else if (reg1 == R10B || reg1 == R10W || reg1 == R10D || reg1 == R10)
	{
		return (reg2 == R10B || reg2 == R10W || reg2 == R10D || reg2 == R10);
	}
	else if (reg1 == R11B || reg1 == R11W || reg1 == R11D || reg1 == R11)
	{
		return (reg2 == R11B || reg2 == R11W || reg2 == R11D || reg2 == R11);
	}
	else if (reg1 == R12B || reg1 == R12W || reg1 == R12D || reg1 == R12)
	{
		return (reg2 == R12B || reg2 == R12W || reg2 == R12D || reg2 == R12);
	}
	else if (reg1 == R13B || reg1 == R13W || reg1 == R13D || reg1 == R13)
	{
		return (reg2 == R13B || reg2 == R13W || reg2 == R13D || reg2 == R13);
	}
	else if (reg1 == R14B || reg1 == R14W || reg1 == R14D || reg1 == R14)
	{
		return (reg2 == R14B || reg2 == R14W || reg2 == R14D || reg2 == R14);
	}
	else if (reg1 == R15B || reg1 == R15W || reg1 == R15D || reg1 == R15)
	{
		return (reg2 == R15B || reg2 == R15W || reg2 == R15D || reg2 == R15);
	}

	return reg1 == reg2;
}

unsigned char getSizeOfRegister(enum Register reg) // in bytes
{
	if (reg >= AL && reg <= R15B) 
	{
		return 1;
	}
	else if (reg >= AX && reg <= IP) 
	{
		return 2;
	}
	else if (reg >= EAX && reg <= EIP) 
	{
		return 4;
	}
	else if ((reg >= RAX && reg <= R15) || (reg >= MM0 && reg <= MM7))
	{
		return 8;
	}
	else if (reg >= ST0 && reg <= ST7)
	{
		return 10;
	}
	else if (reg >= XMM0 && reg <= XMM7) 
	{
		return 16;
	}
	else if (reg >= YMM0 && reg <= YMM7) 
	{
		return 32;
	}

	return 0;
}

enum Register extendRegister(enum Register reg)
{
	return (enum Register)(reg + (R8 - RAX));
}

enum Register increaseRegisterSize(enum Register reg)
{
	return (enum Register)(reg + (RAX - EAX));
}