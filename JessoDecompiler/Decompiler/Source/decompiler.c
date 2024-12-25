#include "../Headers/decompiler.h"
#include "../../Disassembler/Headers/opcodes.h"
#include "../../Disassembler/Headers/registers.h"

#include <stdio.h>
#include <string.h>

const char* operationStrs[] =
{
	"",		// MOV
	" + ",	// ADD
	" - ",	// SUB
	" * ",	// IMUL
	" / "	// IDIV
};

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

unsigned char findNextFunction(struct DisassembledInstruction* instructions, unsigned long long* addresses, unsigned short numOfInstructions, struct Function* result, int* instructionIndex)
{
	struct Function function = { 0 };

	unsigned long long addressToJumpTo = 0;
	
	char foundFirstInstruction = 0;
	for (int i = 0; i < numOfInstructions; i++) 
	{
		(*instructionIndex)++;

		if (addressToJumpTo != 0 && addressToJumpTo != addresses[i])
		{
			continue;
		}
		else 
		{
			addressToJumpTo = 0;
		}
		
		struct DisassembledInstruction* currentInstruction = &instructions[i];

		if (currentInstruction->opcode == INT3) 
		{
			if (foundFirstInstruction) 
			{
				*result = function;
				return 1;
			}
			else 
			{
				continue;
			}
		}

		if (!foundFirstInstruction)
		{
			function.address = &addresses[i];
			function.firstInstruction = &instructions[i];
			function.numOfInstructions = 1;

			function.callingConvention = __CDECL;

			foundFirstInstruction = 1;
		}
		else
		{
			function.numOfInstructions++;

			if (currentInstruction->opcode >= JA_SHORT && currentInstruction->opcode <= JMP_SHORT)
			{
				addressToJumpTo = addresses[i] + currentInstruction->operands[0].immediate;
			}
			else if (currentInstruction->opcode == RET_NEAR || currentInstruction->opcode == RET_FAR || currentInstruction->opcode == JMP_NEAR || currentInstruction->opcode == JMP_FAR)
			{
				*result = function;
				return 1;
			}
		}
	}
	
	return 0;
}

unsigned short decompileFunction(struct Function* function, struct LineOfC* resultBuffer, unsigned short resultBufferLen)
{
	if (resultBufferLen < 4) { return 0; }
	
	int numOfLinesDecompiled = 0;
	
	struct Scope scopes[5];
	if(!getAllScopes(function, scopes, 5))
	{
		return 0;
	}

	strcpy((&resultBuffer[numOfLinesDecompiled])->line, "}");
	numOfLinesDecompiled++;

	if (!handleReturnStatement(function, &resultBuffer[numOfLinesDecompiled]))
	{
		return 0;
	}
	numOfLinesDecompiled++;

	strcpy((&resultBuffer[numOfLinesDecompiled])->line, "{");
	numOfLinesDecompiled++;

	

	return numOfLinesDecompiled;
}

static unsigned char getAllScopes(struct Function* function, struct Scope* resultBuffer, unsigned char resultBufferLen)
{
	int resultBufferIndex = 0;
	
	for (int i = 0; i < resultBufferLen; i++)
	{
		resultBuffer[i].start = 0;
		resultBuffer[i].end = 0;
	}
	
	for (int i = 0; i < function->numOfInstructions - 1; i++)
	{
		struct DisassembledInstruction* currentInstruction = &function->firstInstruction[i];

		if (currentInstruction->opcode >= JA_SHORT && currentInstruction->opcode <= JMP_SHORT) 
		{
			if (resultBufferIndex >= resultBufferLen) { return 0; }
			
			resultBuffer[resultBufferIndex].start = function->address[i + 1];
			resultBuffer[resultBufferIndex].end = function->address[i] + currentInstruction->operands[0].immediate;

			for (int j = 0; j < function->numOfInstructions - 1; j++)
			{
				if (function->address[j + 1] == resultBuffer[resultBufferIndex].end)
				{
					resultBuffer[resultBufferIndex].end = function->address[j];
					break;
				}
			}

			resultBufferIndex++;
		}
	}

	return 1;
}

static unsigned char handleReturnStatement(struct Function* function, struct LineOfC* result)
{
	char returnExpressions[5][20];
	int returnExpressionIndex = 0;
	int symbolIndex = 0;
	char isNotFirstOperation = 0;

	for (int i = function->numOfInstructions - 1; i >= 0; i--) 
	{
		struct DisassembledInstruction* currentInstruction = &function->firstInstruction[i];
		
		if (currentInstruction->operands[0].type != REGISTER) { continue; }

		unsigned char reg = currentInstruction->operands[0].reg;
		if (reg == AX || reg == EAX || reg == RAX)
		{
			if (currentInstruction->opcode >= MOV && currentInstruction->opcode <= IDIV)
			{
				char isLocalVar = 0;
				char operandStr[10];
				if (!operandToC(&function->firstInstruction[i - 1], i, &currentInstruction->operands[1], operandStr, 10, &isLocalVar))
				{
					return 0;
				}

				if (currentInstruction->opcode == MOV) 
				{
					isNotFirstOperation = 0;
				}

				if (isNotFirstOperation) 
				{
					returnExpressions[returnExpressionIndex][0] = '(';
				}

				if (isLocalVar) 
				{
					sprintf(returnExpressions[returnExpressionIndex] + isNotFirstOperation, "\\%s", operationStrs[currentInstruction->opcode]);
					strcpy(result->symbols[symbolIndex], operandStr);
					symbolIndex++;
				}
				else 
				{
					sprintf(returnExpressions[returnExpressionIndex] + isNotFirstOperation, "%s%s", operandStr, operationStrs[currentInstruction->opcode]);
				}

				returnExpressionIndex++;

				if (currentInstruction->opcode == MOV) 
				{
					break;
				}
				
				isNotFirstOperation = 1;
			}
		}
	}

	strcpy(result->line, "\treturn ");

	for (int i = 0; i < returnExpressionIndex; i++) 
	{
		strcpy(result->line + strlen(result->line), returnExpressions[i]);
	}

	for (int i = 0; i < returnExpressionIndex - 1; i++)
	{
		strcpy(result->line + strlen(result->line), ")");
	}

	result->line[strlen(result->line) - 1] = ';';

	return 1;
}

static unsigned char operandToC(struct DisassembledInstruction* instructions, unsigned short numOfInstructions, struct Operand* operand, char* resultBuffer, unsigned char resultBufferSize, char* isLocalVar) 
{
	if (operand->type == IMMEDIATE) 
	{
		sprintf(resultBuffer, "%llu", operand->immediate);

		*isLocalVar = 0;
		return 1;
	}
	else if (operand->type == MEM_ADDRESS && (operand->memoryAddress.reg == BP || operand->memoryAddress.reg == EBP || operand->memoryAddress.reg == RBP)) 
	{
		if (operand->memoryAddress.constDisplacement < 0)
		{
			sprintf(resultBuffer, "var%X", -operand->memoryAddress.constDisplacement);
		}
		else
		{
			sprintf(resultBuffer, "arg%X", operand->memoryAddress.constDisplacement);
		}

		*isLocalVar = 1;
		return 1;
	}
	else if(operand->type == REGISTER)
	{
		for (int i = numOfInstructions - 1; i >= 0; i--)
		{
			struct DisassembledInstruction* currentInstruction = &instructions[i];

			if (currentInstruction->operands[0].type != REGISTER) { continue; }

			if (currentInstruction->operands[0].reg == operand->reg && currentInstruction->opcode == MOV)
			{
				return operandToC(&instructions[i - 1], i, &currentInstruction->operands[1], resultBuffer, resultBufferSize, isLocalVar);
			}
		}
	}

	return 0;
}

static unsigned char handleFunctionCall(struct DisassembledInstruction* instructions, unsigned short numOfInstructions, char* resultBuffer, unsigned char resultBufferSize) 
{
	
	
	return 1;
}





