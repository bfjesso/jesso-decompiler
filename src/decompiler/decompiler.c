#include "decompiler.h"
#include "../disassembler/opcodes.h"
#include "../disassembler/registers.h"

#include <stdio.h>
#include <string.h>

static const char* primitiveTypeStrs[] =
{
	"void",
	
	"char",
	"short",
	"int",
	"long long",

	"float",
	"double"
};

unsigned short decompileFunction(struct DecompilationParameters params, const char* functionName, struct LineOfC* resultBuffer, unsigned short resultBufferLen)
{
	if (resultBufferLen < 4) { return 0; }

	resetDecompilationState(params.currentFunc);
	
	int numOfLinesDecompiled = 0;

	if (!generateFunctionHeader(params.currentFunc, functionName, &resultBuffer[numOfLinesDecompiled]))
	{
		return 0;
	}
	numOfLinesDecompiled++;

	strcpy(resultBuffer[numOfLinesDecompiled].line, "{");
	resultBuffer[numOfLinesDecompiled].indents = 0;
	numOfLinesDecompiled++;

	if (params.currentFunc->numOfLocalVars > 0)
	{
		if (!declareAllLocalVariables(params.currentFunc, resultBuffer, &numOfLinesDecompiled, resultBufferLen))
		{
			return 0;
		}

		strcpy(resultBuffer[numOfLinesDecompiled].line, "");
		resultBuffer[numOfLinesDecompiled].indents = 1;
		numOfLinesDecompiled++;
	}

	unsigned char scopesDepth = 1;

	struct DisassembledInstruction* lastJccEndpoint = 0;

	for (int i = 0; i < params.currentFunc->numOfInstructions; i++)
	{
		if (numOfLinesDecompiled > resultBufferLen) 
		{ 
			return 0; 
		}

		struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[i]);

		if (i == params.currentFunc->numOfInstructions - 1 && params.currentFunc->returnType != VOID_TYPE)
		{
			params.startInstructionIndex = i;
			if (decompileReturnStatement(params, nextScope != 0 ? nextScope->end : currentScope != 0 ? currentScope->start : params.currentFunc->addresses[0], &resultBuffer[numOfLinesDecompiled]))
			{
				resultBuffer[numOfLinesDecompiled].indents = scopesDepth;
				numOfLinesDecompiled++;
			}
		}

		for (int j = 0; j < numOfScopes; j++)
		{
			if (params.currentFunc->addresses[i] == scopes[j].start)
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

				params.startInstructionIndex = i;
				if (!decompileCondition(params, &scopes[j], &resultBuffer[numOfLinesDecompiled]))
				{
					return 0;
				}
				resultBuffer[numOfLinesDecompiled].indents = scopesDepth;
				numOfLinesDecompiled++;

				currentScope = currentScope != 0 ? currentScope->lastScope : 0;

				break;
			}
		}

		while (nextScope != 0 && params.currentFunc->addresses[i] == nextScope->end)
		{
			strcpy(resultBuffer[numOfLinesDecompiled].line, "}");
			resultBuffer[numOfLinesDecompiled].indents = scopesDepth;
			numOfLinesDecompiled++;

			scopesDepth++;

			nextScope->lastScope = currentScope;
			currentScope = nextScope;

			scopeIndex++;
			nextScope = scopeIndex >= numOfScopes ? 0 : &scopes[scopeIndex];
			
			if (params.currentFunc->returnType != VOID_TYPE)
			{
				params.startInstructionIndex = i;
				if (decompileReturnStatement(params, nextScope != 0 ? nextScope->end : currentScope->start, &resultBuffer[numOfLinesDecompiled]))
				{
					resultBuffer[numOfLinesDecompiled].indents = scopesDepth;
					numOfLinesDecompiled++;
				}
			}
		}

		if (checkForAssignment(&(params.currentFunc->instructions[i])))
		{
			struct LocalVariable* localVar = getLocalVarByOffset(params.currentFunc, (int)(currentInstruction->operands[0].memoryAddress.constDisplacement));

			unsigned char type = 0;
			if (!localVar) 
			{
				type = getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[0]);
			}
			else 
			{
				type = localVar->type;
			}

			params.startInstructionIndex = i;
			if (decompileAssignment(params, type, &resultBuffer[numOfLinesDecompiled]))
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
		params.startInstructionIndex = i;
		if (checkForFunctionCall(params, &callee))
		{
			params.startInstructionIndex = i;
			if (decompileFunctionCall(params, callee, &resultBuffer[numOfLinesDecompiled]))
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

	strcpy(resultBuffer[numOfLinesDecompiled].line, "}");
	resultBuffer[numOfLinesDecompiled].indents = 0;
	numOfLinesDecompiled++;

	return numOfLinesDecompiled;
}

static unsigned short generateFunctionHeader(struct Function* function, const char* functionName, struct LineOfC* result) 
{
	sprintf(result->line, "%s %s %s(", primitiveTypeStrs[function->returnType], callingConventionStrs[function->callingConvention], functionName);

	result->indents = 0;

	for (int i = 0; i < function->numOfRegArgs; i++) 
	{
		char regArgStr[10];

		if (function->regArgRegs[i] >= RAX && function->regArgRegs[i] <= RIP)
		{
			sprintf(regArgStr, "arg%s", registerStrs[(function->regArgRegs[i]-RAX)+AX]);
		} 
		else 
		{
			sprintf(regArgStr, "arg%s", registerStrs[function->regArgRegs[i]]);
		}
		
		if (i == function->numOfRegArgs - 1 && function->numOfStackArgs == 0) 
		{
			sprintf(result->line + strlen(result->line), "%s %s", primitiveTypeStrs[function->regArgTypes[i]], regArgStr);
		}
		else 
		{
			sprintf(result->line + strlen(result->line), "%s %s, ", primitiveTypeStrs[function->regArgTypes[i]], regArgStr);
		}
	}

	for (int i = 0; i < function->numOfStackArgs; i++)
	{
		char stackArgStr[10];
		sprintf(stackArgStr, "arg%X", function->stackArgBpOffsets[i]);
		
		if (i == function->numOfStackArgs - 1)
		{
			sprintf(result->line + strlen(result->line), "%s %s", primitiveTypeStrs[function->stackArgTypes[i]], stackArgStr);
		}
		else
		{
			sprintf(result->line + strlen(result->line), "%s %s, ", primitiveTypeStrs[function->stackArgTypes[i]], stackArgStr);
		}
	}

	strcpy(result->line + strlen(result->line), ")");

	return 1;
}

static unsigned char declareAllLocalVariables(struct Function* function, struct LineOfC* resultBuffer, int* resultBufferIndex, unsigned short resultBufferLen)
{
	for (int i = 0; i < function->numOfLocalVars; i++)
	{
		sprintf(resultBuffer[*resultBufferIndex].line, "%s var%X;", primitiveTypeStrs[function->localVars[i].type], -function->localVars[i].stackOffset);
		resultBuffer[*resultBufferIndex].indents = 1;
		(*resultBufferIndex)++;
	}

	return 1;
}

static unsigned char getAllScopes(struct Function* function, struct Scope* resultBuffer, unsigned char resultBufferLen)
{
	unsigned char resultBufferIndex = 0;
	unsigned long long lastJmpDstAddress = 0;
	
	for (int i = function->numOfInstructions - 1; i > 0; i--)
	{
		struct DisassembledInstruction* currentInstruction = &function->instructions[i];

		if (currentInstruction->opcode >= JA_SHORT && currentInstruction->opcode <= JMP_SHORT)
		{
			if (resultBufferIndex >= resultBufferLen) { return 0; }

			unsigned long long jmpDst = function->addresses[i] + currentInstruction->operands[0].immediate;

			if (currentInstruction->opcode == JMP_SHORT)
			{
				if (lastJmpDstAddress == jmpDst)
				{
					continue;
				}
				else
				{
					lastJmpDstAddress = jmpDst;
				}
			}
			
			resultBuffer[resultBufferIndex].lastScope = 0;
			resultBuffer[resultBufferIndex].start = function->addresses[i];
			resultBuffer[resultBufferIndex].end = jmpDst;
			resultBuffer[resultBufferIndex].orJccInstructionIndex = -1;
			resultBuffer[resultBufferIndex].isElseIf = 0;

			// get the address of the instruction right before the one jumped to
			for(int j = i; j < function->numOfInstructions - 1; j++)
			{
				if(function->addresses[j + 1] == jmpDst)
				{
					resultBuffer[resultBufferIndex].end = function->addresses[j];
					
					// check for else if
					if (resultBufferIndex > 0 && function->instructions[j].opcode == JMP_SHORT && resultBufferIndex > 0 && resultBuffer[resultBufferIndex].end != resultBuffer[resultBufferIndex - 1].end)
					{
						if (function->addresses[j] + function->instructions[j].operands[0].immediate > resultBuffer[resultBufferIndex - 1].end) 
						{
							resultBuffer[resultBufferIndex - 1].isElseIf = 1;
						}
					}
					else if (function->instructions[j].opcode >= JA_SHORT && function->instructions[j].opcode <= JZ_SHORT) // check for ||
					{
						resultBufferIndex--;
						resultBuffer[resultBufferIndex].orJccInstructionIndex = i; // this needs to be updated to support more than one || condition in decompileCondition
					}
					
					break;
				}
			}

			i--;

			resultBufferIndex++;
		}
	}

	return resultBufferIndex;
}

static unsigned char modifiesReturnRegister(struct DecompilationParameters params)
{
	struct DisassembledInstruction* instruction = &(params.currentFunc->instructions[params.startInstructionIndex]);
	unsigned long long address = params.currentFunc->addresses[params.startInstructionIndex];
	
	if (instruction->hasBeenDecompiled) { return 0; }
	
	if (params.currentFunc->returnType == FLOAT_TYPE || params.currentFunc->returnType == DOUBLE_TYPE)
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
			int calleIndex = findFunctionByAddress(params.functions, 0, params.numOfFunctions - 1, calleeAddress);

			if (calleIndex == -1 || params.functions[calleIndex].returnType == VOID_TYPE)
			{
				return 0;
			}

			return 1;
		}
	}
	
	return 0;
}

static unsigned char checkForReturnStatement() 
{
	
}

static unsigned char checkForAssignment(struct DisassembledInstruction* instruction)
{
	if (instruction->hasBeenDecompiled) { return 0; }
	
	if (doesInstructionModifyOperand(instruction, 0, 0) && instruction->operands[0].type == MEM_ADDRESS) 
	{
		return 1;
	}

	return 0;
}

static unsigned char checkForFunctionCall(struct DecompilationParameters params, struct Function** calleeRef)
{
	struct DisassembledInstruction* instruction = &(params.currentFunc->instructions[params.startInstructionIndex]);
	unsigned long long address = params.currentFunc->addresses[params.startInstructionIndex];
	
	if (instruction->hasBeenDecompiled) { return 0; }
	
	if (instruction->opcode == CALL_NEAR || instruction->opcode == JMP_NEAR)
	{
		unsigned long long calleeAddress = address + instruction->operands[0].immediate;
		int calleIndex = findFunctionByAddress(params.functions, 0, params.numOfFunctions - 1, calleeAddress);

		if (calleIndex == -1)
		{
			return 0;
		}

		*calleeRef = &(params.functions[calleIndex]);

		return 1;
	}

	return 0;
}

static unsigned char decompileCondition(struct DecompilationParameters params, struct Scope* scope, struct LineOfC* result)
{
	struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[params.startInstructionIndex]);

	char compOperator1[3] = { 0 };
	
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
			strcpy(compOperator1, "!=");
			break;
		case JNZ_SHORT:
			strcpy(compOperator1, "==");
			break;
		case JG_SHORT:
			strcpy(compOperator1, "<=");
			break;
		case JL_SHORT:
			strcpy(compOperator1, ">=");
			break;
		case JLE_SHORT:
		case JBE_SHORT:
			strcpy(compOperator1, ">");
			break;
		case JGE_SHORT:
			strcpy(compOperator1, "<");
			break;
		}

		currentInstruction->hasBeenDecompiled = 1;
	}

	char compOperator2[3] = { 0 };
	if(scope->orJccInstructionIndex != -1)
	{
		switch (params.currentFunc->instructions[scope->orJccInstructionIndex].opcode)
		{
		case JZ_SHORT:
			strcpy(compOperator2, "==");
			break;
		case JNZ_SHORT:
			strcpy(compOperator2, "!=");
			break;
		case JG_SHORT:
			strcpy(compOperator2, ">=");
			break;
		case JL_SHORT:
			strcpy(compOperator2, "<=");
			break;
		case JLE_SHORT:
		case JBE_SHORT:
			strcpy(compOperator2, "<");
			break;
		case JGE_SHORT:
			strcpy(compOperator2, ">");
			break;
		}

		params.currentFunc->instructions[scope->orJccInstructionIndex].hasBeenDecompiled = 1;
	}
	
	char condition1[100] = { 0 };

	for (int i = params.startInstructionIndex - 1; i >= 0; i--)
	{
		currentInstruction = &(params.currentFunc->instructions[i]);

		unsigned char type = 0;
		switch (currentInstruction->opcode) 
		{
		case CMP:
			type = INT_TYPE;
			break;
		case COMISS:
			type = FLOAT_TYPE;
			break;
		case COMISD:
			type = DOUBLE_TYPE;
			break;
		}

		if (type != 0)
		{
			params.startInstructionIndex = i;
			
			char operand1Str[100] = { 0 };
			if (!decompileOperand(params, &currentInstruction->operands[0], type, operand1Str, 20))
			{
				return 0;
			}

			char operand2Str[100] = { 0 };
			if (!decompileOperand(params, &currentInstruction->operands[1], type, operand2Str, 20))
			{
				return 0;
			}

			sprintf(condition1, "%s %s %s", operand1Str, compOperator1, operand2Str);

			currentInstruction->hasBeenDecompiled = 1;

			break;
		}
	}

	char condition2[100] = { 0 };

	if (scope->orJccInstructionIndex != -1) 
	{
		for (int i = scope->orJccInstructionIndex - 1; i >= 0; i--)
		{
			currentInstruction = &(params.currentFunc->instructions[i]);

			unsigned char type = 0;
			switch (currentInstruction->opcode)
			{
			case CMP:
				type = INT_TYPE;
				break;
			case COMISS:
				type = FLOAT_TYPE;
				break;
			case COMISD:
				type = DOUBLE_TYPE;
				break;
			}

			if (type != 0)
			{
				params.startInstructionIndex = i;
				
				char operand1Str[100] = { 0 };
				if (!decompileOperand(params, &currentInstruction->operands[0], type, operand1Str, 100))
				{
					return 0;
				}

				char operand2Str[100] = { 0 };
				if (!decompileOperand(params, &currentInstruction->operands[1], type, operand2Str, 100))
				{
					return 0;
				}

				sprintf(condition2, "%s %s %s || ", operand1Str, compOperator2, operand2Str);

				currentInstruction->hasBeenDecompiled = 1;

				break;
			}
		}
	}

	if (scope->isElseIf) 
	{
		sprintf(result->line, "else if(%s%s)", condition2, condition1);
	}
	else 
	{
		sprintf(result->line, "if(%s%s)", condition2, condition1);
	}

	return 1;
}

static unsigned char decompileReturnStatement(struct DecompilationParameters params, unsigned long long scopeStart, struct LineOfC* result)
{
	char returnExpression[100] = { 0 };

	int newStartInstruction = -1;

	for (int i = params.startInstructionIndex; i >= 0; i--)
	{
		if (params.currentFunc->addresses[i] <= scopeStart)
		{
			break;
		}
		
		params.startInstructionIndex = i;
		if (modifiesReturnRegister(params))
		{
			newStartInstruction = i;
			break;
		}
	}

	if (newStartInstruction == -1) 
	{
		return 0;
	}
	
	if (params.currentFunc->instructions[newStartInstruction].opcode == FLD)
	{
		params.startInstructionIndex = newStartInstruction - 1;
		if (!decompileOperand(params, &(params.currentFunc->instructions[newStartInstruction].operands[0]), params.currentFunc->returnType, returnExpression, 100))
		{
			return 0;
		}
	}
	else 
	{
		struct Operand eax = { 0 };
		eax.type = REGISTER;
		eax.reg = AX;

		params.startInstructionIndex = newStartInstruction;
		if (!decompileOperand(params, &eax, params.currentFunc->returnType, returnExpression, 100))
		{
			return 0;
		}
	}
	

	sprintf(result->line, "return %s;", returnExpression);

	return 1;
}

static unsigned char decompileAssignment(struct DecompilationParameters params, unsigned char type, struct LineOfC* result)
{
	struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[params.startInstructionIndex]);
	
	char assignee[100] = { 0 };
	if (!decompileOperand(params, &currentInstruction->operands[0], type, assignee, 100)) 
	{
		return 0;
	}

	char valueToAssign[100] = { 0 };
	struct Operand* operand = &currentInstruction->operands[getLastOperand(currentInstruction)];
	if (!decompileOperand(params, operand, type, valueToAssign, 100))
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

static unsigned char decompileOperand(struct DecompilationParameters params, struct Operand* operand, unsigned char type, char* resultBuffer, unsigned char resultBufferSize)
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
				sprintf(resultBuffer, "var%llX", -operand->memoryAddress.constDisplacement);
			}
			else
			{
				sprintf(resultBuffer, "arg%llX", operand->memoryAddress.constDisplacement);
			}
		}
		else if (compareRegisters(operand->memoryAddress.reg, IP))
		{
			sprintf(resultBuffer, "*(%s*)(0x%llX)", primitiveTypeStrs[type], params.currentFunc->addresses[params.startInstructionIndex+1] + operand->memoryAddress.constDisplacement);
		}
		else if (operand->memoryAddress.reg == NO_REG) 
		{
			sprintf(resultBuffer, "*(%s*)(0x%llX)", primitiveTypeStrs[type], operand->memoryAddress.constDisplacement);
		}
		else 
		{
			struct Operand baseReg = { 0 };
			baseReg.type = REGISTER;
			baseReg.reg = operand->memoryAddress.reg;
			
			char baseOperandStr[100] = { 0 };
			if (!decompileOperand(params, &baseReg, type, baseOperandStr, 100)) 
			{
				return 0;
			}

			if (operand->memoryAddress.constDisplacement != 0) 
			{
				sprintf(resultBuffer, "*(%s*)(%s + 0x%llX)", primitiveTypeStrs[type], baseOperandStr, operand->memoryAddress.constDisplacement);
			}
			else 
			{
				sprintf(resultBuffer, "*(%s*)(%s)", primitiveTypeStrs[type], baseOperandStr);
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
		
		if (!decompileExpression(params, operand->reg, type, resultBuffer, resultBufferSize))
		{
			// thiscall or fastcall argument

			if (compareRegisters(operand->reg, CX))
			{
				strcpy(resultBuffer, "argCX");
			}
			else if (compareRegisters(operand->reg, DX))
			{
				strcpy(resultBuffer, "argDX");
			}
			else if (compareRegisters(operand->reg, R8))
			{
				strcpy(resultBuffer, "argR8");
			}
			else if (compareRegisters(operand->reg, R9))
			{
				strcpy(resultBuffer, "argR9");
			}
			else if (compareRegisters(operand->reg, DI))
			{
				strcpy(resultBuffer, "argDI");
			}
			else if (compareRegisters(operand->reg, SI))
			{
				strcpy(resultBuffer, "argSI");
			}
			else 
			{
				sprintf(resultBuffer, "arg%s", registerStrs[operand->reg]);
			}

			return 1;
		}

		return 1;
	}

	return 0;
}

static unsigned char decompileExpression(struct DecompilationParameters params, unsigned char targetReg, unsigned char type, char* resultBuffer, unsigned char resultBufferSize)
{
	char expressions[5][100] = { 0 };
	int expressionIndex = 0;

	unsigned char finished = 0;

	for (int i = params.startInstructionIndex; i >= 0; i--)
	{
		if (finished)
		{
			break;
		}
		
		struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[i]);

		if ((currentInstruction->operands[0].type == REGISTER && compareRegisters(currentInstruction->operands[0].reg, targetReg) && doesInstructionModifyOperand(currentInstruction, 0, &finished)) || doesOpcodeModifyRegister(currentInstruction->opcode, targetReg, &finished))
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

			params.startInstructionIndex = i - 1;

			char operandStr[100] = { 0 };
			if (!decompileOperand(params, &currentInstruction->operands[targetOperand], type, operandStr, 100))
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
			unsigned long long calleeAddress = params.currentFunc->addresses[i] + currentInstruction->operands[0].immediate;
			int calleeIndex = findFunctionByAddress(params.functions, 0, params.numOfFunctions - 1, calleeAddress);
			if (calleeIndex == -1)
			{
				return 0;
			}

			if (params.functions[calleeIndex].returnType == VOID_TYPE) { continue; }

			params.startInstructionIndex = i;
			struct Function* callee = &(params.functions[calleeIndex]);

			struct LineOfC functionCall = { 0 };
			if (!decompileFunctionCall(params, callee, &functionCall))
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
		char temp[100] = { 0 };
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

static unsigned char decompileFunctionCall(struct DecompilationParameters params, struct Function* callee, struct LineOfC* result)
{
	struct DisassembledInstruction* firstInstruction = &(params.currentFunc->instructions[params.startInstructionIndex]);

	firstInstruction->hasBeenDecompiled = 1;

	if (firstInstruction->opcode == JMP_NEAR || firstInstruction->opcode == JMP_FAR)
	{
		sprintf(result->line, "%s()", callee->name);
		return 1;
	}
	
	sprintf(result->line, "%s(", callee->name);

	unsigned short ogStartInstructionIndex = params.startInstructionIndex;

	for (int i = 0; i < callee->numOfRegArgs; i++)
	{
		for (int j = ogStartInstructionIndex; j >= 0; j--)
		{
			struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[j]);

			if (currentInstruction->operands[0].type == REGISTER && doesInstructionModifyOperand(currentInstruction, 0, 0))
			{
				unsigned char reg = currentInstruction->operands[0].reg;

				if (compareRegisters(reg, callee->regArgRegs[i]))
				{
					params.startInstructionIndex = j;
					
					char argStr[100] = { 0 };
					if (!decompileExpression(params, reg, callee->regArgTypes[i], argStr, 100))
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
	for (int i = ogStartInstructionIndex; i >= 0; i--)
	{
		if (stackArgsFound == callee->numOfStackArgs) { break; }

		struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[i]);

		if (currentInstruction->opcode == PUSH)
		{
			params.startInstructionIndex = i;
			
			char argStr[100] = { 0 };
			if (!decompileOperand(params, &currentInstruction->operands[0], callee->stackArgTypes[stackArgsFound], argStr, 100))
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

				params.startInstructionIndex = i;

				char argStr[100] = { 0 };
				if (!decompileOperand(params, &currentInstruction->operands[operandIndex], callee->stackArgTypes[stackArgsFound], argStr, 100))
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
	case MOVZX:
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