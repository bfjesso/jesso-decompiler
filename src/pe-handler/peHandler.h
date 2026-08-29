#pragma once
#include "../fileStructs.h"

// dbghelp.h needs windows.h
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifdef linux
#include "windowsStructs.h"
#endif

// this exists because there are 32 and 64 bit versions of the ImageNtHeaders/OptionalHeader
struct IMAGE_NT_HEADERS_INFO
{
	LONG e_lfanew; // this is really in the DOS header
	DWORD Signature;
	IMAGE_FILE_HEADER FileHeader;
	ULONGLONG ImageBase;
	DWORD AddressOfEntryPoint;
	DWORD NumberOfRvaAndSizes;
	IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
};

unsigned char isFilePE(const wchar_t* filePath, unsigned char* isPE);

unsigned char isPEX64(const wchar_t* filePath, unsigned char* isX64);

static unsigned char getImageNTHeadersInfo(FILE* file, unsigned char is64Bit, struct IMAGE_NT_HEADERS_INFO* result);

unsigned long long getPEImageBase(const wchar_t* filePath, unsigned char is64Bit);

unsigned long long getPEEntryPoint(const wchar_t* filePath, unsigned char is64Bit);

int getNumOfPESections(const wchar_t* filePath, unsigned char is64Bit);

unsigned char getAllPESectionHeaders(const wchar_t* filePath, unsigned char is64Bit, struct FileSection* buffer, int bufferLen);

unsigned char getPESymbolByValue(const wchar_t* filePath, unsigned char is64Bit, DWORD value, struct JdcStr* result);

int getNumOfPEImports(const wchar_t* filePath, unsigned char is64Bit, int* numOfLibrariesRef);

int getAllPEImports(const wchar_t* filePath, unsigned char is64Bit, struct ImportedFunction* importsBuffer, int importsBufferLen, struct JdcStr* libraryNamesBuffer, int libraryNamesBufferLen);

static DWORD rvaToFileOffsetPE(FILE* file, unsigned char is64Bit, DWORD rva);

unsigned char generatePEHeadersInfoStr(const wchar_t* filePath, struct JdcStr* result);

static void generateDOSHeaderInfoStr(IMAGE_DOS_HEADER* dosHeader, struct JdcStr* result);

static void generateFileHeaderInfoStr(IMAGE_FILE_HEADER* fileHeader, struct JdcStr* result);

static void generateOptionalHeaderInfoStr(IMAGE_OPTIONAL_HEADER64* optionalHeader, struct JdcStr* result);