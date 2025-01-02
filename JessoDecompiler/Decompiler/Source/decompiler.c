#include "../Headers/decompiler.h"
#include "../../Disassembler/Headers/opcodes.h"
#include "../../Disassembler/Headers/registers.h"

#include <stdio.h>
#include <string.h>

const char* primitiveTypeStrs[] =
{
	"char",
	"short",
	"int",
	"long long",

	"float",
	"double",

	"void"
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
	struct Scope* nextScope = scopeIndex < 0 ? 0 : &scopes[scopeIndex];
	struct Scope* currentScope = 0;

	for (int i = functions[functionIndex].numOfInstructions - 1; i >= 0; i--)
	{
		functions[functionIndex].instructions[i].hasBeenDecompiled = 0; // reset instructions incase this function has been decompiled before.
	}

	strcpy(resultBuffer[numOfLinesDecompiled].line, "}");
	resultBuffer[numOfLinesDecompiled].indents = 0;
	numOfLinesDecompiled++;

	unsigned char scopesDepth = 1;

	for (int i = functions[functionIndex].numOfInstructions - 1; i >= 0; i--)
	{
		if (numOfLinesDecompiled > resultBufferLen) 
		{ 
			return 0; 
		}

		struct DisassembledInstruction* currentInstruction = &functions[functionIndex].instructions[i];

		if (i == functions[functionIndex].numOfInstructions - 1 && functions[functionIndex].returnType.primitiveType != VOID_TYPE)
		{
			if (decompileReturnStatement(functions, numOfFunctions, i, functionIndex, nextScope != 0 ? nextScope->end : currentScope != 0 ? currentScope->start : functions[functionIndex].addresses[0], &resultBuffer[numOfLinesDecompiled]))
			{
				resultBuffer[numOfLinesDecompiled].indents = scopesDepth;
				numOfLinesDecompiled++;
			}
		}

		if (currentScope != 0 && functions[functionIndex].addresses[i] == currentScope->start)
		{
			if (resultBuffer[numOfLinesDecompiled - 1].line[0] == '}')
			{
				strcpy(resultBuffer[numOfLinesDecompiled].line, "return;");
				resultBuffer[numOfLinesDecompiled].indents = scopesDepth;
				numOfLinesDecompiled++;
			}

			scopesDepth--;

			strcpy(resultBuffer[numOfLinesDecompiled].line, "{");
			resultBuffer[numOfLinesDecompiled].indents = scopesDepth;
			numOfLinesDecompiled++;

			if (!decompileCondition(functions, numOfFunctions, i, functionIndex, &resultBuffer[numOfLinesDecompiled]))
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

			if (functions[functionIndex].returnType.primitiveType != VOID_TYPE) 
			{
				if (decompileReturnStatement(functions, numOfFunctions, i, functionIndex, nextScope != 0 ? nextScope->end : currentScope->start, &resultBuffer[numOfLinesDecompiled]))
				{
					resultBuffer[numOfLinesDecompiled].indents = scopesDepth;
					numOfLinesDecompiled++;
				}
			}
		}

		if (checkForAssignment(&functions[functionIndex].instructions[i]))
		{
			struct LocalVariable* localVar = getLocalVarByOffset(&functions[functionIndex], currentInstruction->operands[0].memoryAddress.constDisplacement);

			if (localVar != 0 && decompileAssignment(functions, numOfFunctions, i, functionIndex, &localVar->type, &resultBuffer[numOfLinesDecompiled]))
			{
				resultBuffer[numOfLinesDecompiled].indents = scopesDepth;
				numOfLinesDecompiled++;
			}
			else
			{
				return 0;
			}
		}

		struct Function* callee;
		if (checkForFunctionCall(&functions[functionIndex].instructions[i], functions[functionIndex].addresses[i], functions, numOfFunctions, functionIndex, &callee))
		{
			if (decompileFunctionCall(functions, numOfFunctions, i, functionIndex, callee, &resultBuffer[numOfLinesDecompiled]))
			{
				resultBuffer[numOfLinesDecompiled].indents = scopesDepth;
				strcpy(resultBuffer[numOfLinesDecompiled].line + strlen(resultBuffer[numOfLinesDecompiled].line), ";");
				numOfLinesDecompiled++;
			}
			else
			{
				return 0;
			}
		}
	}

	if (functions[functionIndex].numOflocalVars > 0)
	{
		strcpy(resultBuffer[numOfLinesDecompiled].line, "");
		resultBuffer[numOfLinesDecompiled].indents = 1;
		numOfLinesDecompiled++;

		if (!declareAllLocalVariables(&functions[functionIndex], resultBuffer, &numOfLinesDecompiled, resultBufferLen))
		{
			return 0;
		}
	}

	strcpy(resultBuffer[numOfLinesDecompiled].line, "{");
	resultBuffer[numOfLinesDecompiled].indents = 0;
	numOfLinesDecompiled++;

	if (!generateFunctionHeader(&functions[functionIndex], functionName, &resultBuffer[numOfLinesDecompiled]))
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
			sprintf(result->line + strlen(result->line), "%s %s", argTypeStr, regArgStr);
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
			sprintf(result->line + strlen(result->line), "%s %s", argTypeStr, stackArgStr);
		}
		else
		{
			sprintf(result->line + strlen(result->line), "%s %s, ", argTypeStr, stackArgStr);
		}
	}

	strcpy(result->line + strlen(result->line), ")");

	return 1;
}

static unsigned char declareAllLocalVariables(struct Function* function, struct LineOfC* resultBuffer, int* resultBufferIndex, unsigned short resultBufferLen)
{
	for (int i = 0; i < function->numOflocalVars; i++)
	{
		char typeStr[20] = { 0 };
		if (!variableTypeToStr(&function->localVars[i].type, typeStr, 20))
		{
			return 0;
		}

		sprintf(resultBuffer[*resultBufferIndex].line, "%s var%X;", typeStr, -function->localVars[i].stackOffset);
		resultBuffer[*resultBufferIndex].indents = 1;
		(*resultBufferIndex)++;
	}

	return 1;
}

static unsigned char variableTypeToStr(struct VariableType* varType, char* buffer, unsigned char bufferLen)
{
	if (bufferLen < 20) { return 0; }

	if (!varType->isSigned)
	{
		strcpy(buffer, "unsigned ");
	}

	strcpy(buffer + strlen(buffer), primitiveTypeStrs[varType->primitiveType]);

	for (int i = 0; i < varType->numOfPtrs; i++)
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
					}

					break;
				}
			}

			resultBufferIndex++;
		}
	}

	*numOfScopesFound = resultBufferIndex;

	return 1;
}

static unsigned char checkForReturnStatement(struct DisassembledInstruction* instruction, unsigned long long address, struct Function* functions, unsigned short numOfFunctions, unsigned short functionIndex)
{
	if (instruction->hasBeenDecompiled) { return 0; }
	
	if (functions[functionIndex].returnType.primitiveType == FLOAT_TYPE || functions[functionIndex].returnType.primitiveType == DOUBLE_TYPE) 
	{
		if (instruction->opcode == FLD)
		{
			return 1;
		}
	}
	else 
	{
		char isFirstOperandAX = (instruction->operands[0].type == REGISTER && compareRegisters(instruction->operands[0].reg, AX));

		if ((isFirstOperandAX && doesInstructionModifyOperand(instruction, 0, 0)) || doesOpcodeModifyRegister(instruction->opcode, AX, 0))
		{
			return 1;
		}
		else if (instruction->opcode == CALL_NEAR)
		{
			unsigned long long calleeAddress = address + instruction->operands[0].immediate;
			int calleIndex = findFunctionByAddress(functions, 0, numOfFunctions - 1, calleeAddress);

			if (calleIndex == -1 || functions[calleIndex].returnType.primitiveType == VOID_TYPE)
			{
				return 0;
			}

			return 1;
		}
	}
	
	return 0;
}

static unsigned char checkForAssignment(struct DisassembledInstruction* instruction)
{
	if (instruction->hasBeenDecompiled) { return 0; }
	
	if (instruction != 0 && doesInstructionModifyOperand(instruction, 0, 0) && isOperandLocalVariable(&instruction->operands[0]))
	{
		return 1;
	}

	return 0;
}

static unsigned char checkForFunctionCall(struct DisassembledInstruction* instruction, unsigned long long address, struct Function* functions, unsigned short numOfFunctions, unsigned short functionIndex, struct Function** calleeRef) 
{
	if (instruction->hasBeenDecompiled) { return 0; }
	
	if (instruction->opcode == CALL_NEAR || instruction->opcode == JMP_NEAR)
	{
		unsigned long long calleeAddress = address + instruction->operands[0].immediate;
		int calleIndex = findFunctionByAddress(functions, 0, numOfFunctions - 1, calleeAddress);

		if (calleIndex == -1)
		{
			return 0;
		}

		*calleeRef = &functions[calleIndex];

		return 1;
	}

	return 0;
}

static unsigned char decompileCondition(struct Function* functions, unsigned short numOfFunctions, unsigned short startInstructionIndex, unsigned short functionIndex, struct LineOfC* result)
{
	struct DisassembledInstruction* currentInstruction = &functions[functionIndex].instructions[startInstructionIndex];

	char compOperator[3] = { 0 };
	
	if (currentInstruction->opcode == JMP_SHORT)
	{
		strcpy(result->line, "else");
		currentInstruction->hasBeenDecompiled = 1;
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
		case JGE_SHORT:
			strcpy(compOperator, "<");
			break;
		}

		currentInstruction->hasBeenDecompiled = 1;
	}
	
	for (int i = startInstructionIndex - 1; i >= 0; i--)
	{
		currentInstruction = &functions[functionIndex].instructions[i];

		if (currentInstruction->opcode == CMP) 
		{
			struct VariableType type = {0};
			type.primitiveType = INT_TYPE;
			type.numOfPtrs = 0;
			type.isSigned = 1;

			char operand1Str[20] = { 0 };
			if (!decompileOperand(functions, numOfFunctions, i, functionIndex, &currentInstruction->operands[0], &type, operand1Str, 20))
			{
				return 0;
			}

			char operand2Str[20] = { 0 };
			if (!decompileOperand(functions, numOfFunctions, i, functionIndex, &currentInstruction->operands[1], &type, operand2Str, 20))
			{
				return 0;
			}

			sprintf(result->line, "if(%s %s %s)", operand1Str, compOperator, operand2Str);

			currentInstruction->hasBeenDecompiled = 1;

			return 1;
		}
	}

	return 0;
}

static unsigned char decompileReturnStatement(struct Function* functions, unsigned short numOfFunctions, unsigned short startInstructionIndex, unsigned short functionIndex, unsigned long long scopeStart, struct LineOfC* result)
{
	char returnExpression[50] = { 0 };

	int newStartInstruction = -1;

	for (int i = startInstructionIndex; i >= 0; i--)
	{
		if (functions[functionIndex].addresses[i] <= scopeStart) 
		{
			break;
		}
		
		struct DisassembledInstruction* currentInstruction = &functions[functionIndex].instructions[i];

		if (checkForReturnStatement(currentInstruction, functions[functionIndex].addresses[i], functions, numOfFunctions, functionIndex)) 
		{
			newStartInstruction = i;
			break;
		}
	}

	if (newStartInstruction == -1) 
	{
		return 0;
	}
	
	if (functions[functionIndex].instructions[newStartInstruction].opcode == FLD)
	{
		if (!decompileOperand(functions, numOfFunctions, newStartInstruction - 1, functionIndex, &functions[functionIndex].instructions[newStartInstruction].operands[0], &functions[functionIndex].returnType, returnExpression, 50))
		{
			return 0;
		}
	}
	else 
	{
		struct Operand eax = { 0 };
		eax.type = REGISTER;
		eax.reg = AX;

		if (!decompileOperand(functions, numOfFunctions, newStartInstruction, functionIndex, &eax, &functions[functionIndex].returnType, returnExpression, 50))
		{
			return 0;
		}
	}
	

	sprintf(result->line, "return %s;", returnExpression);

	return 1;
}

static unsigned char decompileAssignment(struct Function* functions, unsigned short numOfFunctions, unsigned short startInstructionIndex, unsigned short functionIndex, struct VariableType* type, struct LineOfC* result)
{
	struct DisassembledInstruction* currentInstruction = &functions[functionIndex].instructions[startInstructionIndex];
	
	char assignee[50] = { 0 };
	if (!decompileOperand(functions, numOfFunctions, startInstructionIndex, functionIndex, &currentInstruction->operands[0], type, assignee, 50)) 
	{
		return 0;
	}

	char valueToAssign[50] = { 0 };
	struct Operand* operand = &currentInstruction->operands[getLastOperand(currentInstruction)];
	if (!decompileOperand(functions, numOfFunctions, startInstructionIndex, functionIndex, operand, type, valueToAssign, 50))
	{
		return 0;
	}

	char assignmentStr[20] = { 0 };
	if (!getOperationStr(currentInstruction->opcode, 1, assignmentStr))
	{
		return 0;
	}
	sprintf(result->line, "%s%s%s;", assignee, assignmentStr, valueToAssign);

	currentInstruction->hasBeenDecompiled = 1;

	return 1;
}

static unsigned char decompileOperand(struct Function* functions, unsigned short numOfFunctions, unsigned short startInstructionIndex, unsigned short functionIndex, struct Operand* operand, struct VariableType* type, char* resultBuffer, unsigned char resultBufferSize)
{
	if (operand->type == IMMEDIATE) 
	{
		sprintf(resultBuffer, "%llu", operand->immediate);
		return 1;
	}
	else if (operand->type == MEM_ADDRESS)
	{
		if (compareRegisters(operand->memoryAddress.reg, EBP)) 
		{
			if (operand->memoryAddress.constDisplacement < 0)
			{
				sprintf(resultBuffer, "var%X", -operand->memoryAddress.constDisplacement);
			}
			else
			{
				sprintf(resultBuffer, "arg%X", operand->memoryAddress.constDisplacement);
			}
		}
		else if (operand->memoryAddress.reg == NO_REG) 
		{
			char typeStr[20] = { 0 };
			if (!variableTypeToStr(type, typeStr, 20))
			{
				return 0;
			}

			sprintf(resultBuffer, "*(%s*)(0x%X)", typeStr, operand->memoryAddress.constDisplacement);
		}
		else 
		{
			struct Operand baseReg = { 0 };
			baseReg.type = REGISTER;
			baseReg.reg = operand->memoryAddress.reg;
			
			char baseOperandStr[50] = { 0 };
			if (!decompileOperand(functions, numOfFunctions, startInstructionIndex, functionIndex, &baseReg, type, baseOperandStr, 50)) 
			{
				return 0;
			}

			char typeStr[20] = { 0 };
			if (!variableTypeToStr(type, typeStr, 20))
			{
				return 0;
			}

			if (operand->memoryAddress.constDisplacement != 0) 
			{
				sprintf(resultBuffer, "*(%s*)(%s + 0x%X)", typeStr, baseOperandStr, operand->memoryAddress.constDisplacement);
			}
			else 
			{
				sprintf(resultBuffer, "*(%s*)(%s)", typeStr, baseOperandStr);
			}
		}
		

		return 1;
	}
	else if(operand->type == REGISTER)
	{
		if (compareRegisters(operand->reg, BP) || compareRegisters(operand->reg, SP) || compareRegisters(operand->reg, IP))
		{
			strcpy(resultBuffer, registerStrs[operand->reg]);
			return 1;
		}
		
		if (!decompileExpression(functions, numOfFunctions, startInstructionIndex, functionIndex, operand->reg, type, resultBuffer, resultBufferSize))
		{
			// thiscall or fastcall argument

			if (compareRegisters(operand->reg, CX))
			{
				strcpy(resultBuffer, "argCX");
				return 1;
			}
			else if (compareRegisters(operand->reg, DX))
			{
				strcpy(resultBuffer, "argDX");
				return 1;
			}
			else if (compareRegisters(operand->reg, R8))
			{
				strcpy(resultBuffer, "argR8");
				return 1;
			}
			else if (compareRegisters(operand->reg, R9))
			{
				strcpy(resultBuffer, "argR9");
				return 1;
			}

			strcpy(resultBuffer, registerStrs[operand->reg]);
		}

		return 1;
	}

	return 0;
}

static unsigned char decompileExpression(struct Function* functions, unsigned short numOfFunctions, unsigned short startInstructionIndex, unsigned short functionIndex, unsigned char targetReg, struct VariableType* type, char* resultBuffer, unsigned char resultBufferSize)
{
	char expressions[5][50];
	int expressionIndex = 0;

	unsigned char finished = 0;

	for (int i = startInstructionIndex; i >= 0; i--)
	{
		if (finished)
		{
			break;
		}
		
		struct DisassembledInstruction* currentInstruction = &functions[functionIndex].instructions[i];

		if ((compareRegisters(currentInstruction->operands[0].reg, targetReg) && doesInstructionModifyOperand(currentInstruction, 0, &finished)) || doesOpcodeModifyRegister(currentInstruction->opcode, targetReg, &finished))
		{
			char targetOperand = getLastOperand(currentInstruction);

			if (currentInstruction->opcode == XOR && compareRegisters(currentInstruction->operands[1].reg, targetReg))
			{
				strcpy(expressions[expressionIndex], ")0");
				expressionIndex++;

				currentInstruction->hasBeenDecompiled = 1;

				finished = 1;
				break;
			}

			char operandStr[50] = { 0 };
			if (!decompileOperand(functions, numOfFunctions, i - 1, functionIndex, &currentInstruction->operands[targetOperand], type, operandStr, 50))
			{
				return 0;
			}

			char operationStr[20] = { 0 };
			if (!getOperationStr(currentInstruction->opcode, 0, operationStr))
			{
				return 0;
			}

			sprintf(expressions[expressionIndex], ")%s%s", operationStr, operandStr);
			expressionIndex++;

			currentInstruction->hasBeenDecompiled = 1;
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
			if (!decompileFunctionCall(functions, numOfFunctions, i, functionIndex, &functions[calleIndex], &functionCall))
			{
				return 0;
			}

			sprintf(expressions[expressionIndex], ")%s", functionCall.line);

			expressionIndex++;

			finished = 1;
			break;
		}
	}

	if (!finished) { return 0; }

	for (int i = expressionIndex - 1; i >= expressionIndex - 2 && i >= 0; i--) // remove last two )
	{
		char temp[50] = { 0 };
		strcpy(temp, expressions[i] + 1);
		expressions[i][0] = 0;
		strcpy(expressions[i], temp);
	}

	if (expressionIndex > 1) 
	{
		for (int i = 0; i < expressionIndex - 1; i++)
		{
			strcpy(resultBuffer + strlen(resultBuffer), "(");
		}
	}
	
	for (int i = expressionIndex - 1; i >= 0; i--)
	{
		strcpy(resultBuffer + strlen(resultBuffer), expressions[i]);
	}

	if (expressionIndex > 1) 
	{ 
		strcpy(resultBuffer + strlen(resultBuffer), ")"); 
	}

	return 1;
}

static unsigned char decompileFunctionCall(struct Function* functions, unsigned short numOfFunctions, unsigned short startInstructionIndex, unsigned short functionIndex, struct Function* callee, struct LineOfC* result)
{
	struct DisassembledInstruction* firstInstruction = &functions[functionIndex].instructions[startInstructionIndex];

	firstInstruction->hasBeenDecompiled = 1;

	if (firstInstruction->opcode == JMP_NEAR || firstInstruction->opcode == JMP_FAR)
	{
		sprintf(result->line, "%s()", callee->name);
		return 1;
	}
	
	sprintf(result->line, "%s(", callee->name);

	for (int i = 0; i < callee->numOfRegArgs; i++)
	{
		for (int j = startInstructionIndex; j >= 0; j--)
		{
			struct DisassembledInstruction* currentInstruction = &functions[functionIndex].instructions[j];

			if (currentInstruction->operands[0].type == REGISTER && doesInstructionModifyOperand(currentInstruction, 0, 0))
			{
				unsigned char reg = currentInstruction->operands[0].reg;

				char cxCheck = i == 0 && compareRegisters(reg, CX);
				char dxCheck = i == 1 && compareRegisters(reg, DX);
				char r8Check = i == 2 && compareRegisters(reg, R8);
				char r9Check = i == 3 && compareRegisters(reg, R9);

				if (cxCheck || dxCheck || r8Check || r9Check)
				{
					char argStr[50] = { 0 };
					if (!decompileExpression(functions, numOfFunctions, j, functionIndex, reg, &callee->regArgs[i], argStr, 50))
					{
						return 0;
					}

					sprintf(result->line + strlen(result->line), "%s, ", argStr);

					currentInstruction->hasBeenDecompiled = 1;

					break;
				}
			}
		}
	}

	int stackArgsFound = 0;
	for (int i = startInstructionIndex; i >= 0; i--)
	{
		if (stackArgsFound == callee->numOfStackArgs) { break; }

		struct DisassembledInstruction* currentInstruction = &functions[functionIndex].instructions[i];

		if (currentInstruction->opcode == PUSH)
		{
			char argStr[20] = { 0 };
			if (!decompileOperand(functions, numOfFunctions, i, functionIndex, &currentInstruction->operands[0], &callee->stackArgs[stackArgsFound], argStr, 20))
			{
				return 0;
			}

			sprintf(result->line + strlen(result->line), "%s, ", argStr);

			currentInstruction->hasBeenDecompiled = 1;

			stackArgsFound++;
		}
		else if (currentInstruction->operands[0].type == MEM_ADDRESS && compareRegisters(currentInstruction->operands[0].memoryAddress.reg, SP))
		{
			unsigned char overwrites = 0;
			if (doesInstructionModifyOperand(currentInstruction, 0, &overwrites) && overwrites)
			{
				int operandIndex = getLastOperand(currentInstruction);

				char argStr[20] = { 0 };
				if (!decompileOperand(functions, numOfFunctions, i, functionIndex, &currentInstruction->operands[operandIndex], &callee->stackArgs[stackArgsFound], argStr, 20))
				{
					return 0;
				}

				sprintf(result->line + strlen(result->line), "%s, ", argStr);

				currentInstruction->hasBeenDecompiled = 1;

				stackArgsFound++;
			}
		}
	}

	if (callee->numOfRegArgs != 0 || callee->numOfStackArgs != 0) 
	{
		result->line[strlen(result->line) - 2] = ')';
		result->line[strlen(result->line) - 1] = 0;
	}
	else 
	{
		strcpy(result->line + strlen(result->line), ")");
	}
	
	return 1;
}

static unsigned char getOperationStr(unsigned char opcode, unsigned char getAssignment, char* resultBuffer) 
{
	switch (opcode) 
	{
	case MOV:
	case MOVUPS:
	case MOVUPD:
	case MOVSS: 
	case MOVSD:
	case MOVSX:
		if (getAssignment) { strcpy(resultBuffer, " = "); }
		else { strcpy(resultBuffer, ""); }
		return 1;
	case LEA:
		if (getAssignment) { strcpy(resultBuffer, " = &"); }
		else { strcpy(resultBuffer, "&"); }
		return 1;
	case ADD:
	case ADDPS:
	case ADDPD:
	case ADDSS:
	case ADDSD:
		if (getAssignment) { strcpy(resultBuffer, " += "); }
		else { strcpy(resultBuffer, " + "); }
		return 1;
	case SUB:
		if (getAssignment) { strcpy(resultBuffer, " -= "); }
		else { strcpy(resultBuffer, " - "); }
		return 1;
	case AND:
		if (getAssignment) { strcpy(resultBuffer, " &= "); }
		else { strcpy(resultBuffer, " & "); }
		return 1;
	case OR:
		if (getAssignment) { strcpy(resultBuffer, " |= "); }
		else { strcpy(resultBuffer, " | "); }
		return 1;
	case XOR:
		if (getAssignment) { strcpy(resultBuffer, " ^= "); }
		else { strcpy(resultBuffer, " ^ "); }
		return 1;
	case SHL:
		if (getAssignment) { strcpy(resultBuffer, " <<= "); }
		else { strcpy(resultBuffer, " << "); }
		return 1;
	case SHR:
		if (getAssignment) { strcpy(resultBuffer, " >>= "); }
		else { strcpy(resultBuffer, " >> "); }
		return 1;
	case IMUL:
		if (getAssignment) { strcpy(resultBuffer, " *= "); }
		else { strcpy(resultBuffer, " * "); }
		return 1;
	case IDIV:
		if (getAssignment) { strcpy(resultBuffer, " /= "); }
		else { strcpy(resultBuffer, " / "); }
		return 1;
	case CVTPS2PD:
	case CVTSS2SD:
		if (getAssignment) { strcpy(resultBuffer, " = (double)"); }
		else { strcpy(resultBuffer, "(double)"); }
		return 1;
	case CVTPD2PS:
	case CVTSD2SS:
		if (getAssignment) { strcpy(resultBuffer, " = (float)"); }
		else { strcpy(resultBuffer, "(float)"); }
		return 1;
	}
	
	return 0;
}

static unsigned char getLastOperand(struct DisassembledInstruction* instruction) 
{
	unsigned char result = 0;

	for (int i = 2; i >= 0; i--)
	{
		if (instruction->operands[i].type != NO_OPERAND) 
		{
			result = i;
			break;
		}
	}

	return result;
}

unsigned char isOperandStackArgument(struct Operand* operand)
{
	return operand->type == MEM_ADDRESS && compareRegisters(operand->memoryAddress.reg, BP) && operand->memoryAddress.constDisplacement > 0;
}

unsigned char isOperandLocalVariable(struct Operand* operand) 
{
	return operand->type == MEM_ADDRESS && compareRegisters(operand->memoryAddress.reg, BP) && operand->memoryAddress.constDisplacement < 0;
}

unsigned char doesInstructionModifyOperand(struct DisassembledInstruction* instruction, unsigned char operandNum, unsigned char* overwrites)
{
	if (operandNum == 0) 
	{
		if (instruction->opcode == XOR && areOperandsEqual(&instruction->operands[0], &instruction->operands[1])) 
		{
			if (overwrites != 0) { *overwrites = 1; }
			return 1;
		}
		
		switch (instruction->opcode)
		{
		case MOV:
		case MOVUPS:
		case MOVUPD:
		case MOVSS:
		case MOVSD:
		case MOVSX:
		case LEA:
		case CVTPS2PD:
		case CVTPD2PS:
		case CVTSS2SD:
		case CVTSD2SS:
			if (overwrites != 0) { *overwrites = 1; }
			return 1;
		case ADD:
		case ADDPS:
		case ADDPD:
		case ADDSS:
		case ADDSD:
		case SUB:
		case AND:
		case OR:
		case SHL:
		case SHR:
		case IMUL:
		case XOR:
			return 1;
		}
	}

	return 0;
}

unsigned char doesOpcodeModifyRegister(unsigned char opcode, unsigned char reg, unsigned char* overwrites) // some opcodes may modify a register even if it isn't an operand
{
	if (compareRegisters(reg, AX)) 
	{
		switch (opcode) 
		{
		case IDIV:
			return 1;
		}
	}

	return 0;
}

static unsigned char areOperandsEqual(struct Operand* op1, struct Operand* op2) 
{
	if(op1->type == op2->type)
	{
		struct MemoryAddress* m1 = &op1->memoryAddress;
		struct MemoryAddress* m2 = &op2->memoryAddress;

		switch (op1->type) 
		{
		case NO_OPERAND:
			return 1;
		case SEGMENT:
			return op1->segment == op2->segment;
		case REGISTER:
			return op1->reg == op2->reg;
		case MEM_ADDRESS:
			return m1->reg == m2->reg && m1->constDisplacement == m2->constDisplacement && m1->ptrSize == m2->ptrSize && m1->scale == m2->scale && m1->constSegment == m2->constSegment && m1->regDisplacement == m2->regDisplacement && m1->segment == m2->segment;
		case IMMEDIATE:
			return op1->immediate == op2->immediate;
		}
	}

	return 0;
}