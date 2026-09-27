#pragma once
#include "decompilationStructs.h"

bool checkForAnyAssignments(struct DecompilationParameters* params, int instructionIndex);

bool doesInstructionAssignToOperand(struct DecompilationParameters* params, int instructionIndex, unsigned char operandNum);

struct RegisterVariable* doesInstructionAssignToRegVar(struct DecompilationParameters* params, int instructionIndex, enum Register reg);