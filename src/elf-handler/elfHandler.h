#include "../jdcTypes.h"
#include "../fileStructs.h"
#include "elfStructs.h"

enum JdcStatus isFileELF(const wchar_t* filePath, bool* isELFRef);

enum JdcStatus isELFX64(const wchar_t* filePath, bool* is64BitRef);

static enum JdcStatus readElfEhdr(FILE* file, bool is64Bit, Elf64_Ehdr* result);

static enum JdcStatus readElfShdr(FILE* file, bool is64Bit, unsigned long long fileOffset, Elf64_Shdr* result);

enum JdcStatus getELFEntryPoint(const wchar_t* filePath, bool is64Bit, unsigned long long* entryPointRef);

enum JdcStatus getELFSymbolByValue(const wchar_t* filePath, bool is64Bit, unsigned long long value, struct JdcStr* result);

enum JdcStatus getNumOfELFSections(const wchar_t* filePath, bool is64Bit, int* numOfSectionsRef);

enum JdcStatus getAllELFSectionHeaders(const wchar_t* filePath, bool is64Bit, struct FileSection* buffer, int bufferLen);

enum JdcStatus getSectionHeaderByName(const wchar_t* filePath, bool is64Bit, const char* name, Elf64_Shdr* result);

enum JdcStatus readSectionBytes(const wchar_t* filePath, Elf64_Shdr* section, char* buffer, unsigned int bufferSize);

enum JdcStatus getSectionHeaderByType(const wchar_t* filePath, bool is64Bit, unsigned int type, int index, Elf64_Shdr* result);

enum JdcStatus getNumOfELFImports(const wchar_t* filePath, bool is64Bit, int* numOfImportsRef, int* numOfLibrariesRef);

enum JdcStatus getAllELFImports(const wchar_t* filePath, bool is64Bit, struct ImportedFunction* importsBuffer, int importsBufferLen, struct JdcStr* libraryNamesBuffer, int libraryNamesBufferLen);

enum JdcStatus generateELFHeadersInfoStr(const wchar_t* filePath, struct JdcStr* result);

static void generateELFHeaderInfoStr(Elf64_Ehdr* ehdr, struct JdcStr* result);
