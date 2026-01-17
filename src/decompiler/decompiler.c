#include "decompiler.h"
#include "expressions.h"
#include "assignment.h"
#include "functionCalls.h"
#include "conditions.h"
#include "dataTypes.h"
#include "../disassembler/opcodes.h"
#include "../disassembler/registers.h"

const char* indent = "    ";

unsigned char decompileFunction(struct DecompilationParameters params, struct JdcStr* result)
{
	if (!generateFunctionHeader(params.currentFunc, result))
	{
		return 0;
	}

	strcatJdc(result, "{\n");

	if (params.currentFunc->numOfLocalVars > 0 || params.currentFunc->numOfReturnVars > 0)
	{
		if (!declareAllLocalVariables(params.currentFunc, result))
		{
			return 0;
		}
	}

	struct Condition conditions[20] = { 0 };
	int numOfConditions = getAllConditions(params, conditions);

	unsigned char isConditionEmpty = 0; // used to check if there is an empty condition that should be removed
	unsigned char numOfIndents = 1;
	unsigned char isInUnreachableState = 0; // if looking at instructions after a ret or jmp
	int originalIndex = -1;

	for (int i = 0; i < params.currentFunc->numOfInstructions; i++)
	{
		params.startInstructionIndex = i;

		struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[i]);

		// checking for end of condition
		for (int j = 0; j < numOfConditions; j++) 
		{
			if (!conditions[j].requiresJumpInDecomp && !conditions[j].isCombinedByOther && i == conditions[j].dstIndex)
			{
				numOfIndents--;

				//if (isConditionEmpty)
				//{
				//	// remove the condition and the {
				//	numOfLinesDecompiled--;
				//	resultBuffer[numOfLinesDecompiled].line[0] = 0;
				//	numOfLinesDecompiled--;
				//	resultBuffer[numOfLinesDecompiled].line[0] = 0;
				//}
				//else
				//{
				//	strcpy(resultBuffer[numOfLinesDecompiled].line, "}");
				//	resultBuffer[numOfLinesDecompiled].indents = numOfIndents;
				//	numOfLinesDecompiled++;
				//}

				addIndents(result, numOfIndents);
				strcatJdc(result, "}\n");

				isInUnreachableState = 0;
				isConditionEmpty = 0;
			}
		}

		int conditionIndex = checkForCondition(i, conditions, numOfConditions);
		if (conditionIndex != -1)
		{
			addIndents(result, numOfIndents);
			
			if (decompileCondition(params, conditions, conditionIndex, result))
			{
				strcatJdc(result, "\n");
				addIndents(result, numOfIndents);
				strcatJdc(result, "{\n");
				
				isConditionEmpty = 1;
				isInUnreachableState = 0;
			}
			else
			{
				return 0;
			}

			numOfIndents++;

			if (conditions[conditionIndex].requiresJumpInDecomp)
			{
				originalIndex = i;
				i = conditions[conditionIndex].dstIndex - 1;
				params.skipUpperBound = originalIndex;
				params.skipLowerBound = i;
				continue;
			}
		}

		if (isInUnreachableState) { continue; }

		struct Function* callee;
		int importIndex = checkForImportCall(params);
		if (importIndex != -1)
		{
			addIndents(result, numOfIndents);
			if (decompileImportCall(params, importIndex, result))
			{
				strcatJdc(result, "\n");
				isConditionEmpty = 0;
			}
			else
			{
				return 0;
			}
		}
		else if (checkForFunctionCall(params, &callee))
		{
			addIndents(result, numOfIndents);
			if (decompileFunctionCall(params, callee, result))
			{
				strcatJdc(result, "\n");
				isConditionEmpty = 0;
			}
			else
			{
				return 0;
			}
		}
		else if (checkForReturnStatement(params)) 
		{
			addIndents(result, numOfIndents);
			if (decompileReturnStatement(params, result))
			{
				strcatJdc(result, "\n");
				isConditionEmpty = 0;
			}
			else 
			{
				return 0;
			}
		}
		else if (checkForAssignment(currentInstruction))
		{
			addIndents(result, numOfIndents);
			if (decompileAssignment(params, result))
			{
				strcatJdc(result, ";\n");
				isConditionEmpty = 0;
			}
			else
			{
				return 0;
			}
		}

		if (currentInstruction->opcode == RET_NEAR || currentInstruction->opcode == RET_FAR || currentInstruction->opcode == JMP_SHORT)
		{
			if (originalIndex != -1) 
			{
				i = originalIndex;
				originalIndex = -1;
				params.skipUpperBound = -1;
				params.skipLowerBound = -1;

				numOfIndents--;
				addIndents(result, numOfIndents);
				strcatJdc(result, "}\n");

				isInUnreachableState = 0;
				isConditionEmpty = 0;
			}
			else 
			{
				isInUnreachableState = 1;
			}
		}
	}

	return strcatJdc(result, "}\n");
}

static void addIndents(struct JdcStr* result, int numOfIndents)
{
	for (int i = 0; i < numOfIndents; i++)
	{
		strcatJdc(result, indent);
	}
}

static unsigned char generateFunctionHeader(struct Function* function, struct JdcStr* result)
{
	if (!sprintfJdc(result, 0, "%s %s %s(", primitiveTypeStrs[function->returnType], callingConventionStrs[function->callingConvention], function->name.buffer)) 
	{
		return 0;
	}

	for (int i = 0; i < function->numOfRegArgs; i++) 
	{
		if (i == function->numOfRegArgs - 1 && function->numOfStackArgs == 0) 
		{
			if (!sprintfJdc(result, 1, "%s %s", primitiveTypeStrs[function->regArgs[i].type], function->regArgs[i].name.buffer))
			{
				return 0;
			}
		}
		else 
		{
			if (!sprintfJdc(result, 1, "%s %s, ", primitiveTypeStrs[function->regArgs[i].type], function->regArgs[i].name.buffer))
			{
				return 0;
			}
		}
	}

	for (int i = 0; i < function->numOfStackArgs; i++)
	{
		if (i == function->numOfStackArgs - 1)
		{
			if (!sprintfJdc(result, 1, "%s %s", primitiveTypeStrs[function->stackArgs[i].type], function->stackArgs[i].name.buffer))
			{
				return 0;
			}
		}
		else
		{
			if (!sprintfJdc(result, 1, "%s %s, ", primitiveTypeStrs[function->stackArgs[i].type], function->stackArgs[i].name.buffer))
			{
				return 0;
			}
		}
	}

	return strcatJdc(result, ")\n");
}

static unsigned char declareAllLocalVariables(struct Function* function, struct JdcStr* result)
{
	for (int i = 0; i < function->numOfLocalVars; i++)
	{
		if (!sprintfJdc(result, 1, "%s%s %s;\n", indent, primitiveTypeStrs[function->localVars[i].type], function->localVars[i].name.buffer))
		{
			return 0;
		}
	}

	for (int i = 0; i < function->numOfReturnVars; i++)
	{
		if (!sprintfJdc(result, 1, "%s%s %s;\n", indent, primitiveTypeStrs[function->returnVars[i].type], function->returnVars[i].name.buffer))
		{
			return 0;
		}
	}

	return strcatJdc(result, "\n");
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
		if (doesInstructionModifyRegister(instruction, AX, 0, 0))
		{
			return 1;
		}
		else if (isOpcodeCall(instruction->opcode))
		{
			unsigned long long calleeAddress = address + instruction->operands[0].immediate;
			int calleIndex = findFunctionByAddress(params.functions, 0, params.numOfFunctions - 1, calleeAddress);

			if (calleIndex == -1)
			{
				return checkForImportCall(params) != -1;
			}
			else if(params.functions[calleIndex].returnType == VOID_TYPE)
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

static unsigned char decompileReturnStatement(struct DecompilationParameters params, struct JdcStr* result)
{
	if (params.currentFunc->returnType == VOID_TYPE) 
	{
		return strcatJdc(result, "return;");
	}
	
	struct JdcStr returnExpression = initializeJdcStr();
	int newStartInstruction = -1;

	// find where a return register is first being modified
	for (int i = params.startInstructionIndex; i >= 0; i--)
	{
		if (i == params.skipLowerBound)
		{
			i = params.skipUpperBound;
			continue;
		}
		
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
		if (!decompileOperand(params, &(params.currentFunc->instructions[newStartInstruction].operands[0]), params.currentFunc->returnType, &returnExpression))
		{
			freeJdcStr(&returnExpression);
			return 0;
		}
	}
	else 
	{
		struct Operand eax = { 0 };
		eax.type = REGISTER;
		eax.reg = AX;

		params.startInstructionIndex = newStartInstruction;
		if (!decompileOperand(params, &eax, params.currentFunc->returnType, &returnExpression))
		{
			freeJdcStr(&returnExpression);
			return 0;
		}
	}
	

	sprintfJdc(result, 1, "return %s;", returnExpression.buffer);
	freeJdcStr(&returnExpression);

	return 1;
}
