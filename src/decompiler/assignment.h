#pragma once
#include "decompilationStructs.h"

unsigned char checkForAnyAssignments(struct DecompilationParameters* params, int instructionIndex);

unsigned char doesInstructionAssignToOperand(struct DecompilationParameters* params, int instructionIndex, unsigned char operandNum);

struct RegisterVariable* doesInstructionAssignToRegVar(struct DecompilationParameters* params, int instructionIndex, enum Register reg);