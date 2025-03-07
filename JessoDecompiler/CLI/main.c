#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Disassembler/Headers/disassembler.h"
#include "../Decompiler/Headers/decompiler.h"

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

int main(int argc, char* argv[])
{
	if(argc > 1)
	{
		if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
		{
			printf("-h, --help: print this menu.\n");
			printf("-da, --disassemble: disassemble bytes into intel-style assembly.\n");
			printf("-dc, --decompile: decompile bytes into C.\n");
			return 0;
		}

		if(strcmp(argv[1], "-da") == 0 || strcmp(argv[1], "--disassemble") == 0)
		{
			if(argc == 3)
			{
				char* bytesStr = argv[2];

				unsigned char bytes[100];
				unsigned char numOfBytes = parseStringBytes(bytesStr, bytes, 100);
				if(numOfBytes == 0)
				{
					printf("Failed to parse bytes. Enter them in the format: 0FBE450B....\n");
					return 0;
				}

				struct DisassemblerOptions options;
				options.is64BitMode = 0;

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
			else
			{
				printf("You need to pass the bytes you want to disassemble.\n");
				return 0;
			}
		}
	}

	printf("Expected arguments. Use -h or --help for help.\n");
	return 0;
}
