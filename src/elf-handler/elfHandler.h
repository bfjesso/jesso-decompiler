#include "../fileStructs.h"
#include "elfStructs.h"

char* demangleSymbol(const char* mangledStr, int* status);

unsigned char isELFX64(const char* filePath, unsigned char* isX64);

void readElfEhdr(FILE* file, unsigned char is64Bit, Elf64_Ehdr* result);

void readElfShdr(FILE* file, unsigned char is64Bit, unsigned long long fileOffset, Elf64_Shdr* result);

unsigned long long getELFEntryPoint(const char* filePath, unsigned char is64Bit);

unsigned char getELFSymbolByValue(const char* filePath, unsigned char is64Bit, unsigned long long value, struct JdcStr* result);

int getNumOfELFSections(const char* filePath, unsigned char is64Bit);

unsigned char getAllELFSectionHeaders(const char* filePath, unsigned char is64Bit, struct FileSection* buffer, int bufferLen);

unsigned char getSectionHeaderByName(const char* filePath, unsigned char is64Bit, const char* name, Elf64_Shdr* result);

unsigned char readSectionBytes(const char* filePath, Elf64_Shdr* section, char* buffer, unsigned int bufferSize);

unsigned char getSectionHeaderByType(const char* filePath, unsigned char is64Bit, unsigned int type, int index, Elf64_Shdr* result);

int getNumOfELFImports(const char* filePath, unsigned char is64Bit);

int getAllELFImports(const char* filePath, unsigned char is64Bit, struct ImportedFunction* buffer, int bufferLen);

unsigned char generateELFHeadersInfoStr(const char* filePath, struct JdcStr* result);

static void generateELFHeaderInfoStr(Elf64_Ehdr* ehdr, struct JdcStr* result);
