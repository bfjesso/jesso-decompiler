#include "peHandler.h"

unsigned char readCodeSection(const wchar_t* filePath, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* codeSection, uintptr_t* imageBase, unsigned char* is64Bit)
{
	HANDLE file = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	
	if (file == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	DWORD binaryType = 0;
	if (GetBinaryTypeW(filePath, &binaryType))
	{
		if (binaryType == SCS_32BIT_BINARY)
		{
			*is64Bit = 0;
			return readCodeSection32(file, buffer, bufferSize, codeSection, imageBase);
		}
		else if (binaryType == SCS_64BIT_BINARY)
		{
			*is64Bit = 1;
			return readCodeSection64(file, buffer, bufferSize, codeSection, imageBase);
		}
	}

	CloseHandle(file);
	return 0;
}

unsigned char readDataSection(const wchar_t* filePath, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* dataSection, uintptr_t* imageBase)
{
	HANDLE file = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (file == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	DWORD binaryType = 0;
	if (GetBinaryTypeW(filePath, &binaryType))
	{
		if (binaryType == SCS_32BIT_BINARY)
		{
			return readDataSection32(file, buffer, bufferSize, dataSection, imageBase);
		}
		else if (binaryType == SCS_64BIT_BINARY)
		{
			return readDataSection64(file, buffer, bufferSize, dataSection, imageBase);
		}
	}

	CloseHandle(file);
	return 0;
}

static unsigned char readCodeSection32(HANDLE file, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* codeSection, uintptr_t* imageBase) 
{
	IMAGE_DOS_HEADER dosHeader = { 0 };
	if (SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { CloseHandle(file); return 0; }
	if (!ReadFile(file, &dosHeader, sizeof(dosHeader), 0, 0)) { CloseHandle(file); return 0; }

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) { CloseHandle(file); return 0; }
	
	IMAGE_NT_HEADERS32 imageNtHeaders = { 0 };
	uintptr_t imageNtHeadersAddress = dosHeader.e_lfanew;
	if (SetFilePointer(file, imageNtHeadersAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { CloseHandle(file); return 0; }
	if (!ReadFile(file, &imageNtHeaders, sizeof(imageNtHeaders), 0, 0)) { CloseHandle(file); return 0; }

	*imageBase = imageNtHeaders.OptionalHeader.ImageBase;

	for (int i = 0; i < imageNtHeaders.FileHeader.NumberOfSections; i++)
	{
		IMAGE_SECTION_HEADER sectionHeader = { 0 };
		uintptr_t sectionAddress = (sizeof(IMAGE_SECTION_HEADER) * i) + imageNtHeadersAddress + sizeof(imageNtHeaders.Signature) + sizeof(imageNtHeaders.FileHeader) + imageNtHeaders.FileHeader.SizeOfOptionalHeader;
		if (SetFilePointer(file, sectionAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { CloseHandle(file); return 0; }
		if (!ReadFile(file, &sectionHeader, sizeof(sectionHeader), 0, 0)) { CloseHandle(file); return 0; }

		if (sectionHeader.Characteristics & IMAGE_SCN_CNT_CODE)
		{
			if (SetFilePointer(file, sectionHeader.PointerToRawData, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { CloseHandle(file); return 0; }

			*codeSection = sectionHeader;

			BOOL result = ReadFile(file, buffer, bufferSize, 0, 0);
			CloseHandle(file);

			return result;
		}
	}

	CloseHandle(file);
	return 0;
}

static unsigned char readCodeSection64(HANDLE file, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* codeSection, uintptr_t* imageBase)
{
	IMAGE_DOS_HEADER dosHeader = { 0 };
	if (SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { CloseHandle(file); return 0; }
	if (!ReadFile(file, &dosHeader, sizeof(dosHeader), 0, 0)) { CloseHandle(file); return 0; }

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) { CloseHandle(file); return 0; }

	IMAGE_NT_HEADERS64 imageNtHeaders = { 0 };
	uintptr_t imageNtHeadersAddress = dosHeader.e_lfanew;
	if (SetFilePointer(file, imageNtHeadersAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { CloseHandle(file); return 0; }
	if (!ReadFile(file, &imageNtHeaders, sizeof(imageNtHeaders), 0, 0)) { CloseHandle(file); return 0; }

	*imageBase = imageNtHeaders.OptionalHeader.ImageBase;

	for (int i = 0; i < imageNtHeaders.FileHeader.NumberOfSections; i++)
	{
		IMAGE_SECTION_HEADER sectionHeader = { 0 };
		uintptr_t sectionAddress = (sizeof(IMAGE_SECTION_HEADER) * i) + imageNtHeadersAddress + sizeof(imageNtHeaders.Signature) + sizeof(imageNtHeaders.FileHeader) + imageNtHeaders.FileHeader.SizeOfOptionalHeader;
		if (SetFilePointer(file, sectionAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { CloseHandle(file); return 0; }
		if (!ReadFile(file, &sectionHeader, sizeof(sectionHeader), 0, 0)) { CloseHandle(file); return 0; }

		if (sectionHeader.Characteristics & IMAGE_SCN_CNT_CODE)
		{
			if (SetFilePointer(file, sectionHeader.PointerToRawData, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { CloseHandle(file); return 0; }

			*codeSection = sectionHeader;

			BOOL result = ReadFile(file, buffer, bufferSize, 0, 0);
			CloseHandle(file);

			return result;
		}
	}

	CloseHandle(file);
	return 0;
}

static unsigned char readDataSection32(HANDLE file, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* dataSection, uintptr_t* imageBase)
{
	IMAGE_DOS_HEADER dosHeader = { 0 };
	if (SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { CloseHandle(file); return 0; }
	if (!ReadFile(file, &dosHeader, sizeof(dosHeader), 0, 0)) { CloseHandle(file); return 0; }

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) { CloseHandle(file); return 0; }

	IMAGE_NT_HEADERS32 imageNtHeaders = { 0 };
	uintptr_t imageNtHeadersAddress = dosHeader.e_lfanew;
	if (SetFilePointer(file, imageNtHeadersAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { CloseHandle(file); return 0; }
	if (!ReadFile(file, &imageNtHeaders, sizeof(imageNtHeaders), 0, 0)) { CloseHandle(file); return 0; }

	*imageBase = imageNtHeaders.OptionalHeader.ImageBase;

	for (int i = 0; i < imageNtHeaders.FileHeader.NumberOfSections; i++)
	{
		IMAGE_SECTION_HEADER sectionHeader = { 0 };
		uintptr_t sectionAddress = (sizeof(IMAGE_SECTION_HEADER) * i) + imageNtHeadersAddress + sizeof(imageNtHeaders.Signature) + sizeof(imageNtHeaders.FileHeader) + imageNtHeaders.FileHeader.SizeOfOptionalHeader;
		if (SetFilePointer(file, sectionAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { CloseHandle(file); return 0; }
		if (!ReadFile(file, &sectionHeader, sizeof(sectionHeader), 0, 0)) { CloseHandle(file); return 0; }

		if (sectionHeader.Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA)
		{
			if (SetFilePointer(file, sectionHeader.PointerToRawData, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { CloseHandle(file); return 0; }

			*dataSection = sectionHeader;

			BOOL result = ReadFile(file, buffer, bufferSize, 0, 0);
			CloseHandle(file);

			return result;
		}
	}

	CloseHandle(file);
	return 0;
}

static unsigned char readDataSection64(HANDLE file, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* dataSection, uintptr_t* imageBase)
{
	IMAGE_DOS_HEADER dosHeader = { 0 };
	if (SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { CloseHandle(file); return 0; }
	if (!ReadFile(file, &dosHeader, sizeof(dosHeader), 0, 0)) { CloseHandle(file); return 0; }

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) { CloseHandle(file); return 0; }

	IMAGE_NT_HEADERS64 imageNtHeaders = { 0 };
	uintptr_t imageNtHeadersAddress = dosHeader.e_lfanew;
	if (SetFilePointer(file, imageNtHeadersAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { CloseHandle(file); return 0; }
	if (!ReadFile(file, &imageNtHeaders, sizeof(imageNtHeaders), 0, 0)) { CloseHandle(file); return 0; }

	*imageBase = imageNtHeaders.OptionalHeader.ImageBase;

	for (int i = 0; i < imageNtHeaders.FileHeader.NumberOfSections; i++)
	{
		IMAGE_SECTION_HEADER sectionHeader = { 0 };
		uintptr_t sectionAddress = (sizeof(IMAGE_SECTION_HEADER) * i) + imageNtHeadersAddress + sizeof(imageNtHeaders.Signature) + sizeof(imageNtHeaders.FileHeader) + imageNtHeaders.FileHeader.SizeOfOptionalHeader;
		if (SetFilePointer(file, sectionAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { CloseHandle(file); return 0; }
		if (!ReadFile(file, &sectionHeader, sizeof(sectionHeader), 0, 0)) { CloseHandle(file); return 0; }

		if (sectionHeader.Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA)
		{
			if (SetFilePointer(file, sectionHeader.PointerToRawData, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { CloseHandle(file); return 0; }

			*dataSection = sectionHeader;

			BOOL result = ReadFile(file, buffer, bufferSize, 0, 0);
			CloseHandle(file);

			return result;
		}
	}

	CloseHandle(file);
	return 0;
}
