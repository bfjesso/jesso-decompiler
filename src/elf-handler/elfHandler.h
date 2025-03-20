#include <elf.h>

unsigned char isFileX64(char* filePath, unsigned char* isX64);

unsigned int getSectionBytesByName64(char* filePath, char* name, char** bytesBufferRef, unsigned long long* startAddress);

unsigned int getSectionBytesByName32(char* filePath, char* name, char** bytesBufferRef, unsigned long long* startAddress);
