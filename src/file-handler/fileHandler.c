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
	return isPEX64(filePath, isX64);
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

int getFileCodeSections(const wchar_t* filePath, unsigned char is64Bit, struct FileSection* buffer, int bufferLen)
{
#ifdef _WIN32
	HANDLE file = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (!file || file == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	if(is64Bit)
	{
		return getCodeSectionHeaders64(file, buffer, bufferLen);
	}
	else
	{
		return getCodeSectionHeaders32(file, buffer, bufferLen);
	}
#endif

#ifdef linux
	char filePathChar[255] = { 0 };
	wcstombs(filePathChar, filePath, 254);

	if (is64Bit)
	{ 
		Elf64_Shdr codeSection;
		if (!getSectionHeaderByName64(filePathChar, ".text", &codeSection))
		{
			return 0;
		}

		result->virtualAddress = codeSection.sh_addr;
		result->fileOffset = codeSection.sh_offset;
		result->size = codeSection.sh_size;
	}
	else 
	{ 
		Elf32_Shdr codeSection;
		if (!getSectionHeaderByName32(filePathChar, ".text", &codeSection))
		{
			return 0;
		}

		result->virtualAddress = codeSection.sh_addr;
		result->fileOffset = codeSection.sh_offset;
		result->size = codeSection.sh_size;
	}

	return 1;
#endif
}

unsigned char getFileDataSection(const wchar_t* filePath, unsigned char is64Bit, struct FileSection* result)
{
#ifdef _WIN32
	HANDLE file = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (!file || file == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	IMAGE_SECTION_HEADER dataSection = { 0 };

	if (is64Bit)
	{
		getDataSectionHeader64(file, &dataSection);
	}
	else
	{
		getDataSectionHeader32(file, &dataSection);
	}

	result->virtualAddress = dataSection.VirtualAddress;
	result->fileOffset = dataSection.PointerToRawData;
	result->size = dataSection.SizeOfRawData;

	return 1;
#endif

#ifdef linux
	char filePathChar[255] = { 0 };
	wcstombs(filePathChar, filePath, 254);

	if (is64Bit)
	{
		Elf64_Shdr dataSection;
		if (!getSectionHeaderByName64(filePathChar, ".data", &dataSection))
		{
			return 0;
		}

		result->virtualAddress = dataSection.sh_addr;
		result->fileOffset = dataSection.sh_offset;
		result->size = dataSection.sh_size;
	}
	else
	{
		Elf32_Shdr dataSection;
		if (!getSectionHeaderByName32(filePathChar, ".data", &dataSection))
		{
			return 0;
		}

		result->virtualAddress = dataSection.sh_addr;
		result->fileOffset = dataSection.sh_offset;
		result->size = dataSection.sh_size;
	}

	return 1;
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

	if (SetFilePointer(file, section->fileOffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
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

unsigned char getSymbolByValue(const wchar_t* filePath, unsigned char is64Bit, unsigned int value, char* buffer)
{
#ifdef _WIN32
	HANDLE file = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (!file || file == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	if (is64Bit)
	{
		return getPESymbolByValue64(file, value, buffer);
	}
	else 
	{
		return getPESymbolByValue32(file, value, buffer);
	}
#endif

#ifdef linux
	char filePathChar[255] = { 0 };
	wcstombs(filePathChar, filePath, 254);

	if (is64Bit)
	{
		return getELFSymbolByValue64(filePathChar, value, buffer);
	}
	else
	{
		return getELFSymbolByValue32(filePathChar, value, buffer);
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
		// not implemented ?
		return 0;
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
		return 0;
	}
#endif
}
