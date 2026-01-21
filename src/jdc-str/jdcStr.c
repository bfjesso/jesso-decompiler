#include "jdcStr.h"

unsigned char wrapJdcStrInParentheses(struct JdcStr* jdcStr)
{
	int len = (int)strlen(jdcStr->buffer);

	if (len >= jdcStr->bufferSize - 2) 
	{
		if (resizeJdcStr(jdcStr, len + 3))
		{
			return wrapJdcStrInParentheses(jdcStr);
		}

		return 0;
	}

	for (int i = len; i > 0; i--)
	{
		jdcStr->buffer[i] = jdcStr->buffer[i - 1];
	}

	jdcStr->buffer[0] = '(';
	jdcStr->buffer[len + 1] = ')';
	jdcStr->buffer[len + 2] = 0;
	return 1;
}

unsigned char strcpyJdc(struct JdcStr* jdcStr, const char* src)
{
	if (jdcStr && jdcStr->buffer && src)
	{
		int srcLen = strlen(src);
		if (srcLen >= jdcStr->bufferSize)
		{
			if (resizeJdcStr(jdcStr, srcLen + 1))
			{
				return strcpyJdc(jdcStr, src);
			}
			else
			{
				return 0;
			}
		}

		memset(jdcStr->buffer, 0, jdcStr->bufferSize);

		if (!strcpy(jdcStr->buffer, src))
		{
			return 0;
		}

		return 1;
	}

	return 0;
}

unsigned char strcatJdc(struct JdcStr* jdcStr, const char* src)
{
	if (jdcStr && jdcStr->buffer && src)
	{
		int newLen = strlen(src) + strlen(jdcStr->buffer);
		if (newLen >= jdcStr->bufferSize)
		{
			if (resizeJdcStr(jdcStr, newLen + 1))
			{
				return strcatJdc(jdcStr, src);
			}
			else
			{
				return 0;
			}
		}

		if (!strcat(jdcStr->buffer, src))
		{
			return 0;
		}

		return 1;
	}

	return 0;
}

unsigned char sprintfJdc(struct JdcStr* jdcStr, unsigned char cat, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	unsigned char result = sprintfJdcArgs(jdcStr, cat, format, args);
	va_end(args);
	return result;
}

static unsigned char sprintfJdcArgs(struct JdcStr* jdcStr, unsigned char cat, const char* format, va_list args)
{
	if (jdcStr && jdcStr->buffer)
	{
		va_list copy;
		va_copy(copy, args);
		if (cat)
		{
			int ogLen = strlen(jdcStr->buffer);
			int result = vsnprintf(jdcStr->buffer + ogLen, jdcStr->bufferSize - ogLen, format, args);
			if (result < 0)
			{
				va_end(copy);
				return 0;
			}
			else if (result >= jdcStr->bufferSize - ogLen)
			{
				if (resizeJdcStr(jdcStr, ogLen + result + 1))
				{
					memset(jdcStr->buffer + ogLen, 0, jdcStr->bufferSize - ogLen);
					unsigned char result = sprintfJdcArgs(jdcStr, 1, format, copy);
					va_end(copy);
					return result;
				}

				va_end(copy);
				return 0;
			}

			va_end(copy);
			return 1;
		}
		else
		{
			memset(jdcStr->buffer, 0, jdcStr->bufferSize);

			int result = vsnprintf(jdcStr->buffer, jdcStr->bufferSize, format, args);
			if (result < 0)
			{
				va_end(copy);
				return 0;
			}
			else if (result >= jdcStr->bufferSize)
			{
				if (resizeJdcStr(jdcStr, result + 1))
				{
					memset(jdcStr->buffer, 0, jdcStr->bufferSize);
					unsigned char result = sprintfJdcArgs(jdcStr, 0, format, copy);
					va_end(copy);
					return result;
				}

				va_end(copy);
				return 0;
			}

			va_end(copy);
			return 1;
		}

		va_end(copy);
		return 1;
	}

	return 0;
}

struct JdcStr copyJdcStr(struct JdcStr* strToCpy)
{
	struct JdcStr result = { 0 };
	
	if (strToCpy && strToCpy->buffer)
	{
		result.buffer = (char*)calloc(strToCpy->bufferSize, sizeof(char));
		if (result.buffer)
		{
			result.bufferSize = strToCpy->bufferSize;
		}

		strcpyJdc(&result, strToCpy->buffer);
	}

	return result;
}

struct JdcStr initializeJdcStr()
{
	struct JdcStr result = { 0 };

	result.buffer = (char*)calloc(10, sizeof(char));
	if (result.buffer)
	{
		result.bufferSize = 10;
	}

	return result;
}

struct JdcStr initializeJdcStrWithVal(const char* initStr)
{
	struct JdcStr result = { 0 };

	if(initStr)
	{
		int len = strlen(initStr);
		result.buffer = (char*)calloc(len + 1, sizeof(char));
		if (result.buffer)
		{
			result.bufferSize = len + 1;
			strcpy(result.buffer, initStr);
		}
	}

	return result;
}

unsigned char freeJdcStr(struct JdcStr* jdcStr)
{
	if (jdcStr && jdcStr->buffer)
	{
		memset(jdcStr->buffer, 0, jdcStr->bufferSize);
		free(jdcStr->buffer);
		jdcStr->buffer = 0;
		jdcStr->bufferSize = 0;
		return 1;
	}

	return 0;
}

static unsigned char resizeJdcStr(struct JdcStr* jdcStr, int newSize)
{
	if (jdcStr && jdcStr->buffer)
	{
		char* newBuffer = (char*)realloc(jdcStr->buffer, newSize);
		if (newBuffer)
		{
			if (newBuffer != jdcStr->buffer) 
			{
				memset(jdcStr->buffer, 0, jdcStr->bufferSize);
			}
			
			jdcStr->buffer = newBuffer;
			jdcStr->bufferSize = newSize;

			int len = strlen(jdcStr->buffer);
			memset(jdcStr->buffer + len, 0, jdcStr->bufferSize - len);
			return 1;
		}

		return 0;
	}

	return 0;
}
