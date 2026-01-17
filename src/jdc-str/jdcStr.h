#pragma once

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct JdcStr
{
	char* buffer;
	int bufferSize;
};

#ifdef __cplusplus
extern "C"
{
#endif

	struct JdcStr initializeJdcStr();

	unsigned char freeJdcStr(struct JdcStr* jdcStr);

#ifdef __cplusplus
}
#endif

unsigned char wrapJdcStrInParentheses(struct JdcStr* jdcStr);

unsigned char strcpyJdc(struct JdcStr* jdcStr, const char* src);

unsigned char strcatJdc(struct JdcStr* jdcStr, const char* src);

unsigned char sprintfJdc(struct JdcStr* jdcStr, unsigned char cat, const char* format, ...);

static unsigned char sprintfJdcArgs(struct JdcStr* jdcStr, unsigned char cat, const char* format, va_list args);

static unsigned char resizeJdcStr(struct JdcStr* jdcStr);