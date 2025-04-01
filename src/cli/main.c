#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../disassembler/disassembler.h"
#include "../decompiler/decompiler.h"
#include "../elf-handler/elfHandler.h"

void printHelp()
{
	printf("-h, --help: print this menu.\n\n");

	printf("When disassembling or decompiling a file, the .text section is what will be read.\n\n");

	printf("-da, --disassemble: disassemble file or bytes into intel-style assembly.\n");
	printf("\tjdc -da [OPTIONS] [FILE PATH OR BYTES]\n\n");
	printf("\t-s: disassemble a string of bytes rather than a file.\n\n");
	printf("\t-x64: disassemble assuming the bytes or file are 64-bit.\n\n");
	printf("\t-x86: disassemble assuming the bytes or file are 32-bit.\n\n");
	printf("\t-a: show addresses (file offset).\n\n");
	printf("\t-b: show bytes for each instruction.\n\n");
	printf("\t-ob: show only bytes for each instruction (no disassembly).\n\n");

	printf("-dc, --decompile: decompile functions into C.\n");
	printf("\tjdc -dc [OPTIONS] [FILE PATH]\n\n");
	printf("\t-x64: decompile assuming the file is 64-bit.\n\n");
	printf("\t-x86: decompile assuming the file is 32-bit.\n\n");
	printf("\t-ns: do not get symbols.\n\n");
}

unsigned char disassembleBytes(unsigned char* bytes, unsigned int numOfBytes, unsigned char isX64, struct DisassembledInstruction** instructionsBufferRef, unsigned long long** addressesBufferRef, int* numOfInstructions, unsigned char print, unsigned char showAddresses, unsigned long long startAddr, unsigned char showBytes, unsigned char showOnlyBytes)
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
		if(!print)
		{
			if(!instructionsBufferRef || !(*instructionsBufferRef) || !(*addressesBufferRef) || !numOfInstructions)
			{
				printf("Bad ptr passed to disassembleBytes.\n");
				return 0;
			}
			
			if(currentIndex == 0)
			{
				*(numOfInstructions) = 0;
			}

			(*instructionsBufferRef)[*numOfInstructions] = currentInstruction;
			(*addressesBufferRef)[*numOfInstructions] = startAddr + currentIndex;
			
			currentIndex += currentInstruction.numOfBytes;	
			(*numOfInstructions)++;
			memset(&currentInstruction, 0, sizeof(currentInstruction));
			continue;
		}
		else if(currentIndex == 0)
		{
			if(showOnlyBytes)
			{
				printf("Bytes");
			}
			else
			{
				if(showAddresses)
				{	
					printf("%-*s", 15, "Address");
				}

				printf("%-*s", 50, "Assembly");
				
				if(showBytes)
				{	
					printf("Bytes");
				}
			}

			printf("\n");
		}

		if(showAddresses) 
		{	
			printf("%#-*X", 15, currentIndex + startAddr);
		}

		if(showOnlyBytes)
		{
			for(int i = 0; i < currentInstruction.numOfBytes; i++)
			{
				printf("0X%02X ", bytes[currentIndex + i]);
			}
			currentIndex += currentInstruction.numOfBytes;
			printf("\n");
			memset(&currentInstruction, 0, sizeof(currentInstruction));
			continue;
		}
	
		char buffer[50];
		if (instructionToStr(&currentInstruction, buffer, 50))
		{	
			printf("%-*s", 50, buffer); // assembly

			if(showBytes)
			{
				for(int i = 0; i < currentInstruction.numOfBytes; i++)
				{
					printf("0X%02X ", bytes[currentIndex + i]);
				}	
			}

			currentIndex += currentInstruction.numOfBytes;
			printf("\n");
		}
		else 
		{
			printf("Error converting instruction to string. Bytes: ");
			for(int i = 0; i < currentInstruction.numOfBytes; i++)
			{
				printf("0X%02X ", bytes[currentIndex + i]);
			}
			printf("\nThe opcode is likely not handled in the disassembler.\n");
			return 0;
		}

		memset(&currentInstruction, 0, sizeof(currentInstruction));
	}

	if(currentIndex < numOfBytes)
	{
		printf("Failed to disassemble all bytes...\n");
		printf("Num of bytes disassemled: %d\n", currentIndex);
		printf("Total num of bytes: %d\n", numOfBytes);
	}

	return 1;
}

unsigned char disassembleStringBytes(char* str, unsigned char isX64)
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

	return disassembleBytes(bytes, numOfBytes, isX64, 0, 0, 0, 1, 0, 0, 0, 0);
}

unsigned char disassembleFile(char* filePath, unsigned char isX64, struct DisassembledInstruction** instructionsBufferRef, unsigned long long** addressesBufferRef, int* numOfInstructions, unsigned char print, unsigned char showAddresses, unsigned char showBytes, unsigned char showOnlyBytes)
{
	char* bytes;

	int sectionSize = 0;
	unsigned long long startAddress = 0;
	if(isX64) { sectionSize = getSectionBytesByName64(filePath, ".text", &bytes, &startAddress); }
	else { sectionSize = getSectionBytesByName32(filePath, ".text", &bytes, &startAddress); }
	
	if(sectionSize == 0)
	{
		return 0;
	}
	
	if(!print)
	{
		*instructionsBufferRef = (struct DisassembledInstruction*)malloc(sectionSize * sizeof(struct DisassembledInstruction)); // to be safe, assume each byte is one instruction
		*addressesBufferRef = (unsigned long long*)malloc(sectionSize * sizeof(unsigned long long));
	}

	unsigned char result = disassembleBytes(bytes, sectionSize, isX64, instructionsBufferRef, addressesBufferRef, numOfInstructions, print, showAddresses, startAddress, showBytes, showOnlyBytes);
	free(bytes);
	
	return result;
}

unsigned char findAllFunctions(struct DisassembledInstruction* instructions, unsigned long long* addresses, int numOfInstructions, struct Function** functionsBufferRef, int functionsBufferLen, int* numOfFunctions) 
{
	if(!functionsBufferRef || !(*functionsBufferRef)) { return 0; }
	
	*numOfFunctions = 0;

	struct Function currentFunction = {};
	int instructionIndex = 0;
	while (findNextFunction(&instructions[instructionIndex], &addresses[instructionIndex], numOfInstructions, &currentFunction, &instructionIndex) && *numOfFunctions < functionsBufferLen)
	{
		currentFunction.name[0] = 0;
		sprintf(currentFunction.name, "func%X", (*currentFunction.addresses));

		numOfInstructions -= currentFunction.numOfInstructions;

		(*functionsBufferRef)[*(numOfFunctions)] = currentFunction;
		(*numOfFunctions)++;
	}

	if ((*numOfFunctions) > 0) 
	{
		fixAllFunctionReturnTypes(*functionsBufferRef, *(numOfFunctions));
		return 1;
	}

	return 0;
}

int main(int argc, char* argv[])
{
	if(argc > 1)
	{	
		unsigned char isDecompiling = 0;
		unsigned char isDisassembling = 0;
		unsigned char isReadingFile = 1;
		unsigned char isX64 = 2;
		unsigned char showAddresses = 0;
		unsigned char showBytes = 0;
		unsigned char showOnlyBytes = 0;
		unsigned char getSymbols = 1;
		char* input = 0;
		
		for(int i = 1; i < argc; i++)
		{
			if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
			{
				printHelp();
				return 0;
			}

			if(strcmp(argv[i], "-dc") == 0 || strcmp(argv[i], "--decompile") == 0)
			{
				isDecompiling = 1;
			}
			else if(strcmp(argv[i], "-da") == 0 || strcmp(argv[i], "--disassemble") == 0)
			{
				isDisassembling = 1;	
			}
			else if(strcmp(argv[i], "-s") == 0)
			{
				isReadingFile = 0;
			}
			else if(strcmp(argv[i], "-x64") == 0)
			{
				isX64 = 1;
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
			else if(strcmp(argv[i], "-ob") == 0)
			{
				showOnlyBytes = 1;
			}
			else if(strcmp(argv[i], "-ns") == 0)
			{
				getSymbols = 0;
			}
			else if(i == argc - 1)
			{
				input = argv[i];
			}
			
		}

		if(isDecompiling)
		{
			if(!input)
			{
				printf("You need to pass a file path to decompile.\n");
				return 0;
			}

			if(isX64 == 2)
			{
				if(!isFileX64(input, &isX64))
				{
					printf("Failed to determine file architecture.\n");
					return 0;
				}
			}

			struct DisassembledInstruction* instructions = 0;
			unsigned long long* addresses = 0;
			int numOfInstructions = 0;
			unsigned char result = disassembleFile(input, isX64, &instructions, &addresses, &numOfInstructions, 0, showAddresses, showBytes, showOnlyBytes);

			if(!result || !instructions || numOfInstructions == 0)
			{
				printf("Error disassembling the file.\n");
				if(instructions) { free(instructions); }
				if(addresses) { free(addresses); }
				return 0;
			}

			int numOfFunctions = 0;
			struct Function* functions = (struct Function*)malloc(100 * sizeof(struct Function));
			if(!findAllFunctions(instructions, addresses, numOfInstructions, &functions, 100, &numOfFunctions))
			{
				printf("Failed to find all functions.\n");
				if(instructions) { free(instructions); }
				if(addresses) { free(addresses); }
				if(functions) { free(functions); }
				return 0;
			}
		
			if(getSymbols)
			{
				for(int i = 0; i < numOfFunctions; i++)
				{
					getSymbolNameByValue(input, functions[i].addresses[0], functions[i].name);
				}
			}

			printf("(jdc) ");
			
			int functionNum = -1;

			char userInput[10];
			while(scanf("%s", userInput)) 
			{	
				if(strcmp(userInput, "h") == 0)
				{
					printf("l: list all functions\n");
					printf("s: select function\n");
					printf("dc: decompile selected function\n");
					printf("q: unselect function, exit if none selected\n");
				}
				else if(strcmp(userInput, "l") == 0)
				{
					printf("Index\tAddress\tName\n");
					for(int i = 0; i < numOfFunctions; i++)
					{
						printf("%d\t", i);
						printf("%#X\t", functions[i].addresses[0]);
						printf("%s\n", functions[i].name);
					}
				}
				else if(strcmp(userInput, "s") == 0)
				{
					printf("Enter function to decompile (index): ");
					if(!scanf("%d", &functionNum) || functionNum < 0 || functionNum >= numOfFunctions)
					{
						printf("Enter a valid index. Use l to list all functions.\n");
					}
				}
				else if(strcmp(userInput, "p") == 0)
				{
					if(functionNum != -1)
					{
						printf("Address: %#X\n", functions[functionNum].addresses[0]);
						printf("Number of instructions: %d\n", functions[functionNum].numOfInstructions);
						printf("Name: %s\n", functions[functionNum].name);
						printf("Number of register arguments: %d\n", functions[functionNum].numOfRegArgs);
						printf("Number of register stack: %d\n", functions[functionNum].numOfStackArgs);
						printf("Number of local variables: %d\n", functions[functionNum].numOfLocalVars);
					}
					else
					{
						printf("Please select a function using s.\n");
					}
				}
				else if(strcmp(userInput, "da") == 0)
				{
					if(functionNum != -1)
					{
						for(int i = 0; i < functions[functionNum].numOfInstructions; i++)
						{
							char buffer[50];
							if (instructionToStr(&functions[functionNum].instructions[i], buffer, 50))
							{
								printf("%#-*X", 15, functions[functionNum].addresses[i]);
								printf("%s\n", buffer);
							}
						}
					}
					else
					{
						printf("Please select a function using s.");
					}
				}
				else if(strcmp(userInput, "dc") == 0)
				{
					if(functionNum != -1)
					{
						char showLineNumsInput[2];
						printf("Show line numbers? (y/n): ");
						scanf("%s", showLineNumsInput);
						char showLineNums = strcmp(showLineNumsInput, "y") == 0;

						struct LineOfC* decompiledFunction = (struct LineOfC*)malloc(100 * sizeof(struct LineOfC));
						unsigned short numOfLinesDecompiled = decompileFunction(functions, numOfFunctions, functionNum, functions[functionNum].name, decompiledFunction, 100);
						if (numOfLinesDecompiled != 0)
						{
							for (int i = numOfLinesDecompiled - 1; i >= 0; i--)
							{
								if(showLineNums)
								{
									printf("%d\t", numOfLinesDecompiled - i);
								}

								for (int j = 0; j < decompiledFunction[i].indents; j++) 
								{
									printf("\t");
								}
								
								printf(decompiledFunction[i].line);
								printf("\n");
							}	
						}
						else
						{
							printf("Error decompiling %s\n", functions[functionNum].name);
						}

						free(decompiledFunction);
					}
					else
					{
						printf("Please select a function to decompile using s.\n");
					}
				}
				else if(strcmp(userInput, "q") == 0)
				{
					if(functionNum != -1)
					{
						functionNum = -1;
					}
					else
					{
						break;
					}
				}
				else
				{
					printf("Unrecognized command. Use h for help.\n");
				}

				if(functionNum != -1)
				{
					printf("(jdc, %s) ", functions[functionNum].name);
				}
				else
				{
					printf("(jdc) ");
				}
			}

			if(instructions) { free(instructions); }
			if(addresses) { free(addresses); }
			if(functions) { free(functions); }
			
			return 0;
		}

		if(isDisassembling)
		{
			if(!input)
			{
				printf("You need to either pass a string of bytes, or a file path to disassemble.\n");
				return 0;
			}

			if(isX64 == 2)
			{
				if(isReadingFile)
				{
					if(!isFileX64(input, &isX64))
					{
						printf("Failed to determine file architecture.\n");
						return 0;
					}
				}
				else
				{
					isX64 = 1;
				}
			}
			
			if(isReadingFile)
			{
				if(!disassembleFile(input, isX64, 0, 0, 0, 1, showAddresses, showBytes, showOnlyBytes))
				{
					printf("Failed to disassemble file.\n");
					return 0;
				}
			}
			else if(!disassembleStringBytes(input, isX64))
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
