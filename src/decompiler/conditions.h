#pragma once
#include "decompilationStructs.h"

enum JdcStatus getAllConditions(struct DecompilationParameters* params);

static int getNumOfOverlappingConditions(struct DecompilationParameters* params, struct Condition* cond1);

static enum JdcStatus handleConditionsResize(struct DecompilationParameters* params);

static enum JdcStatus removeCondition(struct DecompilationParameters* params, int conditionIndex);

static enum JdcStatus handleCombinedJccResize(struct Condition* condition);

enum JdcStatus decompileConditionEnds(struct DecompilationParameters* params, int instructionIndex, bool* isInUnreachableStateRef, struct JdcStr* result);

enum JdcStatus decompileConditionStarts(struct DecompilationParameters* params, int instructionIndex, struct JdcStr* result);

static enum JdcStatus decompileCondition(struct DecompilationParameters* params, int conditionIndex, bool decompileStart, struct JdcStr* result);

bool isConditionDirectJmp(struct Condition* condition);

struct Condition* getConditionFromDstInstruction(struct DecompilationParameters* params, int instructionIndex);

struct Condition* getConditionFromFirstBodyInstruction(struct DecompilationParameters* params, int instructionIndex);

struct Condition* getConditionFromLastBodyInstruction(struct DecompilationParameters* params, int instructionIndex);

int getConditionChainFirstBodyInstruction(struct DecompilationParameters* params, struct Condition* condition);

int getConditionChainLastBodyInstruction(struct DecompilationParameters* params, struct Condition* condition);

bool checkForConditionalReturn(struct DecompilationParameters* params, int instructionIndex);