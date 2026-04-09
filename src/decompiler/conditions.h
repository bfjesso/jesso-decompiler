#pragma once
#include "decompilationStructs.h"

unsigned char getAllConditions(struct DecompilationParameters* params);

static unsigned char handleConditionsResize(struct DecompilationParameters* params);

static unsigned char handleCombinedJccResize(struct Condition* condition);

unsigned char decompileConditions(struct DecompilationParameters* params, struct JdcStr* result);

static unsigned char decompileCondition(struct DecompilationParameters* params, int conditionIndex, unsigned char decompileStart, struct JdcStr* result);

// a regular condition is not a conditional goto/return, and its not combined with another condition
unsigned char isConditionRegular(struct Condition* condition);

int checkForConditionStart(struct DecompilationParameters* params);

int checkForConditionEnd(struct DecompilationParameters* params);

unsigned char checkForConditionDst(struct DecompilationParameters* params);