#include "directJmps.h"
#include "decompilationUtils.h"
#include "returnStatements.h"

unsigned char getAllDirectJmps(struct DecompilationParameters* params)
{
	for (int i = params->currentFunc->firstInstructionIndex; i <= params->currentFunc->lastInstructionIndex; i++) 
	{
		struct DisassembledInstruction* instruction = &(params->instructions[i]);

		params->startInstructionIndex = i;

		if (isOpcodeJmp(instruction->opcode))
		{
			if (checkForReturnStatement(params))
			{
				continue;
			}
			
			unsigned long long jmpDstAddr = resolveJmpChain(params);
			int dstIndex = findInstructionByAddress(params->instructions, 0, params->numOfInstructions - 1, jmpDstAddr);

			if (dstIndex == -1 || dstIndex == i + 1 || dstIndex < params->currentFunc->firstInstructionIndex || dstIndex > params->currentFunc->lastInstructionIndex)
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
				// checking if the jmp is part of a condtion
				if (i == params->currentFunc->conditions[j].jccIndex - 1 || (i == params->currentFunc->conditions[j].dstIndex - 1 && params->currentFunc->conditions[j].conditionType == LOOP_CT))
				{
					directJmpType = NONE_DJT;
					break;
				}
				else if (params->currentFunc->conditions[j].conditionType == LOOP_CT || params->currentFunc->conditions[j].conditionType == DO_WHILE_CT)
				{
					int loopStart = params->currentFunc->conditions[j].startIndex;
					int loopEnd = params->currentFunc->conditions[j].endIndex;
					
					if (i > loopStart && i < loopEnd) 
					{
						if (dstIndex == loopStart)
						{
							directJmpType = CONTINUE_DJT;
							break;
						}
						else if (dstIndex == loopEnd)
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

unsigned char decompileDirectJmps(struct DecompilationParameters* params, unsigned char* isInUnreachableStateRef, struct JdcStr* result)
{
	for (int i = 0; i < params->currentFunc->numOfDirectJmps; i++)
	{
		if (params->startInstructionIndex == params->currentFunc->directJmps[i].dstIndex && params->currentFunc->directJmps[i].type == GO_TO_DJT)
		{
			addIndents(result, params->numOfIndents - 1);
			sprintfJdc(result, 1, "label_%llX:\n", params->instructions[params->currentFunc->directJmps[i].dstIndex].address - params->imageBase);
			break;
		}
		else if (params->startInstructionIndex == params->currentFunc->directJmps[i].jmpIndex)
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
			}

			break;
		}
	}

	return 1;
}

int checkForDirectJmpDst(struct DecompilationParameters* params)
{
	for (int i = 0; i < params->currentFunc->numOfDirectJmps; i++)
	{
		if (params->startInstructionIndex == params->currentFunc->directJmps[i].dstIndex)
		{
			return i;
		}
	}

	return -1;
}
