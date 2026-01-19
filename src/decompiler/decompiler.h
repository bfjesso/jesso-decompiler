#pragma once
#include "decompilationUtils.h"

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned char decompileFunction(struct DecompilationParameters params, struct JdcStr* result);

#ifdef __cplusplus
}
#endif

static unsigned char getAllLocalVars(struct DecompilationParameters params);

static unsigned char getAllReturnedVars(struct DecompilationParameters params);

static void addIndents(struct JdcStr* result, int numOfIndents);

static unsigned char generateFunctionHeader(struct Function* function, struct JdcStr* result);

static unsigned char declareAllLocalVariables(struct Function* function, struct JdcStr* result);