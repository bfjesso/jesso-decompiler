#include <elf.h>

unsigned int getSectionBytesByName64(char* filePath, char* name, char** bytesBufferRef);

unsigned int getSectionBytesByName32(char* filePath, char* name, char** bytesBufferRef);
