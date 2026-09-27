#pragma once
#include "decompilationStructs.h"

enum JdcStatus getAllConditions(struct DecompilationParameters* params);

static int32_t getNumOfOverlappingConditions(struct DecompilationParameters* params, struct Condition* cond1);

static enum JdcStatus handleConditionsResize(struct DecompilationParameters* params);

static enum JdcStatus removeCondition(struct DecompilationParameters* params, int32_t conditionIndex);

static enum JdcStatus handleCombinedJccResize(struct Condition* condition);

enum JdcStatus decompileConditionEnds(struct DecompilationParameters* params, int32_t instructionIndex, bool* isInUnreachableStateRef, struct JdcStr* result);

enum JdcStatus decompileConditionStarts(struct DecompilationParameters* params, int32_t instructionIndex, struct JdcStr* result);

static enum JdcStatus decompileCondition(struct DecompilationParameters* params, int32_t conditionIndex, bool decompileStart, struct JdcStr* result);

bool isConditionDirectJmp(struct Condition* condition);

struct Condition* getConditionFromDstInstruction(struct DecompilationParameters* params, int32_t instructionIndex);

struct Condition* getConditionFromFirstBodyInstruction(struct DecompilationParameters* params, int32_t instructionIndex);

struct Condition* getConditionFromLastBodyInstruction(struct DecompilationParameters* params, int32_t instructionIndex);

int32_t getConditionChainFirstBodyInstruction(struct DecompilationParameters* params, struct Condition* condition);

int32_t getConditionChainLastBodyInstruction(struct DecompilationParameters* params, struct Condition* condition);

bool checkForConditionalReturn(struct DecompilationParameters* params, int32_t instructionIndex);