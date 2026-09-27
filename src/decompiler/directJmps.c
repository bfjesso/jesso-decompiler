#include "directJmps.h"
#include "functions.h"
#include "functionCalls.h"
#include "decompilationUtils.h"
#include "returnStatements.h"

enum JdcStatus getAllDirectJmps(struct DecompilationParameters* params)
{
	for (int32_t i = params->currentFunc->firstInstructionIndex; i <= params->currentFunc->lastInstructionIndex; i++) 
	{
		struct DisassembledInstruction* instruction = &(params->instructions[i]);
		if (isOpcodeJmp(instruction->opcode))
		{
			int32_t dstIndex = findInstructionByAddress(params->instructions, params->numOfInstructions, resolveJmpChain(params, i));
			if (dstIndex == -1 || dstIndex == i + 1) 
			{
				continue;
			}
			else if ((dstIndex < params->currentFunc->firstInstructionIndex || dstIndex > params->currentFunc->lastInstructionIndex) && !checkForKnownFunctionCall(params, i, 0) && !checkForUnknownFunctionCall(params, i))
			{
				if (ERROR_JDC == handleDirectJmpsResize(params))
				{
					return ERROR_JDC;
				}

				params->currentFunc->directJmps[params->currentFunc->numOfDirectJmps].dstIndex = dstIndex;
				params->currentFunc->directJmps[params->currentFunc->numOfDirectJmps].jmpIndex = i;
				params->currentFunc->directJmps[params->currentFunc->numOfDirectJmps].type = JUMP_TO_DJT;
				params->currentFunc->numOfDirectJmps++;
				continue;
			}
			else if (checkForReturnStatement(params, i))
			{
				continue;
			}

			int32_t start = i;
			int32_t end = dstIndex;
			if (start > end) 
			{
				start = dstIndex;
				end = i;
			}
			bool doesJmpSkipNothing = true;
			for (int32_t j = start + 1; j < end; j++) 
			{
				if (!doesInstructionDoNothing(&params->instructions[j]))
				{
					doesJmpSkipNothing = false;
					break;
				}
			}
			if (doesJmpSkipNothing) 
			{
				continue;
			}
			
			enum DirectJmpType directJmpType = GO_TO_DJT;
			for (int32_t j = 0; j < params->currentFunc->numOfConditions; j++)
			{
				struct Condition* cond = &params->currentFunc->conditions[j];

				// checking if the jmp is part of a condtion
				if (i == cond->jccIndex || (i == cond->dstIndex - 1 && cond->conditionType == WHILE_CT))
				{
					directJmpType = NONE_DJT;
					break;
				}
				else if (cond->conditionType == WHILE_CT || cond->conditionType == DO_WHILE_CT)
				{
					int32_t loopStart = cond->firstBodyIndex;
					int32_t loopEnd = cond->lastBodyIndex;
					
					if (i >= loopStart && i <= loopEnd) 
					{
						if (dstIndex == loopStart)
						{
							directJmpType = CONTINUE_DJT;
							break;
						}
						else if (dstIndex == loopEnd + 1)
						{
							directJmpType = BREAK_DJT;
							break;
						}
					}
				}
			}

			if (directJmpType != NONE_DJT)
			{
				if (ERROR_JDC == handleDirectJmpsResize(params))
				{
					return ERROR_JDC;
				}
				
				params->currentFunc->directJmps[params->currentFunc->numOfDirectJmps].dstIndex = dstIndex;
				params->currentFunc->directJmps[params->currentFunc->numOfDirectJmps].jmpIndex = i;
				params->currentFunc->directJmps[params->currentFunc->numOfDirectJmps].type = directJmpType;
				params->currentFunc->numOfDirectJmps++;
			}
		}
	}

	return SUCCESS_JDC;
}

static enum JdcStatus handleDirectJmpsResize(struct DecompilationParameters* params)
{
	if (params->currentFunc->numOfDirectJmps % 5 == 0)
	{
		struct DirectJmp* newDirectJmps = (struct DirectJmp*)realloc(params->currentFunc->directJmps, (params->currentFunc->numOfDirectJmps + 5) * sizeof(struct DirectJmp));
		if (!newDirectJmps)
		{
			return ERROR_JDC;
		}

		params->currentFunc->directJmps = newDirectJmps;
	}

	return SUCCESS_JDC;
}

enum JdcStatus decompileDirectJmps(struct DecompilationParameters* params, int32_t instructionIndex, bool* isInUnreachableStateRef, struct JdcStr* result)
{
	for (int32_t i = 0; i < params->currentFunc->numOfDirectJmps; i++)
	{
		if (instructionIndex == params->currentFunc->directJmps[i].dstIndex && params->currentFunc->directJmps[i].type == GO_TO_DJT)
		{
			params->numOfIndents--;
			addDecompiledLine(params, result, instructionIndex, "label_%llX:", params->instructions[params->currentFunc->directJmps[i].dstIndex].address - params->imageBase);
			params->numOfIndents++;
			return SUCCESS_JDC;
		}
		else if (instructionIndex == params->currentFunc->directJmps[i].jmpIndex)
		{
			switch (params->currentFunc->directJmps[i].type)
			{
			case GO_TO_DJT:
				addDecompiledLine(params, result, instructionIndex, "goto label_%llX;", params->instructions[params->currentFunc->directJmps[i].dstIndex].address - params->imageBase);
				if (isInUnreachableStateRef) { *isInUnreachableStateRef = true; }
				break;
			case CONTINUE_DJT:
				addDecompiledLine(params, result, instructionIndex, "continue;");
				break;
			case BREAK_DJT:
				addDecompiledLine(params, result, instructionIndex, "break;");
				break;
			case JUMP_TO_DJT:
				addDecompiledLine(params, result, instructionIndex, "jumpTo(0x%llX);", params->instructions[params->currentFunc->directJmps[i].dstIndex].address);
				if (isInUnreachableStateRef) { *isInUnreachableStateRef = true; }
				break;
			}

			return SUCCESS_JDC;
		}
	}

	for (int32_t i = 0; i < params->currentFunc->numOfConditions; i++)
	{
		struct Condition* condition = &params->currentFunc->conditions[i];
		if (condition->conditionType == CONDITIONAL_GOTO_CT && instructionIndex == condition->dstIndex)
		{
			params->numOfIndents--;
			addDecompiledLine(params, result, instructionIndex, "label_%llX:", params->instructions[condition->dstIndex].address - params->imageBase);
			params->numOfIndents++;
			break;
		}
	}

	return SUCCESS_JDC;
}

bool checkForDirectJmpDst(struct DecompilationParameters* params, int32_t instructionIndex)
{
	for (int32_t i = 0; i < params->currentFunc->numOfDirectJmps; i++)
	{
		if (instructionIndex == params->currentFunc->directJmps[i].dstIndex)
		{
			return true;
		}
	}

	for (int32_t i = 0; i < params->currentFunc->numOfConditions; i++)
	{
		struct Condition* condition = &params->currentFunc->conditions[i];
		if (condition->conditionType == CONDITIONAL_GOTO_CT && instructionIndex == condition->dstIndex)
		{
			return true;
		}
	}

	return false;
}