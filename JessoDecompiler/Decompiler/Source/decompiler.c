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
			function.addresses = &addresses[i];
			function.instructions = &instructions[i];
			function.numOfInstructions = 1;

			function.returnType.primitiveType = VOID_TYPE;
			function.returnType.isPointer = 0;
			function.returnType.isSigned = 0;

			function.addressOfReturnFunction = 0;

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

					function.addressOfReturnFunction = 0;
				}
			}
			else if (currentInstruction->opcode == CALL_NEAR) 
			{
				function.addressOfReturnFunction = addresses[i] + currentInstruction->operands[0].immediate;
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

unsigned char fixAllFunctionReturnTypes(struct Function* functions, unsigned short numOfFunctions) // if a function's return type was that of another function, it must be resolved
{
	for (int i = 0; i < numOfFunctions; i++) 
	{
		if (functions[i].addressOfReturnFunction != 0)
		{
			int returnFunctionIndex = findFunctionByAddress(functions, 0, numOfFunctions - 1, functions[i].addressOfReturnFunction);

			while (returnFunctionIndex != -1 && functions[returnFunctionIndex].addressOfReturnFunction != 0)
			{
				returnFunctionIndex = findFunctionByAddress(functions, 0, numOfFunctions - 1, functions[returnFunctionIndex].addressOfReturnFunction);
			}
			
			if (returnFunctionIndex != -1) 
			{
				functions[i].returnType = functions[returnFunctionIndex].returnType;
			}
			else 
			{
				return 0;
			}
		}
	}

	return 1;
}

unsigned short decompileFunction(struct Function* functions, unsigned short numOfFunctions, unsigned short functionIndex, const char* functionName, struct LineOfC* resultBuffer, unsigned short resultBufferLen)
{
	if (resultBufferLen < 4) { return 0; }
	
	int numOfLinesDecompiled = 0;
	
	struct Scope scopes[5];
	if(!getAllScopes(&functions[functionIndex], scopes, 5))
	{
		return 0;
	}

	strcpy((&resultBuffer[numOfLinesDecompiled])->line, "}");
	numOfLinesDecompiled++;

	if (!decompileReturnStatement(functions, numOfFunctions, functionIndex, &resultBuffer[numOfLinesDecompiled]))
	{
		return 0;
	}
	numOfLinesDecompiled++;

	strcpy((&resultBuffer[numOfLinesDecompiled])->line, "{");
	numOfLinesDecompiled++;

	if (!generateFunctionHeader(&functions[functionIndex], functionName, &resultBuffer[numOfLinesDecompiled]))
	{
		return 0;
	}
	numOfLinesDecompiled++;

	return numOfLinesDecompiled;
}

static int findFunctionByAddress(struct Function* functions, int low, int high, unsigned long long address)
{
	while (low <= high)
	{
		int mid = low + (high - low) / 2;

		if (*functions[mid].addresses == address) { return mid; }

		if (*functions[mid].addresses < address) { low = mid + 1; }
		else { high = mid - 1; }
	}

	return -1;
}

static unsigned short generateFunctionHeader(struct Function* function, const char* functionName, struct LineOfC* result) 
{
	char returnTypeStr[20] = { 0 };
	variableTypeToStr(&function->returnType, returnTypeStr, 20);
	
	sprintf(result->line, "%s %s %s(", returnTypeStr, callingConventionStrs[function->callingConvention], functionName);

	for (int i = 0; i < function->numOfRegArgs; i++) 
	{
		char argTypeStr[20] = { 0 };
		variableTypeToStr(&function->regArgs[i], argTypeStr, 20);

		char regArgStr[6];
		switch (i)
		{
		case 0:
			strcpy(regArgStr, "argCX");
			break;
		case 1:
			strcpy(regArgStr, "argDX");
			break;
		case 2:
			strcpy(regArgStr, "argR8");
			break;
		case 3:
			strcpy(regArgStr, "argR9");
			break;
		}
		
		if (i == function->numOfRegArgs - 1 && function->numOfStackArgs == 0) 
		{
			sprintf(result->line + strlen(result->line), "%s %s)", argTypeStr, regArgStr);
		}
		else 
		{
			sprintf(result->line + strlen(result->line), "%s %s, ", argTypeStr, regArgStr);
		}
	}

	for (int i = 0; i < function->numOfStackArgs; i++)
	{
		char argTypeStr[20] = { 0 };
		variableTypeToStr(&function->stackArgs[i], argTypeStr, 20);

		char stackArgStr[10];
		sprintf(stackArgStr, "arg%X", function->stackArgBpOffsets[i]);
		
		if (i == function->numOfStackArgs - 1)
		{
			sprintf(result->line + strlen(result->line), "%s %s)", argTypeStr, stackArgStr);
		}
		else
		{
			sprintf(result->line + strlen(result->line), "%s %s, ", argTypeStr, stackArgStr);
		}
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
		struct DisassembledInstruction* currentInstruction = &function->instructions[i];

		if (currentInstruction->opcode >= JA_SHORT && currentInstruction->opcode <= JMP_SHORT) 
		{
			if (resultBufferIndex >= resultBufferLen) { return 0; }
			
			resultBuffer[resultBufferIndex].start = function->addresses[i + 1];
			resultBuffer[resultBufferIndex].end = function->addresses[i] + currentInstruction->operands[0].immediate;

			for (int j = 0; j < function->numOfInstructions - 1; j++)
			{
				if (function->addresses[j + 1] == resultBuffer[resultBufferIndex].end)
				{
					resultBuffer[resultBufferIndex].end = function->addresses[j];
					break;
				}
			}

			resultBufferIndex++;
		}
	}

	return 1;
}

static unsigned char decompileReturnStatement(struct Function* functions, unsigned short numOfFunctions, unsigned short functionIndex, struct LineOfC* result)
{
	char returnExpressions[5][50];
	int returnExpressionIndex = 0;

	int numOfOperations = 0;

	for (int i = functions[functionIndex].numOfInstructions - 1; i >= 0; i--)
	{
		struct DisassembledInstruction* currentInstruction = &functions[functionIndex].instructions[i];

		unsigned char reg = currentInstruction->operands[0].reg;
		char isFirstOperandAX = (currentInstruction->operands[0].type == REGISTER && (reg == AX || reg == EAX || reg == RAX));

		if ((isFirstOperandAX && currentInstruction->opcode >= MOV && currentInstruction->opcode <= IMUL) || currentInstruction->opcode == IDIV)
		{
			char targetOperand = 1;
			if (currentInstruction->opcode == IDIV) { targetOperand = 0; }
			if (currentInstruction->opcode == IMUL && currentInstruction->operands[2].type != NO_OPERAND) { targetOperand = 2; }

			char operandStr[10];
			if (!operandToC(functions[functionIndex].instructions, i, &currentInstruction->operands[targetOperand], operandStr, 10))
			{
				return 0;
			}

			sprintf(returnExpressions[returnExpressionIndex], ")%s%s", operationStrs[currentInstruction->opcode], operandStr);
			returnExpressionIndex++;

			if (currentInstruction->opcode == MOV)
			{
				break;
			}
		}
		else if (currentInstruction->opcode == CALL_NEAR) 
		{
			unsigned long long calleeAddress = functions[functionIndex].addresses[i] + currentInstruction->operands[0].immediate;
			int calleIndex = findFunctionByAddress(functions, 0, numOfFunctions - 1, calleeAddress);
			if (calleIndex == -1) 
			{
				return 0;
			}

			if (&functions[calleIndex].returnType.primitiveType == VOID_TYPE) { continue; }
			
			struct LineOfC functionCall = { 0 };
			if (!decompileFunctionCall(functions[functionIndex].instructions, i, &functions[calleIndex], &functionCall))
			{
				return 0;
			}

			sprintf(returnExpressions[returnExpressionIndex], ")%s", functionCall.line);

			returnExpressionIndex++;
			
			break;
		}
	}

	strcpy(result->line, "\treturn ");

	for (int i = returnExpressionIndex - 1; i >= returnExpressionIndex - 2 && i >= 0; i--) // for the last two expressions, remove the parentheses
	{
		char temp[50] = { 0 };
		strcpy(temp, returnExpressions[i] + 1);
		returnExpressions[i][0] = 0;
		strcpy(returnExpressions[i], temp);
	}

	for (int i = 0; i < returnExpressionIndex - 2; i++)
	{
		strcpy(result->line + strlen(result->line), "(");
	}

	for (int i = returnExpressionIndex - 1; i >= 0; i--)
	{
		strcpy(result->line + strlen(result->line), returnExpressions[i]);
	}

	strcpy(result->line + strlen(result->line), ";");

	return 1;
}

static unsigned char operandToC(struct DisassembledInstruction* instructions, unsigned short numOfInstructions, struct Operand* operand, char* resultBuffer, unsigned char resultBufferSize) 
{
	if (operand->type == IMMEDIATE) 
	{
		sprintf(resultBuffer, "%llu", operand->immediate);
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
				return operandToC(instructions, i, &currentInstruction->operands[1], resultBuffer, resultBufferSize);
			}
		}

		// thiscall or fastcall argument

		if (operand->reg == CX || operand->reg == ECX || operand->reg == RCX)
		{
			strcpy(resultBuffer, "argCX");
			return 1;
		}
		else if (operand->reg == DX || operand->reg == EDX || operand->reg == RDX)
		{
			strcpy(resultBuffer, "argDX");
			return 1;
		}
		else if (operand->reg == R8)
		{
			strcpy(resultBuffer, "argR8");
			return 1;
		}
		else if (operand->reg == R9)
		{
			strcpy(resultBuffer, "argR9");
			return 1;
		}
	}

	return 0;
}

static unsigned char decompileFunctionCall(struct DisassembledInstruction* instructions, unsigned short numOfInstructions, struct Function* callee, struct LineOfC* result)
{
	sprintf(result->line, "%s(", callee->name);
	
	for (int i = 0; i < callee->numOfRegArgs; i++) 
	{
		for (int j = numOfInstructions - 1; j >= 0; j--)
		{
			struct DisassembledInstruction* currentInstruction = &instructions[j];

			if (currentInstruction->opcode == MOV && currentInstruction->operands[0].type == REGISTER)
			{
				unsigned char reg = currentInstruction->operands[0].reg;
				
				char cxCheck = i == 0 && (reg == CX || reg == ECX || reg == RCX);
				char dxCheck = i == 1 && (reg == DX || reg == EDX || reg == RDX);
				char r8Check = i == 2 && reg == R8;
				char r9Check = i == 3 && reg == R9;

				if (cxCheck || dxCheck || r8Check || r9Check)
				{
					char argStr[10];
					if (!operandToC(&instructions[j - 1], j, &currentInstruction->operands[1], argStr, 10))
					{
						return 0;
					}

					sprintf(result->line + strlen(result->line), "%s, ", argStr);

					break;
				}
			}
		}
	}
	
	int stackArgsFound = 0;
	for (int i = numOfInstructions - 1; i >= 0; i--)
	{
		if (stackArgsFound == callee->numOfStackArgs) { break; }
		
		struct DisassembledInstruction* currentInstruction = &instructions[i];

		if (currentInstruction->opcode == PUSH)
		{
			char argStr[10];
			if (!operandToC(instructions, i, &currentInstruction->operands[0], argStr, 10))
			{
				return 0;
			}

			sprintf(result->line + strlen(result->line), "%s, ", argStr);

			stackArgsFound++;
		}
	}

	result->line[strlen(result->line) - 2] = ')';
	result->line[strlen(result->line) - 1] = 0;
	
	return 1;
}

