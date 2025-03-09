#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>

#include "../Disassembler/Headers/disassembler.h"
#include "../Decompiler/Headers/decompiler.h"

void printHelp()
{
	printf("-h, --help: print this menu.\n\n");

	printf("-da, --disassemble: disassemble file or bytes into intel-style assembly.\n");
	printf("\tjdc -da [OPTIONS], [FILE PATH OR BYTES]\n\n");
	printf("\t-f: disassemble a file's .text section rather than a string of bytes.\n\n");
	printf("\t-x86: disassemble assuming the bytes or file are 32-bit.\n\n");

	printf("-dc, --decompile: decompile bytes into C.\n");
}

unsigned char parseStringBytes(char* str, unsigned char* bytesBuffer, unsigned char bytesBufferLen)
{
	int strLen = strlen(str);
	if (strLen < 2 || strLen % 2 != 0)
	{
		return 0;
	}

	int currentByteIndex = 0;
	for (int i = 0; i < strLen-1; i += 2)
	{
		if (currentByteIndex > bytesBufferLen - 1)
		{
			return 0;
		}

		char subStr[2];
		strncpy(subStr, str + i, 2);
		bytesBuffer[currentByteIndex] = (unsigned char)strtol(subStr, 0, 16);

		currentByteIndex++;
	}

	return currentByteIndex;
}

unsigned char disassembleStringBytes(char* bytesStr, unsigned char isX64)
{
	unsigned char bytes[100];
	unsigned char numOfBytes = parseStringBytes(bytesStr, bytes, 100);
	if(numOfBytes == 0)
	{
		printf("Failed to parse bytes. Enter them in the format: 0FBE450B....\n");
		return 0;
	}

	struct DisassemblerOptions options;
	options.is64BitMode = isX64;

	struct DisassembledInstruction currentInstruction;
	if (disassembleInstruction(bytes, bytes + numOfBytes - 1, &options, &currentInstruction))
	{
		char buffer[50];
		if (instructionToStr(&currentInstruction, buffer, 50))
		{
			printf("%s\n", buffer);
		}
		else 
		{
			printf("Error converting instruction to string.\n");
			printf("The opcode is likely not handled in the disassembler.\n");
		}
	}
	else
	{
		printf("Error disassembling the bytes.\n");
	}

	return 0;

}

unsigned char disassembleFile(char* filePath, unsigned char isX64)
{
#if isX64
#define ElfW(type) Elf64_ ## type
#else
#define ElfW(type) Elf32_ ## type
#endif
	
	ElfW(Ehdr) elfHeader;
	ElfW(Shdr) textSectionHeader;
	FILE* file = fopen(filePath, "r");

	if(file)
	{
		fread(&elfHeader, sizeof(elfHeader), 1, file);

		if(memcmp(elfHeader.e_ident, ELFMAG, SELFMAG) == 0)
		{
			for(int i = 0; i < elfHeader.e_shnum; i++)
			{
				fseek(file, elfHeader.e_shoff + i * sizeof(textSectionHeader), SEEK_SET);
				fread(&textSectionHeader, 1, sizeof(textSectionHeader), file);

				printf("Section %d: %s\n", i, textSectionHeader.sh_name);
			}
		}
		else
		{
			printf("Not a valid ELF binary.\n");
			close(file);
			return 0;
		}

		close(file);
	}
	else
	{
		printf("Failed to open file.\n");
		return 0;
	}

	return 1;
}

int main(int argc, char* argv[])
{
	if(argc > 1)
	{
		unsigned char isDisassembling = 0;
		unsigned char isReadingFile = 0;
		unsigned char isX64 = 1;
		char* disassemblyInput = 0;
		
		for(int i = 1; i < argc; i++)
		{
			if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
			{
				printHelp();
				return 0;
			}

			if(strcmp(argv[i], "-da") == 0 || strcmp(argv[i], "--disassemble") == 0)
			{
				isDisassembling = 1;	
			}
			else if(strcmp(argv[i], "-f") == 0)
			{
				isReadingFile = 1;
			}
			else if(strcmp(argv[i], "-x86") == 0)
			{
				isX64 = 0;
			}
			else if(i == argc - 1)
			{
				disassemblyInput = argv[i];
			}
			
		}

		if(isDisassembling)
		{
			if(!disassemblyInput)
			{
				printf("You need to either pass a string of bytes, or a file path to disassemble.\n");
				return 0;
			}
			
			if(isReadingFile)
			{
				if(!disassembleFile(disassemblyInput, isX64))
				{
					printf("Failed to disassemble file.\n");
					return 0;
				}
			}
			else if(!disassembleStringBytes(disassemblyInput, isX64))
			{
				printf("Failed to disassemble bytes.\n");
				return 0;
			}		
		}
	}
	else
	{
		printf("Expected arguments. Use -h or --help for help.\n");
		return 0;

	}
}
