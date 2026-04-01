#pragma once
#include "decompilationStructs.h"

unsigned char decompileOperation(struct DecompilationParameters* params, enum Register targetReg, unsigned char getAssignment, struct JdcStr* result);