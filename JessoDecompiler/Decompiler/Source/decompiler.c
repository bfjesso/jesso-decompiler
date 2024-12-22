#include "../Headers/decompiler.h"
#include "../../Disassembler/Headers/opcodes.h"
#include "../../Disassembler/Headers/registers.h"

#include <stdio.h>
#include <string.h>

// The idea:

// 1. begining at the top, identify all local variables and parameters by their offset from the BP

// 2. starting from the top again, identify the scope of any conditions (start and end)

// 3. starting at the bottom and going up, check every instrucion

// 4. ignore each instructions unless:
//	a. assigning to a memory address 
//	b. assigning return statement to AX
//	c. condition
//	d. function call

// 5. call the apropriate handler function if one of the above is the case

unsigned short decompileFunction(struct DisassembledInstruction* instructions, unsigned long long* addresses, unsigned short numOfInstructions, struct LineOfC* resultBuffer, unsigned short resultBufferLen)
{
	int numOfLinesDecompiled = 0;
	
	struct LocalVariable localVariables[10];
	if (!getAllLocalVariables(instructions, numOfInstructions, localVariables, 10)) 
	{
		return 0;
	}

	// temporary
	for (int i = 0; i < 10; i++) 
	{
		if (localVariables[i].name[0] == 0) { break; }

		strcpy(resultBuffer[i].line, localVariables[i].name);

		numOfLinesDecompiled++;
	}

	struct Scope scopes[5];
	if(!getAllScopes(instructions, addresses, numOfInstructions, scopes, 5))
	{
		return 0;
	}

	int resultBufferIndex = resultBufferLen - 1;
	for (int i = numOfInstructions - 1; i >= 0; i--) 
	{

	}

	return numOfLinesDecompiled;
}

static unsigned char getAllLocalVariables(struct DisassembledInstruction* instructions, unsigned short numOfInstructions, struct LocalVariable* resultBuffer, unsigned char resultBufferLen) 
{
	int resultBufferIndex = 0;

	for (int i = 0; i < resultBufferLen; i++) 
	{
		resultBuffer[i].name[0] = 0;
	}

	for (int i = 0; i < numOfInstructions; i++) 
	{
		struct DisassembledInstruction* currentInstruction = &instructions[i];

		for (int j = 0; j < 3; j++) 
		{
			if (currentInstruction->operands[j].type != MEM_ADDRESS) { continue; }

			unsigned char reg = currentInstruction->operands[j].memoryAddress.reg;

			if (reg == BP || reg == EBP || reg == RBP)
			{
				int bpOffset = currentInstruction->operands[j].memoryAddress.constDisplacement;
				char isAlreadyInList = 0;
				
				for (int k = 0; k < resultBufferIndex; k++) 
				{
					if (resultBuffer[k].bpOffset == bpOffset) 
					{
						isAlreadyInList = 1;
						break;
					}
				}

				if (isAlreadyInList) { break; }
				
				struct LocalVariable localVar;
				localVar.bpOffset = bpOffset;

				if (bpOffset < 0)
				{
					sprintf(localVar.name, "var%X", -bpOffset);
				}
				else
				{
					sprintf(localVar.name, "arg%X", bpOffset);
				}

				if (resultBufferIndex >= resultBufferLen) { return 0; }
				resultBuffer[resultBufferIndex] = localVar;
				resultBufferIndex++;
			}
		}
	}

	return 1;
}

static unsigned char getAllScopes(struct DisassembledInstruction* instructions, unsigned long long* addresses, unsigned short numOfInstructions, struct Scope* resultBuffer, unsigned char resultBufferLen)
{
	int resultBufferIndex = 0;
	
	for (int i = 0; i < resultBufferLen; i++)
	{
		resultBuffer[i].start = 0;
		resultBuffer[i].end = 0;
	}
	
	for (int i = 0; i < numOfInstructions - 1; i++) 
	{
		struct DisassembledInstruction* currentInstruction = &instructions[i];

		if (currentInstruction->opcode >= JA && currentInstruction->opcode <= JMP_SHORT) 
		{
			if (resultBufferIndex >= resultBufferLen) { return 0; }
			
			resultBuffer[resultBufferIndex].start = addresses[i + 1];
			resultBuffer[resultBufferIndex].end = addresses[i] + currentInstruction->operands[0].immediate;

			for (int j = 0; j < numOfInstructions - 1; j++) 
			{
				if (addresses[j + 1] == resultBuffer[resultBufferIndex].end) 
				{
					resultBuffer[resultBufferIndex].end = addresses[j];
					break;
				}
			}

			resultBufferIndex++;
		}
	}

	return 1;
}

static unsigned char handleReturnStatement(struct DisassembledInstruction* instructions, unsigned short numOfInstructions, struct LineOfC* result) 
{
	for (int i = numOfInstructions - 1; i >= 0; i--) 
	{
		struct DisassembledInstruction* currentInstruction = &instructions[i];
		
		if (currentInstruction->operands[0].type != REGISTER) { continue; }

		unsigned char reg = currentInstruction->operands[0].reg;
		if (reg == AX || reg == EAX || reg == RAX)
		{
			if (currentInstruction->opcode == MOV) 
			{
				strcpy(result->line, "return ");
			}
		}
	}

	return 1;
}

