#include "registers.h"

extern const char* registerStrs[NUM_OF_REGISTERS] =
{
	"NO REG",

	"AL", "CL", "DL", "BL", "AH", "CH", "DH", "BH",
	"R8B", "R9B", "R10B", "R11B", "R12B", "R13B", "R14B", "R15B",
	"AX", "CX", "DX", "BX", "SP", "BP", "SI", "DI", "IP",
	"R8W", "R9W", "R10W", "R11W", "R12W", "R13W", "R14W", "R15W",
	"EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI", "EIP",
	"R8D", "R9D", "R10D", "R11D", "R12D", "R13D", "R14D", "R15D",
	"RAX", "RCX", "RDX", "RBX", "RSP", "RBP", "RSI", "RDI", "RIP",
	"R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15",
	"MM0", "MM1", "MM2", "MM3", "MM4", "MM5", "MM6", "MM7",

	"XMM0", "XMM1", "XMM2", "XMM3", "XMM4", "XMM5", "XMM6", "XMM7", "XMM8", "XMM9", "XMM10", "XMM11", "XMM12", "XMM13", "XMM14", "XMM15",
	"XMM16", "XMM17", "XMM18", "XMM19", "XMM20", "XMM21", "XMM22", "XMM23", "XMM24", "XMM25", "XMM26", "XMM27", "XMM28", "XMM29", "XMM30", "XMM31",

	"YMM0", "YMM1", "YMM2", "YMM3", "YMM4", "YMM5", "YMM6", "YMM7", "YMM8", "YMM9", "YMM10", "YMM11", "YMM12", "YMM13", "YMM14", "YMM15",
	"YMM16", "YMM17", "YMM18", "YMM19", "YMM20", "YMM21", "YMM22", "YMM23", "YMM24", "YMM25", "YMM26", "YMM27", "YMM28", "YMM29", "YMM30", "YMM31",

	"ZMM0", "ZMM1", "ZMM2", "ZMM3", "ZMM4", "ZMM5", "ZMM6", "ZMM7", "ZMM8", "ZMM9", "ZMM10", "ZMM11", "ZMM12", "ZMM13", "ZMM14", "ZMM15",
	"ZMM16", "ZMM17", "ZMM18", "ZMM19", "ZMM20", "ZMM21", "ZMM22", "ZMM23", "ZMM24", "ZMM25", "ZMM26", "ZMM27", "ZMM28", "ZMM29", "ZMM30", "ZMM31",

	"ST(0)", "ST(1)", "ST(2)", "ST(3)", "ST(4)", "ST(5)", "ST(6)", "ST(7)",

	"ES", "CS", "SS", "DS", "FS", "GS",

	"CR0", "CR1", "CR2", "CR3", "CR4", "CR5", "CR6", "CR7", "CR8", "CR9", "CR10", "CR11", "CR12", "CR13", "CR14", "CR15",
	"CR0", "CR1", "CR2", "CR3", "CR4", "CR5", "CR6", "CR7", "CR8", "CR9", "CR10", "CR11", "CR12", "CR13", "CR14", "CR15",
	"DR0", "DR1", "DR2", "DR3", "DR4", "DR5", "DR6", "DR7", "DR8", "DR9", "DR10", "DR11", "DR12", "DR13", "DR14", "DR15",

	"K0", "K1", "K2", "K3", "K4", "K5", "K6", "K7",

	"CF", "PF", "AF", "ZF", "SF", "OF",


	// LC3
	"PC",
	"R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7",
};

// there should only be 4 reg args max in one function. The XMM regs can take the place of RCX-R9
const enum Register platformRegArgsPE[NUM_PLATFORM_REG_ARGS_PE] = { RCX_X86_REG, RDX_X86_REG, R8_X86_REG, R9_X86_REG };
const enum Register altPlatformRegArgsPE[NUM_PLATFORM_REG_ARGS_PE] = { XMM0_X86_REG, XMM1_X86_REG, XMM2_X86_REG, XMM3_X86_REG };

const enum Register platformRegArgsELF[NUM_PLATFORM_REG_ARGS_ELF] = { RDI_X86_REG, RSI_X86_REG, RDX_X86_REG, RCX_X86_REG, R8_X86_REG, R9_X86_REG };
const enum Register altPlatformRegArgsELF[NUM_PLATFORM_REG_ARGS_ELF] = { NO_REG };

int32_t getNumOfPlatformRegArgs(enum FileFormat fileFormat)
{
	switch (fileFormat)
	{
	case PE_FF:
		return NUM_PLATFORM_REG_ARGS_PE;
	case ELF_FF:
		return NUM_PLATFORM_REG_ARGS_ELF;
	}

#ifdef _WIN32
	return NUM_PLATFORM_REG_ARGS_PE;
#endif

#ifdef linux
	return NUM_PLATFORM_REG_ARGS_ELF;
#endif

	return 0;
}

const enum Register* getPlatformRegArgs(enum FileFormat fileFormat)
{
	switch (fileFormat)
	{
	case PE_FF:
		return platformRegArgsPE;
	case ELF_FF:
		return platformRegArgsELF;
	}

#ifdef _WIN32
	return platformRegArgsPE;
#endif

#ifdef linux
	return platformRegArgsELF;
#endif

	return 0;
}

const enum Register* getAltPlatformRegArgs(enum FileFormat fileFormat)
{
	switch (fileFormat)
	{
	case PE_FF:
		return altPlatformRegArgsPE;
	case ELF_FF:
		return altPlatformRegArgsELF;
	}

#ifdef _WIN32
	return altPlatformRegArgsPE;
#endif

#ifdef linux
	return altPlatformRegArgsELF;
#endif

	return 0;
}

bool compareRegisters(enum Register reg1, enum Register reg2)
{
	if (reg1 == AL_X86_REG || reg1 == AH_X86_REG || reg1 == AX_X86_REG || reg1 == EAX_X86_REG || reg1 == RAX_X86_REG)
	{
		if ((reg1 == AL_X86_REG && reg2 == AH_X86_REG) || (reg1 == AH_X86_REG && reg2 == AL_X86_REG))
		{
			return 0;
		}

		return (reg2 == AL_X86_REG || reg2 == AH_X86_REG || reg2 == AX_X86_REG || reg2 == EAX_X86_REG || reg2 == RAX_X86_REG);
	}
	else if (reg1 == CL_X86_REG || reg1 == CH_X86_REG || reg1 == CX_X86_REG || reg1 == ECX_X86_REG || reg1 == RCX_X86_REG)
	{
		if ((reg1 == CL_X86_REG && reg2 == CH_X86_REG) || (reg1 == CH_X86_REG && reg2 == CL_X86_REG))
		{
			return 0;
		}

		return (reg2 == CL_X86_REG || reg2 == CH_X86_REG || reg2 == CX_X86_REG || reg2 == ECX_X86_REG || reg2 == RCX_X86_REG);
	}
	else if (reg1 == DL_X86_REG || reg1 == DH_X86_REG || reg1 == DX_X86_REG || reg1 == EDX_X86_REG || reg1 == RDX_X86_REG)
	{
		if ((reg1 == DL_X86_REG && reg2 == DH_X86_REG) || (reg1 == DH_X86_REG && reg2 == DL_X86_REG))
		{
			return 0;
		}

		return (reg2 == DL_X86_REG || reg2 == DH_X86_REG || reg2 == DX_X86_REG || reg2 == EDX_X86_REG || reg2 == RDX_X86_REG);
	}
	else if (reg1 == BL_X86_REG || reg1 == BH_X86_REG || reg1 == BX_X86_REG || reg1 == EBX_X86_REG || reg1 == RBX_X86_REG)
	{
		if ((reg1 == BL_X86_REG && reg2 == BH_X86_REG) || (reg1 == BH_X86_REG && reg2 == BL_X86_REG))
		{
			return 0;
		}

		return (reg2 == BL_X86_REG || reg2 == BH_X86_REG || reg2 == BX_X86_REG || reg2 == EBX_X86_REG || reg2 == RBX_X86_REG);
	}
	else if (reg1 == SP_X86_REG || reg1 == ESP_X86_REG || reg1 == RSP_X86_REG)
	{
		return (reg2 == SP_X86_REG || reg2 == ESP_X86_REG || reg2 == RSP_X86_REG);
	}
	else if (reg1 == BP_X86_REG || reg1 == EBP_X86_REG || reg1 == RBP_X86_REG)
	{
		return (reg2 == BP_X86_REG || reg2 == EBP_X86_REG || reg2 == RBP_X86_REG);
	}
	else if (reg1 == SI_X86_REG || reg1 == ESI_X86_REG || reg1 == RSI_X86_REG)
	{
		return (reg2 == SI_X86_REG || reg2 == ESI_X86_REG || reg2 == RSI_X86_REG);
	}
	else if (reg1 == IP_X86_REG || reg1 == EIP_X86_REG || reg1 == RIP_X86_REG)
	{
		return (reg2 == IP_X86_REG || reg2 == EIP_X86_REG || reg2 == RIP_X86_REG);
	}
	else if (reg1 == DI_X86_REG || reg1 == EDI_X86_REG || reg1 == RDI_X86_REG)
	{
		return (reg2 == DI_X86_REG || reg2 == EDI_X86_REG || reg2 == RDI_X86_REG);
	}
	else if (reg1 == R8B_X86_REG || reg1 == R8W_X86_REG || reg1 == R8D_X86_REG || reg1 == R8_X86_REG)
	{
		return (reg2 == R8B_X86_REG || reg2 == R8W_X86_REG || reg2 == R8D_X86_REG || reg2 == R8_X86_REG);
	}
	else if (reg1 == R9B_X86_REG || reg1 == R9W_X86_REG || reg1 == R9D_X86_REG || reg1 == R9_X86_REG)
	{
		return (reg2 == R9B_X86_REG || reg2 == R9W_X86_REG || reg2 == R9D_X86_REG || reg2 == R9_X86_REG);
	}
	else if (reg1 == R10B_X86_REG || reg1 == R10W_X86_REG || reg1 == R10D_X86_REG || reg1 == R10_X86_REG)
	{
		return (reg2 == R10B_X86_REG || reg2 == R10W_X86_REG || reg2 == R10D_X86_REG || reg2 == R10_X86_REG);
	}
	else if (reg1 == R11B_X86_REG || reg1 == R11W_X86_REG || reg1 == R11D_X86_REG || reg1 == R11_X86_REG)
	{
		return (reg2 == R11B_X86_REG || reg2 == R11W_X86_REG || reg2 == R11D_X86_REG || reg2 == R11_X86_REG);
	}
	else if (reg1 == R12B_X86_REG || reg1 == R12W_X86_REG || reg1 == R12D_X86_REG || reg1 == R12_X86_REG)
	{
		return (reg2 == R12B_X86_REG || reg2 == R12W_X86_REG || reg2 == R12D_X86_REG || reg2 == R12_X86_REG);
	}
	else if (reg1 == R13B_X86_REG || reg1 == R13W_X86_REG || reg1 == R13D_X86_REG || reg1 == R13_X86_REG)
	{
		return (reg2 == R13B_X86_REG || reg2 == R13W_X86_REG || reg2 == R13D_X86_REG || reg2 == R13_X86_REG);
	}
	else if (reg1 == R14B_X86_REG || reg1 == R14W_X86_REG || reg1 == R14D_X86_REG || reg1 == R14_X86_REG)
	{
		return (reg2 == R14B_X86_REG || reg2 == R14W_X86_REG || reg2 == R14D_X86_REG || reg2 == R14_X86_REG);
	}
	else if (reg1 == R15B_X86_REG || reg1 == R15W_X86_REG || reg1 == R15D_X86_REG || reg1 == R15_X86_REG)
	{
		return (reg2 == R15B_X86_REG || reg2 == R15W_X86_REG || reg2 == R15D_X86_REG || reg2 == R15_X86_REG);
	}

	return reg1 == reg2;
}

bool isRegisterInstructionPointer(enum Register reg)
{
	return compareRegisters(reg, IP_X86_REG) || compareRegisters(reg, PC_LC3_REG);
}

bool isRegisterPointer(enum Register reg)
{
	return compareRegisters(reg, BP_X86_REG) || compareRegisters(reg, SP_X86_REG) || isRegisterInstructionPointer(reg);
}

bool isRegisterStatusFlag(enum Register reg)
{
	return reg >= CF_X86_REG && reg <= OF_X86_REG;
}

bool isRegisterHighByte(enum Register reg)
{
	return reg == AH_X86_REG || reg == CH_X86_REG || reg == DH_X86_REG || reg == BH_X86_REG;
}

bool isRegisterPlatformArg(enum Register reg, enum FileFormat fileFormat)
{
	int32_t numOfPlatformRegArgs = getNumOfPlatformRegArgs(fileFormat);
	const enum Register* platformRegArgs = getPlatformRegArgs(fileFormat);
	const enum Register* altPlatformRegArgs = getAltPlatformRegArgs(fileFormat);

	for (int32_t i = 0; i < numOfPlatformRegArgs; i++)
	{
		if (compareRegisters(reg, platformRegArgs[i]) || compareRegisters(reg, altPlatformRegArgs[i]))
		{
			return true;
		}
	}

	return false;
}

uint8_t getSizeOfRegister(enum Register reg) // in bytes
{
	if ((reg >= AL_X86_REG && reg <= R15B_X86_REG) || isRegisterStatusFlag(reg)) // the status flags are actually single bits in the EFLAGS reg
	{
		return 1;
	}
	else if ((reg >= AX_X86_REG && reg <= R15W_X86_REG) || (reg >= ES_X86_REG && reg <= GS_X86_REG))
	{
		return 2;
	}
	else if (reg >= EAX_X86_REG && reg <= R15D_X86_REG || (reg >= CR0D_X86_REG && reg <= CR15D_X86_REG))
	{
		return 4;
	}
	else if ((reg >= RAX_X86_REG && reg <= R15_X86_REG) ||
		(reg >= MM0_X86_REG && reg <= MM7_X86_REG) ||
		(reg >= CR0_X86_REG && reg <= CR15_X86_REG) ||
		(reg >= DR0_X86_REG && reg <= DR15_X86_REG) ||
		(reg >= K0_X86_REG && reg <= K7_X86_REG))
	{
		return 8;
	}
	else if (reg >= ST0_X86_REG && reg <= ST7_X86_REG)
	{
		return 10;
	}
	else if (reg >= XMM0_X86_REG && reg <= XMM31_X86_REG)
	{
		return 16;
	}
	else if (reg >= YMM0_X86_REG && reg <= YMM31_X86_REG)
	{
		return 32;
	}
	else if (reg >= ZMM0_X86_REG && reg <= ZMM31_X86_REG)
	{
		return 64;
	}

	return 0;
}

enum Register extendRegister(enum Register reg)
{
	if (reg >= AL_X86_REG && reg <= BH_X86_REG)
	{
		return (enum Register)(reg + (R8B_X86_REG - AL_X86_REG));
	}
	else if ((reg >= XMM0_X86_REG && reg <= XMM7_X86_REG) ||
		(reg >= XMM16_X86_REG && reg <= XMM23_X86_REG) ||
		(reg >= YMM0_X86_REG && reg <= YMM7_X86_REG) ||
		(reg >= YMM16_X86_REG && reg <= YMM23_X86_REG) ||
		(reg >= ZMM0_X86_REG && reg <= ZMM7_X86_REG) ||
		(reg >= ZMM16_X86_REG && reg <= ZMM23_X86_REG))
	{
		return (enum Register)(reg + (XMM8_X86_REG - XMM0_X86_REG));
	}
	else
	{
		return (enum Register)(reg + (R8_X86_REG - RAX_X86_REG));
	}
}

enum Register increaseRegisterSize(enum Register reg)
{
	if (reg >= AL_X86_REG && reg <= BL_X86_REG)
	{
		return (enum Register)(reg + (AX_X86_REG - AL_X86_REG));
	}
	else if (reg >= AH_X86_REG && reg <= BH_X86_REG)
	{
		return (enum Register)(reg + (AX_X86_REG - AH_X86_REG));
	}
	else
	{
		return (enum Register)(reg + (RAX_X86_REG - EAX_X86_REG));
	}
}

bool isRegMM(enum Register reg)
{
	return reg >= MM0_X86_REG && reg <= MM7_X86_REG;
}

bool isRegXMM(enum Register reg)
{
	return reg >= XMM0_X86_REG && reg <= XMM31_X86_REG;
}