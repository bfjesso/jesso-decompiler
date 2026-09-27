#include "assignment.h"
#include "decompilationUtils.h"
#include "functions.h"

bool checkForAnyAssignments(struct DecompilationParameters* params, int32_t instructionIndex)
{
	struct DisassembledInstruction* instruction = &(params->instructions[instructionIndex]);

	for (int32_t i = 0; i < instruction->numOfOperands; i++) 
	{
		if (doesInstructionAssignToOperand(params, instructionIndex, i)) 
		{
			return true;
		}
	}

	for (int32_t i = 0; i < params->currentFunc->numOfRegVars; i++) 
	{
		if (doesInstructionAssignToRegVar(params, instructionIndex, params->currentFunc->regVars[i].reg)) 
		{
			return true;
		}
	}

	return false;
}

bool doesInstructionAssignToOperand(struct DecompilationParameters* params, int32_t instructionIndex, uint8_t operandNum)
{
	struct Operand* operand = &params->instructions[instructionIndex].operands[operandNum];
	if (doesInstructionModifyOperand(params, instructionIndex, operandNum, 0))
	{
		if (operand->type == MEM_ADDRESS) 
		{
			return true;
		}
		else if (operand->type == REGISTER)
		{
			struct RegisterVariable* regVar = getLocalRegVarByReg(params->currentFunc, operand->reg);
			return regVar && !regVar->isArgument && checkRegVarScope(params, regVar, instructionIndex);
		}
	}

	return false;
}

struct RegisterVariable* doesInstructionAssignToRegVar(struct DecompilationParameters* params, int32_t instructionIndex, enum Register reg)
{
	struct RegisterVariable* regVar = getLocalRegVarByReg(params->currentFunc, reg);
	if (regVar && !regVar->isArgument && doesInstructionModifyRegister(params, instructionIndex, reg, 0, 0) && checkRegVarScope(params, regVar, instructionIndex))
	{
		return regVar;
	}

	return 0;
}
