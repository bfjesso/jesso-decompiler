#pragma once
#include "decompilationStructs.h"

unsigned char checkForAssignment(struct DecompilationParameters params);

unsigned char decompileAssignment(struct DecompilationParameters params, struct JdcStr* result);
