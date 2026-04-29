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

				if (!(lookupValue & 0x80000000)) // import by name
				{
					// checking for null name
					char firstChar = 0;
					DWORD nameFileOffset = rvaToFileOffset32(file, lookupValue + 2);
					if (SetFilePointer(file, nameFileOffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
					if (!ReadFile(file, &firstChar, 1, 0, 0)) { return 0; }

					if (firstChar == 0) 
					{
						continue;
					}

					result++;
				}

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

				
				if (!(lookupValue & 0x80000000))
				{
					// checking for null name
					char firstChar = 0;
					DWORD nameFileOffset = rvaToFileOffset64(file, lookupValue + 2);
					if (SetFilePointer(file, nameFileOffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
					if (!ReadFile(file, &firstChar, 1, 0, 0)) { return 0; }

					if (firstChar == 0)
					{
						continue;
					}

					result++;
				}

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

				if(!(lookupValue & 0x80000000)) // import by name. import by ordinal needs to be implemented
				{
					buffer[bufferIndex].name = initializeJdcStrWithSize(255);
					DWORD nameFileOffset = rvaToFileOffset32(file, lookupValue + 2);
					if (SetFilePointer(file, nameFileOffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
					if (!ReadFile(file, buffer[bufferIndex].name.buffer, buffer[bufferIndex].name.bufferSize, 0, 0)) { return 0; }

					if (buffer[bufferIndex].name.buffer[0] == 0) 
					{
						freeJdcStr(&buffer[bufferIndex].name);
						continue;
					}

					buffer[bufferIndex].address = imageNtHeaders.OptionalHeader.ImageBase + importDescriptor.FirstThunk + j;

					bufferIndex++;
				}

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

				if (!(lookupValue & 0x80000000)) // import by name. import by ordinal needs to be implemented
				{
					char tmp[255] = { 0 };
					DWORD nameFileOffset = rvaToFileOffset64(file, lookupValue + 2);
					if (SetFilePointer(file, nameFileOffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
					if (!ReadFile(file, tmp, 255, 0, 0)) { return 0; }

					if (tmp[0] == 0)
					{
						continue;
					}

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

					buffer[bufferIndex].address = imageNtHeaders.OptionalHeader.ImageBase + importDescriptor.FirstThunk + j;

					bufferIndex++;
				}

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

unsigned char generatePEHeadersInfoStr(HANDLE file, struct JdcStr* result)
{
	IMAGE_DOS_HEADER dosHeader = { 0 };
	if (SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &dosHeader, sizeof(dosHeader), 0, 0)) { return 0; }

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) { return 0; }

	generateDOSHeaderInfoStr(&dosHeader, result);

	IMAGE_NT_HEADERS64 imageNtHeaders = { 0 };
	LONG imageNtHeadersAddress = dosHeader.e_lfanew;
	if (SetFilePointer(file, imageNtHeadersAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &imageNtHeaders, sizeof(imageNtHeaders), 0, 0)) { return 0; }

	generateFileHeaderInfoStr(&imageNtHeaders.FileHeader, result);
	generateOptionalHeaderInfoStr(&imageNtHeaders.OptionalHeader, result);

	return 1;
}

static void generateDOSHeaderInfoStr(IMAGE_DOS_HEADER* dosHeader, struct JdcStr* result)
{
	strcatJdc(result, "IMAGE_DOS_HEADER\n\n");
	sprintfJdc(result, 1, "0x0\te_magic\t0x%llX\tMagic number\n", dosHeader->e_magic);
	sprintfJdc(result, 1, "0x2\te_cblp\t%d\tBytes on last page of file\n", dosHeader->e_cblp);
	sprintfJdc(result, 1, "0x4\te_cp\t%d\tPages in file\n", dosHeader->e_cp);
	sprintfJdc(result, 1, "0x6\te_crlc\t%d\tRelocations\n", dosHeader->e_crlc);
	sprintfJdc(result, 1, "0x8\te_cparhdr\t0x%llX\tSize of header in paragraphs\n", dosHeader->e_cparhdr);
	sprintfJdc(result, 1, "0xA\te_minalloc\t0x%llX\tMinimum extra paragraphs needed\n", dosHeader->e_minalloc);
	sprintfJdc(result, 1, "0xC\te_maxalloc\t0x%llX\tMaximum extra paragraphs needed\n", dosHeader->e_maxalloc);
	sprintfJdc(result, 1, "0xE\te_ss\t0x%llX\tInitial (relative) SS value\n", dosHeader->e_ss);
	sprintfJdc(result, 1, "0x10\te_sp\t0x%llX\tInitial SP value\n", dosHeader->e_sp);
	sprintfJdc(result, 1, "0x12\te_csum\t0x%llX\tChecksum\n", dosHeader->e_csum);
	sprintfJdc(result, 1, "0x14\te_ip\t0x%llX\tInitial IP value\n", dosHeader->e_ip);
	sprintfJdc(result, 1, "0x16\te_cs\t0x%llX\tInitial (relative) CS value\n", dosHeader->e_cs);
	sprintfJdc(result, 1, "0x18\te_lfarlc\t0x%llX\tFile address of relocation table\n", dosHeader->e_lfarlc);
	sprintfJdc(result, 1, "0x1A\te_ovno\t0x%llX\tOverlay number\n", dosHeader->e_ovno);
	sprintfJdc(result, 1, "0x1C\te_res[4]\t");
	for (int i = 0; i < 4; i++)
	{
		sprintfJdc(result, 1, "0x%llX", dosHeader->e_res[i]);
		if (i != 3)
		{
			strcatJdc(result, ", ");
		}
		else
		{
			strcatJdc(result, "\tReserved words\n");
		}
	}

	sprintfJdc(result, 1, "0x24\te_oemid\t0x%llX\tOEM identifier (for e_oeminfo)\n", dosHeader->e_oemid);
	sprintfJdc(result, 1, "0x26\te_oeminfo\t0x%llX\tOEM information; e_oemid specific\n", dosHeader->e_oeminfo);

	sprintfJdc(result, 1, "0x28\te_res2[10]\t");
	for (int i = 0; i < 10; i++)
	{
		sprintfJdc(result, 1, "0x%llX", dosHeader->e_res2[i]);
		if (i != 9)
		{
			strcatJdc(result, ", ");
		}
		else
		{
			strcatJdc(result, "\tReserved words\n");
		}
	}

	sprintfJdc(result, 1, "0x3C\te_lfanew\t0x%llX\tFile address of new exe header\n", dosHeader->e_lfanew);
}

static void generateFileHeaderInfoStr(IMAGE_FILE_HEADER* fileHeader, struct JdcStr* result)
{
	strcatJdc(result, "IMAGE_FILE_HEADER\n\n");

	strcatJdc(result, "0x0\tMachine\t");
	switch (fileHeader->Machine)
	{
	case IMAGE_FILE_MACHINE_UNKNOWN:
		strcatJdc(result, "IMAGE_FILE_MACHINE_UNKNOWN (0x0)\tThe content of this field is assumed to be applicable to any machine type\n");
		break;
	case IMAGE_FILE_MACHINE_ALPHA:
		strcatJdc(result, "IMAGE_FILE_MACHINE_ALPHA (0x184)\tAlpha AXP, 32-bit address space\n");
		break;
	case IMAGE_FILE_MACHINE_ALPHA64: // same as IMAGE_FILE_MACHINE_AXP64
		strcatJdc(result, "IMAGE_FILE_MACHINE_ALPHA64, IMAGE_FILE_MACHINE_AXP64 (0x284)\tAlpha 64, 64-bit address space\n");
		break;
	case IMAGE_FILE_MACHINE_AM33:
		strcatJdc(result, "IMAGE_FILE_MACHINE_AM33 (0x1D3)\tMatsushita AM33\n");
		break;
	case IMAGE_FILE_MACHINE_AMD64:
		strcatJdc(result, "IMAGE_FILE_MACHINE_AMD64 (0x8664)\tx64\n");
		break;
	case IMAGE_FILE_MACHINE_ARM:
		strcatJdc(result, "IMAGE_FILE_MACHINE_ARM (0x1C0)\tARM little endian\n");
		break;
	case IMAGE_FILE_MACHINE_ARM64:
		strcatJdc(result, "IMAGE_FILE_MACHINE_ARM64 (0xAA64)\tARM64 little endian\n");
		break;
	case 0xA641:
		strcatJdc(result, "IMAGE_FILE_MACHINE_ARM64EC (0xA641)\tABI that enables interoperability between native ARM64 and emulated x64 code.\n");
		break;
	case 0xA64E:
		strcatJdc(result, "IMAGE_FILE_MACHINE_ARM64X (0xA64E)\tBinary format that allows both native ARM64 and ARM64EC code to coexist in the same file.\n");
		break;
	case IMAGE_FILE_MACHINE_ARMNT:
		strcatJdc(result, "IMAGE_FILE_MACHINE_ARMNT (0x1C4)\tARM Thumb-2 little endian\n");
		break;
	case IMAGE_FILE_MACHINE_EBC:
		strcatJdc(result, "IMAGE_FILE_MACHINE_EBC (0xEBC)\tEFI byte code\n");
		break;
	case IMAGE_FILE_MACHINE_I386:
		strcatJdc(result, "IMAGE_FILE_MACHINE_I386 (0x14C)\tIntel 386 or later processors and compatible processors\n");
		break;
	case IMAGE_FILE_MACHINE_IA64:
		strcatJdc(result, "IMAGE_FILE_MACHINE_IA64 (0x200)\tIntel Itanium processor family\n");
		break;
	case 0x6232:
		strcatJdc(result, "IMAGE_FILE_MACHINE_LOONGARCH32 (0x6232)\tLoongArch 32-bit processor family\n");
		break;
	case 0x6264:
		strcatJdc(result, "IMAGE_FILE_MACHINE_LOONGARCH64 (0x6264)\tLoongArch 64-bit processor family\n");
		break;
	case IMAGE_FILE_MACHINE_M32R:
		strcatJdc(result, "IMAGE_FILE_MACHINE_M32R (0x9041)\tMitsubishi M32R little endian\n");
		break;
	case IMAGE_FILE_MACHINE_MIPS16:
		strcatJdc(result, "IMAGE_FILE_MACHINE_MIPS16 (0x266)\tMIPS16\n");
		break;
	case IMAGE_FILE_MACHINE_MIPSFPU:
		strcatJdc(result, "IMAGE_FILE_MACHINE_MIPSFPU (0x366)\tMIPS with FPU\n");
		break;
	case IMAGE_FILE_MACHINE_MIPSFPU16:
		strcatJdc(result, "IMAGE_FILE_MACHINE_MIPSFPU16 (0x466)\tMIPS16 with FPU\n");
		break;
	case IMAGE_FILE_MACHINE_POWERPC:
		strcatJdc(result, "IMAGE_FILE_MACHINE_POWERPC (0x1F0)\tPower PC little endian\n");
		break;
	case IMAGE_FILE_MACHINE_POWERPCFP:
		strcatJdc(result, "IMAGE_FILE_MACHINE_POWERPCFP (0x1F1)\tPower PC with floating point support\n");
		break;
	case 0x160:
		strcatJdc(result, "IMAGE_FILE_MACHINE_R3000BE (0x160)\tMIPS I compatible 32-bit big endian\n");
		break;
	case IMAGE_FILE_MACHINE_R3000:
		strcatJdc(result, "IMAGE_FILE_MACHINE_R3000 (0x162)\tMIPS I compatible 32-bit little endian\n");
		break;
	case IMAGE_FILE_MACHINE_R4000:
		strcatJdc(result, "IMAGE_FILE_MACHINE_R4000 (0x166)\tMIPS III compatible 64-bit little endian\n");
		break;
	case IMAGE_FILE_MACHINE_R10000:
		strcatJdc(result, "IMAGE_FILE_MACHINE_R10000 (0x168)\tMIPS IV compatible 64-bit little endian\n");
		break;
	case 0x5032:
		strcatJdc(result, "IMAGE_FILE_MACHINE_RISCV32 (0x5032)\tRISC-V 32-bit address space\n");
		break;
	case 0x5064:
		strcatJdc(result, "IMAGE_FILE_MACHINE_RISCV64 (0x5064)\tRISC-V 64-bit address space\n");
		break;
	case 0x5128:
		strcatJdc(result, "IMAGE_FILE_MACHINE_RISCV128 (0x5128)\tRISC-V 128-bit address space\n");
		break;
	case IMAGE_FILE_MACHINE_SH3:
		strcatJdc(result, "IMAGE_FILE_MACHINE_SH3 (0x1A2)\tHitachi SH3\n");
		break;
	case IMAGE_FILE_MACHINE_SH3DSP:
		strcatJdc(result, "IMAGE_FILE_MACHINE_SH3DSP (0x1A3)\tHitachi SH3 DSP\n");
		break;
	case IMAGE_FILE_MACHINE_SH4:
		strcatJdc(result, "IMAGE_FILE_MACHINE_SH4 (0x1A6)\tHitachi SH4\n");
		break;
	case IMAGE_FILE_MACHINE_SH5:
		strcatJdc(result, "IMAGE_FILE_MACHINE_SH5 (0x1A8)\tHitachi SH5\n");
		break;
	case IMAGE_FILE_MACHINE_THUMB:
		strcatJdc(result, "IMAGE_FILE_MACHINE_THUMB (0x1C2)\tThumb\n");
		break;
	case IMAGE_FILE_MACHINE_WCEMIPSV2:
		strcatJdc(result, "IMAGE_FILE_MACHINE_WCEMIPSV2 (0x169)\tMIPS little-endian WCE v2\n");
		break;
	}

	sprintfJdc(result, 1, "0x2\tNumberOfSections\t%d\tNumber of sections\n", fileHeader->NumberOfSections);
	sprintfJdc(result, 1, "0x4\tTimeDateStamp\t%d\tThe low 32 bits of the number of seconds since 00:00 January 1, 1970 (a C run-time time_t value), which indicates when the file was created.\n", fileHeader->TimeDateStamp);
	sprintfJdc(result, 1, "0x8\tPointerToSymbolTable\t0x%llX\tFile offset of the COFF symbol table\n", fileHeader->PointerToSymbolTable);
	sprintfJdc(result, 1, "0xC\tNumberOfSymbols\t%d\tNumber of entries in the symbol table\n", fileHeader->NumberOfSymbols);
	sprintfJdc(result, 1, "0x10\tSizeOfOptionalHeader\t0x%llX\tSize of the optional header\n", fileHeader->SizeOfOptionalHeader);

	sprintfJdc(result, 1, "0x12\tCharacteristics\t0x%llX", fileHeader->Characteristics);
	unsigned char gotFirstFlag = 0;
	if (fileHeader->Characteristics & IMAGE_FILE_RELOCS_STRIPPED) 
	{
		strcatJdc(result, " (IMAGE_FILE_RELOCS_STRIPPED");
		gotFirstFlag = 1;
	}
	if (fileHeader->Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE)
	{
		sprintfJdc(result, 1, "%sIMAGE_FILE_EXECUTABLE_IMAGE", gotFirstFlag ? " | " : " (");
		gotFirstFlag = 1;
	}
	if (fileHeader->Characteristics & IMAGE_FILE_LINE_NUMS_STRIPPED)
	{
		sprintfJdc(result, 1, "%sIMAGE_FILE_LINE_NUMS_STRIPPED", gotFirstFlag ? " | " : " (");
		gotFirstFlag = 1;
	}
	if (fileHeader->Characteristics & IMAGE_FILE_LOCAL_SYMS_STRIPPED)
	{
		sprintfJdc(result, 1, "%sIMAGE_FILE_LOCAL_SYMS_STRIPPED", gotFirstFlag ? " | " : " (");
		gotFirstFlag = 1;
	}
	if (fileHeader->Characteristics & 0x0010)
	{
		sprintfJdc(result, 1, "%sIMAGE_FILE_AGGRESSIVE_WS_TRIM", gotFirstFlag ? " | " : " (");
		gotFirstFlag = 1;
	}
	if (fileHeader->Characteristics & IMAGE_FILE_LARGE_ADDRESS_AWARE)
	{
		sprintfJdc(result, 1, "%sIMAGE_FILE_LARGE_ADDRESS_AWARE", gotFirstFlag ? " | " : " (");
		gotFirstFlag = 1;
	}
	if (fileHeader->Characteristics & IMAGE_FILE_BYTES_REVERSED_LO)
	{
		sprintfJdc(result, 1, "%sIMAGE_FILE_BYTES_REVERSED_LO", gotFirstFlag ? " | " : " (");
		gotFirstFlag = 1;
	}
	if (fileHeader->Characteristics & IMAGE_FILE_32BIT_MACHINE)
	{
		sprintfJdc(result, 1, "%sIMAGE_FILE_32BIT_MACHINE", gotFirstFlag ? " | " : " (");
		gotFirstFlag = 1;
	}
	if (fileHeader->Characteristics & IMAGE_FILE_DEBUG_STRIPPED)
	{
		sprintfJdc(result, 1, "%sIMAGE_FILE_DEBUG_STRIPPED", gotFirstFlag ? " | " : " (");
		gotFirstFlag = 1;
	}
	if (fileHeader->Characteristics & IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP)
	{
		sprintfJdc(result, 1, "%sIMAGE_FILE_REMOVABLE_RUN_FROM_SWAP", gotFirstFlag ? " | " : " (");
		gotFirstFlag = 1;
	}
	if (fileHeader->Characteristics & IMAGE_FILE_NET_RUN_FROM_SWAP)
	{
		sprintfJdc(result, 1, "%sIMAGE_FILE_NET_RUN_FROM_SWAP", gotFirstFlag ? " | " : " (");
		gotFirstFlag = 1;
	}
	if (fileHeader->Characteristics & IMAGE_FILE_SYSTEM)
	{
		sprintfJdc(result, 1, "%sIMAGE_FILE_SYSTEM", gotFirstFlag ? " | " : " (");
		gotFirstFlag = 1;
	}
	if (fileHeader->Characteristics & IMAGE_FILE_DLL)
	{
		sprintfJdc(result, 1, "%sIMAGE_FILE_DLL", gotFirstFlag ? " | " : " (");
		gotFirstFlag = 1;
	}
	if (fileHeader->Characteristics & IMAGE_FILE_UP_SYSTEM_ONLY)
	{
		sprintfJdc(result, 1, "%sIMAGE_FILE_UP_SYSTEM_ONLY", gotFirstFlag ? " | " : " (");
		gotFirstFlag = 1;
	}
	if (fileHeader->Characteristics & IMAGE_FILE_BYTES_REVERSED_HI)
	{
		sprintfJdc(result, 1, "%sIMAGE_FILE_BYTES_REVERSED_HI", gotFirstFlag ? " | " : " (");
		gotFirstFlag = 1;
	}

	if(gotFirstFlag)
	{
		strcatJdc(result, ")");
	}
	strcatJdc(result, "\n");

	strcatJdc(result, "\tFlags that indicate the attributes of the file\n");
}

static void generateOptionalHeaderInfoStr(IMAGE_OPTIONAL_HEADER64* optionalHeader, struct JdcStr* result) 
{
	strcatJdc(result, "IMAGE_OPTIONAL_HEADER\n\n");

	sprintfJdc(result, 1, "0x0\tMagic\t0x%llX\t", optionalHeader->Magic);
	switch (optionalHeader->Magic) 
	{
	case 0x10B:
		strcatJdc(result, "PE32\n");
		break;
	case 0x107:
		strcatJdc(result, "ROM image\n");
		break;
	case 0x20B:
		strcatJdc(result, "PE32+\n");
		break;
	}

	sprintfJdc(result, 1, "0x2\tMajorLinkerVersion\t0x%llX\tLinker major version number\n", optionalHeader->MajorLinkerVersion);
	sprintfJdc(result, 1, "0x3\tMinorLinkerVersion\t0x%llX\tLinker minor version number\n", optionalHeader->MinorLinkerVersion);
	sprintfJdc(result, 1, "0x4\tSizeOfCode\t0x%llX\tSize of all code sections\n", optionalHeader->SizeOfCode);
	sprintfJdc(result, 1, "0x8\tSizeOfInitializedData\t0x%llX\tSize of all initialized data sections\n", optionalHeader->SizeOfInitializedData);
	sprintfJdc(result, 1, "0xC\tSizeOfUninitializedData\t0x%llX\tSize of all uninitialized data sections\n", optionalHeader->SizeOfUninitializedData);
	sprintfJdc(result, 1, "0x10\tAddressOfEntryPoint\t0x%llX\tAddress of the entry point relative to the image base when the executable file is loaded into memory\n", optionalHeader->AddressOfEntryPoint);
	sprintfJdc(result, 1, "0x14\tBaseOfCode\t0x%llX\tAddress that is relative to the image base of the beginning-of-code section when it is loaded into memory\n", optionalHeader->BaseOfCode);

	// after this point there are differenences between 32 and 64 bit versions
	unsigned char x64 = optionalHeader->Magic == 0x20b;
	if (x64) 
	{
		sprintfJdc(result, 1, "0x18\tImageBase\t0x%llX\tPreferred address of the first byte of image when loaded into memory\n", optionalHeader->ImageBase);
	}
	else 
	{
		sprintfJdc(result, 1, "0x18\tBaseOfData\t0x%llX\tAddress that is relative to the image base of the beginning-of-data section when it is loaded into memory\n", ((IMAGE_OPTIONAL_HEADER32*)optionalHeader)->BaseOfData);
		sprintfJdc(result, 1, "0x1C\tImageBase\t0x%llX\tPreferred address of the first byte of image when loaded into memory\n", ((IMAGE_OPTIONAL_HEADER32*)optionalHeader)->ImageBase);
	}

	// the offsets for the 32 and 64 bit versions are the same for the next few fields
	sprintfJdc(result, 1, "0x20\tSectionAlignment\t0x%llX\tAlignment (in bytes) of sections when they are loaded into memory\n", optionalHeader->SectionAlignment);
	sprintfJdc(result, 1, "0x24\tFileAlignment\t0x%llX\tAlignment factor (in bytes) that is used to align the raw data of sections in the image file\n", optionalHeader->FileAlignment);
	sprintfJdc(result, 1, "0x28\tMajorOperatingSystemVersion\t0x%llX\tMajor version number of the required operating system\n", optionalHeader->MajorOperatingSystemVersion);
	sprintfJdc(result, 1, "0x2A\tMinorOperatingSystemVersion\t0x%llX\tMinor version number of the required operating system\n", optionalHeader->MinorOperatingSystemVersion);
	sprintfJdc(result, 1, "0x2C\tMajorImageVersion\t0x%llX\tMajor version number of the image\n", optionalHeader->MajorImageVersion);
	sprintfJdc(result, 1, "0x2E\tMinorImageVersion\t0x%llX\tMinor version number of the image\n", optionalHeader->MinorImageVersion);
	sprintfJdc(result, 1, "0x30\tMajorSubsystemVersion\t0x%llX\tMajor version number of the subsystem\n", optionalHeader->MajorSubsystemVersion);
	sprintfJdc(result, 1, "0x32\tMinorSubsystemVersion\t0x%llX\tMinor version number of the subsystem\n", optionalHeader->MinorSubsystemVersion);
	sprintfJdc(result, 1, "0x34\tWin32VersionValue\t0x%llX\tReserved, must be zero\n", optionalHeader->Win32VersionValue);
	sprintfJdc(result, 1, "0x38\tSizeOfImage\t0x%llX\tSize (in bytes) of the image, including all headers\n", optionalHeader->SizeOfImage);
	sprintfJdc(result, 1, "0x3C\tSizeOfHeaders\t0x%llX\tCombined size of an MS-DOS stub, PE header, and section headers rounded up to a multiple of FileAlignment\n", optionalHeader->SizeOfHeaders);
	sprintfJdc(result, 1, "0x40\tCheckSum\t0x%llX\tImage file checksum\n", optionalHeader->CheckSum);

	strcatJdc(result, "0x44\tSubsystem\t");
	switch (optionalHeader->Subsystem) 
	{
	case IMAGE_SUBSYSTEM_UNKNOWN:
		strcatJdc(result, "IMAGE_SUBSYSTEM_UNKNOWN (0)\tAn unknown subsystem\n");
		break;
	case IMAGE_SUBSYSTEM_NATIVE:
		strcatJdc(result, "IMAGE_SUBSYSTEM_NATIVE (1)\tDevice drivers and native Windows processes\n");
		break;
	case IMAGE_SUBSYSTEM_WINDOWS_GUI:
		strcatJdc(result, "IMAGE_SUBSYSTEM_WINDOWS_GUI (2)\tThe Windows graphical user interface (GUI) subsystem\n");
		break;
	case IMAGE_SUBSYSTEM_WINDOWS_CUI:
		strcatJdc(result, "IMAGE_SUBSYSTEM_WINDOWS_CUI (3)\tThe Windows character subsystem\n");
		break;
	case IMAGE_SUBSYSTEM_OS2_CUI:
		strcatJdc(result, "IMAGE_SUBSYSTEM_OS2_CUI (5)\tThe OS/2 character subsystem\n");
		break;
	case IMAGE_SUBSYSTEM_POSIX_CUI:
		strcatJdc(result, "IMAGE_SUBSYSTEM_POSIX_CUI (7)\tThe Posix character subsystem\n");
		break;
	case IMAGE_SUBSYSTEM_NATIVE_WINDOWS:
		strcatJdc(result, "IMAGE_SUBSYSTEM_NATIVE_WINDOWS (8)\tNative Win9x driver\n");
		break;
	case IMAGE_SUBSYSTEM_WINDOWS_CE_GUI:
		strcatJdc(result, "IMAGE_SUBSYSTEM_WINDOWS_CE_GUI (9)\tWindows CE\n");
		break;
	case IMAGE_SUBSYSTEM_EFI_APPLICATION:
		strcatJdc(result, "IMAGE_SUBSYSTEM_EFI_APPLICATION (10)\tAn Extensible Firmware Interface (EFI) application\n");
		break;
	case IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER:
		strcatJdc(result, "IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER (11)\tAn EFI driver with boot services\n");
		break;
	case IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER:
		strcatJdc(result, "IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER (12)\tAn EFI driver with run-time services\n");
		break;
	case IMAGE_SUBSYSTEM_EFI_ROM:
		strcatJdc(result, "IMAGE_SUBSYSTEM_EFI_ROM (13)\tAn EFI ROM image\n");
		break;
	case IMAGE_SUBSYSTEM_XBOX:
		strcatJdc(result, "IMAGE_SUBSYSTEM_XBOX (14)\tXBOX\n");
		break;
	case IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION:
		strcatJdc(result, "IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION (16)\tWindows boot application\n");
		break;
	}

	sprintfJdc(result, 1, "0x46\tDllCharacteristics\t0x%llX", optionalHeader->DllCharacteristics);
	unsigned char gotFirstFlag = 0;
	if (optionalHeader->DllCharacteristics & IMAGE_DLLCHARACTERISTICS_HIGH_ENTROPY_VA)
	{
		strcatJdc(result, " (IMAGE_DLLCHARACTERISTICS_HIGH_ENTROPY_VA ");
		gotFirstFlag = 1;
	}
	if (optionalHeader->DllCharacteristics & IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE)
	{
		sprintfJdc(result, 1, "%sIMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE", gotFirstFlag ? " | " : " (");
		gotFirstFlag = 1;
	}
	if (optionalHeader->DllCharacteristics & IMAGE_DLLCHARACTERISTICS_FORCE_INTEGRITY)
	{
		sprintfJdc(result, 1, "%sIMAGE_DLLCHARACTERISTICS_FORCE_INTEGRITY", gotFirstFlag ? " | " : " (");
		gotFirstFlag = 1;
	}
	if (optionalHeader->DllCharacteristics & IMAGE_DLLCHARACTERISTICS_NX_COMPAT)
	{
		sprintfJdc(result, 1, "%sIMAGE_DLLCHARACTERISTICS_NX_COMPAT", gotFirstFlag ? " | " : " (");
		gotFirstFlag = 1;
	}
	if (optionalHeader->DllCharacteristics & IMAGE_DLLCHARACTERISTICS_NO_ISOLATION)
	{
		sprintfJdc(result, 1, "%sIMAGE_DLLCHARACTERISTICS_NO_ISOLATION", gotFirstFlag ? " | " : " (");
		gotFirstFlag = 1;
	}
	if (optionalHeader->DllCharacteristics & IMAGE_DLLCHARACTERISTICS_NO_SEH)
	{
		sprintfJdc(result, 1, "%sIMAGE_DLLCHARACTERISTICS_NO_SEH", gotFirstFlag ? " | " : " (");
		gotFirstFlag = 1;
	}
	if (optionalHeader->DllCharacteristics & IMAGE_DLLCHARACTERISTICS_NO_BIND)
	{
		sprintfJdc(result, 1, "%sIMAGE_DLLCHARACTERISTICS_NO_BIND", gotFirstFlag ? " | " : " (");
		gotFirstFlag = 1;
	}
	if (optionalHeader->DllCharacteristics & IMAGE_DLLCHARACTERISTICS_APPCONTAINER)
	{
		sprintfJdc(result, 1, "%sIMAGE_DLLCHARACTERISTICS_APPCONTAINER", gotFirstFlag ? " | " : " (");
		gotFirstFlag = 1;
	}
	if (optionalHeader->DllCharacteristics & IMAGE_DLLCHARACTERISTICS_WDM_DRIVER)
	{
		sprintfJdc(result, 1, "%sIMAGE_DLLCHARACTERISTICS_WDM_DRIVER", gotFirstFlag ? " | " : " (");
		gotFirstFlag = 1;
	}
	if (optionalHeader->DllCharacteristics & IMAGE_DLLCHARACTERISTICS_GUARD_CF)
	{
		sprintfJdc(result, 1, "%sIMAGE_DLLCHARACTERISTICS_GUARD_CF ", gotFirstFlag ? " | " : " (");
		gotFirstFlag = 1;
	}
	if (optionalHeader->DllCharacteristics & IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE)
	{
		sprintfJdc(result, 1, "%sIMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE ", gotFirstFlag ? " | " : " (");
		gotFirstFlag = 1;
	}

	if (gotFirstFlag)
	{
		strcatJdc(result, ")");
	}
	strcatJdc(result, "\n");

	sprintfJdc(result, 1, "0x48\tSizeOfStackReserve\t0x%llX\tSize of the stack to reserve\n", optionalHeader->SizeOfStackReserve);

	if (x64) 
	{
		sprintfJdc(result, 1, "0x50\tSizeOfStackCommit\t0x%llX\tSize of the stack to commit\n", optionalHeader->SizeOfStackCommit);
		sprintfJdc(result, 1, "0x58\tSizeOfHeapReserve\t0x%llX\tSize of the local heap space to reserve\n", optionalHeader->SizeOfHeapReserve);;
		sprintfJdc(result, 1, "0x60\tSizeOfHeapCommit\t0x%llX\tSize of the local heap space to commit\n", optionalHeader->SizeOfHeapCommit);
		sprintfJdc(result, 1, "0x68\tLoaderFlags\t0x%llX\tReserved, must be zero\n", optionalHeader->LoaderFlags);
		sprintfJdc(result, 1, "0x6C\tNumberOfRvaAndSizes\t0x%llX\tNumber of data-directory entries in the remainder of the optional header\n", optionalHeader->NumberOfRvaAndSizes);
	}
	else
	{
		sprintfJdc(result, 1, "0x4C\tSizeOfStackCommit\t0x%llX\tSize of the stack to commit\n", ((IMAGE_OPTIONAL_HEADER32*)optionalHeader)->SizeOfStackCommit);
		sprintfJdc(result, 1, "0x50\tSizeOfHeapReserve\t0x%llX\tSize of the local heap space to reserve\n", ((IMAGE_OPTIONAL_HEADER32*)optionalHeader)->SizeOfHeapReserve);;
		sprintfJdc(result, 1, "0x54\tSizeOfHeapCommit\t0x%llX\tSize of the local heap space to commit\n", ((IMAGE_OPTIONAL_HEADER32*)optionalHeader)->SizeOfHeapCommit);
		sprintfJdc(result, 1, "0x58\tLoaderFlags\t0x%llX\tReserved, must be zero\n", ((IMAGE_OPTIONAL_HEADER32*)optionalHeader)->LoaderFlags);
		sprintfJdc(result, 1, "0x5C\tNumberOfRvaAndSizes\t0x%llX\tNumber of data-directory entries in the remainder of the optional header\n", ((IMAGE_OPTIONAL_HEADER32*)optionalHeader)->NumberOfRvaAndSizes);
	}
}