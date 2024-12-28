#pragma once
#include "../../Disassembler/Headers/disassembler.h"

enum CallingConvention 
{
	__CDECL,
	__STDCALL,
	__FASTCALL,
	__THISCALL
};

enum PrimitiveType
{
	CHAR_TYPE,
	SHORT_TYPE,
	INT_TYPE,
	LONG_LONG_TYPE,

	FLOAT_TYPE,
	DOUBLE_TYPE,

	VOID_TYPE
};

struct LineOfC 
{
	char line[50];

	unsigned char numOfSymbols;
	char symbols[5][10];
};

struct VariableType 
{
	unsigned char primitiveType;
	unsigned char isSigned;
	unsigned char isPointer;
};

struct Function 
{
	unsigned long long* address;
	struct DisassembledInstruction* firstInstruction;
	unsigned char numOfInstructions;

	struct VariableType returnType;

	unsigned char callingConvention;

	unsigned char numOfRegArgs;
	struct VariableType regArgs[4];

	unsigned char numOfStackArgs;
	struct VariableType stackArgs[6];
	unsigned char stackArgBpOffsets[6];
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

	const char* callingConventionStrs[];
	
	unsigned char findNextFunction(struct DisassembledInstruction* instructions, unsigned long long* addresses, unsigned short numOfInstructions, struct Function* result, int* instructionIndex);
	unsigned short decompileFunction(struct Function* function, const char* functionName, struct LineOfC* resultBuffer, unsigned short resultBufferLen);

#ifdef __cplusplus
}
#endif

static unsigned short generateFunctionHeader(struct Function* function, const char* functionName, struct LineOfC* result);

static unsigned short variableTypeToStr(struct VariableType* varType, char* buffer, unsigned char bufferLen);

static unsigned char getAllScopes(struct Function* function, struct Scope* resultBuffer, unsigned char resultBufferLen);

static unsigned char handleReturnStatement(struct Function* function, struct LineOfC* result);

static unsigned char operandToC(struct DisassembledInstruction* instructions, unsigned short numOfInstructions, struct Operand* operand, char* resultBuffer, unsigned char resultBufferSize, char* isLocalVar);

