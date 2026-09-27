#pragma once
#include "decompilationStructs.h"

bool checkForAnyAssignments(struct DecompilationParameters* params, int32_t instructionIndex);

bool doesInstructionAssignToOperand(struct DecompilationParameters* params, int32_t instructionIndex, uint8_t operandNum);

struct RegisterVariable* doesInstructionAssignToRegVar(struct DecompilationParameters* params, int32_t instructionIndex, enum Register reg);