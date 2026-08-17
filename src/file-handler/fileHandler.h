#pragma once
#include "../fileStructs.h"
#include <wchar.h>

FILE* openFile(const wchar_t* filePath);

unsigned char demangleCppSymbol(char* mangledStr, char* buffer, int bufferLen);

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned char identifyFileFormat(const wchar_t* filePath, enum FileFormat* fileFormat);

	const char* fileFormatToStr(enum FileFormat fileFormat);

	const char* fileSectionTypeToStr(enum FileSectionType fileSectionType);

	unsigned char isFile64Bit(const wchar_t* filePath, enum FileFormat fileFormat, unsigned char* isX64);

	unsigned long long getFileImageBase(const wchar_t* filePath, enum FileFormat fileFormat, unsigned char is64Bit);

	unsigned long long getFileEntryPoint(const wchar_t* filePath, enum FileFormat fileFormat, unsigned char is64Bit);

	int getNumOfSections(const wchar_t* filePath, enum FileFormat fileFormat, unsigned char is64Bit);

	int getAllFileSectionHeaders(const wchar_t* filePath, enum FileFormat fileFormat, unsigned char is64Bit, struct FileSection* buffer, int bufferLen);

	unsigned int getNumOfFileBytes(const wchar_t* filePath);

	unsigned char readFileBytes(const wchar_t* filePath, unsigned char* buffer, unsigned int bufferSize);

	unsigned char getSymbolByValue(const wchar_t* filePath, enum FileFormat fileFormat, unsigned char is64Bit, unsigned int value, struct JdcStr* result);

	int getNumOfImports(const wchar_t* filePath, enum FileFormat fileFormat, unsigned char is64Bit);

	int getAllImports(const wchar_t* filePath, enum FileFormat fileFormat, unsigned char is64Bit, struct ImportedFunction* buffer, int bufferLen);

	unsigned char generateFileHeadersInfoStr(const wchar_t* filePath, enum FileFormat fileFormat, struct JdcStr* result);

	unsigned long long rvaToFileOffset(struct FileSection* sections, int numOfSections, unsigned long long rva, struct FileSection** section);

#ifdef __cplusplus
}
#endif