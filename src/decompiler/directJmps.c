#include "directJmps.h"
#include "decompilationUtils.h"
#include "returnStatements.h"
#include "conditions.h"

unsigned char getAllDirectJmps(struct DecompilationParameters* params)
{
	params->currentFunc->directJmps = (struct DirectJmp*)calloc(20, sizeof(struct DirectJmp));
	
	for (int i = 0; i < params->currentFunc->numOfInstructions; i++) 
	{
		struct DisassembledInstruction* instruction = &(params->currentFunc->instructions[i]);

		if (isOpcodeJmp(instruction->opcode))
		{
			if (checkForReturnStatement(params->currentFunc, i, params->currentFunc->instructions, params->currentFunc->numOfInstructions))
			{
				continue;
			}
			
			unsigned long long jmpDst = params->currentFunc->instructions[i].address + instruction->operands[0].immediate.value;
			int dstIndex = findInstructionByAddress(params->currentFunc->instructions, 0, params->currentFunc->numOfInstructions - 1, jmpDst);

			if (dstIndex == -1 || dstIndex == i + 1 || dstIndex >= params->currentFunc->numOfInstructions)
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
				if (!doesInstructionDoNothing(&params->currentFunc->instructions[j]))
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
					if (dstIndex == getConditionStart(&params->currentFunc->conditions[j]))
					{
						directJmpType = CONTINUE_DJT;
						break;
					}
					else if(dstIndex == getConditionEnd(&params->currentFunc->conditions[j]))
					{
						directJmpType = BREAK_DJT;
						break;
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

unsigned char decompileDirectJmps(struct DecompilationParameters* params, struct JdcStr* result)
{
	for (int i = 0; i < params->currentFunc->numOfDirectJmps; i++)
	{
		if (params->startInstructionIndex == params->currentFunc->directJmps[i].dstIndex && params->currentFunc->directJmps[i].type == GO_TO_DJT)
		{
			addIndents(result, params->numOfIndents - 1);
			sprintfJdc(result, 1, "label_%llX:\n", params->currentFunc->instructions[params->currentFunc->directJmps[i].dstIndex].address - params->imageBase);
			break;
		}
		else if (params->startInstructionIndex == params->currentFunc->directJmps[i].jmpIndex)
		{
			addIndents(result, params->numOfIndents);

			switch (params->currentFunc->directJmps[i].type)
			{
			case GO_TO_DJT:
				sprintfJdc(result, 1, "goto label_%llX;\n", params->currentFunc->instructions[params->currentFunc->directJmps[i].dstIndex].address - params->imageBase);
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

unsigned char checkForDirectJmpStart(struct DecompilationParameters* params)
{
	for (int i = 0; i < params->currentFunc->numOfDirectJmps; i++)
	{
		if (params->startInstructionIndex == params->currentFunc->directJmps[i].jmpIndex && params->currentFunc->directJmps[i].type == GO_TO_DJT)
		{
			return 1;
		}
	}

	return 0;
}

unsigned char checkForDirectJmpEnd(struct DecompilationParameters* params)
{
	for (int i = 0; i < params->currentFunc->numOfDirectJmps; i++)
	{
		if (params->startInstructionIndex == params->currentFunc->directJmps[i].dstIndex && params->currentFunc->directJmps[i].type == GO_TO_DJT)
		{
			return 1;
		}
	}

	return 0;
}