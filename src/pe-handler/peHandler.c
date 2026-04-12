#include "peHandler.h"

#include "dbghelp.h"
#pragma comment(lib, "dbghelp.lib")

unsigned char isPEX64(HANDLE file, unsigned char* isX64)
{
	IMAGE_DOS_HEADER dosHeader = { 0 };
	if (SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &dosHeader, sizeof(dosHeader), 0, 0)) { return 0; }

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) { return 0; }

	IMAGE_NT_HEADERS32 imageNtHeaders = { 0 };
	LONG imageNtHeadersAddress = dosHeader.e_lfanew;
	if (SetFilePointer(file, imageNtHeadersAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &imageNtHeaders, sizeof(imageNtHeaders), 0, 0)) { return 0; }

	*isX64 = imageNtHeaders.OptionalHeader.Magic == 0x20b; // PE32+
	return 1;
}

unsigned long long getPEImageBase32(HANDLE file)
{
	IMAGE_DOS_HEADER dosHeader = { 0 };
	if (SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &dosHeader, sizeof(dosHeader), 0, 0)) { return 0; }

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) { return 0; }

	IMAGE_NT_HEADERS32 imageNtHeaders = { 0 };
	LONG imageNtHeadersAddress = dosHeader.e_lfanew;
	if (SetFilePointer(file, imageNtHeadersAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &imageNtHeaders, sizeof(imageNtHeaders), 0, 0)) { return 0; }

	return imageNtHeaders.OptionalHeader.ImageBase;
}

unsigned long long getPEImageBase64(HANDLE file)
{
	IMAGE_DOS_HEADER dosHeader = { 0 };
	if (SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &dosHeader, sizeof(dosHeader), 0, 0)) { return 0; }

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) { return 0; }

	IMAGE_NT_HEADERS64 imageNtHeaders = { 0 };
	LONG imageNtHeadersAddress = dosHeader.e_lfanew;
	if (SetFilePointer(file, imageNtHeadersAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &imageNtHeaders, sizeof(imageNtHeaders), 0, 0)) { return 0; }

	return imageNtHeaders.OptionalHeader.ImageBase;
}

int getNumOfPESections32(HANDLE file)
{
	IMAGE_DOS_HEADER dosHeader = { 0 };
	if (SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &dosHeader, sizeof(dosHeader), 0, 0)) { return 0; }

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) { return 0; }

	IMAGE_NT_HEADERS32 imageNtHeaders = { 0 };
	LONG imageNtHeadersAddress = dosHeader.e_lfanew;
	if (SetFilePointer(file, imageNtHeadersAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &imageNtHeaders, sizeof(imageNtHeaders), 0, 0)) { return 0; }

	return imageNtHeaders.FileHeader.NumberOfSections;
}

int getNumOfPESections64(HANDLE file)
{
	IMAGE_DOS_HEADER dosHeader = { 0 };
	if (SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &dosHeader, sizeof(dosHeader), 0, 0)) { return 0; }

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) { return 0; }

	IMAGE_NT_HEADERS64 imageNtHeaders = { 0 };
	LONG imageNtHeadersAddress = dosHeader.e_lfanew;
	if (SetFilePointer(file, imageNtHeadersAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &imageNtHeaders, sizeof(imageNtHeaders), 0, 0)) { return 0; }

	return imageNtHeaders.FileHeader.NumberOfSections;
}

unsigned char getAllPESectionHeaders32(HANDLE file, struct FileSection* buffer, int bufferLen)
{
	IMAGE_DOS_HEADER dosHeader = { 0 };
	if (SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &dosHeader, sizeof(dosHeader), 0, 0)) { return 0; }

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) { return 0; }
	
	IMAGE_NT_HEADERS32 imageNtHeaders = { 0 };
	LONG imageNtHeadersAddress = dosHeader.e_lfanew;
	if (SetFilePointer(file, imageNtHeadersAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &imageNtHeaders, sizeof(imageNtHeaders), 0, 0)) { return 0; }

	if (bufferLen != imageNtHeaders.FileHeader.NumberOfSections) 
	{
		return 0;
	}

	for (int i = 0; i < imageNtHeaders.FileHeader.NumberOfSections; i++)
	{
		IMAGE_SECTION_HEADER sectionHeader = { 0 };
		LONG sectionAddress = (sizeof(IMAGE_SECTION_HEADER) * i) + imageNtHeadersAddress + sizeof(imageNtHeaders.Signature) + sizeof(imageNtHeaders.FileHeader) + imageNtHeaders.FileHeader.SizeOfOptionalHeader;
		if (SetFilePointer(file, sectionAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
		if (!ReadFile(file, &sectionHeader, sizeof(sectionHeader), 0, 0)) { return 0; }

		buffer[i].name = initializeJdcStrWithVal(sectionHeader.Name);

		if (sectionHeader.Characteristics & IMAGE_SCN_CNT_CODE)
		{
			buffer[i].type = CODE_FST;
		}
		else if (sectionHeader.Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA)
		{
			buffer[i].type = INIT_DATA_FST;
		}
		else if (sectionHeader.Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA)
		{
			buffer[i].type = UNINIT_DATA_FST;
		}
		else
		{
			buffer[i].type = OTHER_FST;
		}

		buffer[i].isReadOnly = !(sectionHeader.Characteristics & IMAGE_SCN_MEM_WRITE);

		buffer[i].virtualAddress = sectionHeader.VirtualAddress;
		buffer[i].fileOffset = sectionHeader.PointerToRawData;
		buffer[i].size = sectionHeader.SizeOfRawData;
	}
	
	return 1;
}

unsigned char getAllPESectionHeaders64(HANDLE file, struct FileSection* buffer, int bufferLen)
{
	IMAGE_DOS_HEADER dosHeader = { 0 };
	if (SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &dosHeader, sizeof(dosHeader), 0, 0)) { return 0; }

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) { return 0; }

	IMAGE_NT_HEADERS64 imageNtHeaders = { 0 };
	LONG imageNtHeadersAddress = dosHeader.e_lfanew;
	if (SetFilePointer(file, imageNtHeadersAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &imageNtHeaders, sizeof(imageNtHeaders), 0, 0)) { return 0; }

	if (bufferLen != imageNtHeaders.FileHeader.NumberOfSections)
	{
		return 0;
	}

	for (int i = 0; i < imageNtHeaders.FileHeader.NumberOfSections; i++)
	{
		IMAGE_SECTION_HEADER sectionHeader = { 0 };
		LONG sectionAddress = (sizeof(IMAGE_SECTION_HEADER) * i) + imageNtHeadersAddress + sizeof(imageNtHeaders.Signature) + sizeof(imageNtHeaders.FileHeader) + imageNtHeaders.FileHeader.SizeOfOptionalHeader;
		if (SetFilePointer(file, sectionAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
		if (!ReadFile(file, &sectionHeader, sizeof(sectionHeader), 0, 0)) { return 0; }

		buffer[i].name = initializeJdcStrWithVal(sectionHeader.Name);

		if (sectionHeader.Characteristics & IMAGE_SCN_CNT_CODE)
		{
			buffer[i].type = CODE_FST;
		}
		else if(sectionHeader.Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA)
		{
			buffer[i].type = INIT_DATA_FST;
		}
		else if (sectionHeader.Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA)
		{
			buffer[i].type = UNINIT_DATA_FST;
		}
		else 
		{
			buffer[i].type = OTHER_FST;
		}

		buffer[i].isReadOnly = !(sectionHeader.Characteristics & IMAGE_SCN_MEM_WRITE);

		buffer[i].virtualAddress = sectionHeader.VirtualAddress;
		buffer[i].fileOffset = sectionHeader.PointerToRawData;
		buffer[i].size = sectionHeader.SizeOfRawData;
	}

	return 1;
}

unsigned char getPESymbolByValue32(HANDLE file, DWORD value, struct JdcStr* result)
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

				strcpyJdc(result, tmpBuffer);
			}
			else 
			{
				strcpyJdc(result, symbol.N.ShortName);
			}

			return 1;
		}
	}
	
	return 0;
}

unsigned char getPESymbolByValue64(HANDLE file, DWORD value, struct JdcStr* result)
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

				strcpyJdc(result, tmpBuffer);
			}
			else
			{
				strcpyJdc(result, symbol.N.ShortName);
			}

			return 1;
		}
	}

	return 0;
}

int getNumOfPEImports32(HANDLE file)
{
	IMAGE_DOS_HEADER dosHeader = { 0 };
	if (SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &dosHeader, sizeof(dosHeader), 0, 0)) { return 0; }

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) { return 0; }

	IMAGE_NT_HEADERS32 imageNtHeaders = { 0 };
	LONG imageNtHeadersAddress = dosHeader.e_lfanew;
	if (SetFilePointer(file, imageNtHeadersAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &imageNtHeaders, sizeof(imageNtHeaders), 0, 0)) { return 0; }

	int result = 0;
	if (imageNtHeaders.OptionalHeader.NumberOfRvaAndSizes > 1)
	{
		DWORD importDirectoryTableAddress = imageNtHeaders.OptionalHeader.DataDirectory[1].VirtualAddress; // this is actually an RVA
		DWORD importDirectoryTableSize = imageNtHeaders.OptionalHeader.DataDirectory[1].Size;

		DWORD importDirectoryTableFileOffset = rvaToFileOffset32(file, importDirectoryTableAddress);

		for (DWORD i = 0; i < importDirectoryTableSize; i += sizeof(IMAGE_IMPORT_DESCRIPTOR))
		{
			IMAGE_IMPORT_DESCRIPTOR importDescriptor = { 0 };
			if (SetFilePointer(file, importDirectoryTableFileOffset + i, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
			if (!ReadFile(file, &importDescriptor, sizeof(importDescriptor), 0, 0)) { return 0; }

			if (!importDescriptor.Characteristics)
			{
				break;
			}

			DWORD importLookupTableFileOffset = rvaToFileOffset32(file, importDescriptor.Characteristics);

			int j = 0;
			while (1)
			{
				DWORD lookupValue = 0;
				if (SetFilePointer(file, importLookupTableFileOffset + j, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
				if (!ReadFile(file, &lookupValue, sizeof(lookupValue), 0, 0)) { return 0; }

				if (!lookupValue)
				{
					break;
				}

				result++;
				j += 4;
			}
		}
	}

	return result;
}

int getNumOfPEImports64(HANDLE file)
{
	IMAGE_DOS_HEADER dosHeader = { 0 };
	if (SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &dosHeader, sizeof(dosHeader), 0, 0)) { return 0; }

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) { return 0; }

	IMAGE_NT_HEADERS64 imageNtHeaders = { 0 };
	LONG imageNtHeadersAddress = dosHeader.e_lfanew;
	if (SetFilePointer(file, imageNtHeadersAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &imageNtHeaders, sizeof(imageNtHeaders), 0, 0)) { return 0; }

	int result = 0;
	if (imageNtHeaders.OptionalHeader.NumberOfRvaAndSizes > 1)
	{
		DWORD importDirectoryTableAddress = imageNtHeaders.OptionalHeader.DataDirectory[1].VirtualAddress; // this is actually an RVA
		DWORD importDirectoryTableSize = imageNtHeaders.OptionalHeader.DataDirectory[1].Size;

		DWORD importDirectoryTableFileOffset = rvaToFileOffset64(file, importDirectoryTableAddress);

		for (DWORD i = 0; i < importDirectoryTableSize; i += sizeof(IMAGE_IMPORT_DESCRIPTOR))
		{
			IMAGE_IMPORT_DESCRIPTOR importDescriptor = { 0 };
			if (SetFilePointer(file, importDirectoryTableFileOffset + i, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
			if (!ReadFile(file, &importDescriptor, sizeof(importDescriptor), 0, 0)) { return 0; }

			if (!importDescriptor.Characteristics)
			{
				break;
			}

			DWORD importLookupTableFileOffset = rvaToFileOffset64(file, importDescriptor.Characteristics);

			int j = 0;
			while (1)
			{
				DWORD lookupValue = 0;
				if (SetFilePointer(file, importLookupTableFileOffset + j, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
				if (!ReadFile(file, &lookupValue, sizeof(lookupValue), 0, 0)) { return 0; }

				if (!lookupValue)
				{
					break;
				}

				result++;
				j += 8;
			}
		}
	}

	return result;
}

int getAllPEImports32(HANDLE file, struct ImportedFunction* buffer, int bufferLen)
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
		
		DWORD importDirectoryTableFileOffset = rvaToFileOffset32(file, importDirectoryTableAddress);

		for (DWORD i = 0; i < importDirectoryTableSize; i += sizeof(IMAGE_IMPORT_DESCRIPTOR))
		{
			IMAGE_IMPORT_DESCRIPTOR importDescriptor = { 0 };
			if (SetFilePointer(file, importDirectoryTableFileOffset + i, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
			if (!ReadFile(file, &importDescriptor, sizeof(importDescriptor), 0, 0)) { return 0; }

			if (!importDescriptor.Characteristics) 
			{
				break;
			}

			DWORD importLookupTableFileOffset = rvaToFileOffset32(file, importDescriptor.Characteristics);

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

				if (lookupValue & 0x80000000) // import by ordinal, needs to be implemented
				{
					buffer[bufferIndex].name = initializeJdcStr();
					strcpyJdc(&buffer[bufferIndex].name, "");
				}
				else // import by name
				{
					buffer[bufferIndex].name = initializeJdcStrWithSize(255);
					DWORD nameFileOffset = rvaToFileOffset32(file, lookupValue + 2);
					if (SetFilePointer(file, nameFileOffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
					if (!ReadFile(file, buffer[bufferIndex].name.buffer, buffer[bufferIndex].name.bufferSize, 0, 0)) { return 0; }
				}

				buffer[bufferIndex].address = imageNtHeaders.OptionalHeader.ImageBase + importDescriptor.FirstThunk + j;

				bufferIndex++;
				j += 4;
			}
		}
	}

	return bufferIndex;
}

int getAllPEImports64(HANDLE file, struct ImportedFunction* buffer, int bufferLen)
{
	IMAGE_DOS_HEADER dosHeader = { 0 };
	if (SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &dosHeader, sizeof(dosHeader), 0, 0)) { return 0; }

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) { return 0; }

	IMAGE_NT_HEADERS64 imageNtHeaders = { 0 };
	LONG imageNtHeadersAddress = dosHeader.e_lfanew;
	if (SetFilePointer(file, imageNtHeadersAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &imageNtHeaders, sizeof(imageNtHeaders), 0, 0)) { return 0; }

	int bufferIndex = 0;
	if (imageNtHeaders.OptionalHeader.NumberOfRvaAndSizes > 1)
	{
		DWORD importDirectoryTableAddress = imageNtHeaders.OptionalHeader.DataDirectory[1].VirtualAddress; // this is actually an RVA
		DWORD importDirectoryTableSize = imageNtHeaders.OptionalHeader.DataDirectory[1].Size;

		DWORD importDirectoryTableFileOffset = rvaToFileOffset64(file, importDirectoryTableAddress);

		for (DWORD i = 0; i < importDirectoryTableSize; i += sizeof(IMAGE_IMPORT_DESCRIPTOR))
		{
			IMAGE_IMPORT_DESCRIPTOR importDescriptor = { 0 };
			if (SetFilePointer(file, importDirectoryTableFileOffset + i, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
			if (!ReadFile(file, &importDescriptor, sizeof(importDescriptor), 0, 0)) { return 0; }

			if (!importDescriptor.Characteristics)
			{
				break;
			}

			DWORD importLookupTableFileOffset = rvaToFileOffset64(file, importDescriptor.Characteristics);

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

				if (lookupValue & 0x80000000) // import by ordinal, needs to be implemented
				{
					buffer[bufferIndex].name = initializeJdcStr();
					strcpyJdc(&buffer[bufferIndex].name, "");
				}
				else // import by name
				{
					char tmp[255] = { 0 };
					DWORD nameFileOffset = rvaToFileOffset64(file, lookupValue + 2);
					if (SetFilePointer(file, nameFileOffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
					if (!ReadFile(file, tmp, 255, 0, 0)) { return 0; }

					buffer[bufferIndex].name = initializeJdcStrWithSize(255);
					if (UnDecorateSymbolName(tmp, buffer[bufferIndex].name.buffer, 255, UNDNAME_NAME_ONLY))
					{
						// removing template parameters
						int nameLen = strlen(buffer[bufferIndex].name.buffer);
						int k = 0;
						int openIndex = -1;
						int openNum = 0;
						int closeNum = 0;
						while (buffer[bufferIndex].name.buffer[k] != 0)
						{
							if (buffer[bufferIndex].name.buffer[k] == '<')
							{
								if (openIndex == -1) { openIndex = k; }
								openNum++;
							}
							else if (buffer[bufferIndex].name.buffer[k] == '>')
							{
								closeNum++;

								if (closeNum == openNum) 
								{
									int len = strlen(buffer[bufferIndex].name.buffer + k + 1);
									memcpy(buffer[bufferIndex].name.buffer + openIndex, buffer[bufferIndex].name.buffer + k + 1, len);
									memset(buffer[bufferIndex].name.buffer + openIndex + len, 0, nameLen - (openIndex + len));

									openIndex = -1;
									openNum = 0;
									closeNum = 0;
								}
								
							}

							k++;
						}
					}
					else 
					{
						strcpyJdc(&buffer[bufferIndex].name, tmp);
					}
				}

				buffer[bufferIndex].address = imageNtHeaders.OptionalHeader.ImageBase + importDescriptor.FirstThunk + j;

				bufferIndex++;
				j += 8;
			}
		}
	}

	return bufferIndex;
}

DWORD rvaToFileOffset32(HANDLE file, DWORD rva)
{
	IMAGE_DOS_HEADER dosHeader = { 0 };
	if (SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &dosHeader, sizeof(dosHeader), 0, 0)) { return 0; }

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) { return 0; }

	IMAGE_NT_HEADERS32 imageNtHeaders = { 0 };
	LONG imageNtHeadersAddress = dosHeader.e_lfanew;
	if (SetFilePointer(file, imageNtHeadersAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &imageNtHeaders, sizeof(imageNtHeaders), 0, 0)) { return 0; }

	DWORD fileOffset = 0;
	for (int i = 0; i < imageNtHeaders.FileHeader.NumberOfSections; i++)
	{
		IMAGE_SECTION_HEADER sectionHeader = { 0 };
		LONG sectionAddress = (sizeof(IMAGE_SECTION_HEADER) * i) + imageNtHeadersAddress + sizeof(imageNtHeaders.Signature) + sizeof(imageNtHeaders.FileHeader) + imageNtHeaders.FileHeader.SizeOfOptionalHeader;
		if (SetFilePointer(file, sectionAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
		if (!ReadFile(file, &sectionHeader, sizeof(sectionHeader), 0, 0)) { return 0; }

		if (rva >= sectionHeader.VirtualAddress && rva < sectionHeader.VirtualAddress + sectionHeader.SizeOfRawData)
		{
			fileOffset = (rva - sectionHeader.VirtualAddress) + sectionHeader.PointerToRawData;
			break;
		}
	}

	return fileOffset;
}

DWORD rvaToFileOffset64(HANDLE file, DWORD rva)
{
	IMAGE_DOS_HEADER dosHeader = { 0 };
	if (SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &dosHeader, sizeof(dosHeader), 0, 0)) { return 0; }

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) { return 0; }

	IMAGE_NT_HEADERS64 imageNtHeaders = { 0 };
	LONG imageNtHeadersAddress = dosHeader.e_lfanew;
	if (SetFilePointer(file, imageNtHeadersAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &imageNtHeaders, sizeof(imageNtHeaders), 0, 0)) { return 0; }

	DWORD fileOffset = 0;
	for (int i = 0; i < imageNtHeaders.FileHeader.NumberOfSections; i++)
	{
		IMAGE_SECTION_HEADER sectionHeader = { 0 };
		LONG sectionAddress = (sizeof(IMAGE_SECTION_HEADER) * i) + imageNtHeadersAddress + sizeof(imageNtHeaders.Signature) + sizeof(imageNtHeaders.FileHeader) + imageNtHeaders.FileHeader.SizeOfOptionalHeader;
		if (SetFilePointer(file, sectionAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
		if (!ReadFile(file, &sectionHeader, sizeof(sectionHeader), 0, 0)) { return 0; }

		if (rva >= sectionHeader.VirtualAddress && rva < sectionHeader.VirtualAddress + sectionHeader.SizeOfRawData)
		{
			fileOffset = (rva - sectionHeader.VirtualAddress) + sectionHeader.PointerToRawData;
			break;
		}
	}

	return fileOffset;
}
