#include "../Headers/peHandler.h"

unsigned char readCodeSection(HANDLE file, unsigned char* buffer, unsigned int bufferSize, IMAGE_SECTION_HEADER* codeSection)
{
	IMAGE_DOS_HEADER dosHeader = { 0 };
	if (SetFilePointer(file, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &dosHeader, sizeof(dosHeader), 0, 0)) { return 0; }

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) { return 0; }

	IMAGE_NT_HEADERS imageNtHeaders = { 0 };
	uintptr_t imageNtHeadersAddress = dosHeader.e_lfanew;
	if (SetFilePointer(file, imageNtHeadersAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
	if (!ReadFile(file, &imageNtHeaders, sizeof(imageNtHeaders), 0, 0)) { return 0; }

	for (int i = 0; i < imageNtHeaders.FileHeader.NumberOfSections; i++)
	{
		IMAGE_SECTION_HEADER sectionHeader = { 0 };
		uintptr_t sectionAddress = (sizeof(IMAGE_SECTION_HEADER) * i) + imageNtHeadersAddress + sizeof(imageNtHeaders.Signature) + sizeof(imageNtHeaders.FileHeader) + imageNtHeaders.FileHeader.SizeOfOptionalHeader;
		if (SetFilePointer(file, sectionAddress, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }
		if (!ReadFile(file, &sectionHeader, sizeof(sectionHeader), 0, 0)) { return 0; }

		if (sectionHeader.Characteristics & IMAGE_SCN_CNT_CODE)
		{
			if (SetFilePointer(file, sectionHeader.PointerToRawData, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) { return 0; }

			*codeSection = sectionHeader;

			return ReadFile(file, buffer, bufferSize, 0, 0);
		}
	}
}