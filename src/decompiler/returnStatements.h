#pragma once
#include "decompilationStructs.h"

unsigned char doesInstructionModifyReturnRegister(struct DecompilationParameters params);

unsigned char checkForReturnStatement(int startInstructionIndex, struct DisassembledInstruction* instructions, int numOfInstructions);

unsigned char decompileReturnStatement(struct DecompilationParameters params, struct JdcStr* result);
