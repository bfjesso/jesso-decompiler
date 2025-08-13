#pragma once
#include "decompilationUtils.h"

unsigned char checkForFunctionCall(struct DecompilationParameters params, struct Function** calleeRef);

unsigned char decompileFunctionCall(struct DecompilationParameters params, struct Function* callee, struct LineOfC* result);

int checkForImportCall(struct DecompilationParameters params);

unsigned char decompileImportCall(struct DecompilationParameters params, const char* name, struct LineOfC* result);

int getFunctionCallNumber(struct DecompilationParameters params, unsigned long long callAddr);