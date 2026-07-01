#include "returnStatements.h"
#include "functions.h"
#include "decompilationUtils.h"
#include "functionCalls.h"
#include "expressions.h"

unsigned char checkForReturnStatement(struct DecompilationParameters* params, int instructionIndex)
{
	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];

	if (isOpcodeReturn(instruction->opcode))
	{
		if (params->currentFunc->lastInstructionIndex == 0) // this is for when this function is called in findNextFunction
		{
			if (instruction->opcode == RET_NEAR && instruction->numOfOperands == 1) // this isn't checked in findNextFunction because this function can return 1 if there is a jmp to a return instruction
			{
				params->currentFunc->callingConvention = __STDCALL;
			}
			else
			{
				params->currentFunc->callingConvention = __CDECL;
			}
		}
		
		return 1;
	}
	else if (params->currentFunc->lastInstructionIndex != 0 && instructionIndex == params->currentFunc->lastInstructionIndex) 
	{
		return 1;
	}

	// check if jump to a return. this will only count if the jump leads directly to a ret, meaning the jmp is effectivly a ret instruction
	if (isOpcodeJmp(instruction->opcode))
	{
		unsigned long long jmpDstAddr = resolveJmpChain(params, instructionIndex);
		int jmpDstIndex = findInstructionByAddress(params->instructions, 0, params->numOfInstructions, jmpDstAddr);

		if (jmpDstIndex == -1)
		{
			params->currentFunc->callingConvention = __UNKNOWNCALL;
			return 1;
		}
		else if (params->currentFunc)
		{
			if (jmpDstIndex < params->currentFunc->firstInstructionIndex)
			{
				params->currentFunc->callingConvention = __UNKNOWNCALL;
				return 1;
			}
			else if (jmpDstIndex > params->currentFunc->lastInstructionIndex && params->currentFunc->lastInstructionIndex != 0)
			{
				params->currentFunc->callingConvention = __UNKNOWNCALL;
				return 1;
			}
		}

		return doesInstructionLeadStraightToReturn(params, jmpDstIndex);
	}

	return 0;
}

unsigned char doesInstructionLeadStraightToReturn(struct DecompilationParameters* params, int startInstructionIndex) // checks if the function leads to a return without doing anything in between
{
	int lastInstruction = params->currentFunc && params->currentFunc->lastInstructionIndex != 0 ? params->currentFunc->lastInstructionIndex : params->numOfInstructions - 1;
	for (int i = startInstructionIndex; i <= lastInstruction; i++)
	{
		if ((params->instructions[i].numOfOperands > 0 && params->instructions[i].operands[0].type == MEM_ADDRESS && doesInstructionModifyOperand(&params->instructions[i], 0, 0)) ||
			isOpcodeCall(params->instructions[i].opcode) || isOpcodeJcc(params->instructions[i].opcode) | isOpcodeJmp(params->instructions[i].opcode))
		{
			return 0;
		}
		else if (params->currentFunc)
		{
			if (params->currentFunc->lastInstructionIndex != 0)
			{
				if (doesInstructionModifyRegister(params, i, params->currentFunc->returnReg, 0, 0))
				{
					return 0;
				}
			}
			else
			{
				if (doesInstructionModifyRegister(params, i, AX, 0, 0))
				{
					return 0;
				}
			}
		}

		if (checkForReturnStatement(params, i))
		{
			return 1;
		}
	}

	return 1;
}

unsigned char decompileReturnStatement(struct DecompilationParameters* params, int instructionIndex, unsigned char* isInUnreachableStateRef, struct JdcStr* result)
{
	for (int i = 0; i < params->currentFunc->numOfConditions; i++) 
	{
		struct Condition* condition = &params->currentFunc->conditions[i];
		if (condition->conditionType == SWITCH_CASE_CT && instructionIndex == condition->dstIndex - 1)
		{
			return 1;
		}
	}

	if (isInUnreachableStateRef) { *isInUnreachableStateRef = 1; }

	if (params->currentFunc->returnType.primitiveType == VOID_TYPE)
	{
		if (instructionIndex != params->currentFunc->lastInstructionIndex || isOpcodeReturn(params->instructions[instructionIndex].opcode))
		{
			addIndents(result, params->numOfIndents);
			addAssociatedInstruction(params->currentFunc, instructionIndex);
			params->currentFunc->numOfLines++;
			return strcatJdc(result, "return;\n");
		}
		
		return 1;
	}

	if (checkForKnownFunctionCall(params, instructionIndex, 0) || checkForUnknownFunctionCall(params, instructionIndex))
	{
		instructionIndex++; // this is because decompileRegister decrements the instruction index
	}

	struct JdcStr returnExpression = initializeJdcStr();
	if (!decompileRegister(params, instructionIndex, params->currentFunc->returnReg, 1, &returnExpression, 0))
	{
		freeJdcStr(&returnExpression);
		return 0;
	}

	addIndents(result, params->numOfIndents);
	addAssociatedInstruction(params->currentFunc, instructionIndex);
	params->currentFunc->numOfLines++;
	sprintfJdc(result, 1, "return %s;\n", returnExpression.buffer);

	freeJdcStr(&returnExpression);
	return 1;
}
