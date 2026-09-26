#pragma once
#include "decompilationStructs.h"

#ifdef __cplusplus
extern "C"
{
#endif

	enum JdcStatus decompileFunction(struct DecompilationParameters* params, struct JdcStr* result, struct JdcStr* statusMessage, int* errorInstructionIndex);

	enum JdcStatus generateFunctionHeader(struct Function* function, struct JdcStr* result);

#ifdef __cplusplus
}
#endif

static enum JdcStatus getAllReturnedVars(struct DecompilationParameters* params);

static enum JdcStatus declareAllLocalVariables(struct DecompilationParameters* params, struct JdcStr* result);
