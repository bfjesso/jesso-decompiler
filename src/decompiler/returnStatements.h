#pragma once
#include "decompilationStructs.h"

unsigned char checkForReturnStatement(struct DecompilationParameters* params, int instructionIndex);

unsigned char doesInstructionLeadStraightToReturn(struct DecompilationParameters* params, int startInstructionIndex); // checks if the function leads to a return without doing anything in between

unsigned char decompileReturnStatement(struct DecompilationParameters* params, int instructionIndex, unsigned char* isInUnreachableStateRef, struct JdcStr* result);
