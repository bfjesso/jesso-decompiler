#pragma once
#include "decompilationStructs.h"

bool checkForKnownFunctionCall(struct DecompilationParameters* params, int32_t instructionIndex, struct Function** calleeRef);

enum JdcStatus decompileKnownFunctionCall(struct DecompilationParameters* params, int32_t callInstructionIndex, struct Function* callee, struct JdcStr* result);

bool checkForUnknownFunctionCall(struct DecompilationParameters* params, int32_t instructionIndex);

enum JdcStatus decompileUnknownFunctionCall(struct DecompilationParameters* params, int32_t callInstructionIndex, struct JdcStr* result);

int32_t getImportIndexByAddress(struct DecompilationParameters* params, uint64_t calleeAddress);