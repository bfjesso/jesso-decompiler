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
	
	if (is64Bit) 
	{
		return getPEImageBase64(file);
	}
	else
	{
		return getPEImageBase32(file);
	}
#endif

#ifdef linux
	// not done
	return 0;
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

	if (is64Bit)
	{
		return getNumOfPESections64(file);
	}
	else
	{
		return getNumOfPESections64(file);
	}
#endif

#ifdef linux
	char filePathChar[255] = { 0 };
	wcstombs(filePathChar, filePath, 254);

	if (is64Bit)
	{
		return getNumOfELFSections64(filePathChar);
	}
	else
	{
		return getNumOfELFSections32(filePathChar);
	}
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

	if(is64Bit)
	{
		return getAllPESectionHeaders64(file, buffer, bufferLen);
	}
	else
	{
		return getAllPESectionHeaders32(file, buffer, bufferLen);
	}
#endif

#ifdef linux
	char filePathChar[255] = { 0 };
	wcstombs(filePathChar, filePath, 254);

	if (is64Bit)
	{ 
		return getAllELFSectionHeaders64(filePathChar, buffer, bufferLen);
	}
	else
	{ 
		return getAllELFSectionHeaders32(filePathChar, buffer, bufferLen);
	}
#endif
}

unsigned char readFileSection(const wchar_t* filePath, struct FileSection* section, unsigned char is64Bit, unsigned char* buffer, unsigned int bufferSize)
{
#ifdef _WIN32
	HANDLE file = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (!file || file == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	if (SetFilePointer(file, (LONG)(section->fileOffset), NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
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
		fseek(file, section->fileOffset, SEEK_SET);
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

	if (is64Bit)
	{
		return getPESymbolByValue64(file, value, result);
	}
	else 
	{
		return getPESymbolByValue32(file, value, result);
	}
#endif

#ifdef linux
	char filePathChar[255] = { 0 };
	wcstombs(filePathChar, filePath, 254);

	if (is64Bit)
	{
		return getELFSymbolByValue64(filePathChar, value, result);
	}
	else
	{
		return getELFSymbolByValue32(filePathChar, value, result);
	}
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

	if (is64Bit)
	{
		return getNumOfPEImports64(file);
	}
	else
	{
		return getNumOfPEImports32(file);
	}
#endif

#ifdef linux
	char filePathChar[255] = { 0 };
	wcstombs(filePathChar, filePath, 254);

	if (is64Bit)
	{
		return getNumOfELFImports64(filePathChar);
	}
	else
	{
		return getNumOfELFImports32(filePathChar);
	}
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
	
	if (is64Bit) 
	{
		return getAllPEImports64(file, buffer, bufferLen);
	}
	else 
	{
		return getAllPEImports32(file, buffer, bufferLen);
	}
#endif

#ifdef linux
	char filePathChar[255] = { 0 };
	wcstombs(filePathChar, filePath, 254);

	if (is64Bit)
	{
		return getAllELFImports64(filePathChar, buffer, bufferLen);
	}
	else
	{
		return getAllELFImports32(filePathChar, buffer, bufferLen);
	}
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

	return 0;
#endif
}