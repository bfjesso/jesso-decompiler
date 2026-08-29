#include "../fileStructs.h"
#include "elfStructs.h"

unsigned char isFileELF(const wchar_t* filePath, unsigned char* isELF);

unsigned char isELFX64(const wchar_t* filePath, unsigned char* isX64);

void readElfEhdr(FILE* file, unsigned char is64Bit, Elf64_Ehdr* result);

void readElfShdr(FILE* file, unsigned char is64Bit, unsigned long long fileOffset, Elf64_Shdr* result);

unsigned long long getELFEntryPoint(const wchar_t* filePath, unsigned char is64Bit);

unsigned char getELFSymbolByValue(const wchar_t* filePath, unsigned char is64Bit, unsigned long long value, struct JdcStr* result);

int getNumOfELFSections(const wchar_t* filePath, unsigned char is64Bit);

unsigned char getAllELFSectionHeaders(const wchar_t* filePath, unsigned char is64Bit, struct FileSection* buffer, int bufferLen);

unsigned char getSectionHeaderByName(const wchar_t* filePath, unsigned char is64Bit, const char* name, Elf64_Shdr* result);

unsigned char readSectionBytes(const wchar_t* filePath, Elf64_Shdr* section, char* buffer, unsigned int bufferSize);

unsigned char getSectionHeaderByType(const wchar_t* filePath, unsigned char is64Bit, unsigned int type, int index, Elf64_Shdr* result);

int getNumOfELFImports(const wchar_t* filePath, unsigned char is64Bit, int* numOfLibrariesRef);

int getAllELFImports(const wchar_t* filePath, unsigned char is64Bit, struct ImportedFunction* importsBuffer, int importsBufferLen, struct JdcStr* libraryNamesBuffer, int libraryNamesBufferLen);

unsigned char generateELFHeadersInfoStr(const wchar_t* filePath, struct JdcStr* result);

static void generateELFHeaderInfoStr(Elf64_Ehdr* ehdr, struct JdcStr* result);
