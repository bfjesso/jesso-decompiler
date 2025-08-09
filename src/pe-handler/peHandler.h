#pragma once
# define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct PEImport 
{
	char name[50];
	unsigned long long address;
};

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned char readCodeSection(HANDLE file, unsigned char is64Bit, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* codeSection, uintptr_t* imageBase);
	unsigned char readDataSection(HANDLE file, unsigned char is64Bit, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* dataSection, uintptr_t* imageBase);
	unsigned char getSymbolByValue(HANDLE file, unsigned char is64Bit, DWORD value, char* buffer);
	int getAllImports(HANDLE file, unsigned char is64Bit, struct PEImport* buffer, int bufferLen);

#ifdef __cplusplus
}
#endif

static unsigned char readCodeSection32(HANDLE file, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* codeSection, uintptr_t* imageBase);

static unsigned char readCodeSection64(HANDLE file, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* codeSection, uintptr_t* imageBase);

static unsigned char readDataSection32(HANDLE file, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* dataSection, uintptr_t* imageBase);

static unsigned char readDataSection64(HANDLE file, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* dataSection, uintptr_t* imageBase);

static unsigned char getSymbolByValue32(HANDLE file, DWORD value, char* buffer);

static unsigned char getSymbolByValue64(HANDLE file, DWORD value, char* buffer);

static int getAllImports32(HANDLE file, struct PEImport* buffer, int bufferLen);

static unsigned long long rvaToFileOffset(HANDLE file, unsigned long long rva);