#pragma once
#include "decompilationStructs.h"
#include "conditions.h"

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned char decompileFunction(struct DecompilationParameters* params, struct JdcStr* result);

#ifdef __cplusplus
}
#endif

static unsigned char getAllReturnedVars(struct DecompilationParameters* params);

static unsigned char getAllRegVars(struct DecompilationParameters* params);

static unsigned char generateFunctionHeader(struct Function* function, struct JdcStr* result);

static unsigned char declareAllLocalVariables(struct DecompilationParameters* params, struct JdcStr* result);
