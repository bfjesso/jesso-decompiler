#include "disassemblyUtils.h"

unsigned long long getUIntFromBytes(unsigned char** bytesPtr, unsigned char resultSize)
{
	unsigned long long result = 0;

	for (int i = 0; i < resultSize; i++)
	{
		result += ((unsigned long long)(*bytesPtr)[0] << (8 * i));
		(*bytesPtr)++;
	}

	return result;
}