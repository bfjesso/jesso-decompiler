#pragma once
#include "decompilationStructs.h"

int getAllConditions(struct DecompilationParameters params, int conditionsBufferSize);

unsigned char decompileCondition(struct DecompilationParameters params, int conditionIndex, struct JdcStr* result);

int checkForConditionStart(struct DecompilationParameters params);
