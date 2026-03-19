#include "directJmps.h"
#include "decompilationUtils.h"

int getAllDirectJmps(struct DecompilationParameters params, struct Condition* conditions, int numOfCondtions, struct DirectJmp** directJmpsRef, int directJmpsBufferSize)
{
	struct DirectJmp* directJmps = *directJmpsRef;
	
	int numOfDirectJmps = 0;
	for (int i = 0; i < params.currentFunc->numOfInstructions; i++) 
	{
		struct DisassembledInstruction* instruction = &(params.currentFunc->instructions[i]);

		if (instruction->opcode == JMP_SHORT || instruction->opcode == JMP_NEAR)
		{
			unsigned long long jmpDst = params.currentFunc->instructions[i].address + instruction->operands[0].immediate.value;
			int dstIndex = findInstructionByAddress(params.currentFunc->instructions, 0, params.currentFunc->numOfInstructions - 1, jmpDst);

			if (dstIndex == -1 || dstIndex >= params.currentFunc->numOfInstructions)
			{
				continue;
			}
			
			enum DirectJmpType directJmpType = GO_TO_DJT;
			for (int j = 0; j < numOfCondtions; j++) 
			{
				if (i == conditions[j].dstIndex - 1 || i == conditions[j].dstIndex - 2 || i == conditions[j].exitIndex - 1 || i == conditions[j].jccIndex) // checking if the jmp is part of a condtion
				{
					directJmpType = NONE_DJT;
					break;
				}
				else if (conditions[j].conditionType == LOOP_CT) 
				{
					if (dstIndex == conditions[j].jccIndex + 1) 
					{
						directJmpType = CONTINUE_DJT;
						break;
					}
					else if(dstIndex == conditions[j].dstIndex)
					{
						directJmpType = BREAK_DJT;
						break;
					}
				}
			}

			if (directJmpType != NONE_DJT)
			{
				if (numOfDirectJmps >= directJmpsBufferSize) 
				{
					directJmpsBufferSize += 5;
					struct DirectJmp* newDirectJmps = (struct DirectJmp*)realloc(directJmps, directJmpsBufferSize * sizeof(struct DirectJmp));
					if (newDirectJmps)
					{
						*directJmpsRef = newDirectJmps;
						directJmps = newDirectJmps;
					}
					else
					{
						return -1;
					}
				}
				
				directJmps[numOfDirectJmps].dstIndex = dstIndex;
				directJmps[numOfDirectJmps].jmpIndex = i;
				directJmps[numOfDirectJmps].type = directJmpType;
				numOfDirectJmps++;
			}
		}
	}

	return numOfDirectJmps;
}
