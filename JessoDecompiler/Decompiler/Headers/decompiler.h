#pragma once
#include "../../Disassembler/Headers/disassembler.h"

struct LineOfC 
{
	char line[20];
	char variables[2][20];
};

struct LocalVariable
{
	char name[10];
	int bpOffset;
};

struct Scope 
{
	unsigned long long start;
	unsigned long long end;
};

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned short decompileFunction(struct DisassembledInstruction* instructions, unsigned long long* addresses, unsigned short numOfInstructions, struct LineOfC* resultBuffer, unsigned short resultBufferLen);

#ifdef __cplusplus
}
#endif

static unsigned char getAllLocalVariables(struct DisassembledInstruction* instructions, unsigned short numOfInstructions, struct LocalVariable* resultBuffer, unsigned char resultBufferLen);

static unsigned char getAllScopes(struct DisassembledInstruction* instructions, unsigned long long* addresses, unsigned short numOfInstructions, struct Scope* resultBuffer, unsigned char resultBufferLen);

static unsigned char handleReturnStatement(struct DisassembledInstruction* instructions, unsigned short numOfInstructions, struct LineOfC* result);

