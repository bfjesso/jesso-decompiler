#pragma once
#include "decompilationStructs.h"

unsigned char checkForAssignment(struct DecompilationParameters* params, int instructionIndex);

unsigned char decompileAssignments(struct DecompilationParameters* params, int instructionIndex, struct JdcStr* result);
