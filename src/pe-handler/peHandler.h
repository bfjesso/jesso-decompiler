#pragma once
#include "../fileStructs.h"

# define WIN32_LEAN_AND_MEAN
#include <windows.h>

// this exists because there are 32 and 64 bit versions of the ImageNtHeaders/OptionalHeader
struct IMAGE_NT_HEADERS_INFO
{
	LONG e_lfanew; // this is really in the DOS header
	DWORD Signature;
	IMAGE_FILE_HEADER FileHeader;
	DWORD ImageBase;
	DWORD AddressOfEntryPoint;
	DWORD NumberOfRvaAndSizes;
	IMAGE_DATA_DIRECTORY DataDirectory[16];
};

unsigned char isPEX64(HANDLE file, unsigned char* isX64);

unsigned char getImageNTHeadersInfo(HANDLE file, unsigned char is64Bit, struct IMAGE_NT_HEADERS_INFO* result);

unsigned long long getPEImageBase(HANDLE file, unsigned char is64Bit);

unsigned long long getPEEntryPoint(HANDLE file, unsigned char is64Bit);

int getNumOfPESections(HANDLE file, unsigned char is64Bit);

unsigned char getAllPESectionHeaders(HANDLE file, unsigned char is64Bit, struct FileSection* buffer, int bufferLen);

unsigned char getPESymbolByValue(HANDLE file, unsigned char is64Bit, DWORD value, struct JdcStr* result);

int getNumOfPEImports(HANDLE file, unsigned char is64Bit);

int getAllPEImports(HANDLE file, unsigned char is64Bit, struct ImportedFunction* buffer, int bufferLen);

unsigned demangleCppSymbol(char* mangledStr, char* buffer, int bufferLen);

DWORD rvaToFileOffsetPE(HANDLE file, unsigned char is64Bit, DWORD rva);

unsigned char generatePEHeadersInfoStr(HANDLE file, struct JdcStr* result);

static void generateDOSHeaderInfoStr(IMAGE_DOS_HEADER* dosHeader, struct JdcStr* result);

static void generateFileHeaderInfoStr(IMAGE_FILE_HEADER* fileHeader, struct JdcStr* result);

static void generateOptionalHeaderInfoStr(IMAGE_OPTIONAL_HEADER* optionalHeader, struct JdcStr* result);