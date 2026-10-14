#pragma once
#include "../decompiler/decompilationStructs.h"

#define CURRENT_SAVE_FILE_VERSION 1

#ifdef __cplusplus
extern "C"
{
#endif

	enum JdcStatus saveToFile(const wchar_t* filePath, struct Function* functions, int32_t numOfFunctions);

#ifdef __cplusplus
}
#endif

static enum JdcStatus writeStackVar(FILE* file, const struct StackVariable* stackVar);

static enum JdcStatus writeReturnedVar(FILE* file, const struct ReturnedVariable* returnedVar);

static enum JdcStatus writeRegVar(FILE* file, const struct RegisterVariable* regVar);

static enum JdcStatus writeCondition(FILE* file, const struct Condition* cond);

static enum JdcStatus writeDirectJmp(FILE* file, const struct DirectJmp* directJmp);

static enum JdcStatus writeAssociatedInstructions(FILE* file, const struct AssociatedInstructions* ai);