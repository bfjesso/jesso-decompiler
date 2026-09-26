#pragma once
#include "decompilationStructs.h"

enum JdcStatus getAllDirectJmps(struct DecompilationParameters* params);

static enum JdcStatus handleDirectJmpsResize(struct DecompilationParameters* params);

enum JdcStatus decompileDirectJmps(struct DecompilationParameters* params, int instructionIndex, unsigned char* isInUnreachableStateRef, struct JdcStr* result);

unsigned char checkForDirectJmpDst(struct DecompilationParameters* params, int instructionIndex);
