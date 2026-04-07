#pragma once
#include "decompilationStructs.h"

unsigned char checkForKnownFunctionCall(struct DecompilationParameters* params, struct Function** calleeRef);

unsigned char decompileKnownFunctionCall(struct DecompilationParameters* params, struct Function* callee, struct JdcStr* result);

unsigned char checkForUnknownFunctionCall(struct DecompilationParameters* params);

unsigned char decompileUnknownFunctionCall(struct DecompilationParameters* params, struct JdcStr* result);

int getImportIndexByAddress(struct DecompilationParameters* params, unsigned long long calleeAddress);

int getFunctionCallNumber(struct DecompilationParameters* params, unsigned long long calleeAddress);
