#pragma once
#include "decompilationStructs.h"

unsigned char checkForKnownFunctionCall(struct DecompilationParameters* params, int instructionIndex, struct Function** calleeRef);

enum JdcStatus decompileKnownFunctionCall(struct DecompilationParameters* params, int callInstructionIndex, struct Function* callee, struct JdcStr* result);

unsigned char checkForUnknownFunctionCall(struct DecompilationParameters* params, int instructionIndex);

enum JdcStatus decompileUnknownFunctionCall(struct DecompilationParameters* params, int callInstructionIndex, struct JdcStr* result);

int getImportIndexByAddress(struct DecompilationParameters* params, unsigned long long calleeAddress);