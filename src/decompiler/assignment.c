#include "expressions.h"
#include "assignment.h"

unsigned char checkForAssignment(struct DisassembledInstruction* instruction)
{
	if (instruction->operands[0].type == MEM_ADDRESS && !compareRegisters(instruction->operands[0].memoryAddress.reg, SP) && doesInstructionModifyOperand(instruction, 0, 0))
	{
		return 1;
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
		type = getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[0], params.is64Bit);
	}
	else
	{
		type = localVar->type;
	}

	struct JdcStr assignee = { 0 };
	initializeJdcStr(&assignee, 255);
	if (!decompileOperand(params, &currentInstruction->operands[0], type, &assignee))
	{
		freeJdcStr(&assignee);
		return 0;
	}

	struct JdcStr operation = { 0 };
	initializeJdcStr(&operation, 255);
	if (!decompileOperation(params, type, 1, &operation))
	{
		freeJdcStr(&assignee);
		freeJdcStr(&operation);
		return 0;
	}

	freeJdcStr(&assignee);
	freeJdcStr(&operation);
	return sprintfJdc(result, 1, "%s%s;", assignee.buffer, operation.buffer);
}