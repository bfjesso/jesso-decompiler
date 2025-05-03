#include <elf.h>

unsigned char isFileX64(char* filePath, unsigned char* isX64);

unsigned char getSymbolNameByValue64(char* filePath, unsigned long long value, char* nameBuffer);

unsigned char getSymbolNameByValue32(char* filePath, unsigned long long value, char* nameBuffer);

unsigned int getSectionBytesByName64(char* filePath, char* name, char** bytesBufferRef, unsigned long long* startAddress);

unsigned int getSectionBytesByName32(char* filePath, char* name, char** bytesBufferRef, unsigned long long* startAddress);
