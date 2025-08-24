#pragma once
#include "../importedFunctions.h"
#include <wchar.h>

struct FileSection 
{
	unsigned long long virtualAddress;
	unsigned long long fileOffset;
	int size;
};

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned char isFile64Bit(const wchar_t* filePath, unsigned char* isX64);

	unsigned long long getFileImageBase(const wchar_t* filePath, unsigned char is64Bit);

	unsigned char getFileCodeSection(const wchar_t* filePath, unsigned char is64Bit, struct FileSection* result);

	unsigned char getFileDataSection(const wchar_t* filePath, unsigned char is64Bit, struct FileSection* result);

	unsigned char readFileSection(const wchar_t* filePath, struct FileSection* section, unsigned char is64Bit, unsigned char* buffer, unsigned int bufferSize);

	unsigned char getSymbolByValue(const wchar_t* filePath, unsigned char is64Bit, unsigned int value, char* buffer);

	int getAllImports(const wchar_t* filePath, unsigned char is64Bit, struct ImportedFunction* buffer, int bufferLen);

#ifdef __cplusplus
}
#endif