#pragma once
# define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned char readCodeSection(const wchar_t* filePath, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* codeSection, uintptr_t* imageBase, unsigned char* is64Bit);
	unsigned char readDataSection(const wchar_t* filePath, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* dataSection, uintptr_t* imageBase);

#ifdef __cplusplus
}
#endif

static unsigned char readCodeSection32(HANDLE file, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* codeSection, uintptr_t* imageBase);

static unsigned char readCodeSection64(HANDLE file, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* codeSection, uintptr_t* imageBase);

static unsigned char readDataSection32(HANDLE file, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* dataSection, uintptr_t* imageBase);

static unsigned char readDataSection64(HANDLE file, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* dataSection, uintptr_t* imageBase);