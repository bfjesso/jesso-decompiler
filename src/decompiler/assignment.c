#include "assignment.h"
#include "decompilationUtils.h"
#include "functions.h"
#include "expressions.h"

unsigned char checkForAssignment(struct DecompilationParameters* params)
{
	struct DisassembledInstruction* currentInstruction = &(params->currentFunc->instructions[params->startInstructionIndex]);

	if (doesInstructionDoNothing(currentInstruction)) 
	{
		return 0;
	}

	if (currentInstruction->operands[0].type == MEM_ADDRESS && doesInstructionModifyOperand(currentInstruction, 0, 0, 0))
	{
		return 1;
	}

	for (int i = 0; i < params->currentFunc->numOfRegVars; i++) 
	{
		if (doesInstructionModifyRegister(currentInstruction, params->currentFunc->regVars[i].reg, 0, 0, 0))
		{
			return 1;
		}
	}

	return 0;
}

unsigned char decompileAssignments(struct DecompilationParameters* params, struct JdcStr* result, int numOfIndents)
{
	struct DisassembledInstruction* currentInstruction = &(params->currentFunc->instructions[params->startInstructionIndex]);

	if (currentInstruction->operands[0].type == MEM_ADDRESS && doesInstructionModifyOperand(currentInstruction, 0, 0, 0))
	{
		struct StackVariable* localVar = getStackVarByOffset(params->currentFunc, (int)(currentInstruction->operands[0].memoryAddress.constDisplacement));
		struct VarType type = { 0 };
		if (!localVar)
		{
			type = getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[0]);
		}
		else
		{
			type = localVar->type;
		}
		
		struct JdcStr operation = initializeJdcStr();
		if (!decompileOperation(params, type, NO_REG, 1, &operation))
		{
			freeJdcStr(&operation);
			return 0;
		}

		addIndents(result, numOfIndents);
		strcatJdc(result, operation.buffer);
		freeJdcStr(&operation);

		if (numOfIndents != 0) 
		{
			strcatJdc(result, ";\n");
		}
	}

	for (int i = 0; i < params->currentFunc->numOfRegVars; i++)
	{
		if (doesInstructionModifyRegister(currentInstruction, params->currentFunc->regVars[i].reg, 0, 0, 0))
		{
			struct JdcStr operation = initializeJdcStr();
			if (!decompileOperation(params, params->currentFunc->regVars[i].type, params->currentFunc->regVars[i].reg, 1, &operation))
			{
				freeJdcStr(&operation);
				return 0;
			}

			addIndents(result, numOfIndents);
			strcatJdc(result, operation.buffer);
			freeJdcStr(&operation);

			if (numOfIndents != 0)
			{
				strcatJdc(result, ";\n");
			}
		}
	}

	return 1;
}
