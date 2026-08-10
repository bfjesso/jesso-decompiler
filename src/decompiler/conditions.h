#pragma once
#include "decompilationStructs.h"

unsigned char getAllConditions(struct DecompilationParameters* params);

static int getNumOfOverlappingConditions(struct DecompilationParameters* params, struct Condition* cond1);

static unsigned char handleConditionsResize(struct DecompilationParameters* params);

static unsigned char removeCondition(struct DecompilationParameters* params, int conditionIndex);

static unsigned char handleCombinedJccResize(struct Condition* condition);

unsigned char decompileConditionEnds(struct DecompilationParameters* params, int instructionIndex, unsigned char* isInUnreachableStateRef, struct JdcStr* result);

unsigned char decompileConditionStarts(struct DecompilationParameters* params, int instructionIndex, struct JdcStr* result);

static unsigned char decompileCondition(struct DecompilationParameters* params, int conditionIndex, unsigned char decompileStart, struct JdcStr* result);

unsigned char isConditionDirectJmp(struct Condition* condition);

struct Condition* getConditionFromDstInstruction(struct DecompilationParameters* params, int instructionIndex);

struct Condition* getConditionFromFirstBodyInstruction(struct DecompilationParameters* params, int instructionIndex);

struct Condition* getConditionFromLastBodyInstruction(struct DecompilationParameters* params, int instructionIndex);

int getConditionChainFirstBodyInstruction(struct DecompilationParameters* params, struct Condition* condition);

int getConditionChainLastBodyInstruction(struct DecompilationParameters* params, struct Condition* condition);

unsigned char checkForConditionalReturn(struct DecompilationParameters* params, int instructionIndex);