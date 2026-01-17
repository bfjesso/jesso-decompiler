#include "jdcStr.h"

unsigned char wrapJdcStrInParentheses(struct JdcStr* jdcStr)
{
	int len = (int)strlen(jdcStr->buffer);

	for (int i = len; i > 0; i--)
	{
		jdcStr->buffer[i] = jdcStr->buffer[i - 1];
	}

	jdcStr->buffer[0] = '(';
	return strcatJdc(jdcStr, ")");
}

unsigned char strcpyJdc(struct JdcStr* jdcStr, const char* src)
{
	if (jdcStr && jdcStr->buffer)
	{
		if (strlen(src) >= jdcStr->bufferSize)
		{
			if (resizeJdcStr(jdcStr))
			{
				return strcpyJdc(jdcStr, src);
			}
			else
			{
				return 0;
			}
		}

		if (strcpy_s(jdcStr->buffer, jdcStr->bufferSize, src) != 0)
		{
			return 0;
		}

		return 1;
	}

	return 0;
}

unsigned char strcatJdc(struct JdcStr* jdcStr, const char* src)
{
	if (jdcStr && jdcStr->buffer)
	{
		if ((strlen(src) + strlen(jdcStr->buffer)) >= jdcStr->bufferSize)
		{
			if (resizeJdcStr(jdcStr))
			{
				return strcatJdc(jdcStr, src);
			}
			else
			{
				return 0;
			}
		}

		if (strcat_s(jdcStr->buffer, jdcStr->bufferSize, src) != 0)
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
	return sprintfJdcArgs(jdcStr, cat, format, args);
}

static unsigned char sprintfJdcArgs(struct JdcStr* jdcStr, unsigned char cat, const char* format, va_list args)
{
	if (jdcStr && jdcStr->buffer)
	{
		if (cat)
		{
			int ogLen = strlen(jdcStr->buffer);
			int result = vsnprintf(jdcStr->buffer + ogLen, jdcStr->bufferSize - ogLen, format, args);
			if (result < 0)
			{
				return 0;
			}
			else if (result >= jdcStr->bufferSize - ogLen)
			{
				if (resizeJdcStr(jdcStr))
				{
					memset(jdcStr->buffer + ogLen, 0, jdcStr->bufferSize - ogLen);
					return sprintfJdcArgs(jdcStr, 1, format, args);
				}
				else
				{
					return 0;
				}
			}
			else
			{
				return 1;
			}
		}
		else
		{
			int result = vsnprintf(jdcStr->buffer, jdcStr->bufferSize, format, args);
			if (result < 0)
			{
				return 0;
			}
			else if (result >= jdcStr->bufferSize)
			{
				if (resizeJdcStr(jdcStr))
				{
					memset(jdcStr->buffer, 0, jdcStr->bufferSize);
					return sprintfJdcArgs(jdcStr, 0, format, args);
				}
				else
				{
					return 0;
				}
			}
			else
			{
				return 1;
			}
		}

		return 1;
	}

	return 0;
}

struct JdcStr initializeJdcStr()
{
	struct JdcStr result = { 0 };

	result.buffer = (char*)calloc(255, sizeof(char));
	if (result.buffer)
	{
		result.bufferSize = 255;
	}

	return result;
}

unsigned char freeJdcStr(struct JdcStr* jdcStr)
{
	if (jdcStr && jdcStr->buffer)
	{
		free(jdcStr->buffer);
		return 1;
	}

	return 0;
}

static unsigned char resizeJdcStr(struct JdcStr* jdcStr)
{
	if (jdcStr && jdcStr->buffer)
	{
		char* newBuffer = (char*)realloc(jdcStr->buffer, jdcStr->bufferSize + 255);
		if (newBuffer)
		{
			jdcStr->buffer = newBuffer;
			jdcStr->bufferSize += 255;
			return 1;
		}

		return 0;
	}

	return 0;
}