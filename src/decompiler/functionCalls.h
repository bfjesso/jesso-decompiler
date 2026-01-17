#pragma once
#include "decompilationUtils.h"

unsigned char checkForFunctionCall(struct DecompilationParameters params, struct Function** calleeRef);

unsigned char decompileFunctionCall(struct DecompilationParameters params, struct Function* callee, struct JdcStr* result);

int checkForImportCall(struct DecompilationParameters params);

unsigned char decompileImportCall(struct DecompilationParameters params, int importIndex, struct JdcStr* result);

int getFunctionCallNumber(struct DecompilationParameters params, unsigned long long callAddr);