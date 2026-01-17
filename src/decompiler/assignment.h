#pragma once
#include "decompilationUtils.h"

unsigned char checkForAssignment(struct DisassembledInstruction* instruction);

unsigned char decompileAssignment(struct DecompilationParameters params, struct JdcStr* result);