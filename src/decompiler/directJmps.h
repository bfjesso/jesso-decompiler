#pragma once
#include "decompilationStructs.h"
#include "conditions.h"

enum DirectJmpType
{
	NONE_DJT,
	GO_TO_DJT,
	BREAK_DJT,
	CONTINUE_DJT
};

struct DirectJmp
{
	int jmpIndex;
	int dstIndex;
	enum DirectJmpType type;
};

int getAllDirectJmps(struct DecompilationParameters params, struct Condition* conditions, int numOfCondtions, struct DirectJmp* directJmps, int bufferSize);