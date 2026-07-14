#pragma once
#include "decompilationStructs.h"

unsigned char getAllConditions(struct DecompilationParameters* params);

static struct Condition* doesConditionOverlapWithAnother(struct DecompilationParameters* params, struct Condition* cond1);

static unsigned char handleConditionsResize(struct DecompilationParameters* params);

static unsigned char handleCombinedJccResize(struct Condition* condition);

unsigned char decompileConditionEnds(struct DecompilationParameters* params, int instructionIndex, struct JdcStr* result);

unsigned char decompileConditionStarts(struct DecompilationParameters* params, int instructionIndex, struct JdcStr* result);

static unsigned char decompileCondition(struct DecompilationParameters* params, int conditionIndex, unsigned char decompileStart, struct JdcStr* result);

// a regular condition is not a conditional goto/return, and its not combined with another condition
unsigned char isConditionRegular(struct Condition* condition);

int getConditionFromFirstBodyInstruction(struct DecompilationParameters* params, int instructionIndex);

int getConditionFromLastBodyInstruction(struct DecompilationParameters* params, int instructionIndex);

unsigned char checkForConditionDst(struct DecompilationParameters* params, int instructionIndex);