#include "directJmps.h"
#include "functions.h"
#include "functionCalls.h"
#include "decompilationUtils.h"
#include "returnStatements.h"

unsigned char getAllDirectJmps(struct DecompilationParameters* params)
{
	for (int i = params->currentFunc->firstInstructionIndex; i <= params->currentFunc->lastInstructionIndex; i++) 
	{
		struct DisassembledInstruction* instruction = &(params->instructions[i]);
		if (isOpcodeJmp(instruction->opcode))
		{
			int dstIndex = findInstructionByAddress(params->instructions, params->numOfInstructions, resolveJmpChain(params, i));
			if (dstIndex == -1 || dstIndex == i + 1) 
			{
				continue;
			}
			else if ((dstIndex < params->currentFunc->firstInstructionIndex || dstIndex > params->currentFunc->lastInstructionIndex) && !checkForKnownFunctionCall(params, i, 0) && !checkForUnknownFunctionCall(params, i))
			{
				if (!handleDirectJmpsResize(params))
				{
					return 0;
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

			int start = i;
			int end = dstIndex;
			if (start > end) 
			{
				start = dstIndex;
				end = i;
			}
			unsigned char doesJmpSkipNothing = 1;
			for (int j = start + 1; j < end; j++) 
			{
				if (!doesInstructionDoNothing(&params->instructions[j]))
				{
					doesJmpSkipNothing = 0;
					break;
				}
			}
			if (doesJmpSkipNothing) 
			{
				continue;
			}
			
			enum DirectJmpType directJmpType = GO_TO_DJT;
			for (int j = 0; j < params->currentFunc->numOfConditions; j++)
			{
				struct Condition* cond = &params->currentFunc->conditions[j];

				// checking if the jmp is part of a condtion
				if (i == cond->jccIndex || (i == cond->dstIndex - 1 && cond->conditionType == LOOP_CT))
				{
					directJmpType = NONE_DJT;
					break;
				}
				else if (cond->conditionType == LOOP_CT || cond->conditionType == DO_WHILE_CT)
				{
					int loopStart = cond->firstBodyIndex;
					int loopEnd = cond->lastBodyIndex;
					
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
				if (!handleDirectJmpsResize(params)) 
				{
					return 0;
				}
				
				params->currentFunc->directJmps[params->currentFunc->numOfDirectJmps].dstIndex = dstIndex;
				params->currentFunc->directJmps[params->currentFunc->numOfDirectJmps].jmpIndex = i;
				params->currentFunc->directJmps[params->currentFunc->numOfDirectJmps].type = directJmpType;
				params->currentFunc->numOfDirectJmps++;
			}
		}
	}

	return 1;
}

static unsigned char handleDirectJmpsResize(struct DecompilationParameters* params)
{
	if (params->currentFunc->numOfDirectJmps % 5 == 0)
	{
		struct DirectJmp* newDirectJmps = (struct DirectJmp*)realloc(params->currentFunc->directJmps, (params->currentFunc->numOfDirectJmps + 5) * sizeof(struct DirectJmp));
		if (newDirectJmps)
		{
			params->currentFunc->directJmps = newDirectJmps;
		}
		else
		{
			return 0;
		}
	}

	return 1;
}

unsigned char decompileDirectJmps(struct DecompilationParameters* params, int instructionIndex, unsigned char* isInUnreachableStateRef, struct JdcStr* result)
{
	for (int i = 0; i < params->currentFunc->numOfDirectJmps; i++)
	{
		if (instructionIndex == params->currentFunc->directJmps[i].dstIndex && params->currentFunc->directJmps[i].type == GO_TO_DJT)
		{
			addIndents(result, params->numOfIndents - 1);
			sprintfJdc(result, 1, "label_%llX:\n", params->instructions[params->currentFunc->directJmps[i].dstIndex].address - params->imageBase);
			addAssociatedInstruction(params->currentFunc, instructionIndex);
			params->currentFunc->numOfLines++;
			break;
		}
		else if (instructionIndex == params->currentFunc->directJmps[i].jmpIndex)
		{
			addIndents(result, params->numOfIndents);

			switch (params->currentFunc->directJmps[i].type)
			{
			case GO_TO_DJT:
				sprintfJdc(result, 1, "goto label_%llX;\n", params->instructions[params->currentFunc->directJmps[i].dstIndex].address - params->imageBase);
				if (isInUnreachableStateRef) { *isInUnreachableStateRef = 1; }
				break;
			case CONTINUE_DJT:
				sprintfJdc(result, 1, "continue;\n");
				break;
			case BREAK_DJT:
				sprintfJdc(result, 1, "break;\n");
				break;
			case JUMP_TO_DJT:
				sprintfJdc(result, 1, "jumpTo(0x%llX);\n", params->instructions[params->currentFunc->directJmps[i].dstIndex].address);
				if (isInUnreachableStateRef) { *isInUnreachableStateRef = 1; }
				break;
			}

			addAssociatedInstruction(params->currentFunc, instructionIndex);
			params->currentFunc->numOfLines++;
			break;
		}
	}

	for (int i = 0; i < params->currentFunc->numOfConditions; i++)
	{
		struct Condition* condition = &params->currentFunc->conditions[i];
		if (condition->conditionType == CONDITIONAL_GOTO_CT && instructionIndex == condition->dstIndex)
		{
			addIndents(result, params->numOfIndents - 1);
			sprintfJdc(result, 1, "label_%llX:\n", params->instructions[condition->dstIndex].address - params->imageBase);
			addAssociatedInstruction(params->currentFunc, instructionIndex);
			params->currentFunc->numOfLines++;
			break;
		}
	}

	return 1;
}

int getDirectJmpDst(struct DecompilationParameters* params, int instructionIndex)
{
	for (int i = 0; i < params->currentFunc->numOfDirectJmps; i++)
	{
		if (instructionIndex == params->currentFunc->directJmps[i].dstIndex)
		{
			return i;
		}
	}

	return -1;
}
