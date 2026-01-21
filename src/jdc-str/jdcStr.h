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

	unsigned char strcpyJdc(struct JdcStr* jdcStr, const char* src);

	unsigned char sprintfJdc(struct JdcStr* jdcStr, unsigned char cat, const char* format, ...);
	
	struct JdcStr initializeJdcStr();

	struct JdcStr initializeJdcStrWithVal(const char* initStr);

	unsigned char freeJdcStr(struct JdcStr* jdcStr);

#ifdef __cplusplus
}
#endif

unsigned char wrapJdcStrInParentheses(struct JdcStr* jdcStr);

unsigned char strcatJdc(struct JdcStr* jdcStr, const char* src);

static unsigned char sprintfJdcArgs(struct JdcStr* jdcStr, unsigned char cat, const char* format, va_list args);

struct JdcStr copyJdcStr(struct JdcStr* strToCpy);

static unsigned char resizeJdcStr(struct JdcStr* jdcStr);
