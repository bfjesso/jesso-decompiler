#pragma once
#include "decompilationUtils.h"
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

static unsigned char decompileCondition(struct DecompilationParameters params, struct Condition* conditions, int conditionIndex, struct LineOfC* result);

static unsigned char decompileReturnStatement(struct DecompilationParameters params, struct LineOfC* result);

static unsigned char decompileAssignment(struct DecompilationParameters params, struct LineOfC* result);