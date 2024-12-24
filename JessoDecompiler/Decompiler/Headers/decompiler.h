#pragma once
#include "../../Disassembler/Headers/disassembler.h"

struct LineOfC 
{
	char line[50];
	char variables[5][10];
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

const char* operationStrs[] =
{
	"",		// MOV
	" + ",	// ADD
	" - ",	// SUB
	" * ",	// IMUL
	" / "	// IDIV
};

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned short decompileFunction(struct DisassembledInstruction* instructions, unsigned long long* addresses, unsigned short numOfInstructions, struct LineOfC* resultBuffer, unsigned short resultBufferLen);

#ifdef __cplusplus
}
#endif

static unsigned char getAllScopes(struct DisassembledInstruction* instructions, unsigned long long* addresses, unsigned short numOfInstructions, struct Scope* resultBuffer, unsigned char resultBufferLen);

static unsigned char handleReturnStatement(struct DisassembledInstruction* instructions, unsigned short numOfInstructions, struct LineOfC* result);

static unsigned char operandToC(struct DisassembledInstruction* instructions, unsigned short numOfInstructions, struct Operand* operand, char* resultBuffer, unsigned char resultBufferSize, char* isLocalVar);




