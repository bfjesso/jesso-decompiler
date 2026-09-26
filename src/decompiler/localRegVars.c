#include "localRegVars.h"
#include "functions.h"
#include "conditions.h"
#include "assignment.h"
#include "returnStatements.h"

unsigned char getAllLocalRegVars(struct DecompilationParameters* params)
{
	// checking for individual instructions that conditionally modify a reg
	for (int i = params->currentFunc->firstInstructionIndex; i <= params->currentFunc->lastInstructionIndex; i++)
	{
		for (int modifiedReg = RAX; modifiedReg < ST0; modifiedReg++)
		{
			struct RegisterVariable* modifiedRegVar = getLocalRegVarByReg(params->currentFunc, modifiedReg);
			if (modifiedReg == RBP || modifiedReg == RSP || modifiedReg == RIP ||
				!doesInstructionConditionallyModifyRegister(params, i, modifiedReg))
			{
				continue;
			}

			if (!modifiedRegVar)
			{
				if (!addRegVar(params, 0, 0, modifiedReg))
				{
					return 0;
				}

				modifiedRegVar = &params->currentFunc->regVars[params->currentFunc->numOfRegVars - 1];
			}

			getLocalRegVarScope(params, i - 1, i + 1, modifiedRegVar);
			break;
		}
	}

	// checking for registers that are modified in a condition
	unsigned char modifiedRegs[NUM_OF_REGISTERS] = { 0 };
	for (int i = 0; i < params->currentFunc->numOfConditions; i++)
	{
		memset(modifiedRegs, 0, NUM_OF_REGISTERS);

		struct Condition* condition = &params->currentFunc->conditions[i];
		if (condition->conditionType != CONDITIONAL_RETURN_CT)
		{
			for (int j = condition->firstBodyIndex; j <= condition->lastBodyIndex; j++)
			{
				struct Condition* cond = getConditionFromFirstBodyInstruction(params, j);
				if (cond && cond != condition && cond->lastBodyIndex > j && cond->lastBodyIndex <= condition->lastBodyIndex)
				{
					j = getConditionChainLastBodyInstruction(params, cond);
					continue;
				}

				if (checkForReturnStatement(params, j) || doesInstructionGenerateInterruptOrException(&params->instructions[j]))
				{
					memset(modifiedRegs, 0, NUM_OF_REGISTERS); // no regs are accessed after the condition because execution doesnt continue
					break;
				}

				for (int k = RAX; k < ST0; k++)
				{
					if (k == RBP || k == RSP || k == RIP)
					{
						continue;
					}

					if (doesInstructionModifyRegister(params, j, k, 0, 0))
					{
						modifiedRegs[k] = 1;
					}
				}
			}

			// checking if the modified regs are accessed before being overwritten after the condition
			for (int j = RAX; j < ST0; j++)
			{
				if (!modifiedRegs[j])
				{
					continue;
				}

				struct RegisterVariable* existingRegVar = getLocalRegVarByReg(params->currentFunc, j);

				if ((condition->conditionType == WHILE_CT || condition->conditionType == DO_WHILE_CT))
				{
					// if condition is a loop, it needs to check from the start of it since the code can run more than once
					if (isRegisterAccessedBeforeInit(params, condition->firstBodyIndex, condition->lastBodyIndex, j, 1, 0))
					{
						if (existingRegVar)
						{
							getLocalRegVarScope(params, condition->firstBodyIndex - 1, condition->lastBodyIndex + 1, existingRegVar);
						}
						else
						{
							if (!addRegVar(params, 0, 0, j))
							{
								return 0;
							}

							struct RegisterVariable* newRegVar = &params->currentFunc->regVars[params->currentFunc->numOfRegVars - 1];
							getLocalRegVarScope(params, condition->firstBodyIndex - 1, condition->lastBodyIndex + 1, newRegVar);
						}


						continue;
					}
				}

				int firstChainBodyIndex = getConditionChainFirstBodyInstruction(params, condition);
				int lastChainBodyIndex = getConditionChainLastBodyInstruction(params, condition);
				if (isRegisterAccessedBeforeInit(params, lastChainBodyIndex + 1, params->currentFunc->lastInstructionIndex, j, 0, 0))
				{
					if (existingRegVar)
					{
						getLocalRegVarScope(params, firstChainBodyIndex - 1, lastChainBodyIndex + 1, existingRegVar);
					}
					else
					{
						if (!addRegVar(params, 0, 0, j))
						{
							return 0;
						}

						struct RegisterVariable* newRegVar = &params->currentFunc->regVars[params->currentFunc->numOfRegVars - 1];
						getLocalRegVarScope(params, firstChainBodyIndex - 1, lastChainBodyIndex + 1, newRegVar);
					}
				}
			}
		}
	}

	unsigned char addedNewRegVar = 0;
	do
	{
		addedNewRegVar = 0;
		for (int i = params->currentFunc->firstInstructionIndex; i <= params->currentFunc->lastInstructionIndex; i++)
		{
			// this is checking for instructions that modify multiple things, and the order that the asignments are decompiled in maters
			if (checkForAnyAssignments(params, i))
			{
				for (int statusFlag = CF; statusFlag <= OF; statusFlag++)
				{
					enum Mnemonic opcode = params->instructions[i].opcode;
					if (opcode == ADD || opcode == SUB || opcode == CMPXCHG ||
						(opcode == NEG && statusFlag == OF))
					{
						if (isRegisterAccessedBeforeInit(params, i + 1, params->currentFunc->lastInstructionIndex, statusFlag, 0, 0))
						{
							struct RegisterVariable* statusFlagVar = getLocalRegVarByReg(params->currentFunc, statusFlag);
							if (!statusFlagVar)
							{
								if (!addRegVar(params, 0, 0, statusFlag))
								{
									return 0;
								}

								statusFlagVar = &params->currentFunc->regVars[params->currentFunc->numOfRegVars - 1];
								addedNewRegVar = 1;
							}

							getLocalRegVarScope(params, i, i + 1, statusFlagVar);
							break;
						}
					}
				}
			}

			// if a reg is modified using a regVar, and then that regVar is modified before the reg is overwritten again, the reg needs to also be a regVar
			for (int modifiedReg = RAX; modifiedReg < ST0; modifiedReg++)
			{
				struct RegisterVariable* modifiedRegVar = getLocalRegVarByReg(params->currentFunc, modifiedReg);
				if (modifiedReg == RBP || modifiedReg == RSP || modifiedReg == RIP ||
					!doesInstructionModifyRegister(params, i, modifiedReg, 0, 0))
				{
					continue;
				}

				for (int accessedReg = RAX; accessedReg < ST0; accessedReg++)
				{
					struct RegisterVariable* accessedRegVar = getLocalRegVarByReg(params->currentFunc, accessedReg);
					if (accessedReg == RBP || accessedReg == RSP || accessedReg == RIP ||
						!doesInstructionAccessRegister(params, i, accessedReg, 0, 0) || !accessedRegVar || !checkRegVarScope(params, accessedRegVar, i))
					{
						continue;
					}

					unsigned char doesAccessedRegVarChange = 0;
					for (int j = i + 1; j <= params->currentFunc->lastInstructionIndex; j++)
					{
						if (checkForReturnStatement(params, j) || doesInstructionGenerateInterruptOrException(&params->instructions[j]))
						{
							break;
						}

						unsigned char overwrites = 0;
						if (!doesAccessedRegVarChange)
						{
							if (doesInstructionModifyRegister(params, j, modifiedReg, 0, &overwrites) && overwrites)
							{
								break;
							}

							if (doesInstructionModifyRegister(params, j, accessedReg, 0, 0))
							{
								doesAccessedRegVarChange = 1;
							}
						}
						else if (doesInstructionModifyRegister(params, j, modifiedReg, 0, &overwrites) && overwrites)
						{
							if (!modifiedRegVar)
							{
								if (!addRegVar(params, 0, 0, modifiedReg))
								{
									return 0;
								}

								modifiedRegVar = &params->currentFunc->regVars[params->currentFunc->numOfRegVars - 1];
								addedNewRegVar = 1;
							}

							addRegVarScope(modifiedRegVar, i, j);
							break;
						}
					}
				}
			}
		}
	} while (addedNewRegVar);

	return 1;
}

void getLocalRegVarScope(struct DecompilationParameters* params, int upperStart, int lowerStart, struct RegisterVariable* regVar)
{
	int startIndex = params->currentFunc->firstInstructionIndex - 1;
	int endIndex = params->currentFunc->lastInstructionIndex + 1;

	for (int i = upperStart; i >= params->currentFunc->firstInstructionIndex; i--)
	{
		struct Condition* condition = getConditionFromLastBodyInstruction(params, i);
		if (condition)
		{
			i = getConditionChainFirstBodyInstruction(params, condition);
			continue;
		}

		unsigned char overwrites = 0;
		if (doesInstructionModifyRegister(params, i, regVar->reg, 0, &overwrites) && overwrites)
		{
			startIndex = i;
			break;
		}
	}

	for (int i = lowerStart; i <= params->currentFunc->lastInstructionIndex; i++)
	{
		struct Condition* condition = getConditionFromFirstBodyInstruction(params, i);
		if (condition)
		{
			i = getConditionChainLastBodyInstruction(params, condition);
			continue;
		}

		if (doesInstructionAccessRegister(params, i, regVar->reg, 1, 0))
		{
			endIndex = i;
		}

		unsigned char overwrites = 0;
		if (doesInstructionModifyRegister(params, i, regVar->reg, 0, &overwrites) && overwrites)
		{
			break;
		}
	}

	addRegVarScope(regVar, startIndex, endIndex);
}

unsigned char isRegisterAccessedBeforeInit(struct DecompilationParameters* params, int startInstructionIndex, int lastInstructionIndex, enum Register reg, unsigned char ignoreInitialization, int callNum)
{
	// preventing recursive loop. this assumes it is accessed
	if (callNum > 9)
	{
		return 1;
	}

	// this happens if the last instruction of the function also initializes the return reg. the start instruction index is incremented before isRegisterAccessedBeforeInit is called, so the loop here wont run
	if (startInstructionIndex > params->currentFunc->lastInstructionIndex && compareRegisters(params->currentFunc->returnReg, reg))
	{
		return 1;
	}

	for (int i = startInstructionIndex; i <= lastInstructionIndex; i++)
	{
		if (doesInstructionAccessRegister(params, i, reg, 1, 0))
		{
			return 1;
		}

		if (!ignoreInitialization)
		{
			unsigned char overwrites = 0;
			if (doesInstructionModifyRegister(params, i, reg, 0, &overwrites) && overwrites)
			{
				return 0;
			}
		}

		if (checkForReturnStatement(params, i))
		{
			if (compareRegisters(params->currentFunc->returnReg, reg))
			{
				return 1;
			}
			return 0;
		}

		struct DisassembledInstruction* instruction = &(params->instructions[i]);
		if (isOpcodeJmp(instruction->opcode) || isOpcodeJcc(instruction->opcode))
		{
			int dstIndex = findInstructionByAddress(params->instructions, params->numOfInstructions, resolveJmpChain(params, i));
			if (dstIndex > i)
			{
				if (isOpcodeJcc(instruction->opcode))
				{
					if (isRegisterAccessedBeforeInit(params, i + 1, dstIndex - 1, reg, ignoreInitialization, callNum + 1))
					{
						return 1;
					}
				}

				i = dstIndex - 1;
			}
		}
	}

	return 0;
}