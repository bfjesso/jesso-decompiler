#pragma once
#include "decompilationStructs.h"

unsigned char getAllConditions(struct DecompilationParameters* params);

static unsigned char handleConditionsResize(struct DecompilationParameters* params);

static unsigned char handleCombinedJccResize(struct Condition* condition);

unsigned char decompileConditions(struct DecompilationParameters* params, struct JdcStr* result);

static unsigned char decompileCondition(struct DecompilationParameters* params, int conditionIndex, unsigned char decompileStart, struct JdcStr* result);

int getConditionStart(struct Condition* condition);

int getConditionEnd(struct Condition* condition);

int checkForConditionStart(struct DecompilationParameters* params);

unsigned char checkForConditionEnd(struct DecompilationParameters* params);

unsigned char checkForConditionDst(struct DecompilationParameters* params);