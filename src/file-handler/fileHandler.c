#include "fileHandler.h"

#ifdef _WIN32
#include "../pe-handler/peHandler.h"
#endif

#ifdef linux
#include "../elf-handler/elfHandler.h"
#endif

unsigned char isFile64Bit(const wchar_t* filePath, unsigned char* isX64)
{
#ifdef _WIN32
	HANDLE file = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (!file || file == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	return isPEX64(file, isX64);
#endif

#ifdef linux
	char filePathChar[255] = { 0 };
	wcstombs(filePathChar, filePath, 254);
	return isELFX64(filePathChar, isX64);
#endif
}

unsigned long long getFileImageBase(const wchar_t* filePath, unsigned char is64Bit)
{
#ifdef _WIN32
	HANDLE file = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (!file || file == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	
	return getPEImageBase(file, is64Bit);
#endif

#ifdef linux
	// not done
	return 0;
#endif
}

unsigned long long getFileEntryPoint(const wchar_t* filePath, unsigned char is64Bit)
{
#ifdef _WIN32
	HANDLE file = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (!file || file == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	return getPEEntryPoint(file, is64Bit);
#endif

#ifdef linux
	char filePathChar[255] = { 0 };
	wcstombs(filePathChar, filePath, 254);
	return getELFEntryPoint(filePathChar, is64Bit);
#endif
}

int getNumOfSections(const wchar_t* filePath, unsigned char is64Bit)
{
#ifdef _WIN32
	HANDLE file = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (!file || file == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	return getNumOfPESections(file, is64Bit);
#endif

#ifdef linux
	char filePathChar[255] = { 0 };
	wcstombs(filePathChar, filePath, 254);
	return getNumOfELFSections(filePathChar, is64Bit);
#endif
}

int getAllFileSectionHeaders(const wchar_t* filePath, unsigned char is64Bit, struct FileSection* buffer, int bufferLen)
{
#ifdef _WIN32
	HANDLE file = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (!file || file == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	return getAllPESectionHeaders(file, is64Bit, buffer, bufferLen);
#endif

#ifdef linux
	char filePathChar[255] = { 0 };
	wcstombs(filePathChar, filePath, 254);
	return getAllELFSectionHeaders(filePathChar, is64Bit, buffer, bufferLen);
#endif
}

unsigned int getNumOfFileBytes(const wchar_t* filePath)
{
#ifdef _WIN32
	HANDLE file = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (!file || file == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	unsigned int result = GetFileSize(file, NULL);
	if (result == INVALID_FILE_SIZE) 
	{
		return 0;
	}

	return result;
#endif

#ifdef linux
	char filePathChar[255] = { 0 };
	wcstombs(filePathChar, filePath, 254);

	FILE* file = fopen(filePathChar, "r");
	if (file)
	{
		fseek(file, 0, SEEK_END);
		return ftell(file);
	}

	return 0;
#endif
}

unsigned char readFileBytes(const wchar_t* filePath, unsigned char* buffer, unsigned int bufferSize)
{
#ifdef _WIN32
	HANDLE file = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (!file || file == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	if (SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		CloseHandle(file);
		return 0;
	}

	BOOL result = ReadFile(file, buffer, bufferSize, 0, 0);
	CloseHandle(file);
	return result;
#endif

#ifdef linux
	char filePathChar[255] = { 0 };
	wcstombs(filePathChar, filePath, 254);

	FILE* file = fopen(filePathChar, "r");
	if (file)
	{
		fseek(file, 0, SEEK_SET);
		fread(buffer, 1, bufferSize, file);
		return 1;
	}

	return 0;
#endif
}

unsigned char getSymbolByValue(const wchar_t* filePath, unsigned char is64Bit, unsigned int value, struct JdcStr* result)
{
#ifdef _WIN32
	HANDLE file = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (!file || file == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	return getPESymbolByValue(file, is64Bit, value, result);
#endif

#ifdef linux
	char filePathChar[255] = { 0 };
	wcstombs(filePathChar, filePath, 254);
	return getELFSymbolByValue(filePathChar, is64Bit, value, result);
#endif
}

int getNumOfImports(const wchar_t* filePath, unsigned char is64Bit)
{
#ifdef _WIN32
	HANDLE file = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (!file || file == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	return getNumOfPEImports(file, is64Bit);
#endif

#ifdef linux
	char filePathChar[255] = { 0 };
	wcstombs(filePathChar, filePath, 254);
	return getNumOfELFImports(filePathChar, is64Bit);
#endif
}

int getAllImports(const wchar_t* filePath, unsigned char is64Bit, struct ImportedFunction* buffer, int bufferLen)
{
#ifdef _WIN32
	HANDLE file = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (!file || file == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	
	return getAllPEImports(file, is64Bit, buffer, bufferLen);
#endif

#ifdef linux
	char filePathChar[255] = { 0 };
	wcstombs(filePathChar, filePath, 254);
	return getAllELFImports(filePathChar, is64Bit, buffer, bufferLen);
#endif
}

unsigned char generateFileHeadersInfoStr(const wchar_t* filePath, struct JdcStr* result)
{
#ifdef _WIN32
	HANDLE file = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (!file || file == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	return generatePEHeadersInfoStr(file, result);
#endif

#ifdef linux
	char filePathChar[255] = { 0 };
	wcstombs(filePathChar, filePath, 254);
	return generateELFHeadersInfoStr(filePathChar, result);
#endif
}

unsigned long long rvaToFileOffset(struct FileSection* sections, int numOfSections, unsigned long long rva, struct FileSection** section)
{
	if (section) { *section = 0; }
	
	unsigned long long maybeResult = 0;
	for (int i = 0; i < numOfSections; i++)
	{
		if (rva >= sections[i].rva && rva < sections[i].rva + sections[i].physicalSize)
		{
			if (section) { *section = &sections[i]; }
			return (rva - sections[i].rva) + sections[i].fileOffset;
		}
		else if (rva == sections[i].rva + sections[i].physicalSize)
		{
			maybeResult = (rva - sections[i].rva) + sections[i].fileOffset; // this may be used as a max file offset even though it is not in the section. it isnt returned immediatley because another section could start here
		}
	}

	return maybeResult;
}