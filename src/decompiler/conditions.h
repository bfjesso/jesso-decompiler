#pragma once
#include "decompilationStructs.h"

int getAllConditions(struct DecompilationParameters params, struct Condition** conditionsRef, int conditionsBufferSize);

int checkForCondition(int instructionIndex, struct Condition* conditions, int numOfConditions);

unsigned char decompileCondition(struct DecompilationParameters params, struct Condition* conditions, int conditionIndex, struct JdcStr* result);
