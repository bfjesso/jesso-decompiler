#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>

#include "../Disassembler/disassembler.h"
#include "../Decompiler/decompiler.h"

void printHelp()
{
	printf("-h, --help: print this menu.\n\n");

	printf("-da, --disassemble: disassemble file or bytes into intel-style assembly.\n");
	printf("\tjdc -da [OPTIONS], [FILE PATH OR BYTES]\n\n");
	printf("\t-f: disassemble a file's .text section rather than a string of bytes.\n\n");
	printf("\t-x86: disassemble assuming the bytes or file are 32-bit.\n\n");
	printf("\t-a: show addresses (offset from .text section).\n\n");
	printf("\t-b: show bytes for each instruction.\n\n");

	printf("-dc, --decompile: decompile bytes into C.\n");
}

unsigned char disassembleBytes(unsigned char* bytes, unsigned char numOfBytes, unsigned char isX64, unsigned char showAddresses, unsigned char showBytes)
{
	if(numOfBytes == 0)
	{
		return 0;
	}

	struct DisassemblerOptions options;
	options.is64BitMode = isX64;

	unsigned int currentIndex = 0;

	struct DisassembledInstruction currentInstruction;
	while (disassembleInstruction(bytes + currentIndex, bytes + numOfBytes - 1, &options, &currentInstruction))
	{
		char buffer[50];
		if (instructionToStr(&currentInstruction, buffer, 50))
		{
			if(showAddresses) 
			{	
				printf("0x%X\t", currentIndex);
			}

			printf("%s", buffer); // assembly

			if(showBytes)
			{
				printf(";%+15s", "");
				for(int i = 0; i < currentInstruction.numOfBytes; i++)
				{
					printf("0x%X ", bytes[currentIndex + i]);
				}	
			}
			
			printf("\n");

			currentIndex += currentInstruction.numOfBytes;
		}
		else 
		{
			printf("Error converting instruction to string. Bytes: ");
			for(int i = 0; i < currentInstruction.numOfBytes; i++)
			{
				printf("0x%02X ", bytes[currentIndex + i]);
			}
			printf("\nThe opcode is likely not handled in the disassembler.\n");
			return 0;
		}
	}
	
	return 1;
}

unsigned char disassembleStringBytes(char* str, unsigned char isX64, unsigned char showAddresses, unsigned char showBytes)
{
	const unsigned char bytesBufferLen = 100;	
	unsigned char bytes[bytesBufferLen];
	
	int strLen = strlen(str);
	if (strLen < 2 || strLen % 2 != 0)
	{
		return 0;
	}

	int numOfBytes = 0;
	for (int i = 0; i < strLen-1; i += 2)
	{
		if (numOfBytes > bytesBufferLen - 1)
		{
			return 0;
		}

		char subStr[2];
		strncpy(subStr, str + i, 2);
		bytes[numOfBytes] = (unsigned char)strtol(subStr, 0, 16);

		numOfBytes++;
	}
	
	if(numOfBytes == 0)
	{
		printf("Failed to parse bytes. Enter them in the format: 0FBE450B....\n");
		return 0;
	}

	return disassembleBytes(bytes, numOfBytes, isX64, showAddresses, showBytes);
}

unsigned char disassembleFile64(char* filePath, unsigned char showAddresses, unsigned char showBytes)
{
	Elf64_Ehdr elfHeader;
	Elf64_Shdr sectionHeader;
	FILE* file = fopen(filePath, "r");

	unsigned char result = 0;

	if(file)
	{
		fread(&elfHeader, sizeof(elfHeader), 1, file);
		
		Elf64_Shdr nameStrTable;
		fseek(file, elfHeader.e_shoff + elfHeader.e_shstrndx * sizeof(nameStrTable), SEEK_SET);
		fread(&nameStrTable, 1, sizeof(nameStrTable), file);

		char* sectionNames = (char*)malloc(nameStrTable.sh_size);
		fseek(file, nameStrTable.sh_offset, SEEK_SET);
		fread(sectionNames, 1, nameStrTable.sh_size, file);

		if(memcmp(elfHeader.e_ident, ELFMAG, SELFMAG) == 0)
		{
			for(int i = 0; i < elfHeader.e_shnum; i++)
			{
				fseek(file, elfHeader.e_shoff + i * sizeof(sectionHeader), SEEK_SET);
				fread(&sectionHeader, 1, sizeof(sectionHeader), file);

				if(strcmp(sectionNames + sectionHeader.sh_name, ".text") == 0)
				{
					unsigned char* textSectionBytes = (unsigned char*)malloc(sectionHeader.sh_size);
					fseek(file, sectionHeader.sh_offset, SEEK_SET);
					fread(textSectionBytes, 1, sectionHeader.sh_size, file);
					result = disassembleBytes(textSectionBytes, sectionHeader.sh_size, 1, showAddresses, showBytes);
					free(textSectionBytes);
				}
			}
		}
		else
		{
			printf("Not a valid ELF binary.\n");
			fclose(file);
			free(sectionNames);
			return 0;
		}

		fclose(file);
		free(sectionNames);
	}
	else
	{
		printf("Failed to open file.\n");
		return 0;
	}

	return result;
}

// this should be a copy of disassembleFile64, but using the 32-bit Elf types
unsigned char disassembleFile32(char* filePath, unsigned char showAddresses, unsigned char showBytes) { return 1; }

int main(int argc, char* argv[])
{
	if(argc > 1)
	{
		unsigned char isDisassembling = 0;
		unsigned char isReadingFile = 0;
		unsigned char isX64 = 1;
		unsigned char showAddresses = 0;
		unsigned char showBytes = 0;
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
			else if(strcmp(argv[i], "-a") == 0)
			{
				showAddresses = 1;
			}
			else if(strcmp(argv[i], "-b") == 0)
			{
				showBytes = 1;
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
				unsigned char result = 0;
				if(isX64) { result = disassembleFile64(disassemblyInput, showAddresses, showBytes);   }
				else { result = disassembleFile32(disassemblyInput, showAddresses, showBytes);   }
				
				if(!result)
				{
					printf("Failed to disassemble file.\n");
					return 0;
				}
			}
			else if(!disassembleStringBytes(disassemblyInput, isX64, showAddresses, showBytes))
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
