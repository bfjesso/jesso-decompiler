#pragma once
#include "decompilationStructs.h"

enum JdcStatus getAllDirectJmps(struct DecompilationParameters* params);

static enum JdcStatus handleDirectJmpsResize(struct DecompilationParameters* params);

enum JdcStatus decompileDirectJmps(struct DecompilationParameters* params, int instructionIndex, bool* isInUnreachableStateRef, struct JdcStr* result);

bool checkForDirectJmpDst(struct DecompilationParameters* params, int instructionIndex);
