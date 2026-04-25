#pragma once
#include "../fileStructs.h"

# define WIN32_LEAN_AND_MEAN
#include <windows.h>


unsigned char isPEX64(HANDLE file, unsigned char* isX64);

unsigned long long getPEImageBase32(HANDLE file);

unsigned long long getPEImageBase64(HANDLE file);

int getNumOfPESections32(HANDLE file);

int getNumOfPESections64(HANDLE file);

unsigned char getAllPESectionHeaders32(HANDLE file, struct FileSection* buffer, int bufferLen);

unsigned char getAllPESectionHeaders64(HANDLE file, struct FileSection* buffer, int bufferLen);

unsigned char getPESymbolByValue32(HANDLE file, DWORD value, struct JdcStr* result);

unsigned char getPESymbolByValue64(HANDLE file, DWORD value, struct JdcStr* result);

int getNumOfPEImports32(HANDLE file);

int getNumOfPEImports64(HANDLE file);

int getAllPEImports32(HANDLE file, struct ImportedFunction* buffer, int bufferLen);

int getAllPEImports64(HANDLE file, struct ImportedFunction* buffer, int bufferLen);

DWORD rvaToFileOffset32(HANDLE file, DWORD rva);

DWORD rvaToFileOffset64(HANDLE file, DWORD rva);

unsigned char generatePEHeadersInfoStr(HANDLE file, struct JdcStr* result);

static void generateDOSHeaderInfoStr(IMAGE_DOS_HEADER* dosHeader, struct JdcStr* result);

static void generateFileHeaderInfoStr(IMAGE_FILE_HEADER* fileHeader, struct JdcStr* result);

static void generateOptionalHeaderInfoStr(IMAGE_OPTIONAL_HEADER* optionalHeader, struct JdcStr* result);