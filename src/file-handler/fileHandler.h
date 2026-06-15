#pragma once
#include "../fileStructs.h"
#include <wchar.h>

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned char isFile64Bit(const wchar_t* filePath, unsigned char* isX64);

	unsigned long long getFileImageBase(const wchar_t* filePath, unsigned char is64Bit);

	unsigned long long getFileEntryPoint(const wchar_t* filePath, unsigned char is64Bit);

	int getNumOfSections(const wchar_t* filePath, unsigned char is64Bit);

	int getAllFileSectionHeaders(const wchar_t* filePath, unsigned char is64Bit, struct FileSection* buffer, int bufferLen);

	unsigned int getNumOfFileBytes(const wchar_t* filePath);

	unsigned char readFileBytes(const wchar_t* filePath, unsigned char* buffer, unsigned int bufferSize);

	unsigned char getSymbolByValue(const wchar_t* filePath, unsigned char is64Bit, unsigned int value, struct JdcStr* result);

	int getNumOfImports(const wchar_t* filePath, unsigned char is64Bit);

	int getAllImports(const wchar_t* filePath, unsigned char is64Bit, struct ImportedFunction* buffer, int bufferLen);

	unsigned char generateFileHeadersInfoStr(const wchar_t* filePath, struct JdcStr* result);

	unsigned long long rvaToFileOffset(struct FileSection* sections, int numOfSections, unsigned long long rva, struct FileSection** section);

#ifdef __cplusplus
}
#endif