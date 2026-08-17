#include "fileHandler.h"
#include "../pe-handler/peHandler.h"
#include "../elf-handler/elfHandler.h"

#ifdef _WIN32
#include "dbghelp.h"
#pragma comment(lib, "dbghelp.lib")
#endif

#ifdef linux
extern char* __cxa_demangle(const char* mangled_name, char* output_buffer, size_t* length, int* status);
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
}

unsigned char demangleCppSymbol(char* mangledStr, char* buffer, int bufferLen) 
{
#ifdef _WIN32
	if (UnDecorateSymbolName(mangledStr, buffer, bufferLen, UNDNAME_NAME_ONLY))
	{
		// removing template parameters
		size_t nameLen = strlen(buffer);
		int k = 0;
		int openIndex = -1;
		int openNum = 0;
		int closeNum = 0;
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

		return 1;
	}

	return 0;
#endif

#ifdef linux
	int status = 0;
	char* demangleResult = __cxa_demangle(mangledStr, 0, 0, &status);
	if (status == 0)
	{
		if (bufferLen <= strlen(demangleResult)) 
		{
			free(demangleResult);
			return 0;
		}

		int startIndex = 0;
		int i = 0;
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

		int numOfBrakets = 0;
		int bufferIndex = 0;
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
		return 1;
	}

	free(demangleResult);
	return 0;
#endif
}

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
	return getPEImageBase(filePath, is64Bit);
#endif

#ifdef linux
	// not done
	return 0;
#endif
}

unsigned long long getFileEntryPoint(const wchar_t* filePath, unsigned char is64Bit)
{
#ifdef _WIN32
	return getPEEntryPoint(filePath, is64Bit);
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
	return getNumOfPESections(filePath, is64Bit);
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
	return getAllPESectionHeaders(filePath, is64Bit, buffer, bufferLen);
#endif

#ifdef linux
	char filePathChar[255] = { 0 };
	wcstombs(filePathChar, filePath, 254);
	return getAllELFSectionHeaders(filePathChar, is64Bit, buffer, bufferLen);
#endif
}

unsigned int getNumOfFileBytes(const wchar_t* filePath)
{
	FILE* file = openFile(filePath);
	if (file)
	{
		fseek(file, 0, SEEK_END);
		unsigned int result = ftell(file);
		fclose(file);
		return result;
	}

	return 0;
}

unsigned char readFileBytes(const wchar_t* filePath, unsigned char* buffer, unsigned int bufferSize)
{
	FILE* file = openFile(filePath);
	if (file)
	{
		fseek(file, 0, SEEK_SET);
		unsigned int bytesRead = fread(buffer, 1, bufferSize, file);
		fclose(file);
		return bytesRead == bufferSize;
	}

	return 0;
}

unsigned char getSymbolByValue(const wchar_t* filePath, unsigned char is64Bit, unsigned int value, struct JdcStr* result)
{
#ifdef _WIN32
	return getPESymbolByValue(filePath, is64Bit, value, result);
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
	return getNumOfPEImports(filePath, is64Bit);
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
	return getAllPEImports(filePath, is64Bit, buffer, bufferLen);
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
	return generatePEHeadersInfoStr(filePath, result);
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