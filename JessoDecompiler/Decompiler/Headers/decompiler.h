#pragma once
#include "../../Disassembler/Headers/disassembler.h"

enum CallingConvention 
{
	__CDECL,
	__STDCALL,
	__FASTCALL,
	__THISCALL
};

struct LineOfC 
{
	char line[50];
	char symbols[5][10];
};

struct Function 
{
	unsigned long long* address;
	struct DisassembledInstruction* firstInstruction;
	unsigned char numOfInstructions;

	unsigned char callingConvention;
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

	unsigned char findNextFunction(struct DisassembledInstruction* instructions, unsigned long long* addresses, unsigned short numOfInstructions, struct Function* result, int* instructionIndex);
	unsigned short decompileFunction(struct Function* function, struct LineOfC* resultBuffer, unsigned short resultBufferLen);

#ifdef __cplusplus
}
#endif

static unsigned char getAllScopes(struct Function* function, struct Scope* resultBuffer, unsigned char resultBufferLen);

static unsigned char handleReturnStatement(struct Function* function, struct LineOfC* result);

static unsigned char operandToC(struct DisassembledInstruction* instructions, unsigned short numOfInstructions, struct Operand* operand, char* resultBuffer, unsigned char resultBufferSize, char* isLocalVar);




