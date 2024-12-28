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

const char* primitiveTypeStrs[] =
{
	"char",
	"short",
	"int",
	"long long",

	"float",
	"double"
};

const char* callingConventionStrs[] =
{
	"__cdecl",
	"__stdcall",
	"__fastcall",
	"__thiscall"
};

unsigned char findNextFunction(struct DisassembledInstruction* instructions, unsigned long long* addresses, unsigned short numOfInstructions, struct Function* result, int* instructionIndex)
{
	struct Function function = { 0 };

	unsigned char initializedCX = 0;
	unsigned char initializedDX = 0;
	unsigned char initializedR8 = 0;
	unsigned char initializedR9 = 0;

	unsigned long long addressToJumpTo = 0;
	
	unsigned char foundFirstInstruction = 0;
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

			function.returnType.primitiveType = VOID_TYPE;
			function.returnType.isPointer = 0;
			function.returnType.isSigned = 0;

			function.callingConvention = __CDECL;

			function.numOfRegArgs = 0;
			function.numOfStackArgs = 0;

			foundFirstInstruction = 1;
		}
		else
		{
			function.numOfInstructions++;

			if (currentInstruction->opcode == PUSH || currentInstruction->opcode == POP) { continue; }

			// check for arguments
			for (int i = 0; i < 3; i++)
			{
				struct Operand* currentOperand = &currentInstruction->operands[i];

				if (currentOperand->type == REGISTER)
				{
					switch (currentOperand->reg)
					{
					case CX:
					case ECX:
					case RCX:
						if (currentInstruction->opcode == MOV && i == 0) { initializedCX = 1; }
						else if (!initializedCX)
						{
							function.regArgs[0].primitiveType = INT_TYPE;
							function.regArgs[0].isPointer = 0;
							function.regArgs[0].isSigned = 1;
							function.numOfRegArgs++;
							function.callingConvention = __FASTCALL;
						}
						break;
					case DX:
					case EDX:
					case RDX:
						if (currentInstruction->opcode == MOV && i == 0) { initializedDX = 1; }
						else if (!initializedDX)
						{
							function.regArgs[1].primitiveType = INT_TYPE;
							function.regArgs[1].isPointer = 0;
							function.regArgs[1].isSigned = 1;
							function.numOfRegArgs++;
							function.callingConvention = __FASTCALL;
						}
						break;
					case R8:
						if (currentInstruction->opcode == MOV && i == 0) { initializedR8 = 1; }
						else if (!initializedR8)
						{
							function.regArgs[2].primitiveType = INT_TYPE;
							function.regArgs[2].isPointer = 0;
							function.regArgs[2].isSigned = 1;
							function.numOfRegArgs++;
							function.callingConvention = __FASTCALL;
						}
						break;
					case R9:
						if (currentInstruction->opcode == MOV && i == 0) { initializedR9 = 1; }
						else if (!initializedR9)
						{
							function.regArgs[3].primitiveType = INT_TYPE;
							function.regArgs[3].isPointer = 0;
							function.regArgs[3].isSigned = 1;
							function.numOfRegArgs++;
							function.callingConvention = __FASTCALL;
						}
						break;
					}
				}
				else if (currentOperand->type == MEM_ADDRESS && (currentOperand->memoryAddress.reg == BP || currentOperand->memoryAddress.reg == EBP || currentOperand->memoryAddress.reg == RBP) && currentOperand->memoryAddress.constDisplacement > 0)
				{
					unsigned char alreadyFound = 0;
					for (int j = 0; j < function.numOfStackArgs; j++)
					{
						if (function.stackArgBpOffsets[j] == currentOperand->memoryAddress.constDisplacement)
						{
							alreadyFound = 1;
							break;
						}
					}

					if (!alreadyFound)
					{
						function.stackArgBpOffsets[function.numOfStackArgs] = currentOperand->memoryAddress.constDisplacement;
						function.stackArgs[function.numOfStackArgs].primitiveType = INT_TYPE;
						function.stackArgs[function.numOfStackArgs].isPointer = 0;
						function.stackArgs[function.numOfStackArgs].isSigned = 1;
						function.numOfStackArgs++;
					}

					// sort
					for (int j = 0; j < function.numOfStackArgs - 1; j++) 
					{
						char swapped = 0;
						for (int k = 0; k < function.numOfStackArgs - j - 1; k++) 
						{
							if (function.stackArgBpOffsets[k] > function.stackArgBpOffsets[k + 1])
							{
								unsigned char temp = function.stackArgBpOffsets[k];
								function.stackArgBpOffsets[k] = function.stackArgBpOffsets[k + 1];
								function.stackArgBpOffsets[k + 1] = temp;

								swapped = 1;
							}
						}
						if (!swapped) { break; }
					}
				}
			}

			if (currentInstruction->opcode == MOV && currentInstruction->operands[0].type == REGISTER) 
			{
				if (currentInstruction->operands[0].reg == AX || currentInstruction->operands[0].reg == EAX || currentInstruction->operands[0].reg == RAX) 
				{
					function.returnType.primitiveType = INT_TYPE;
					function.returnType.isPointer = 0;
					function.returnType.isSigned = 1;
				}
			}

			if (currentInstruction->opcode >= JA_SHORT && currentInstruction->opcode <= JMP_SHORT)
			{
				addressToJumpTo = addresses[i] + currentInstruction->operands[0].immediate;
			}
			else if (currentInstruction->opcode == RET_NEAR || currentInstruction->opcode == RET_FAR)
			{
				if (function.callingConvention == __CDECL && currentInstruction->operands[0].type != NO_OPERAND) 
				{
					function.callingConvention = __STDCALL;
				}
				else if (function.numOfStackArgs != 0 && function.numOfRegArgs == 1) 
				{
					function.callingConvention = __THISCALL;
				}
				
				*result = function;
				return 1;
			}
			else if (currentInstruction->opcode == JMP_NEAR || currentInstruction->opcode == JMP_FAR) 
			{
				*result = function;
				return 1;
			}
		}
	}
	
	return 0;
}

unsigned short decompileFunction(struct Function* function, const char* functionName, struct LineOfC* resultBuffer, unsigned short resultBufferLen)
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

	if (!generateFunctionHeader(function, functionName, &resultBuffer[numOfLinesDecompiled])) 
	{
		return 0;
	}
	numOfLinesDecompiled++;

	return numOfLinesDecompiled;
}

static unsigned short generateFunctionHeader(struct Function* function, const char* functionName, struct LineOfC* result) 
{
	char returnTypeStr[20] = { 0 };
	variableTypeToStr(&function->returnType, returnTypeStr, 20);

	int symbolIndex = 0;
	
	sprintf(result->line, "%s %s \\(", returnTypeStr, callingConventionStrs[function->callingConvention]);
	strcpy(result->symbols[symbolIndex], functionName);
	symbolIndex++;

	for (int i = 0; i < function->numOfRegArgs; i++) 
	{
		char argTypeStr[20] = { 0 };
		variableTypeToStr(&function->regArgs[i], argTypeStr, 20);
		
		if (i == function->numOfRegArgs - 1 && function->numOfStackArgs == 0) 
		{
			sprintf(result->line + strlen(result->line), "%s \\)", argTypeStr);
		}
		else 
		{
			sprintf(result->line + strlen(result->line), "%s \\, ", argTypeStr);
		}

		
		switch (i) 
		{
		case 0:
			strcpy(result->symbols[symbolIndex], "argCX");
			break;
		case 1:
			strcpy(result->symbols[symbolIndex], "argDX");
			break;
		case 2:
			strcpy(result->symbols[symbolIndex], "argR8");
			break;
		case 3:
			strcpy(result->symbols[symbolIndex], "argR9");
			break;
		}

		symbolIndex++;
	}

	for (int i = 0; i < function->numOfStackArgs; i++)
	{
		char argTypeStr[20] = { 0 };
		variableTypeToStr(&function->stackArgs[i], argTypeStr, 20);
		
		if (i == function->numOfStackArgs - 1)
		{
			sprintf(result->line + strlen(result->line), "%s \\)", argTypeStr);
		}
		else
		{
			sprintf(result->line + strlen(result->line), "%s \\, ", argTypeStr);
		}

		sprintf(result->symbols[symbolIndex], "arg%X", function->stackArgBpOffsets[i]);

		symbolIndex++;
	}

	return 1;
}

static unsigned short variableTypeToStr(struct VariableType* varType, char* buffer, unsigned char bufferLen)
{
	if (bufferLen < 20) { return 0; }

	if (!varType->isSigned)
	{
		strcpy(buffer, "unsigned ");
	}

	strcpy(buffer + strlen(buffer), primitiveTypeStrs[varType->primitiveType]);

	if (varType->isPointer)
	{
		strcpy(buffer + strlen(buffer), "*");
	}

	return 1;
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
	int symbolIndex = -1;

	for (int i = function->numOfInstructions - 1; i >= 0; i--) 
	{
		struct DisassembledInstruction* currentInstruction = &function->firstInstruction[i];
		
		if (currentInstruction->operands[0].type != REGISTER && currentInstruction->opcode != IDIV) { continue; }

		unsigned char reg = currentInstruction->operands[0].reg;
		if (reg == AX || reg == EAX || reg == RAX || currentInstruction->opcode == IDIV)
		{
			if (currentInstruction->opcode >= MOV && currentInstruction->opcode <= IDIV)
			{
				char targetOperand = 1;
				if (currentInstruction->opcode == IDIV) { targetOperand = 0; }

				char isLocalVar = 0;
				char operandStr[10];
				if (!operandToC(&function->firstInstruction[i - 1], i, &currentInstruction->operands[targetOperand], operandStr, 10, &isLocalVar))
				{
					return 0;
				}

				if (isLocalVar) 
				{
					sprintf(returnExpressions[returnExpressionIndex], "%s\\)", operationStrs[currentInstruction->opcode]);
					for (int i = symbolIndex; i >= 0; i--)
					{
						strcpy(result->symbols[i + 1], result->symbols[i]);
					}

					strcpy(result->symbols[0], operandStr);
					
					symbolIndex++;
				}
				else 
				{
					sprintf(returnExpressions[returnExpressionIndex], "%s%s)", operationStrs[currentInstruction->opcode], operandStr);
				}

				returnExpressionIndex++;

				if (currentInstruction->opcode == MOV) 
				{
					break;
				}
			}
		}
	}

	result->numOfSymbols = returnExpressionIndex;

	strcpy(result->line, "\treturn ");

	for (int i = 0; i < returnExpressionIndex - 1; i++)
	{
		strcpy(result->line + strlen(result->line), "(");
	}

	for (int i = returnExpressionIndex - 1; i >= 0; i--)
	{
		strcpy(result->line + strlen(result->line), returnExpressions[i]);
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

		if (operand->reg == ECX || operand->reg == RCX || operand->reg == EDX || operand->reg == RDX || operand->reg == R8 || operand->reg == R9) // this or fast call argument
		{
			*isLocalVar = 1;
			sprintf(resultBuffer, "arg%s", registerStrs[operand->reg]);
			return 1;
		}
	}

	return 0;
}

static unsigned char handleFunctionCall(struct DisassembledInstruction* instructions, unsigned short numOfInstructions, char* resultBuffer, unsigned char resultBufferSize) 
{
	
	
	return 1;
}

