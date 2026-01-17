#pragma once
#include "../fileStructs.h"

# define WIN32_LEAN_AND_MEAN
#include <windows.h>


unsigned char isPEX64(const wchar_t* filePath, unsigned char* isX64);

unsigned long long getPEImageBase32(HANDLE file);

unsigned long long getPEImageBase64(HANDLE file);

int getCodeSectionHeaders32(HANDLE file, struct FileSection* buffer, int bufferLen);

int getCodeSectionHeaders64(HANDLE file, struct FileSection* buffer, int bufferLen);

int getDataSectionHeaders32(HANDLE file, struct FileSection* buffer, int bufferLen);

int getDataSectionHeaders64(HANDLE file, struct FileSection* buffer, int bufferLen);

unsigned char getPESymbolByValue32(HANDLE file, DWORD value, struct JdcStr* result);

unsigned char getPESymbolByValue64(HANDLE file, DWORD value, struct JdcStr* result);

int getAllPEImports32(HANDLE file, struct ImportedFunction* buffer, int bufferLen);

unsigned long long rvaToFileOffset(HANDLE file, unsigned long long rva);