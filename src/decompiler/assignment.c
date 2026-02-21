#include "assignment.h"
#include "functions.h"
#include "expressions.h"

unsigned char checkForAssignment(struct DecompilationParameters params)
{
	struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[params.startInstructionIndex]);

	if (currentInstruction->operands[0].type == MEM_ADDRESS && doesInstructionModifyOperand(currentInstruction, 0, 0))
	{
		return 1;
	}

	for (int i = 0; i < params.currentFunc->numOfRegVars; i++) 
	{
		if (doesInstructionModifyRegister(currentInstruction, params.currentFunc->regVars[i].reg, 0, 0))
		{
			return 1;
		}
	}

	return 0;
}

unsigned char decompileAssignment(struct DecompilationParameters params, struct JdcStr* result)
{
	struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[params.startInstructionIndex]);

	struct StackVariable* localVar = getLocalVarByOffset(params.currentFunc, (int)(currentInstruction->operands[0].memoryAddress.constDisplacement));
	enum PrimitiveType type;
	if (!localVar)
	{
		type = getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[0]);
	}
	else
	{
		type = localVar->type;
	}

	struct JdcStr assignee = initializeJdcStr();
	if (!decompileOperand(params, &currentInstruction->operands[0], type, &assignee))
	{
		freeJdcStr(&assignee);
		return 0;
	}

	struct JdcStr operation = initializeJdcStr();
	if (!decompileOperation(params, type, 1, &operation))
	{
		freeJdcStr(&assignee);
		freeJdcStr(&operation);
		return 0;
	}

	unsigned char succeeded = sprintfJdc(result, 1, "%s%s", assignee.buffer, operation.buffer);
	freeJdcStr(&assignee);
	freeJdcStr(&operation);
	return succeeded;
}
