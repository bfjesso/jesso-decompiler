#pragma once
#include "../jdcTypes.h"
#include "../fileStructs.h"
#include <wchar.h>

FILE* openFile(const wchar_t* filePath);

int32_t seek64(FILE* file, uint64_t fileOffset);

enum JdcStatus demangleCppSymbol(char* mangledStr, char* buffer, size_t bufferLen);

#ifdef __cplusplus
extern "C"
{
#endif

	enum JdcStatus identifyFileFormat(const wchar_t* filePath, enum FileFormat* fileFormat);

	const char* fileFormatToStr(enum FileFormat fileFormat);

	const char* fileSectionTypeToStr(enum FileSectionType fileSectionType);

	enum JdcStatus isFile64Bit(const wchar_t* filePath, enum FileFormat fileFormat, bool* is64BitRef);

	enum JdcStatus getFileImageBase(const wchar_t* filePath, enum FileFormat fileFormat, bool is64Bit, uint64_t* imageBaseRef);

	enum JdcStatus getFileEntryPoint(const wchar_t* filePath, enum FileFormat fileFormat, bool is64Bit, uint64_t* entryPointRef);

	enum JdcStatus getNumOfSections(const wchar_t* filePath, enum FileFormat fileFormat, bool is64Bit, int32_t* numOfSectionsRef);

	enum JdcStatus getAllFileSectionHeaders(const wchar_t* filePath, enum FileFormat fileFormat, bool is64Bit, struct FileSection* buffer, int32_t bufferLen);

	enum JdcStatus getNumOfFileBytes(const wchar_t* filePath, uint64_t* numOfBytesRef);

	enum JdcStatus readFileBytes(const wchar_t* filePath, uint8_t* buffer, uint64_t bufferSize);

	enum JdcStatus getSymbolByValue(const wchar_t* filePath, enum FileFormat fileFormat, bool is64Bit, uint32_t value, struct JdcStr* result);

	enum JdcStatus getNumOfImports(const wchar_t* filePath, enum FileFormat fileFormat, bool is64Bit, int32_t* numOfImportsRef, int32_t* numOfLibrariesRef);

	enum JdcStatus getAllImports(const wchar_t* filePath, enum FileFormat fileFormat, bool is64Bit, struct ImportedFunction* importsBuffer, int32_t importsBufferLen, struct JdcStr* libraryNamesBuffer, int32_t libraryNamesBufferLen);

	enum JdcStatus generateFileHeadersInfoStr(const wchar_t* filePath, enum FileFormat fileFormat, struct JdcStr* result);

	uint64_t rvaToFileOffset(struct FileSection* sections, int32_t numOfSections, uint64_t rva, struct FileSection** section);

#ifdef __cplusplus
}
#endif