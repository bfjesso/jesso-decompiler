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

#include "functions.h"
#include "../importedFunctions.h"

struct DecompilationParameters
{
	struct Function* functions;
	unsigned short numOfFunctions;
	struct ImportedFunction* imports;
	unsigned short numOfImports;
	struct Function* currentFunc; // function being decompiled
	unsigned short startInstructionIndex; // index of instruction to start decompiling from
};

#include "conditions.h"

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned short decompileFunction(struct DecompilationParameters params, struct LineOfC* resultBuffer, unsigned short resultBufferLen);

#ifdef __cplusplus
}
#endif

static unsigned short generateFunctionHeader(struct Function* function, struct LineOfC* result);

static unsigned char declareAllLocalVariables(struct Function* function, struct LineOfC* resultBuffer, int* resultBufferIndex, unsigned short resultBufferLen);

// params.startInstructionIndex is the index for the instruction in question
static unsigned char doesInstructionModifyReturnRegister(struct DecompilationParameters params);

static unsigned char checkForReturnStatement(struct DecompilationParameters params);

static unsigned char checkForAssignment(struct DisassembledInstruction* instruction);

static unsigned char checkForFunctionCall(struct DecompilationParameters params, struct Function** calleeRef);

static int checkForImportCall(struct DecompilationParameters params);

static unsigned char decompileCondition(struct DecompilationParameters params, struct Condition* conditions, int conditionIndex, struct LineOfC* result);

static unsigned char decompileConditionExpression(struct DecompilationParameters params, char* resultBuffer, unsigned char invertOperator);

static unsigned char decompileReturnStatement(struct DecompilationParameters params, struct LineOfC* result);

static unsigned char decompileAssignment(struct DecompilationParameters params, struct LineOfC* result);

static unsigned char decompileOperand(struct DecompilationParameters params, struct Operand* operand, unsigned char type, char* resultBuffer, unsigned char resultBufferSize);

static unsigned char decompileExpression(struct DecompilationParameters params, unsigned char targetReg, unsigned char type, char* resultBuffer, unsigned char resultBufferSize);

static unsigned char decompileFunctionCall(struct DecompilationParameters params, struct Function* callee, struct LineOfC* result);

static unsigned char decompileImportCall(struct DecompilationParameters params, const char* name, struct LineOfC* result);

static int getFunctionCallNumber(struct DecompilationParameters params, unsigned long long callAddr);

static unsigned char getOperationStr(unsigned char opcode, unsigned char getAssignment, char* resultBuffer);

static void wrapStrInParentheses(char* str);