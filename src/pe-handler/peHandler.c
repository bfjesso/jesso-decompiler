#include "peHandler.h"

unsigned char readCodeSection(HANDLE file, unsigned char is64Bit, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* codeSection, uintptr_t* imageBase)
{
	if (!file || file == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	if (is64Bit)
	{
		return readCodeSection64(file, buffer, bufferSize, codeSection, imageBase);
	}
	else
	{
		return readCodeSection32(file, buffer, bufferSize, codeSection, imageBase);
	}
}

unsigned char readDataSection(HANDLE file, unsigned char is64Bit, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* dataSection, uintptr_t* imageBase)
{
	if (!file || file == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	if (is64Bit)
	{
		return readDataSection64(file, buffer, bufferSize, dataSection, imageBase);
	}
	else
	{
		return readDataSection32(file, buffer, bufferSize, dataSection, imageBase);
	}
}

unsigned char getSymbolByValue(HANDLE file, unsigned char is64Bit, DWORD value, char* buffer)
{
	if (!file || file == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	if (is64Bit)
	{
		return getSymbolByValue64(file, value, buffer);
	}
	else
	{
		return getSymbolByValue32(file, value, buffer);
	}
}

int getAllImports(HANDLE file, unsigned char is64Bit, struct ImportedFunction* buffer, int bufferLen)
{
	if (!file || file == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	if (is64Bit)
	{
		// return readCodeSection64(file, buffer, bufferSize, codeSection, imageBase);
		return 0;
	}
	else
	{
		return getAllImports32(file, buffer, bufferLen);
	}
}

static unsigned char readCodeSection32(HANDLE file, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* codeSection, uintptr_t* imageBase) 
{
	IMAGE_DOS_HEADER dosHeader = { 0 };
	if (SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &dosHeader, sizeof(dosHeader), 0, 0)) { return 0; }

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) { return 0; }
	
	IMAGE_NT_HEADERS32 imageNtHeaders = { 0 };
	LONG imageNtHeadersAddress = dosHeader.e_lfanew;
	if (SetFilePointer(file, imageNtHeadersAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &imageNtHeaders, sizeof(imageNtHeaders), 0, 0)) { return 0; }

	*imageBase = imageNtHeaders.OptionalHeader.ImageBase;

	for (int i = 0; i < imageNtHeaders.FileHeader.NumberOfSections; i++)
	{
		IMAGE_SECTION_HEADER sectionHeader = { 0 };
		LONG sectionAddress = (sizeof(IMAGE_SECTION_HEADER) * i) + imageNtHeadersAddress + sizeof(imageNtHeaders.Signature) + sizeof(imageNtHeaders.FileHeader) + imageNtHeaders.FileHeader.SizeOfOptionalHeader;
		if (SetFilePointer(file, sectionAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
		if (!ReadFile(file, &sectionHeader, sizeof(sectionHeader), 0, 0)) { return 0; }

		if (sectionHeader.VirtualAddress == imageNtHeaders.OptionalHeader.BaseOfCode)
		{
			if (SetFilePointer(file, sectionHeader.PointerToRawData, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }

			*codeSection = sectionHeader;

			return ReadFile(file, buffer, bufferSize, 0, 0);
		}
	}
	
	return 0;
}

static unsigned char readCodeSection64(HANDLE file, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* codeSection, uintptr_t* imageBase)
{
	IMAGE_DOS_HEADER dosHeader = { 0 };
	if (SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &dosHeader, sizeof(dosHeader), 0, 0)) { return 0; }

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) { return 0; }

	IMAGE_NT_HEADERS64 imageNtHeaders = { 0 };
	LONG imageNtHeadersAddress = dosHeader.e_lfanew;
	if (SetFilePointer(file, imageNtHeadersAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &imageNtHeaders, sizeof(imageNtHeaders), 0, 0)) { return 0; }

	*imageBase = imageNtHeaders.OptionalHeader.ImageBase;

	for (int i = 0; i < imageNtHeaders.FileHeader.NumberOfSections; i++)
	{
		IMAGE_SECTION_HEADER sectionHeader = { 0 };
		LONG sectionAddress = (sizeof(IMAGE_SECTION_HEADER) * i) + imageNtHeadersAddress + sizeof(imageNtHeaders.Signature) + sizeof(imageNtHeaders.FileHeader) + imageNtHeaders.FileHeader.SizeOfOptionalHeader;
		if (SetFilePointer(file, sectionAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
		if (!ReadFile(file, &sectionHeader, sizeof(sectionHeader), 0, 0)) { return 0; }

		if (sectionHeader.VirtualAddress == imageNtHeaders.OptionalHeader.BaseOfCode)
		{
			if (SetFilePointer(file, sectionHeader.PointerToRawData, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }

			*codeSection = sectionHeader;

			return ReadFile(file, buffer, bufferSize, 0, 0);
		}
	}
	
	return 0;
}

static unsigned char readDataSection32(HANDLE file, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* dataSection, uintptr_t* imageBase)
{
	IMAGE_DOS_HEADER dosHeader = { 0 };
	if (SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &dosHeader, sizeof(dosHeader), 0, 0)) { return 0; }

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) { return 0; }

	IMAGE_NT_HEADERS32 imageNtHeaders = { 0 };
	LONG imageNtHeadersAddress = dosHeader.e_lfanew;
	if (SetFilePointer(file, imageNtHeadersAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &imageNtHeaders, sizeof(imageNtHeaders), 0, 0)) { return 0; }

	*imageBase = imageNtHeaders.OptionalHeader.ImageBase;

	for (int i = 0; i < imageNtHeaders.FileHeader.NumberOfSections; i++)
	{
		IMAGE_SECTION_HEADER sectionHeader = { 0 };
		LONG sectionAddress = (sizeof(IMAGE_SECTION_HEADER) * i) + imageNtHeadersAddress + sizeof(imageNtHeaders.Signature) + sizeof(imageNtHeaders.FileHeader) + imageNtHeaders.FileHeader.SizeOfOptionalHeader;
		if (SetFilePointer(file, sectionAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
		if (!ReadFile(file, &sectionHeader, sizeof(sectionHeader), 0, 0)) { return 0; }

		if (sectionHeader.VirtualAddress == imageNtHeaders.OptionalHeader.BaseOfData)
		{
			if (SetFilePointer(file, sectionHeader.PointerToRawData, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }

			*dataSection = sectionHeader;

			return ReadFile(file, buffer, bufferSize, 0, 0);
		}
	}

	return 0;
}

static unsigned char readDataSection64(HANDLE file, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* dataSection, uintptr_t* imageBase)
{
	IMAGE_DOS_HEADER dosHeader = { 0 };
	if (SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &dosHeader, sizeof(dosHeader), 0, 0)) { return 0; }

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) { return 0; }

	IMAGE_NT_HEADERS64 imageNtHeaders = { 0 };
	LONG imageNtHeadersAddress = dosHeader.e_lfanew;
	if (SetFilePointer(file, imageNtHeadersAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &imageNtHeaders, sizeof(imageNtHeaders), 0, 0)) { return 0; }

	*imageBase = imageNtHeaders.OptionalHeader.ImageBase;

	for (int i = 0; i < imageNtHeaders.FileHeader.NumberOfSections; i++)
	{
		IMAGE_SECTION_HEADER sectionHeader = { 0 };
		LONG sectionAddress = (sizeof(IMAGE_SECTION_HEADER) * i) + imageNtHeadersAddress + sizeof(imageNtHeaders.Signature) + sizeof(imageNtHeaders.FileHeader) + imageNtHeaders.FileHeader.SizeOfOptionalHeader;
		if (SetFilePointer(file, sectionAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
		if (!ReadFile(file, &sectionHeader, sizeof(sectionHeader), 0, 0)) { return 0; }

		if (sectionHeader.Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA)
		{
			if (SetFilePointer(file, sectionHeader.PointerToRawData, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }

			*dataSection = sectionHeader;

			BOOL result = ReadFile(file, buffer, bufferSize, 0, 0);
			
			return result;
		}
	}

	return 0;
}

static unsigned char getSymbolByValue32(HANDLE file, DWORD value, char* buffer)
{
	IMAGE_DOS_HEADER dosHeader = { 0 };
	if (SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &dosHeader, sizeof(dosHeader), 0, 0)) { return 0; }

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) { return 0; }

	IMAGE_NT_HEADERS32 imageNtHeaders = { 0 };
	LONG imageNtHeadersAddress = dosHeader.e_lfanew;
	if (SetFilePointer(file, imageNtHeadersAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &imageNtHeaders, sizeof(imageNtHeaders), 0, 0)) { return 0; }

	for (DWORD i = 0; i < imageNtHeaders.FileHeader.NumberOfSymbols; i++)
	{
		IMAGE_SYMBOL symbol = { 0 };
		LONG symbolAddress = (sizeof(IMAGE_SYMBOL) * i) + imageNtHeaders.FileHeader.PointerToSymbolTable;
		if (SetFilePointer(file, symbolAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
		if (!ReadFile(file, &symbol, sizeof(symbol), 0, 0)) { return 0; }

		if (symbol.Value == value)
		{
			if (!symbol.N.Name.Short) 
			{
				IMAGE_SYMBOL symbol = { 0 };
				LONG strTableAddress = imageNtHeaders.FileHeader.PointerToSymbolTable + imageNtHeaders.FileHeader.NumberOfSymbols;
				LONG nameAddress = strTableAddress + symbol.N.Name.Long;
				if (SetFilePointer(file, nameAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }

				char tmpBuffer[50] = { 0 };
				if (!ReadFile(file, tmpBuffer, 50, 0, 0)) { return 0; }
				tmpBuffer[49] = 0;

				strcpy(buffer, tmpBuffer);
			}
			else 
			{
				strcpy(buffer, symbol.N.ShortName);
			}

			return 1;
		}
	}
	
	return 0;
}

static unsigned char getSymbolByValue64(HANDLE file, DWORD value, char* buffer)
{
	IMAGE_DOS_HEADER dosHeader = { 0 };
	if (SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &dosHeader, sizeof(dosHeader), 0, 0)) { return 0; }

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) { return 0; }

	IMAGE_NT_HEADERS64 imageNtHeaders = { 0 };
	LONG imageNtHeadersAddress = dosHeader.e_lfanew;
	if (SetFilePointer(file, imageNtHeadersAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &imageNtHeaders, sizeof(imageNtHeaders), 0, 0)) { return 0; }

	for (DWORD i = 0; i < imageNtHeaders.FileHeader.NumberOfSymbols; i++)
	{
		IMAGE_SYMBOL symbol = { 0 };
		LONG symbolAddress = (sizeof(IMAGE_SYMBOL) * i) + imageNtHeaders.FileHeader.PointerToSymbolTable;
		if (SetFilePointer(file, symbolAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
		if (!ReadFile(file, &symbol, sizeof(symbol), 0, 0)) { return 0; }

		if (symbol.Value == value)
		{
			if (!symbol.N.Name.Short)
			{
				IMAGE_SYMBOL symbol = { 0 };
				LONG strTableAddress = imageNtHeaders.FileHeader.PointerToSymbolTable + imageNtHeaders.FileHeader.NumberOfSymbols;
				LONG nameAddress = strTableAddress + symbol.N.Name.Long;
				if (SetFilePointer(file, nameAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }

				char tmpBuffer[50] = { 0 };
				if (!ReadFile(file, tmpBuffer, 50, 0, 0)) { return 0; }
				tmpBuffer[49] = 0;

				strcpy(buffer, tmpBuffer);
			}
			else
			{
				strcpy(buffer, symbol.N.ShortName);
			}

			return 1;
		}
	}

	return 0;
}

static int getAllImports32(HANDLE file, struct ImportedFunction* buffer, int bufferLen)
{
	IMAGE_DOS_HEADER dosHeader = { 0 };
	if (SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &dosHeader, sizeof(dosHeader), 0, 0)) { return 0; }

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) { return 0; }

	IMAGE_NT_HEADERS32 imageNtHeaders = { 0 };
	LONG imageNtHeadersAddress = dosHeader.e_lfanew;
	if (SetFilePointer(file, imageNtHeadersAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &imageNtHeaders, sizeof(imageNtHeaders), 0, 0)) { return 0; }

	int bufferIndex = 0;

	if (imageNtHeaders.OptionalHeader.NumberOfRvaAndSizes > 1) 
	{
		DWORD importDirectoryTableAddress = imageNtHeaders.OptionalHeader.DataDirectory[1].VirtualAddress; // this is actually an RVA
		DWORD importDirectoryTableSize = imageNtHeaders.OptionalHeader.DataDirectory[1].Size;
		
		DWORD importDirectoryTableFileOffset = (DWORD)rvaToFileOffset(file, importDirectoryTableAddress);

		for (DWORD i = 0; i < importDirectoryTableSize; i += sizeof(IMAGE_IMPORT_DESCRIPTOR))
		{
			IMAGE_IMPORT_DESCRIPTOR importDescriptor = { 0 };
			if (SetFilePointer(file, importDirectoryTableFileOffset + i, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
			if (!ReadFile(file, &importDescriptor, sizeof(importDescriptor), 0, 0)) { return 0; }

			if (!importDescriptor.Characteristics) 
			{
				break;
			}

			DWORD importLookupTableFileOffset = (DWORD)rvaToFileOffset(file, importDescriptor.Characteristics);

			int j = 0;
			while (bufferIndex < bufferLen)
			{
				DWORD lookupValue = 0;
				if (SetFilePointer(file, importLookupTableFileOffset + j, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
				if (!ReadFile(file, &lookupValue, sizeof(lookupValue), 0, 0)) { return 0; }

				if (!lookupValue)
				{
					break;
				}

				if (lookupValue & 0x80000000) // import by ordinal
				{
					buffer[bufferIndex].name[0] = 0;
				}
				else // import by name
				{
					DWORD nameFileOffset = (DWORD)rvaToFileOffset(file, lookupValue + 2);
					if (SetFilePointer(file, nameFileOffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }

					buffer[bufferIndex].name[0] = 0;
					if (!ReadFile(file, buffer[bufferIndex].name, 50, 0, 0)) { return 0; }
					buffer[bufferIndex].name[49] = 0;
				}

				buffer[bufferIndex].address = imageNtHeaders.OptionalHeader.ImageBase + importDescriptor.FirstThunk + j;

				bufferIndex++;

				j += 4;
			}
		}
	}

	return bufferIndex;
}

static unsigned long long rvaToFileOffset(HANDLE file, unsigned long long rva)
{
	IMAGE_DOS_HEADER dosHeader = { 0 };
	if (SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &dosHeader, sizeof(dosHeader), 0, 0)) { return 0; }

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) { return 0; }

	IMAGE_NT_HEADERS32 imageNtHeaders = { 0 };
	LONG imageNtHeadersAddress = dosHeader.e_lfanew;
	if (SetFilePointer(file, imageNtHeadersAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &imageNtHeaders, sizeof(imageNtHeaders), 0, 0)) { return 0; }

	unsigned long long fileOffset = 0;
	for (int i = 0; i < imageNtHeaders.FileHeader.NumberOfSections; i++)
	{
		IMAGE_SECTION_HEADER sectionHeader = { 0 };
		LONG sectionAddress = (sizeof(IMAGE_SECTION_HEADER) * i) + imageNtHeadersAddress + sizeof(imageNtHeaders.Signature) + sizeof(imageNtHeaders.FileHeader) + imageNtHeaders.FileHeader.SizeOfOptionalHeader;
		if (SetFilePointer(file, sectionAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
		if (!ReadFile(file, &sectionHeader, sizeof(sectionHeader), 0, 0)) { return 0; }

		if (rva > sectionHeader.VirtualAddress && rva < sectionHeader.VirtualAddress + sectionHeader.SizeOfRawData)
		{
			fileOffset = (rva - sectionHeader.VirtualAddress) + sectionHeader.PointerToRawData;
			break;
		}
	}

	return fileOffset;
}