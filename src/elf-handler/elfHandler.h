#include "../fileStructs.h"
#include <elf.h>

char* demangleSymbol(const char* mangledStr, int* status);

unsigned char isELFX64(const char* filePath, unsigned char* isX64);

unsigned char getELFSymbolByValue64(const char* filePath, unsigned long long value, struct JdcStr* result);

unsigned char getELFSymbolByValue32(const char* filePath, unsigned long long value, struct JdcStr* result);

int getNumOfELFSections64(const char* filePath);

int getNumOfELFSections32(const char* filePath);

int getNumOfELFSections64(const char* filePath);

int getNumOfELFSections32(const char* filePath);

unsigned char getAllELFSectionHeaders64(const char* filePath, struct FileSection* buffer, int bufferLen);

unsigned char getAllELFSectionHeaders32(const char* filePath, struct FileSection* buffer, int bufferLen);

unsigned char getSectionHeaderByName64(const char* filePath, char* name, Elf64_Shdr* result);

unsigned char getSectionHeaderByName32(const char* filePath, char* name, Elf32_Shdr* result);

unsigned char readSectionBytes64(const char* filePath, Elf64_Shdr* section, unsigned char* buffer, unsigned int bufferSize);

unsigned char readSectionBytes32(const char* filePath, Elf32_Shdr* section, unsigned char* buffer, unsigned int bufferSize);

unsigned char getSectionHeaderByType64(const char* filePath, unsigned int type, int index, Elf64_Shdr* result);

unsigned char getSectionHeaderByType32(const char* filePath, unsigned int type, int index, Elf32_Shdr* result);

int getNumOfELFImports64(const char* filePath);

int getNumOfELFImports32(const char* filePath);

int getAllELFImports64(const char* filePath, struct ImportedFunction* buffer, int bufferLen);

int getAllELFImports32(const char* filePath, struct ImportedFunction* buffer, int bufferLen);

unsigned char generateELFHeadersInfoStr(const char* filePath, struct JdcStr* result);

static void generateELFHeaderInfoStr(Elf64_Ehdr* ehdr, struct JdcStr* result);
