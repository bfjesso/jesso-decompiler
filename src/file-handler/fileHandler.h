#pragma once
#include "../jdcTypes.h"
#include "../fileStructs.h"
#include <wchar.h>

FILE* openFile(const wchar_t* filePath);

enum JdcStatus demangleCppSymbol(char* mangledStr, char* buffer, int bufferLen);

#ifdef __cplusplus
extern "C"
{
#endif

	enum JdcStatus identifyFileFormat(const wchar_t* filePath, enum FileFormat* fileFormat);

	const char* fileFormatToStr(enum FileFormat fileFormat);

	const char* fileSectionTypeToStr(enum FileSectionType fileSectionType);

	enum JdcStatus isFile64Bit(const wchar_t* filePath, enum FileFormat fileFormat, bool* is64BitRef);

	enum JdcStatus getFileImageBase(const wchar_t* filePath, enum FileFormat fileFormat, bool is64Bit, unsigned long long* imageBaseRef);

	enum JdcStatus getFileEntryPoint(const wchar_t* filePath, enum FileFormat fileFormat, bool is64Bit, unsigned long long* entryPointRef);

	enum JdcStatus getNumOfSections(const wchar_t* filePath, enum FileFormat fileFormat, bool is64Bit, int* numOfSectionsRef);

	enum JdcStatus getAllFileSectionHeaders(const wchar_t* filePath, enum FileFormat fileFormat, bool is64Bit, struct FileSection* buffer, int bufferLen);

	enum JdcStatus getNumOfFileBytes(const wchar_t* filePath, unsigned int* numOfBytesRef);

	enum JdcStatus readFileBytes(const wchar_t* filePath, unsigned char* buffer, unsigned int bufferSize);

	enum JdcStatus getSymbolByValue(const wchar_t* filePath, enum FileFormat fileFormat, bool is64Bit, unsigned int value, struct JdcStr* result);

	enum JdcStatus getNumOfImports(const wchar_t* filePath, enum FileFormat fileFormat, bool is64Bit, int* numOfImportsRef, int* numOfLibrariesRef);

	enum JdcStatus getAllImports(const wchar_t* filePath, enum FileFormat fileFormat, bool is64Bit, struct ImportedFunction* importsBuffer, int importsBufferLen, struct JdcStr* libraryNamesBuffer, int libraryNamesBufferLen);

	enum JdcStatus generateFileHeadersInfoStr(const wchar_t* filePath, enum FileFormat fileFormat, struct JdcStr* result);

	unsigned long long rvaToFileOffset(struct FileSection* sections, int numOfSections, unsigned long long rva, struct FileSection** section);

#ifdef __cplusplus
}
#endif