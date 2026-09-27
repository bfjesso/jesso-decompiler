#pragma once
#include "../jdcTypes.h"
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

enum JdcStatus isFilePE(const wchar_t* filePath, bool* isPERef);

enum JdcStatus isPEX64(const wchar_t* filePath, bool* is64BitRef);

static enum JdcStatus getImageNTHeadersInfo(FILE* file, bool is64Bit, struct IMAGE_NT_HEADERS_INFO* result);

enum JdcStatus getPEImageBase(const wchar_t* filePath, bool is64Bit, unsigned long long* imageBaseRef);

enum JdcStatus getPEEntryPoint(const wchar_t* filePath, bool is64Bit, unsigned long long* entryPointRef);

enum JdcStatus getNumOfPESections(const wchar_t* filePath, bool is64Bit, int* numOfSectionsRef);

enum JdcStatus getAllPESectionHeaders(const wchar_t* filePath, bool is64Bit, struct FileSection* buffer, int bufferLen);

enum JdcStatus getPESymbolByValue(const wchar_t* filePath, bool is64Bit, DWORD value, struct JdcStr* result);

enum JdcStatus getNumOfPEImports(const wchar_t* filePath, bool is64Bit, int* numOfImportsRef, int* numOfLibrariesRef);

enum JdcStatus getAllPEImports(const wchar_t* filePath, bool is64Bit, struct ImportedFunction* importsBuffer, int importsBufferLen, struct JdcStr* libraryNamesBuffer, int libraryNamesBufferLen);

static enum JdcStatus rvaToFileOffsetPE(FILE* file, bool is64Bit, DWORD rva, DWORD* fileOffsetRef);

enum JdcStatus generatePEHeadersInfoStr(const wchar_t* filePath, struct JdcStr* result);

static void generateDOSHeaderInfoStr(IMAGE_DOS_HEADER* dosHeader, struct JdcStr* result);

static void generateFileHeaderInfoStr(IMAGE_FILE_HEADER* fileHeader, struct JdcStr* result);

static void generateOptionalHeaderInfoStr(IMAGE_OPTIONAL_HEADER64* optionalHeader, struct JdcStr* result);