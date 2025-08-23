#pragma once
# define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "../importedFunctions.h"

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned char readCodeSection(HANDLE file, unsigned char is64Bit, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* codeSection, uintptr_t* imageBase);
	unsigned char getDataSectionHeader(HANDLE file, unsigned char is64Bit, IMAGE_SECTION_HEADER* result);
	unsigned char readDataSection(HANDLE file, unsigned char* buffer, IMAGE_SECTION_HEADER* dataSection);
	unsigned char getSymbolByValue(HANDLE file, unsigned char is64Bit, DWORD value, char* buffer);
	int getAllImports(HANDLE file, unsigned char is64Bit, struct ImportedFunction* buffer, int bufferLen);

#ifdef __cplusplus
}
#endif

static unsigned char readCodeSection32(HANDLE file, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* codeSection, uintptr_t* imageBase);

static unsigned char readCodeSection64(HANDLE file, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* codeSection, uintptr_t* imageBase);

static unsigned char getDataSectionHeader32(HANDLE file, IMAGE_SECTION_HEADER* result);

static unsigned char getDataSectionHeader64(HANDLE file, IMAGE_SECTION_HEADER* result);

static unsigned char getSymbolByValue32(HANDLE file, DWORD value, char* buffer);

static unsigned char getSymbolByValue64(HANDLE file, DWORD value, char* buffer);

static int getAllImports32(HANDLE file, struct ImportedFunction* buffer, int bufferLen);

static unsigned long long rvaToFileOffset(HANDLE file, unsigned long long rva);