#pragma once
# define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "../fileStructs.h"

unsigned char isPEX64(const wchar_t* filePath, unsigned char* isX64);

unsigned long long getPEImageBase32(HANDLE file);

unsigned long long getPEImageBase64(HANDLE file);

int getCodeSectionHeaders32(HANDLE file, struct FileSection* buffer, int bufferLen);

int getCodeSectionHeaders64(HANDLE file, struct FileSection* buffer, int bufferLen);

unsigned char getDataSectionHeader32(HANDLE file, IMAGE_SECTION_HEADER* result);

unsigned char getDataSectionHeader64(HANDLE file, IMAGE_SECTION_HEADER* result);

unsigned char getPESymbolByValue32(HANDLE file, DWORD value, char* buffer);

unsigned char getPESymbolByValue64(HANDLE file, DWORD value, char* buffer);

int getAllPEImports32(HANDLE file, struct ImportedFunction* buffer, int bufferLen);

unsigned long long rvaToFileOffset(HANDLE file, unsigned long long rva);