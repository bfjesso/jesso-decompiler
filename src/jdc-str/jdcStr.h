#pragma once
#include "../jdcTypes.h"
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct JdcStr
{
	char* buffer;
	int32_t bufferSize;
};

#ifdef __cplusplus
extern "C"
{
#endif

	enum JdcStatus strcpyJdc(struct JdcStr* jdcStr, const char* src);

	enum JdcStatus sprintfJdc(struct JdcStr* jdcStr, bool cat, const char* format, ...);
	
	struct JdcStr initializeJdcStr();

	struct JdcStr initializeJdcStrWithVal(const char* initStr);

	struct JdcStr initializeJdcStrWithSize(int32_t size);

	enum JdcStatus freeJdcStr(struct JdcStr* jdcStr);

#ifdef __cplusplus
}
#endif

enum JdcStatus wrapJdcStrInParentheses(struct JdcStr* jdcStr);

enum JdcStatus replaceJdc(struct JdcStr* jdcStr, const char* oldStr, const char* newStr);

enum JdcStatus strcatJdc(struct JdcStr* jdcStr, const char* src);

enum JdcStatus strcatStartJdc(struct JdcStr* jdcStr, const char* src);

enum JdcStatus sprintfJdcArgs(struct JdcStr* jdcStr, bool cat, const char* format, va_list args);

struct JdcStr copyJdcStr(struct JdcStr* strToCpy);

static enum JdcStatus resizeJdcStr(struct JdcStr* jdcStr, int32_t newSize);