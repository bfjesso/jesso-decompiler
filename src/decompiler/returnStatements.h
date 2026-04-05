#pragma once
#include "decompilationStructs.h"

unsigned char doesInstructionModifyReturnRegister(struct DecompilationParameters* params);

unsigned char checkForReturnStatement(struct DecompilationParameters* params);

unsigned char checkForJumpToReturnStatement(struct DecompilationParameters* params);

unsigned char decompileReturnStatement(struct DecompilationParameters* params, unsigned char* isInUnreachableStateRef, struct JdcStr* result);
