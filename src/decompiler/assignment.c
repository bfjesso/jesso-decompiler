#include "assignment.h"
#include "decompilationUtils.h"
#include "functions.h"

unsigned char checkForAnyAssignments(struct DecompilationParameters* params, int instructionIndex)
{
	struct DisassembledInstruction* instruction = &(params->instructions[instructionIndex]);

	for (int i = 0; i < instruction->numOfOperands; i++) 
	{
		if (doesInstructionAssignToOperand(params, instructionIndex, i)) 
		{
			return 1;
		}
	}

	for (int i = 0; i < params->currentFunc->numOfRegVars; i++) 
	{
		if (doesInstructionAssignToRegVar(params, instructionIndex, params->currentFunc->regVars[i].reg)) 
		{
			return 1;
		}
	}

	return 0;
}

unsigned char doesInstructionAssignToOperand(struct DecompilationParameters* params, int instructionIndex, unsigned char operandNum)
{
	struct DisassembledInstruction* instruction = &(params->instructions[instructionIndex]);
	struct Operand* operand = &instruction->operands[operandNum];
	if (doesInstructionModifyOperand(instruction, operandNum, 0))
	{
		if (operand->type == MEM_ADDRESS) 
		{
			return 1;
		}
		else if (operand->type == REGISTER)
		{
			struct RegisterVariable* regVar = getLocalRegVarByReg(params->currentFunc, operand->reg);
			return regVar && !regVar->isArgument && checkRegVarScope(params, regVar, instructionIndex);
		}
	}

	return 0;
}

struct RegisterVariable* doesInstructionAssignToRegVar(struct DecompilationParameters* params, int instructionIndex, enum Register reg)
{
	struct RegisterVariable* regVar = getLocalRegVarByReg(params->currentFunc, reg);
	if (regVar && !regVar->isArgument && doesInstructionModifyRegister(params, instructionIndex, reg, 0, 0) && checkRegVarScope(params, regVar, instructionIndex))
	{
		return regVar;
	}

	return 0;
}
