#pragma once
#include "../fileStructs.h"
#include <wchar.h>

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned char isFile64Bit(const wchar_t* filePath, unsigned char* isX64);

	unsigned long long getFileImageBase(const wchar_t* filePath, unsigned char is64Bit);

	int getFileCodeSections(const wchar_t* filePath, unsigned char is64Bit, struct FileSection* buffer, int bufferLen);

	int getFileDataSections(const wchar_t* filePath, unsigned char is64Bit, struct FileSection* buffer, int bufferLen);

	unsigned char readFileSection(const wchar_t* filePath, struct FileSection* section, unsigned char is64Bit, unsigned char* buffer, unsigned int bufferSize);

	unsigned char getSymbolByValue(const wchar_t* filePath, unsigned char is64Bit, unsigned int value, struct JdcStr* result);

	int getAllImports(const wchar_t* filePath, unsigned char is64Bit, struct ImportedFunction* buffer, int bufferLen);

#ifdef __cplusplus
}
#endif