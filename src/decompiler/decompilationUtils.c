#include "decompilationUtils.h"

void wrapStrInParentheses(char* str)
{
	int len = (int)strlen(str);

	for (int i = len; i > 0; i--)
	{
		str[i] = str[i - 1];
	}

	str[0] = '(';
	strcat(str, ")");
}

unsigned char strcpyJdc(struct JdcStr* jdcStr, const char* src)
{
	if (jdcStr && jdcStr->buffer)
	{
		if (jdcStr->buffer[jdcStr->bufferSize - 1] != 0)
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
		if (jdcStr->buffer[jdcStr->bufferSize - 1] != 0)
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
	if (jdcStr && jdcStr->buffer)
	{
		va_list args;
		va_start(args, format);

		if (jdcStr->buffer[jdcStr->bufferSize - 1] != 0)
		{
			if (resizeJdcStr(jdcStr))
			{
				return sprintfJdc(jdcStr, format, args);
			}
			else 
			{
				return 0;
			}
		}

		if (cat) 
		{
			if (vsnprintf(jdcStr->buffer + strlen(jdcStr->buffer), jdcStr->bufferSize - strlen(jdcStr->buffer), format, args) < 0)
			{
				return 0;
			}
		}
		else 
		{
			if (vsnprintf(jdcStr->buffer, jdcStr->bufferSize, format, args) < 0)
			{
				return 0;
			}
		}

		return 1;
	}

	return 0;
}

unsigned char initializeJdcStr(struct JdcStr* jdcStr, int bufferSize)
{
	if (jdcStr)
	{
		jdcStr->buffer = (char*)calloc(bufferSize, sizeof(char));
		if (jdcStr->buffer)
		{
			jdcStr->bufferSize = bufferSize;
			return 1;
		}

		return 0;
	}

	return 0;
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