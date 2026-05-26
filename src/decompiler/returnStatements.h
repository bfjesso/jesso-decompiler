#pragma once
#include "decompilationStructs.h"

unsigned char checkForReturnStatement(struct DecompilationParameters* params);

unsigned char checkForJumpToReturnStatement(struct DecompilationParameters* params);

unsigned char doesInstructionLeadStraightToReturn(struct DecompilationParameters* params); // checks if the function leads to a return without doing anything in between

unsigned char decompileReturnStatement(struct DecompilationParameters* params, unsigned char* isInUnreachableStateRef, struct JdcStr* result);
