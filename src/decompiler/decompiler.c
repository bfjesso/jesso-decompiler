#include "decompiler.h"
#include "expressions.h"
#include "assignment.h"
#include "functionCalls.h"
#include "conditions.h"
#include "dataTypes.h"
#include "../disassembler/opcodes.h"
#include "../disassembler/registers.h"

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
		params.startInstructionIndex = i;

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

		// checking for begining of condition
		int conditionIndex = checkForCondition(i, conditions, numOfConditions);
		if (conditionIndex != -1)
		{
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
		if (checkForFunctionCall(params, &callee))
		{
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

		int importIndex = checkForImportCall(params);
		if (importIndex != -1) 
		{
			if (decompileImportCall(params, params.imports[importIndex].name, &resultBuffer[numOfLinesDecompiled]))
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
		if ((*resultBufferIndex) >= resultBufferLen) { return 0; }
		
		sprintf(resultBuffer[*resultBufferIndex].line, "%s %s;", primitiveTypeStrs[function->localVars[i].type], function->localVars[i].name);
		resultBuffer[*resultBufferIndex].indents = 1;
		(*resultBufferIndex)++;
	}

	return 1;
}

static unsigned char doesInstructionModifyReturnRegister(struct DecompilationParameters params)
{
	struct DisassembledInstruction* instruction = &(params.currentFunc->instructions[params.startInstructionIndex]);
	unsigned long long address = params.currentFunc->addresses[params.startInstructionIndex];
	
	if ((params.currentFunc->returnType == FLOAT_TYPE || params.currentFunc->returnType == DOUBLE_TYPE) && instruction->opcode == FLD)
	{
		return 1;
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
				return checkForImportCall(params) != -1;
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

		int jmpDstIndex = findInstructionByAddress(params.currentFunc->addresses, 0, params.currentFunc->numOfInstructions - 1, jmpDst);
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

static unsigned char decompileReturnStatement(struct DecompilationParameters params, struct LineOfC* result)
{
	if (params.currentFunc->returnType == VOID_TYPE) 
	{
		strcpy(result->line, "return;");
		return 1;
	}
	
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