#include "jdcStr.h"

enum JdcStatus wrapJdcStrInParentheses(struct JdcStr* jdcStr)
{
	if (jdcStr && jdcStr->buffer && jdcStr->bufferSize >= 3)
	{
		jdcStr->buffer[jdcStr->bufferSize - 1] = 0;
		int32_t len = (int32_t)strlen(jdcStr->buffer);

		if (len >= jdcStr->bufferSize - 2)
		{
			if (ERROR_JDC == resizeJdcStr(jdcStr, len + 3))
			{
				return ERROR_JDC;
			}
		}

		for (int32_t i = len; i > 0; i--)
		{
			jdcStr->buffer[i] = jdcStr->buffer[i - 1];
		}

		jdcStr->buffer[0] = '(';
		jdcStr->buffer[len + 1] = ')';
		jdcStr->buffer[len + 2] = 0;
		return SUCCESS_JDC;
	}
	
	return ERROR_JDC;
}

enum JdcStatus replaceJdc(struct JdcStr* jdcStr, const char* oldStr, const char* newStr)
{
	if (jdcStr && jdcStr->buffer && oldStr && newStr)
	{
		struct JdcStr result = initializeJdcStr();

		jdcStr->buffer[jdcStr->bufferSize - 1] = 0;
		int32_t jdcStrLen = (int32_t)strlen(jdcStr->buffer);

		int32_t oldLen = (int32_t)strlen(oldStr);
		for (int32_t i = 0; i < jdcStrLen; i++)
		{
			bool foundStr = true;
			for (int32_t j = 0; j < oldLen; j++)
			{
				if (i + j >= jdcStrLen || jdcStr->buffer[i + j] != oldStr[j])
				{
					foundStr = false;
					break;
				}
			}

			if (foundStr)
			{
				strcatJdc(&result, newStr);
				i += oldLen - 1;
			}
			else
			{
				const char c[2] = { jdcStr->buffer[i], 0 };
				strcatJdc(&result, (const char*)(&c));
			}
		}

		strcpyJdc(jdcStr, result.buffer);
		freeJdcStr(&result);
		return SUCCESS_JDC;
	}

	return ERROR_JDC;
}

enum JdcStatus strcpyJdc(struct JdcStr* jdcStr, const char* src)
{
	if (jdcStr && jdcStr->buffer && src)
	{
		int32_t srcLen = (int32_t)strlen(src);
		if (srcLen >= jdcStr->bufferSize)
		{
			if (SUCCESS_JDC == resizeJdcStr(jdcStr, srcLen + 1))
			{
				return strcpyJdc(jdcStr, src);
			}
			else
			{
				return ERROR_JDC;
			}
		}

		memset(jdcStr->buffer, 0, jdcStr->bufferSize);

		return strcpy(jdcStr->buffer, src) == jdcStr->buffer;
	}

	return ERROR_JDC;
}

enum JdcStatus strcatJdc(struct JdcStr* jdcStr, const char* src)
{
	if (jdcStr && jdcStr->buffer && src)
	{
		jdcStr->buffer[jdcStr->bufferSize - 1] = 0;
		int32_t newLen = (int32_t)strlen(src) + (int32_t)strlen(jdcStr->buffer);
		if (newLen >= jdcStr->bufferSize)
		{
			if (SUCCESS_JDC == resizeJdcStr(jdcStr, newLen + 1))
			{
				return strcatJdc(jdcStr, src);
			}
			else
			{
				return ERROR_JDC;
			}
		}

		return strcat(jdcStr->buffer, src) == jdcStr->buffer;
	}

	return ERROR_JDC;
}

enum JdcStatus strcatStartJdc(struct JdcStr* jdcStr, const char* src)
{
	if (jdcStr && jdcStr->buffer && src)
	{
		jdcStr->buffer[jdcStr->bufferSize - 1] = 0;
		int32_t currentLen = (int32_t)strlen(jdcStr->buffer);
		int32_t srcLen = (int32_t)strlen(src);
		if (currentLen + srcLen >= jdcStr->bufferSize)
		{
			if (SUCCESS_JDC == resizeJdcStr(jdcStr, currentLen + srcLen + 1))
			{
				return strcatStartJdc(jdcStr, src);
			}
			else
			{
				return ERROR_JDC;
			}
		}

		char* tmp = (char*)calloc(currentLen + 1, 1);
		if (tmp) 
		{
			strcpy(tmp, jdcStr->buffer);
			strcpy(jdcStr->buffer, src);
			strcat(jdcStr->buffer, tmp);
			free(tmp);
			return SUCCESS_JDC;
		}

		return ERROR_JDC;
	}

	return ERROR_JDC;
}

enum JdcStatus sprintfJdc(struct JdcStr* jdcStr, bool cat, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	enum JdcStatus result = sprintfJdcArgs(jdcStr, cat, format, args);
	va_end(args);
	return result;
}

enum JdcStatus sprintfJdcArgs(struct JdcStr* jdcStr, bool cat, const char* format, va_list args)
{
	if (jdcStr && jdcStr->buffer)
	{
		va_list copy;
		va_copy(copy, args);
		if (cat)
		{
			jdcStr->buffer[jdcStr->bufferSize - 1] = 0;
			int32_t ogLen = (int32_t)strlen(jdcStr->buffer);
			int32_t result = vsnprintf(jdcStr->buffer + ogLen, jdcStr->bufferSize - ogLen, format, args);
			if (result < 0)
			{
				va_end(copy);
				return ERROR_JDC;
			}
			else if (result >= jdcStr->bufferSize - ogLen)
			{
				if (resizeJdcStr(jdcStr, ogLen + result + 1))
				{
					memset(jdcStr->buffer + ogLen, 0, jdcStr->bufferSize - ogLen);
					enum JdcStatus statusResult = sprintfJdcArgs(jdcStr, true, format, copy);
					va_end(copy);
					return statusResult;
				}

				va_end(copy);
				return ERROR_JDC;
			}
		}
		else
		{
			memset(jdcStr->buffer, 0, jdcStr->bufferSize);

			int32_t result = vsnprintf(jdcStr->buffer, jdcStr->bufferSize, format, args);
			if (result < 0)
			{
				va_end(copy);
				return ERROR_JDC;
			}
			else if (result >= jdcStr->bufferSize)
			{
				if (resizeJdcStr(jdcStr, result + 1))
				{
					memset(jdcStr->buffer, 0, jdcStr->bufferSize);
					enum JdcStatus statusResult = sprintfJdcArgs(jdcStr, 0, format, copy);
					va_end(copy);
					return statusResult;
				}

				va_end(copy);
				return ERROR_JDC;
			}
		}

		va_end(copy);
		return SUCCESS_JDC;
	}

	return ERROR_JDC;
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
			strcpyJdc(&result, strToCpy->buffer);
		}
	}

	return result;
}

struct JdcStr initializeJdcStr()
{
	return initializeJdcStrWithSize(25);
}

struct JdcStr initializeJdcStrWithSize(int32_t size)
{
	struct JdcStr result = { 0 };

	if (size > 0) 
	{
		result.buffer = (char*)calloc(size, sizeof(char));
		if (result.buffer)
		{
			result.bufferSize = size;
		}
	}
	
	return result;
}

struct JdcStr initializeJdcStrWithVal(const char* initStr)
{
	struct JdcStr result = { 0 };

	if(initStr)
	{
		int32_t len = (int32_t)strlen(initStr);
		result.buffer = (char*)calloc(len + 1, sizeof(char));
		if (result.buffer)
		{
			result.bufferSize = len + 1;
			strcpy(result.buffer, initStr);
		}
	}

	return result;
}

enum JdcStatus freeJdcStr(struct JdcStr* jdcStr)
{
	if (jdcStr && jdcStr->buffer)
	{
		memset(jdcStr->buffer, 0, jdcStr->bufferSize);
		free(jdcStr->buffer);
		jdcStr->buffer = 0;
		jdcStr->bufferSize = 0;
		return SUCCESS_JDC;
	}

	return ERROR_JDC;
}

static enum JdcStatus resizeJdcStr(struct JdcStr* jdcStr, int32_t newSize)
{
	if (jdcStr && jdcStr->buffer && newSize > 0)
	{
		char* newBuffer = (char*)realloc(jdcStr->buffer, newSize);
		if (newBuffer)
		{
			jdcStr->buffer = newBuffer;
			jdcStr->bufferSize = newSize;

			jdcStr->buffer[jdcStr->bufferSize - 1] = 0;
			int32_t len = (int32_t)strlen(jdcStr->buffer);
			memset(jdcStr->buffer + len, 0, jdcStr->bufferSize - len);
			return SUCCESS_JDC;
		}
	}

	return ERROR_JDC;
}
