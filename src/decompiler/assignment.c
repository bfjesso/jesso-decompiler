#include "assignment.h"
#include "functions.h"
#include "expressions.h"

unsigned char checkForAssignment(struct DecompilationParameters params)
{
	struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[params.startInstructionIndex]);

	if (doesInstructionDoNothing(currentInstruction)) 
	{
		return 0;
	}

	if (currentInstruction->operands[0].type == MEM_ADDRESS && doesInstructionModifyOperand(currentInstruction, 0, 0, 0))
	{
		return 1;
	}

	for (int i = 0; i < params.currentFunc->numOfRegVars; i++) 
	{
		if (doesInstructionModifyRegister(currentInstruction, params.currentFunc->regVars[i].reg, 0, 0, 0))
		{
			return 1;
		}
	}

	return 0;
}

unsigned char decompileAssignment(struct DecompilationParameters params, struct JdcStr* result)
{
	struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[params.startInstructionIndex]);

	struct StackVariable* localVar = getStackVarByOffset(params.currentFunc, (int)(currentInstruction->operands[0].memoryAddress.constDisplacement));
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
	if (!decompileOperation(params, type, 1, &operation))
	{
		freeJdcStr(&operation);
		return 0;
	}

	unsigned char succeeded = strcatJdc(result, operation.buffer);
	freeJdcStr(&operation);
	return succeeded;
}
