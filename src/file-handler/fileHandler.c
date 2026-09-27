#include "fileHandler.h"
#include "../pe-handler/peHandler.h"
#include "../elf-handler/elfHandler.h"

#ifdef _WIN32
#include "dbghelp.h"
#pragma comment(lib, "dbghelp.lib")
#endif

#ifdef linux
extern char* __cxa_demangle(const char* mangled_name, char* output_buffer, size_t* length, int32_t* status);
#endif

FILE* openFile(const wchar_t* filePath)
{
#ifdef _WIN32
	return _wfopen(filePath, L"rb");
#endif

#ifdef linux
	char filePathChar[255] = { 0 };
	wcstombs(filePathChar, filePath, 254);
	return fopen(filePathChar, "rb");
#endif

	return 0;
}

int32_t seek64(FILE* file, uint64_t fileOffset) 
{
#ifdef _WIN32
	return _fseeki64(file, fileOffset, SEEK_SET);
#endif

#ifdef linux
	return fseeko(file, fileOffset, SEEK_SET);
#endif

	return -1;
}

enum JdcStatus demangleCppSymbol(char* mangledStr, char* buffer, size_t bufferLen)
{
#ifdef _WIN32
	if (UnDecorateSymbolName(mangledStr, buffer, (DWORD)bufferLen, UNDNAME_NAME_ONLY))
	{
		// removing template parameters
		size_t nameLen = strlen(buffer);
		int32_t k = 0;
		int32_t openIndex = -1;
		int32_t openNum = 0;
		int32_t closeNum = 0;
		while (buffer[k] != 0)
		{
			if (buffer[k] == '<')
			{
				if (openIndex == -1) { openIndex = k; }
				openNum++;
			}
			else if (buffer[k] == '>')
			{
				closeNum++;

				if (closeNum == openNum)
				{
					size_t len = strlen(buffer + k + 1);
					memcpy(buffer + openIndex, buffer + k + 1, len);
					memset(buffer + openIndex + len, 0, nameLen - (openIndex + len));

					openIndex = -1;
					openNum = 0;
					closeNum = 0;
				}

			}

			k++;
		}

		return SUCCESS_JDC;
	}

	return ERROR_JDC;
#endif

#ifdef linux
	int32_t status = 0;
	char* demangleResult = __cxa_demangle(mangledStr, 0, 0, &status);
	if (status == 0)
	{
		if (bufferLen <= strlen(demangleResult)) 
		{
			free(demangleResult);
			return ERROR_JDC;
		}

		int32_t startIndex = 0;
		int32_t i = 0;
		while (demangleResult[i] != 0)
		{
			if (demangleResult[i] == ' ') // this is looking for the return type
			{
				startIndex = i + 1;
				break;
			}
			else if (demangleResult[i] == '<' || demangleResult[i] == '(')
			{
				break;
			}

			i++;
		}

		int32_t numOfBrakets = 0;
		int32_t bufferIndex = 0;
		i = startIndex;
		while (demangleResult[i] != 0)
		{
			if (demangleResult[i] == '<' || demangleResult[i] == '(')
			{
				numOfBrakets++;
			}
			else if (demangleResult[i] == '>' || demangleResult[i] == ')')
			{
				numOfBrakets--;
			}
			else if (numOfBrakets == 0 && demangleResult[i] != ' ')
			{
				buffer[bufferIndex] = demangleResult[i];
				bufferIndex++;
			}

			i++;
		}

		free(demangleResult);
		return SUCCESS_JDC;
	}

	free(demangleResult);
	return ERROR_JDC;
#endif
}

enum JdcStatus identifyFileFormat(const wchar_t* filePath, enum FileFormat* fileFormat)
{
	if (!fileFormat) 
	{
		return ERROR_JDC;
	}
	
	bool isPE = false;
	if (ERROR_JDC == isFilePE(filePath, &isPE)) 
	{
		return ERROR_JDC;
	}

	if (isPE) 
	{
		*fileFormat = PE_FF;
		return SUCCESS_JDC;
	}

	bool isELF = false;
	if (ERROR_JDC == isFileELF(filePath, &isELF))
	{
		return ERROR_JDC;
	}

	if (isELF)
	{
		*fileFormat = ELF_FF;
		return SUCCESS_JDC;
	}

	*fileFormat = UNKNOWN_FF;
	return SUCCESS_JDC;
}

const char* fileFormatToStr(enum FileFormat fileFormat) 
{
	switch (fileFormat) 
	{
	case UNKNOWN_FF:
		return "Unknown format";
	case PE_FF:
		return "Portable executable (PE)";
	case ELF_FF:
		return "Executable and linkable format (ELF)";
	}

	return "";
}

const char* fileSectionTypeToStr(enum FileSectionType fileSectionType) 
{
	switch (fileSectionType)
	{
	case OTHER_FST:
		return "Unknown";
	case CODE_FST:
		return "Code";
	case INIT_DATA_FST:
		return "Initialized data";
	case UNINIT_DATA_FST:
		return "Uninitialized data";
	}

	return "";
}

enum JdcStatus isFile64Bit(const wchar_t* filePath, enum FileFormat fileFormat, bool* is64BitRef)
{
	if (fileFormat == PE_FF) 
	{
		return isPEX64(filePath, is64BitRef);
	}
	else if (fileFormat == ELF_FF) 
	{
		return isELFX64(filePath, is64BitRef);
	}

	return ERROR_JDC;
}

enum JdcStatus getFileImageBase(const wchar_t* filePath, enum FileFormat fileFormat, bool is64Bit, uint64_t* imageBaseRef)
{
	if (fileFormat == PE_FF)
	{
		return getPEImageBase(filePath, is64Bit, imageBaseRef);
	}
	else if (fileFormat == ELF_FF)
	{
		*imageBaseRef = 0; // ELF files do not have an image base like PE files
		return SUCCESS_JDC;
	}

	return ERROR_JDC;
}

enum JdcStatus getFileEntryPoint(const wchar_t* filePath, enum FileFormat fileFormat, bool is64Bit, uint64_t* entryPointRef)
{
	if (fileFormat == PE_FF)
	{
		return getPEEntryPoint(filePath, is64Bit, entryPointRef);
	}
	else if (fileFormat == ELF_FF)
	{
		return getELFEntryPoint(filePath, is64Bit, entryPointRef);
	}

	return ERROR_JDC;
}

enum JdcStatus getNumOfSections(const wchar_t* filePath, enum FileFormat fileFormat, bool is64Bit, int32_t* numOfSectionsRef)
{
	if (fileFormat == PE_FF)
	{
		return getNumOfPESections(filePath, is64Bit, numOfSectionsRef);
	}
	else if (fileFormat == ELF_FF)
	{
		return getNumOfELFSections(filePath, is64Bit, numOfSectionsRef);
	}

	return ERROR_JDC;
}

enum JdcStatus getAllFileSectionHeaders(const wchar_t* filePath, enum FileFormat fileFormat, bool is64Bit, struct FileSection* buffer, int32_t bufferLen)
{
	if (fileFormat == PE_FF)
	{
		return getAllPESectionHeaders(filePath, is64Bit, buffer, bufferLen);
	}
	else if (fileFormat == ELF_FF)
	{
		return getAllELFSectionHeaders(filePath, is64Bit, buffer, bufferLen);
	}

	return ERROR_JDC;
}

enum JdcStatus getNumOfFileBytes(const wchar_t* filePath, uint64_t* numOfBytesRef)
{
	FILE* file = openFile(filePath);
	if (file && numOfBytesRef)
	{
		uint64_t result = 0;

#ifdef _WIN32
		_fseeki64(file, 0, SEEK_END);
		result = _ftelli64(file);
#endif

#ifdef linux
		fseeko(file, 0, SEEK_END);
		result = ftello(file);
#endif

		fclose(file);
		*numOfBytesRef = result;
		return SUCCESS_JDC;
	}

	return ERROR_JDC;
}

enum JdcStatus readFileBytes(const wchar_t* filePath, uint8_t* buffer, uint64_t bufferSize)
{
	FILE* file = openFile(filePath);
	if (file)
	{
		seek64(file, 0);
		size_t bytesRead = fread(buffer, 1, bufferSize, file);
		fclose(file);

		if (bytesRead == bufferSize) 
		{
			return SUCCESS_JDC;
		}
	}

	return ERROR_JDC;
}

enum JdcStatus getSymbolByValue(const wchar_t* filePath, enum FileFormat fileFormat, bool is64Bit, uint32_t value, struct JdcStr* result)
{
	if (fileFormat == PE_FF)
	{
		return getPESymbolByValue(filePath, is64Bit, value, result);
	}
	else if (fileFormat == ELF_FF)
	{
		return getELFSymbolByValue(filePath, is64Bit, value, result);
	}

	return ERROR_JDC;
}

enum JdcStatus getNumOfImports(const wchar_t* filePath, enum FileFormat fileFormat, bool is64Bit, int32_t* numOfImportsRef, int32_t* numOfLibrariesRef)
{
	if (fileFormat == PE_FF)
	{
		return getNumOfPEImports(filePath, is64Bit, numOfImportsRef, numOfLibrariesRef);
	}
	else if (fileFormat == ELF_FF)
	{
		return getNumOfELFImports(filePath, is64Bit, numOfImportsRef, numOfLibrariesRef);
	}

	return ERROR_JDC;
}

enum JdcStatus getAllImports(const wchar_t* filePath, enum FileFormat fileFormat, bool is64Bit, struct ImportedFunction* importsBuffer, int32_t importsBufferLen, struct JdcStr* libraryNamesBuffer, int32_t libraryNamesBufferLen)
{
	if (fileFormat == PE_FF)
	{
		return getAllPEImports(filePath, is64Bit, importsBuffer, importsBufferLen, libraryNamesBuffer, libraryNamesBufferLen);
	}
	else if (fileFormat == ELF_FF)
	{
		return getAllELFImports(filePath, is64Bit, importsBuffer, importsBufferLen, libraryNamesBuffer, libraryNamesBufferLen);
	}

	return ERROR_JDC;
}

enum JdcStatus generateFileHeadersInfoStr(const wchar_t* filePath, enum FileFormat fileFormat, struct JdcStr* result)
{
	if (fileFormat == PE_FF)
	{
		return generatePEHeadersInfoStr(filePath, result);
	}
	else if (fileFormat == ELF_FF)
	{
		return generateELFHeadersInfoStr(filePath, result);
	}

	return ERROR_JDC;
}

uint64_t rvaToFileOffset(struct FileSection* sections, int32_t numOfSections, uint64_t rva, struct FileSection** section)
{
	if (section) { *section = 0; }
	
	uint64_t maybeResult = 0;
	for (int32_t i = 0; i < numOfSections; i++)
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