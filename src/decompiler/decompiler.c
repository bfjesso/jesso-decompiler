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

	struct Condition conditions[20] = { 0 };
	int numOfConditions = getAllConditions(params, conditions);
	int conditionIndex = -1; 

	unsigned char isConditionEmpty = 0; // used to check if there is an empty condition that should be removed

	unsigned char numOfIndents = 1;

	unsigned char isInUnreachableState = 0; // if looking at instructions after a ret or jmp

	for (int i = 0; i < params.currentFunc->numOfInstructions; i++)
	{
		if (numOfLinesDecompiled > resultBufferLen)
		{
			return 0;
		}

		struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[i]);

		if (conditionIndex > -1 && i == conditions[conditionIndex].dstIndex)
		{
			// conditionIndex--; // needs to be calculated ?
			numOfIndents--;

			if (isConditionEmpty) 
			{
				// remove the condition and the {
				numOfLinesDecompiled--;
				resultBuffer[numOfLinesDecompiled].line[0] = 0;
				numOfLinesDecompiled--;
				resultBuffer[numOfLinesDecompiled].line[0] = 0;
			}
			else 
			{
				strcpy(resultBuffer[numOfLinesDecompiled].line, "}");
				resultBuffer[numOfLinesDecompiled].indents = numOfIndents;
				numOfLinesDecompiled++;
			}
			
			isInUnreachableState = 0;

			isConditionEmpty = 0;
		}

		if (isInUnreachableState) { continue; }

		params.startInstructionIndex = i;
		if (checkForReturnStatement(params)) 
		{
			if (decompileReturnStatement(params, &resultBuffer[numOfLinesDecompiled]))
			{
				resultBuffer[numOfLinesDecompiled].indents = numOfIndents;
				numOfLinesDecompiled++;

				isConditionEmpty = 0;
			}
			else 
			{
				return 0;
			}
		}

		if (checkForCondition(i, conditions, numOfConditions))
		{
			conditionIndex++;

			params.startInstructionIndex = i;
			if (decompileCondition(params, &conditions[conditionIndex], &resultBuffer[numOfLinesDecompiled]))
			{
				resultBuffer[numOfLinesDecompiled].indents = numOfIndents;
				numOfLinesDecompiled++;

				strcpy(resultBuffer[numOfLinesDecompiled].line, "{");
				resultBuffer[numOfLinesDecompiled].indents = numOfIndents;
				numOfLinesDecompiled++;

				isConditionEmpty = 1;
			}
			else 
			{
				return 0;
			}

			numOfIndents++;
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
				resultBuffer[numOfLinesDecompiled].indents = numOfIndents;
				numOfLinesDecompiled++;

				isConditionEmpty = 0;
			}
			else
			{
				return 0;
			}
		}

		struct Function* callee;
		params.startInstructionIndex = i;
		if (checkForFunctionCall(params, &callee) && callee->returnType == VOID_TYPE) // if it does return something, this call will be decompiled later when its return value is used
		{
			params.startInstructionIndex = i;
			if (decompileFunctionCall(params, callee, &resultBuffer[numOfLinesDecompiled]))
			{
				resultBuffer[numOfLinesDecompiled].indents = numOfIndents;
				strcpy(resultBuffer[numOfLinesDecompiled].line + strlen(resultBuffer[numOfLinesDecompiled].line), ";");
				numOfLinesDecompiled++;

				isConditionEmpty = 0;
			}
			else
			{
				return 0;
			}
		}

		if (currentInstruction->opcode == RET_NEAR || currentInstruction->opcode == RET_FAR || currentInstruction->opcode == JMP_SHORT)
		{
			isInUnreachableState = 1;
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

static int getAllConditions(struct DecompilationParameters params, struct Condition* conditionsBuffer)
{
	// not hadling && and || yet
	// also not handling for or while loops yet


	// find all conditional jumps
	int jccIndexes[20] = { 0 };
	int jccDstIndexes[20] = { 0 }; // the index of the instruction jumped to by the jcc
	int exitDstIndexes[20] = { 0 }; // if the last instruction of the condition is a jmp, this is the index of the instruction jumped to by that jmp
	int jccCount = 0;
	for (int i = 0; i < params.currentFunc->numOfInstructions; i++)
	{
		struct DisassembledInstruction* instruction = &(params.currentFunc->instructions[i]);

		if (instruction->opcode >= JA_SHORT && instruction->opcode < JMP_SHORT) 
		{
			jccIndexes[jccCount] = i;

			// getting index of instruction jumped to by jcc (minus one)
			unsigned long long jccDst = params.currentFunc->addresses[i] + instruction->operands[0].immediate;
			int jccDstIndex = findInstructionByAddress(params.currentFunc, 0, params.currentFunc->numOfInstructions - 1, jccDst);
			jccDstIndexes[jccCount] = jccDstIndex;

			// if the conditions ends with a jmp, this will get the index of the instruction jumped to by that jmp
			if (params.currentFunc->instructions[jccDstIndex - 1].opcode == JMP_SHORT) 
			{
				unsigned long long jmpDst = params.currentFunc->addresses[jccDstIndex - 1] + params.currentFunc->instructions[jccDstIndex - 1].operands[0].immediate;
				int jmpDstIndex = findInstructionByAddress(params.currentFunc, 0, params.currentFunc->numOfInstructions - 1, jmpDst);
				exitDstIndexes[jccCount] = jmpDstIndex;
			}
			else 
			{
				exitDstIndexes[jccCount] = -1;
			}
			
			jccCount++;
		}
	}

	// set types
	int conditionsIndex = 0;
	for (int i = 0; i < jccCount; i++) 
	{
		conditionsBuffer[conditionsIndex].jccIndex = jccIndexes[i];
		conditionsBuffer[conditionsIndex].dstIndex = jccDstIndexes[i];
		
		// check for &&
		int andCount = 0;
		for (int j = i + 1; j < jccCount; j++) 
		{
			if (jccDstIndexes[i] == jccDstIndexes[j]) 
			{
				conditionsBuffer[conditionsIndex].andJccIndexes[andCount] = jccIndexes[j];
				andCount++;
			}
			else 
			{
				break;
			}
		}
		conditionsBuffer[conditionsIndex].numOfAnds = andCount;
		
		// check for else if
		if (i > 0 && exitDstIndexes[i] != -1 && exitDstIndexes[i - 1] == exitDstIndexes[i])
		{
			conditionsBuffer[conditionsIndex].type = ELSE_IF_CT;
		}
		else
		{
			conditionsBuffer[conditionsIndex].type = IF_CT;

			// add else
			if (i > 0 && exitDstIndexes[i - 1] != -1)
			{
				conditionsIndex++;

				conditionsBuffer[conditionsIndex].jccIndex = jccDstIndexes[i - 1];
				conditionsBuffer[conditionsIndex].dstIndex = exitDstIndexes[i - 1];
				conditionsBuffer[conditionsIndex].type = ELSE_CT;
			}
		}

		conditionsIndex++;

		i += andCount;
	}
	// add else
	if (jccCount > 0 && exitDstIndexes[jccCount - 1] != -1)
	{
		conditionsBuffer[conditionsIndex].jccIndex = jccDstIndexes[jccCount - 1];
		conditionsBuffer[conditionsIndex].dstIndex = exitDstIndexes[jccCount - 1];
		conditionsBuffer[conditionsIndex].type = ELSE_CT;

		conditionsIndex++;
	}

	return conditionsIndex;
}

static unsigned char doesInstructionModifyReturnRegister(struct DecompilationParameters params)
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

static unsigned char checkForCondition(int instructionIndex, struct Condition* conditions, int numOfConditions)
{
	for (int i = 0; i < numOfConditions; i++) 
	{
		if (conditions[i].jccIndex == instructionIndex) 
		{
			return 1;
		}
	}

	return 0;
}

static unsigned char checkForReturnStatement(struct DecompilationParameters params)
{
	struct DisassembledInstruction* instruction = &(params.currentFunc->instructions[params.startInstructionIndex]);
	
	if (instruction->opcode == RET_NEAR || instruction->opcode == RET_FAR) 
	{
		return 1;
	}

	// check if jump to a return. this will only count if the jump leads directly to a ret, meaning the jmp is effectivly a ret instruction
	if (instruction->opcode == JMP_SHORT) 
	{
		unsigned long long jmpDst = params.currentFunc->addresses[params.startInstructionIndex] + instruction->operands[0].immediate;

		int jmpDstIndex = findInstructionByAddress(params.currentFunc, 0, params.currentFunc->numOfInstructions - 1, jmpDst);
		for (int i = jmpDstIndex; i < params.currentFunc->numOfInstructions; i++) // checking if the function leads to a return without doing anything in between
		{
			params.startInstructionIndex = i;

			if (checkForReturnStatement(params)) 
			{
				return 1;
			}

			if (checkForAssignment(&(params.currentFunc->instructions[i])) || doesInstructionModifyReturnRegister(params))
			{
				return 0;
			}
		}
	}

	return 0;
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

static unsigned char decompileCondition(struct DecompilationParameters params, struct Condition* condition, struct LineOfC* result)
{
	struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[params.startInstructionIndex]);

	if (condition->type == ELSE_CT)
	{
		strcpy(result->line, "else");
		return 1;
	}

	char conditionExpression[100] = { 0 };
	if (!decompileConditionExpression(params, conditionExpression))
	{
		return 0;
	}

	for (int i = 0; i < condition->numOfAnds; i++) 
	{
		char currentConditionExpression[100] = { 0 };
		params.startInstructionIndex = condition->andJccIndexes[i];
		if (!decompileConditionExpression(params, currentConditionExpression)) 
		{
			return 0;
		}

		strcat(conditionExpression, " && ");
		strcat(conditionExpression, currentConditionExpression);
	}

	if (condition->type == IF_CT)
	{
		sprintf(result->line, "if(%s)", conditionExpression);
	}
	else 
	{
		sprintf(result->line, "else if(%s)", conditionExpression);
	}

	return 1;
}

static unsigned char decompileConditionExpression(struct DecompilationParameters params, char* resultBuffer) 
{
	struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[params.startInstructionIndex]);

	char compOperator[3] = { 0 };
	
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
	case JBE_SHORT:
		strcpy(compOperator, ">");
		break;
	case JGE_SHORT:
		strcpy(compOperator, "<");
		break;
	default:
		return 0;
	}

	currentInstruction->hasBeenDecompiled = 1;

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

			sprintf(resultBuffer, "%s %s %s", operand1Str, compOperator, operand2Str);

			currentInstruction->hasBeenDecompiled = 1;

			return 1;
		}
	}

	return 0;
}

static unsigned char decompileReturnStatement(struct DecompilationParameters params, struct LineOfC* result)
{
	char returnExpression[100] = { 0 };

	int newStartInstruction = -1;

	// find where a return register is first being modified
	for (int i = params.startInstructionIndex; i >= 0; i--)
	{
		params.startInstructionIndex = i;
		if (doesInstructionModifyReturnRegister(params))
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