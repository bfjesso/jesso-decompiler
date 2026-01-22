#pragma once
#include "mnemonics.h"
#include "registers.h"
#include "prefixes.h"

struct DisassemblerOptions
{
	char is64BitMode;
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

struct DisassembledInstruction
{
	enum Mnemonic opcode;
	struct Operand operands[4];
	enum LegacyPrefix group1Prefix;

	unsigned long long address;
	unsigned char isInvalid;
};
