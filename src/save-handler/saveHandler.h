#pragma once
#include "../decompiler/decompilationStructs.h"

#define CURRENT_SAVE_FILE_VERSION 1
#define JDC_SIG 0x4A444300 // JDC

#define JDC_SIG_FILE_OFFSET 0 // sig is uint32_t
#define SAVE_VERSION_FILE_OFFSET JDC_SIG_FILE_OFFSET + sizeof(uint32_t) // version is uint32_t
#define FILE_PATH_LEN_FILE_OFFSET SAVE_VERSION_FILE_OFFSET + sizeof(uint32_t) // path len is size_t
#define FILE_PATH_FILE_OFFSET FILE_PATH_LEN_FILE_OFFSET + sizeof(size_t)

#ifdef __cplusplus
extern "C"
{
#endif

	enum JdcStatus saveToFile(const wchar_t* filePath, struct Function* functions, int32_t numOfFunctions);

#ifdef __cplusplus
}
#endif

static enum JdcStatus writeFunction(FILE* file, const struct Function* func);

static enum JdcStatus writeStackVar(FILE* file, const struct StackVariable* stackVar);

static enum JdcStatus writeReturnedVar(FILE* file, const struct ReturnedVariable* returnedVar);

static enum JdcStatus writeRegVar(FILE* file, const struct RegisterVariable* regVar);

static enum JdcStatus writeCondition(FILE* file, const struct Condition* cond);

static enum JdcStatus writeDirectJmp(FILE* file, const struct DirectJmp* directJmp);

static enum JdcStatus writeAssociatedInstructions(FILE* file, const struct AssociatedInstructions* ai);