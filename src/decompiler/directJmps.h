#pragma once
#include "decompilationStructs.h"

unsigned char getAllDirectJmps(struct DecompilationParameters* params);

static unsigned char handleDirectJmpsResize(struct DecompilationParameters* params);

unsigned char decompileDirectJmps(struct DecompilationParameters* params, int instructionIndex, unsigned char* isInUnreachableStateRef, struct JdcStr* result);

int getDirectJmpDst(struct DecompilationParameters* params, int instructionIndex);
