#include "assignment.h"
#include "decompilationUtils.h"
#include "operations.h"

unsigned char checkForAssignment(struct DecompilationParameters* params, int instructionIndex)
{
	struct DisassembledInstruction* currentInstruction = &(params->instructions[instructionIndex]);

	if (doesInstructionDoNothing(currentInstruction)) 
	{
		return 0;
	}

	if (currentInstruction->numOfOperands > 0 && currentInstruction->operands[0].type == MEM_ADDRESS && doesInstructionModifyOperand(currentInstruction, 0, 0))
	{
		return 1;
	}

	for (int i = 0; i < params->currentFunc->numOfRegVars; i++) 
	{
		if (doesInstructionModifyRegister(params, instructionIndex, params->currentFunc->regVars[i].reg, 0, 0))
		{
			return 1;
		}
	}

	return 0;
}

unsigned char decompileAssignments(struct DecompilationParameters* params, int instructionIndex, struct JdcStr* result)
{
	struct DisassembledInstruction* currentInstruction = &(params->instructions[instructionIndex]);

	for (int i = 0; i < currentInstruction->numOfOperands; i++)
	{
		if (currentInstruction->operands[i].type == MEM_ADDRESS && doesInstructionModifyOperand(currentInstruction, i, 0))
		{
			struct JdcStr operation = initializeJdcStr();
			if (!decompileOperation(params, instructionIndex, NO_REG, 1, &operation))
			{
				freeJdcStr(&operation);
				return 0;
			}

			addIndents(result, params->numOfIndents);
			strcatJdc(result, operation.buffer);
			freeJdcStr(&operation);
			strcatJdc(result, ";\n");
			params->currentFunc->numOfLines++;
		}
	}

	for (int i = 0; i < params->currentFunc->numOfRegVars; i++)
	{
		if (doesInstructionModifyRegister(params, instructionIndex, params->currentFunc->regVars[i].reg, 0, 0))
		{
			struct JdcStr operation = initializeJdcStr();
			if (!decompileOperation(params, instructionIndex, params->currentFunc->regVars[i].reg, 1, &operation))
			{
				freeJdcStr(&operation);
				return 0;
			}

			addIndents(result, params->numOfIndents);
			strcatJdc(result, operation.buffer);
			freeJdcStr(&operation);
			strcatJdc(result, ";\n");
			params->currentFunc->numOfLines++;
		}
	}

	return 1;
}
