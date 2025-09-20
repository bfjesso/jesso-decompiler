#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <elf.h>
#include "../fileStructs.h"

unsigned char isELFX64(const char* filePath, unsigned char* isX64);

unsigned char getELFSymbolByValue64(const char* filePath, unsigned long long value, char* nameBuffer);

unsigned char getELFSymbolByValue32(const char* filePath, unsigned long long value, char* nameBuffer);

int getELFCodeSections64(const char* filePath, struct FileSection* buffer, int bufferLen);

int getELFCodeSections32(const char* filePath, struct FileSection* buffer, int bufferLen);

int getELFDataSections64(const char* filePath, struct FileSection* buffer, int bufferLen);

int getELFDataSections32(const char* filePath, struct FileSection* buffer, int bufferLen);

unsigned char getSectionHeaderByName64(const char* filePath, char* name, Elf64_Shdr* result);

unsigned char getSectionHeaderByName32(const char* filePath, char* name, Elf32_Shdr* result);

unsigned char readSectionBytes64(const char* filePath, Elf64_Shdr* section, unsigned char* buffer, unsigned int bufferSize);

unsigned char readSectionBytes32(const char* filePath, Elf32_Shdr* section, unsigned char* buffer, unsigned int bufferSize);

unsigned char getSectionHeaderByType64(const char* filePath, unsigned int type, int index, Elf64_Shdr* result);

unsigned char getAllELFImports64(const char* filePath, struct ImportedFunction* buffer, int bufferLen);
