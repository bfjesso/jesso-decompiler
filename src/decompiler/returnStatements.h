#pragma once
#include "decompilationStructs.h"

bool checkForReturnStatement(struct DecompilationParameters* params, int instructionIndex);

bool doesInstructionLeadStraightToReturn(struct DecompilationParameters* params, int startInstructionIndex); // checks if the function leads to a return without doing anything in between

enum JdcStatus decompileReturnStatement(struct DecompilationParameters* params, int instructionIndex, bool* isInUnreachableStateRef, struct JdcStr* result);
