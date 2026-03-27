#pragma once
#include "decompilationStructs.h"
#include "conditions.h"

int getAllDirectJmps(struct DecompilationParameters params, struct Condition* conditions, int numOfCondtions, struct DirectJmp** directJmpsRef, int directJmpsBufferSize);
