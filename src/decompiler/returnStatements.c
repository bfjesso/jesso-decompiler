#include "returnStatements.h"
#include "decompilationUtils.h"
#include "expressions.h"

unsigned char checkForReturnStatement(struct DecompilationParameters* params, int instructionIndex)
{
	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];

	if (isOpcodeReturn(instruction->opcode) || (params->currentFunc && params->currentFunc->lastInstructionIndex != 0 && instructionIndex == params->currentFunc->lastInstructionIndex))
	{
		return 1;
	}

	// check if jump to a return. this will only count if the jump leads directly to a ret, meaning the jmp is effectivly a ret instruction
	if (isOpcodeJmp(instruction->opcode))
	{
		return checkForJumpToReturnStatement(params, instructionIndex);
	}

	return 0;
}

unsigned char checkForJumpToReturnStatement(struct DecompilationParameters* params, int instructionIndex)
{
	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];

	if (instruction->operands[0].type != IMMEDIATE || (!isOpcodeJmp(instruction->opcode) && !isOpcodeJcc(instruction->opcode)))
	{
		return 0;
	}

	unsigned long long jmpDstAddr = resolveJmpChain(params, instructionIndex);
	int jmpDstIndex = findInstructionByAddress(params->instructions, 0, params->numOfInstructions, jmpDstAddr);

	if (jmpDstIndex == -1)
	{
		return 1;
	}
	else if(params->currentFunc)
	{
		if(jmpDstIndex < params->currentFunc->firstInstructionIndex)
		{
			return 1;
		}
		else if(jmpDstIndex > params->currentFunc->lastInstructionIndex && params->currentFunc->lastInstructionIndex != 0)
		{
			return 1;
		}
	}

	return doesInstructionLeadStraightToReturn(params, jmpDstIndex);
}

unsigned char doesInstructionLeadStraightToReturn(struct DecompilationParameters* params, int startInstructionIndex) // checks if the function leads to a return without doing anything in between
{
	int lastInstruction = params->currentFunc && params->currentFunc->lastInstructionIndex != 0 ? params->currentFunc->lastInstructionIndex : params->numOfInstructions - 1;
	for (int i = startInstructionIndex; i <= lastInstruction; i++)
	{
		if ((params->instructions[i].operands[0].type == MEM_ADDRESS && doesInstructionModifyOperand(&params->instructions[i], 0, 0, 0)) ||
			isOpcodeCall(params->instructions[i].opcode) || isOpcodeJcc(params->instructions[i].opcode) | isOpcodeJmp(params->instructions[i].opcode))
		{
			return 0;
		}
		else if (params->currentFunc)
		{
			if (params->currentFunc->lastInstructionIndex != 0)
			{
				if (doesInstructionModifyRegister(params, i, params->currentFunc->returnReg, 0, 0, 0))
				{
					return 0;
				}
			}
			else
			{
				if (doesInstructionModifyRegister(params, i, AX, 0, 0, 0))
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

	addIndents(result, params->numOfIndents);
	
	if (params->currentFunc->returnType.primitiveType == VOID_TYPE)
	{
		return strcatJdc(result, "return;\n");
	}

	struct JdcStr returnExpression = initializeJdcStr();
	if (!decompileRegister(params, instructionIndex + 1, params->currentFunc->returnReg, 1, &returnExpression, 0)) // the + 1 is incase the current instruction is also an import call
	{
		freeJdcStr(&returnExpression);
		return 0;
	}

	sprintfJdc(result, 1, "return %s;\n", returnExpression.buffer);
	freeJdcStr(&returnExpression);

	return 1;
}
