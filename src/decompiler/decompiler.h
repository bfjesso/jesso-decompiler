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

enum ConditionType 
{
	IF_CT,
	ELSE_IF_CT,
	ELSE_CT
};

struct Condition 
{
	int jccIndex;
	int dstIndex; // the index of the instruction jumped to by the jcc
	int exitIndex; // if the last instruction of the condition (the one before dstIndex) is a jmp, this is the index of the instruction jumped to by that jmp
	unsigned char type;

	int andJccIndexes[3];
	unsigned char numOfAnds;

	int orJccIndexes[3];
	unsigned char numOfOrs;
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

static int getAllConditions(struct DecompilationParameters params, struct Condition* conditionsBuffer);

// params.startInstructionIndex is the index for the instruction in question
static unsigned char doesInstructionModifyReturnRegister(struct DecompilationParameters params);

static int checkForCondition(int instructionIndex, struct Condition* conditions, int numOfConditions);

static unsigned char checkForReturnStatement(struct DecompilationParameters params);

static unsigned char checkForAssignment(struct DisassembledInstruction* instruction);

static unsigned char checkForFunctionCall(struct DecompilationParameters params, struct Function** calleeRef);

static unsigned char decompileCondition(struct DecompilationParameters params, struct Condition* condition, struct LineOfC* result);

static unsigned char decompileConditionExpression(struct DecompilationParameters params, char* resultBuffer, unsigned char invertOperator);

static unsigned char decompileReturnStatement(struct DecompilationParameters params, struct LineOfC* result);

static unsigned char decompileAssignment(struct DecompilationParameters params, unsigned char type, struct LineOfC* result);

static unsigned char decompileOperand(struct DecompilationParameters params, struct Operand* operand, unsigned char type, char* resultBuffer, unsigned char resultBufferSize);

static unsigned char decompileExpression(struct DecompilationParameters params, unsigned char targetReg, unsigned char type, char* resultBuffer, unsigned char resultBufferSize);

static unsigned char decompileFunctionCall(struct DecompilationParameters params, struct Function* callee, struct LineOfC* result);

static unsigned char getOperationStr(unsigned char opcode, unsigned char getAssignment, char* resultBuffer);