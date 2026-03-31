#pragma once
#include "decompilationStructs.h"

unsigned char getAllConditions(struct DecompilationParameters* params);

static unsigned char handleConditionsResize(struct DecompilationParameters* params);

static unsigned char handleCombinedJccResize(struct Condition* condition);

unsigned char decompileConditionDsts(struct DecompilationParameters* params, struct JdcStr* result);

unsigned char decompileConditionJccs(struct DecompilationParameters* params, struct JdcStr* result);

static unsigned char decompileCondition(struct DecompilationParameters* params, int conditionIndex, struct JdcStr* result);

int checkForConditionStart(struct DecompilationParameters* params);