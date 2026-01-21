#pragma once
#include "mnemonics.h"
#include "operands.h"
#include "prefixes.h"

struct DisassemblerOptions
{
	char is64BitMode;
};

struct DisassembledInstruction
{
	enum Mnemonic opcode;
	struct Operand operands[4];
	enum LegacyPrefix group1Prefix;

	unsigned char numOfBytes;
};

unsigned long long getUIntFromBytes(unsigned char** bytesPtr, unsigned char resultSize);