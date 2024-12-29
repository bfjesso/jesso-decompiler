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
	" & ",	// AND
	" | ",	// OR
	" ^ ",	// XOR
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

unsigned short decompileFunction(struct Function* functions, unsigned short numOfFunctions, unsigned short functionIndex, const char* functionName, struct LineOfC* resultBuffer, unsigned short resultBufferLen)
{
	if (resultBufferLen < 4) { return 0; }
	
	int numOfLinesDecompiled = 0;
	
	struct Scope scopes[10];
	unsigned char numOfScopes = 1;
	if(!getAllScopes(&functions[functionIndex], scopes, 10, &numOfScopes))
	{
		return 0;
	}

	int scopeIndex = numOfScopes - 1;
	struct Scope* nextScope = &scopes[scopeIndex];
	struct Scope* currentScope = 0;
	unsigned char scopesDepth = 0;

	int lastInstructionIndex = -1;
	for (int i = functions[functionIndex].numOfInstructions - 1; i >= 0; i--)
	{
		struct DisassembledInstruction* currentInstruction = &functions[functionIndex].instructions[i];

		if (numOfLinesDecompiled > resultBufferLen) { return 0; }

		if (currentScope != 0 && functions[functionIndex].addresses[i] == currentScope->start)
		{
			scopesDepth--;

			strcpy(resultBuffer[numOfLinesDecompiled].line, "{");
			resultBuffer[numOfLinesDecompiled].indents = scopesDepth;
			numOfLinesDecompiled++;

			if (scopesDepth == 0) 
			{
				if (!generateFunctionHeader(&functions[functionIndex], functionName, &resultBuffer[numOfLinesDecompiled]))
				{
					return 0;
				}
				numOfLinesDecompiled++;

				return numOfLinesDecompiled;
			}
			else if (!decompileCondition(functions[functionIndex].instructions, i + 1, &resultBuffer[numOfLinesDecompiled])) 
			{
				return 0;
			}
			resultBuffer[numOfLinesDecompiled].indents = scopesDepth;
			numOfLinesDecompiled++;

			currentScope = currentScope->lastScope;
		}

		if (nextScope != 0 && functions[functionIndex].addresses[i] == nextScope->end)
		{
			strcpy(resultBuffer[numOfLinesDecompiled].line, "}");
			resultBuffer[numOfLinesDecompiled].indents = scopesDepth;
			numOfLinesDecompiled++;
			
			scopesDepth++;
			scopeIndex--;

			nextScope->lastScope = currentScope;
			currentScope = nextScope;
			nextScope = scopeIndex < 0 ? 0 : &scopes[scopeIndex];
		}

		if (lastInstructionIndex != -1 && i >= lastInstructionIndex) { continue; }

		if (currentScope != 0) 
		{
			if (functions[functionIndex].returnType.primitiveType != VOID_TYPE && !currentScope->foundReturnStatement)
			{
				if (checkForReturnStatement(functions, numOfFunctions, i, functionIndex, &resultBuffer[numOfLinesDecompiled], &lastInstructionIndex))
				{
					currentScope->foundReturnStatement = 1;
					resultBuffer[numOfLinesDecompiled].indents = scopesDepth;
					numOfLinesDecompiled++;
				}
			}
		}
	}

	return 0;
}

static unsigned short generateFunctionHeader(struct Function* function, const char* functionName, struct LineOfC* result) 
{
	char returnTypeStr[20] = { 0 };
	variableTypeToStr(&function->returnType, returnTypeStr, 20);
	
	sprintf(result->line, "%s %s %s(", returnTypeStr, callingConventionStrs[function->callingConvention], functionName);

	result->indents = 0;

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

static unsigned char getAllScopes(struct Function* function, struct Scope* resultBuffer, unsigned char resultBufferLen, unsigned char* numOfScopesFound)
{
	unsigned char resultBufferIndex = 0;
	
	for (int i = 0; i < function->numOfInstructions - 1; i++)
	{
		struct DisassembledInstruction* currentInstruction = &function->instructions[i];

		if (currentInstruction->opcode >= JA_SHORT && currentInstruction->opcode <= JZ_SHORT)
		{
			if (resultBufferIndex >= resultBufferLen) { return 0; }
			
			resultBuffer[resultBufferIndex].lastScope = 0;
			resultBuffer[resultBufferIndex].start = function->addresses[i];
			resultBuffer[resultBufferIndex].end = function->addresses[i] + currentInstruction->operands[0].immediate;
			resultBuffer[resultBufferIndex].foundReturnStatement = 0;

			for (int j = 0; j < function->numOfInstructions - 1; j++)
			{
				if (function->addresses[j + 1] == resultBuffer[resultBufferIndex].end)
				{
					resultBuffer[resultBufferIndex].end = function->addresses[j];

					if (function->instructions[j].opcode == JMP_SHORT) 
					{
						resultBufferIndex++;
						resultBuffer[resultBufferIndex].lastScope = 0;
						resultBuffer[resultBufferIndex].start = function->addresses[j];
						resultBuffer[resultBufferIndex].end = function->addresses[j] + function->instructions[j].operands[0].immediate;
						resultBuffer[resultBufferIndex].foundReturnStatement = 0;
					}

					break;
				}
			}

			resultBufferIndex++;
		}
	}

	resultBuffer[resultBufferIndex].lastScope = 0;
	resultBuffer[resultBufferIndex].start = function->addresses[0];
	resultBuffer[resultBufferIndex].end = function->addresses[function->numOfInstructions - 1];
	resultBuffer[resultBufferIndex].foundReturnStatement = 0;

	resultBufferIndex++;

	*numOfScopesFound = resultBufferIndex;

	return 1;
}

static unsigned char decompileCondition(struct DisassembledInstruction* instructions, unsigned short numOfInstructions, struct LineOfC* result) 
{
	struct DisassembledInstruction* currentInstruction = &instructions[numOfInstructions - 1];

	char compOperator[3];
	
	if (currentInstruction->opcode == JMP_SHORT)
	{
		strcpy(result->line, "else");
		return 1;
	}
	else
	{
		switch (currentInstruction->opcode) 
		{
		case JZ_SHORT:
			strcpy(compOperator, "!=");
			break;
		case JNZ_SHORT:
			strcpy(compOperator, "==");
			break;
		case JG_SHORT:
			strcpy(compOperator, "<=");
			break;
		case JL_SHORT:
			strcpy(compOperator, ">=");
			break;
		case JLE_SHORT:
			strcpy(compOperator, ">");
			break;
		case JNL_SHORT:
			strcpy(compOperator, "<");
			break;
		}
	}
	
	for (int i = numOfInstructions - 2; i >= 0; i--)
	{
		currentInstruction = &instructions[i];

		if (currentInstruction->opcode == CMP) 
		{
			char operand1Str[10];
			if (!operandToC(instructions, i, &currentInstruction->operands[0], operand1Str, 10))
			{
				return 0;
			}

			char operand2Str[10];
			if (!operandToC(instructions, i, &currentInstruction->operands[1], operand2Str, 10))
			{
				return 0;
			}

			sprintf(result->line, "if(%s %s %s)", operand1Str, compOperator, operand2Str);

			return 1;
		}
	}

	return 0;
}

static unsigned char checkForReturnStatement(struct Function* functions, unsigned short numOfFunctions, unsigned short startInstructionIndex, unsigned short functionIndex, struct LineOfC* result, int* lastInstructionIndex)
{
	char returnExpressions[5][50];
	int returnExpressionIndex = 0;

	unsigned char checkedFirstInstruction = 0;

	for (int i = startInstructionIndex; i >= 0; i--)
	{
		*lastInstructionIndex = i;
		
		struct DisassembledInstruction* currentInstruction = &functions[functionIndex].instructions[i];

		unsigned char reg = currentInstruction->operands[0].reg;
		char isFirstOperandAX = (currentInstruction->operands[0].type == REGISTER && (reg == AX || reg == EAX || reg == RAX));
		char isSecondOperandAX = (currentInstruction->operands[1].type == REGISTER && (reg == AX || reg == EAX || reg == RAX));

		if ((isFirstOperandAX && currentInstruction->opcode >= MOV && currentInstruction->opcode <= IMUL) || currentInstruction->opcode == IDIV)
		{
			checkedFirstInstruction = 1;
			
			char targetOperand = 1;
			if (currentInstruction->opcode == IDIV) { targetOperand = 0; }
			if (currentInstruction->opcode == IMUL && currentInstruction->operands[2].type != NO_OPERAND) { targetOperand = 2; }

			if (currentInstruction->opcode == XOR && isSecondOperandAX) 
			{
				strcpy(returnExpressions[returnExpressionIndex], ")0");
				returnExpressionIndex++;
				break;
			}

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
			checkedFirstInstruction = 1;

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
		else if (!checkedFirstInstruction) 
		{
			return 0;
		}
	}

	strcpy(result->line, "return ");

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
					if (!operandToC(instructions, j, &currentInstruction->operands[1], argStr, 10))
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

