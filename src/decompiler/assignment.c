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

unsigned char decompileAssignment(struct DecompilationParameters params, struct LineOfC* result)
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

	char assignee[255] = { 0 };
	if (!decompileOperand(params, &currentInstruction->operands[0], type, assignee, 255))
	{
		return 0;
	}

	char operation[255] = { 0 };
	if (!decompileOperation(params, type, 1, operation))
	{
		return 0;
	}

	sprintf(result->line, "%s%s;", assignee, operation);
	return 1;
}