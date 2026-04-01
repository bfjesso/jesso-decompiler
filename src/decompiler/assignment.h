#pragma once
#include "decompilationStructs.h"

unsigned char checkForAssignment(struct DecompilationParameters* params);

unsigned char decompileAssignments(struct DecompilationParameters* params, struct JdcStr* result);
