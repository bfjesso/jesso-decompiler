#pragma once
#include "../disassembler/disassembler.h"

enum PrimitiveType
{
	VOID_TYPE,
	
	CHAR_TYPE,
	SHORT_TYPE,
	INT_TYPE,
	LONG_LONG_TYPE,

	FLOAT_TYPE,
	DOUBLE_TYPE
};

struct LineOfC 
{
	char line[150];
	unsigned char indents;
};

struct Scope
{
	struct Scope* lastScope;
	unsigned long long start; // the conditional jmp (Jcc)
	unsigned long long end; // the last instruction of the scope
	short orJccInstructionIndex;
	unsigned char isElseIf;
};

#include "functions.h"

struct DecompilationParameters
{
	struct Function* functions; // all functions
	unsigned short numOfFunctions; // length of functions array
	struct Function* currentFunc; // function being decompiled
	unsigned short startInstructionIndex; // index of instruction to start decompiling from
};

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned short decompileFunction(struct DecompilationParameters params, const char* functionName, struct LineOfC* resultBuffer, unsigned short resultBufferLen);

#ifdef __cplusplus
}
#endif

static unsigned short generateFunctionHeader(struct Function* function, const char* functionName, struct LineOfC* result);

static unsigned char declareAllLocalVariables(struct Function* function, struct LineOfC* resultBuffer, int* resultBufferIndex, unsigned short resultBufferLen);

static unsigned char getAllScopes(struct Function* function, struct Scope* resultBuffer, unsigned char resultBufferLen);

static unsigned char checkForReturnStatement(struct DecompilationParameters params);

static unsigned char checkForAssignment(struct DisassembledInstruction* instruction);

static unsigned char checkForFunctionCall(struct DecompilationParameters params, struct Function** calleeRef);

static unsigned char decompileCondition(struct DecompilationParameters params, struct Scope* scope, struct LineOfC* result);

static unsigned char decompileReturnStatement(struct DecompilationParameters params, unsigned long long scopeStart, struct LineOfC* result);

static unsigned char decompileAssignment(struct DecompilationParameters params, unsigned char type, struct LineOfC* result);

static unsigned char decompileOperand(struct DecompilationParameters params, struct Operand* operand, unsigned char type, char* resultBuffer, unsigned char resultBufferSize);

static unsigned char decompileExpression(struct DecompilationParameters params, unsigned char targetReg, unsigned char type, char* resultBuffer, unsigned char resultBufferSize);

static unsigned char decompileFunctionCall(struct DecompilationParameters params, int calleeIndex, struct LineOfC* result);

static unsigned char getOperationStr(unsigned char opcode, unsigned char getAssignment, char* resultBuffer);