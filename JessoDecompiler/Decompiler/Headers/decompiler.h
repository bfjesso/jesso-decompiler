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
	char line[150];
};

struct VariableType 
{
	unsigned char primitiveType;
	unsigned char isSigned;
	unsigned char isPointer;
};

struct Function 
{
	unsigned long long* addresses;
	struct DisassembledInstruction* instructions;
	unsigned char numOfInstructions;

	char name[20];

	struct VariableType returnType;
	unsigned long long addressOfReturnFunction; // if the function's return value is that of another function, this will be the address of that function

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
	unsigned char fixAllFunctionReturnTypes(struct Function* functions, unsigned short numOfFunctions);
	unsigned short decompileFunction(struct Function* functions, unsigned short numOfFunctions, unsigned short functionIndex, const char* functionName, struct LineOfC* resultBuffer, unsigned short resultBufferLen);

#ifdef __cplusplus
}
#endif

static int findFunctionByAddress(struct Function* functions, int low, int high, unsigned long long address);

static unsigned short generateFunctionHeader(struct Function* function, const char* functionName, struct LineOfC* result);

static unsigned short variableTypeToStr(struct VariableType* varType, char* buffer, unsigned char bufferLen);

static unsigned char getAllScopes(struct Function* function, struct Scope* resultBuffer, unsigned char resultBufferLen);

static unsigned char decompileReturnStatement(struct Function* functions, unsigned short numOfFunctions, unsigned short functionIndex, struct LineOfC* result);

static unsigned char operandToC(struct DisassembledInstruction* instructions, unsigned short numOfInstructions, struct Operand* operand, char* resultBuffer, unsigned char resultBufferSize);

static unsigned char decompileFunctionCall(struct DisassembledInstruction* instructions, unsigned short numOfInstructions, struct Functions* callee, struct LineOfC* result);