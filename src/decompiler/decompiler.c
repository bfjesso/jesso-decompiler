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

unsigned short decompileFunction(struct DecompilationParameters params, struct LineOfC* resultBuffer, unsigned short resultBufferLen)
{
	if (resultBufferLen < 4) { return 0; }

	int numOfLinesDecompiled = 0;

	if (!generateFunctionHeader(params.currentFunc, &resultBuffer[numOfLinesDecompiled]))
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

		// checking for end of condition
		for (int j = 0; j < numOfConditions; j++) 
		{
			if (!conditions[j].isCombinedByOther && i == conditions[j].dstIndex)
			{
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
		}

		int conditionIndex = checkForCondition(i, conditions, numOfConditions);
		if (conditionIndex != -1)
		{
			params.startInstructionIndex = i;
			if (decompileCondition(params, conditions, conditionIndex, &resultBuffer[numOfLinesDecompiled]))
			{
				resultBuffer[numOfLinesDecompiled].indents = numOfIndents;
				numOfLinesDecompiled++;

				strcpy(resultBuffer[numOfLinesDecompiled].line, "{");
				resultBuffer[numOfLinesDecompiled].indents = numOfIndents;
				numOfLinesDecompiled++;

				isConditionEmpty = 1;
				isInUnreachableState = 0;
			}
			else
			{
				return 0;
			}

			numOfIndents++;
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

		if (checkForAssignment(currentInstruction))
		{
			params.startInstructionIndex = i;
			if (decompileAssignment(params, &resultBuffer[numOfLinesDecompiled]))
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
		if (checkForFunctionCall(params, &callee))
		{
			params.startInstructionIndex = i;
			if (decompileFunctionCall(params, callee, &resultBuffer[numOfLinesDecompiled]))
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

static unsigned short generateFunctionHeader(struct Function* function, struct LineOfC* result) 
{
	sprintf(result->line, "%s %s %s(", primitiveTypeStrs[function->returnType], callingConventionStrs[function->callingConvention], function->name);

	result->indents = 0;

	for (int i = 0; i < function->numOfRegArgs; i++) 
	{
		if (i == function->numOfRegArgs - 1 && function->numOfStackArgs == 0) 
		{
			sprintf(result->line + strlen(result->line), "%s %s", primitiveTypeStrs[function->regArgs[i].type], function->regArgs[i].name);
		}
		else 
		{
			sprintf(result->line + strlen(result->line), "%s %s, ", primitiveTypeStrs[function->regArgs[i].type], function->regArgs[i].name);
		}
	}

	for (int i = 0; i < function->numOfStackArgs; i++)
	{
		if (i == function->numOfStackArgs - 1)
		{
			sprintf(result->line + strlen(result->line), "%s %s", primitiveTypeStrs[function->stackArgs[i].type], function->stackArgs[i].name);
		}
		else
		{
			sprintf(result->line + strlen(result->line), "%s %s, ", primitiveTypeStrs[function->stackArgs[i].type], function->stackArgs[i].name);
		}
	}

	strcat(result->line, ")");

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

static unsigned char doesInstructionModifyReturnRegister(struct DecompilationParameters params)
{
	struct DisassembledInstruction* instruction = &(params.currentFunc->instructions[params.startInstructionIndex]);
	unsigned long long address = params.currentFunc->addresses[params.startInstructionIndex];
	
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

static unsigned char decompileCondition(struct DecompilationParameters params, struct Condition* conditions, int conditionIndex, struct LineOfC* result)
{
	if (conditions[conditionIndex].conditionType == ELSE_CT)
	{
		strcpy(result->line, "else");
		return 1;
	}

	char conditionExpression[100] = { 0 };

	if (conditions[conditionIndex].otherJccsLogicType == OR_LT)
	{
		if (!decompileConditionExpression(params, conditionExpression, 0))
		{
			return 0;
		}

		for (int i = 0; i < conditions[conditionIndex].numOfOtherJccs; i++)
		{
			char currentConditionExpression[100] = { 0 };
			params.startInstructionIndex = conditions[conditionIndex].otherJccIndexes[i];
			if (!decompileConditionExpression(params, currentConditionExpression, i == conditions[conditionIndex].numOfOtherJccs - 1))
			{
				return 0;
			}

			strcat(conditionExpression, " || ");
			strcat(conditionExpression, currentConditionExpression);
		}
	}
	else 
	{
		if (!decompileConditionExpression(params, conditionExpression, 1)) // this needs to run if otherJccsLogicType is either AND_LT or NONE_LT. if it is NONE_LT, the loop wont run because numOfOtherJccs will be 0 
		{
			return 0;
		}

		for (int i = 0; i < conditions[conditionIndex].numOfOtherJccs; i++)
		{
			char currentConditionExpression[100] = { 0 };
			params.startInstructionIndex = conditions[conditionIndex].otherJccIndexes[i];
			if (!decompileConditionExpression(params, currentConditionExpression, 1))
			{
				return 0;
			}

			strcat(conditionExpression, " && ");
			strcat(conditionExpression, currentConditionExpression);
		}
	}

	struct LineOfC combinedConditionExpression = { 0 };
	if (conditions[conditionIndex].combinedConditionIndex)
	{
		params.startInstructionIndex = conditions[conditions[conditionIndex].combinedConditionIndex].jccIndex;
		if (decompileCondition(params, conditions, conditions[conditionIndex].combinedConditionIndex, &combinedConditionExpression))
		{
			wrapStrInParentheses(conditionExpression);
			
			if (conditions[conditionIndex].combinationLogicType == AND_LT)
			{
				strcat(conditionExpression, " && ");
			}
			else 
			{
				strcat(conditionExpression, " || ");
			}

			strcat(conditionExpression, combinedConditionExpression.line);
		}
		else 
		{
			return 0;
		}
	}

	if (conditions[conditionIndex].isCombinedByOther)
	{
		strcpy(result->line, conditionExpression);
		return 1;
	}

	if (conditions[conditionIndex].conditionType == LOOP_CT)
	{
		// check for for loop
		if (params.currentFunc->instructions[conditions[conditionIndex].exitIndex - 1].opcode == JMP_SHORT)
		{
			struct LineOfC assignmentExpression = { 0 };
			for (int i = conditions[conditionIndex].exitIndex; i < conditions[conditionIndex].jccIndex; i++) 
			{
				struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[i]);
				if (checkForAssignment(currentInstruction))
				{
					params.startInstructionIndex = i;
					if (decompileAssignment(params, &assignmentExpression))
					{
						break;
					}
					else 
					{
						return 0;
					}
				}
			}
			int len = strlen(assignmentExpression.line);
			if (len > 0) 
			{
				assignmentExpression.line[len - 1] = 0; // remove ;
			}
			
			sprintf(result->line, "for (; %s; %s)", conditionExpression, assignmentExpression.line);
		}
		else 
		{
			sprintf(result->line, "while (%s)", conditionExpression);
		}
	}
	else if (conditions[conditionIndex].conditionType == IF_CT)
	{
		sprintf(result->line, "if (%s)", conditionExpression);
	}
	else 
	{
		sprintf(result->line, "else if (%s)", conditionExpression);
	}

	return 1;
}

static unsigned char decompileConditionExpression(struct DecompilationParameters params, char* resultBuffer, unsigned char invertOperator)
{
	struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[params.startInstructionIndex]);

	char compOperator[3] = { 0 };
	
	if (invertOperator) 
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
		case JBE_SHORT:
			strcpy(compOperator, ">");
			break;
		case JGE_SHORT:
			strcpy(compOperator, "<");
			break;
		default:
			return 0;
		}
	}
	else 
	{
		switch (currentInstruction->opcode)
		{
		case JZ_SHORT:
			strcpy(compOperator, "==");
			break;
		case JNZ_SHORT:
			strcpy(compOperator, "!=");
			break;
		case JG_SHORT:
			strcpy(compOperator, ">");
			break;
		case JL_SHORT:
			strcpy(compOperator, "<");
			break;
		case JLE_SHORT:
		case JBE_SHORT:
			strcpy(compOperator, "<=");
			break;
		case JGE_SHORT:
			strcpy(compOperator, ">=");
			break;
		default:
			return 0;
		}
	}

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

static unsigned char decompileAssignment(struct DecompilationParameters params, struct LineOfC* result)
{
	struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[params.startInstructionIndex]);

	struct StackVariable* localVar = getLocalVarByOffset(params.currentFunc, (int)(currentInstruction->operands[0].memoryAddress.constDisplacement));
	unsigned char type = 0;
	if (!localVar)
	{
		type = getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[0]);
	}
	else
	{
		type = localVar->type;
	}
	
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
		if (compareRegisters(operand->memoryAddress.reg, BP)) 
		{
			if (operand->memoryAddress.constDisplacement < 0)
			{
				struct StackVariable* localVar = getLocalVarByOffset(params.currentFunc, operand->memoryAddress.constDisplacement);
				if (localVar) 
				{
					strcpy(resultBuffer, localVar->name);
				}
				else 
				{
					return 0;
				}
			}
			else
			{
				struct StackVariable* stackArg = getStackArgByOffset(params.currentFunc, operand->memoryAddress.constDisplacement);
				if (stackArg)
				{
					strcpy(resultBuffer, stackArg->name);
				}
				else
				{
					return 0;
				}
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
			// register argument
			struct RegisterVariable* regArg = getRegArgByReg(params.currentFunc, operand->reg);
			if (regArg)
			{
				strcpy(resultBuffer, regArg->name);
				return 1;
			}
			else 
			{
				return 0;
			}
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

		if ((currentInstruction->operands[0].type == REGISTER && 
			compareRegisters(currentInstruction->operands[0].reg, targetReg) && 
			doesInstructionModifyOperand(currentInstruction, 0, &finished)) 
			|| doesOpcodeModifyRegister(currentInstruction->opcode, targetReg, &finished))
		{
			char targetOperand = getLastOperand(currentInstruction);

			params.startInstructionIndex = i - 1;

			if (currentInstruction->opcode == XOR && compareRegisters(currentInstruction->operands[1].reg, targetReg))
			{
				strcpy(expressions[expressionIndex], "0");
				expressionIndex++;

				break;
			}
			else if (currentInstruction->opcode == IMUL && currentInstruction->operands[2].type != NO_OPERAND) 
			{
				char operandStr1[100] = { 0 };
				if (!decompileOperand(params, &currentInstruction->operands[1], type, operandStr1, 100))
				{
					return 0;
				}
				char operandStr2[100] = { 0 };
				if (!decompileOperand(params, &currentInstruction->operands[2], type, operandStr2, 100))
				{
					return 0;
				}

				sprintf(expressions[expressionIndex], "(%s * %s)", operandStr1, operandStr2);
				expressionIndex++;

				break;
			}
			else 
			{
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

				sprintf(expressions[expressionIndex], "%s%s", operationStr, operandStr);
				expressionIndex++;
			}
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

			int callNum = getFunctionCallNumber(params, calleeAddress);
			sprintf(expressions[expressionIndex], "%sRetVal%d", params.functions[calleeIndex].name, callNum);

			expressionIndex++;

			finished = 1;
			break;
		}
	}

	if (!finished) { return 0; }

	for (int i = expressionIndex - 1; i >= 0; i--) 
	{
		if (i < expressionIndex - 2)
		{
			wrapStrInParentheses(resultBuffer);
		}

		strcat(resultBuffer, expressions[i]);
	}

	if (expressionIndex > 1) 
	{
		wrapStrInParentheses(resultBuffer);
	}

	return 1;
}

static unsigned char decompileFunctionCall(struct DecompilationParameters params, struct Function* callee, struct LineOfC* result)
{
	struct DisassembledInstruction* firstInstruction = &(params.currentFunc->instructions[params.startInstructionIndex]);

	if (firstInstruction->opcode == JMP_NEAR || firstInstruction->opcode == JMP_FAR)
	{
		sprintf(result->line, "%s();", callee->name);
		return 1;
	}

	if (callee->returnType != VOID_TYPE) 
	{
		unsigned long long calleeAddress = params.currentFunc->addresses[params.startInstructionIndex] + firstInstruction->operands[0].immediate;
		int callNum = getFunctionCallNumber(params, calleeAddress);
		sprintf(result->line, "%s %sRetVal%d = ", primitiveTypeStrs[callee->returnType], callee->name, callNum);
	}
	
	sprintf(&(result->line[strlen(result->line)]), "%s(", callee->name);

	unsigned short ogStartInstructionIndex = params.startInstructionIndex;

	for (int i = 0; i < callee->numOfRegArgs; i++)
	{
		for (int j = ogStartInstructionIndex; j >= 0; j--)
		{
			struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[j]);

			if (currentInstruction->operands[0].type == REGISTER && doesInstructionModifyOperand(currentInstruction, 0, 0))
			{
				unsigned char reg = currentInstruction->operands[0].reg;

				if (compareRegisters(reg, callee->regArgs[i].reg))
				{
					params.startInstructionIndex = j;
					
					char argStr[100] = { 0 };
					if (!decompileExpression(params, reg, callee->regArgs[i].type, argStr, 100))
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
	for (int i = ogStartInstructionIndex; i >= 0; i--)
	{
		if (stackArgsFound == callee->numOfStackArgs) { break; }

		struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[i]);

		if (currentInstruction->opcode == PUSH)
		{
			params.startInstructionIndex = i;
			
			char argStr[100] = { 0 };
			if (!decompileOperand(params, &currentInstruction->operands[0], callee->stackArgs[stackArgsFound].type, argStr, 100))
			{
				return 0;
			}

			sprintf(result->line + strlen(result->line), "%s, ", argStr);

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
				if (!decompileOperand(params, &currentInstruction->operands[operandIndex], callee->stackArgs[stackArgsFound].type, argStr, 100))
				{
					return 0;
				}

				sprintf(result->line + strlen(result->line), "%s, ", argStr);

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
		strcat(result->line, ")");
	}

	strcat(result->line, ";");
	
	return 1;
}

static int getFunctionCallNumber(struct DecompilationParameters params, unsigned long long callAddr) 
{
	int result = 0;
	
	for (int i = 0; i < params.currentFunc->numOfInstructions; i++) 
	{
		if (params.currentFunc->instructions[i].opcode == CALL_NEAR || params.currentFunc->instructions[i].opcode == CALL_FAR) 
		{
			if (params.currentFunc->addresses[i] + params.currentFunc->instructions[i].operands[0].immediate == callAddr) 
			{
				result++;
			}
		}
	}

	return result;
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

static void wrapStrInParentheses(char* str) 
{
	int len = (int)strlen(str);

	for (int i = len; i > 0; i--)
	{
		str[i] = str[i - 1];
	}

	str[0] = '(';
	strcat(str, ")");
}