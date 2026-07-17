#include "conditions.h"
#include "functions.h"
#include "decompilationUtils.h"
#include "returnStatements.h"
#include "expressions.h"
#include "assignment.h"
#include "../disassembler/operands.h"

unsigned char getAllConditions(struct DecompilationParameters* params)
{
	int combinationCount = 0;
	unsigned char stopCombination = 0;
	for (int i = params->currentFunc->firstInstructionIndex; i <= params->currentFunc->lastInstructionIndex; i++)
	{
		struct DisassembledInstruction* instruction = &(params->instructions[i]);
		if (isOpcodeJcc(instruction->opcode))
		{
			unsigned long long dstAddress = instruction->address + instruction->numOfBytes + instruction->operands[0].immediate.value;
			int dstIndex = findInstructionByAddress(params->instructions, params->numOfInstructions, dstAddress);
			if (dstIndex == -1)
			{
				continue;
			}

			// the jmp chain result should only be used for conditional gotos and conditional returns
			int dstJmpChainIndex = findInstructionByAddress(params->instructions, params->numOfInstructions, resolveJmpChain(params, i));
			if (dstJmpChainIndex == -1)
			{
				dstJmpChainIndex = dstIndex;
			}

			// if the conditions ends with a jmp, this will get the index of the instruction jumped to by that jmp
			int exitIndex = -1;
			if (dstIndex > 0 && isOpcodeJmp(params->instructions[dstIndex - 1].opcode))
			{
				exitIndex = findInstructionByAddress(params->instructions, params->numOfInstructions, resolveJmpChain(params, dstIndex - 1));
			}

			struct Condition* lastCondition = 0;
			if (params->currentFunc->numOfConditions > 0) 
			{
				lastCondition = &params->currentFunc->conditions[params->currentFunc->numOfConditions - 1];
			}

			// a series of Jcc instructions that have the same destination are combined with a logical AND
			// if the series ends with a Jcc that does not have the same destination, then if the instruction immediatly before the destination of the previous Jcc is the this Jcc, they are all combined with a logical OR

			if (lastCondition && dstIndex == lastCondition->dstIndex && !stopCombination)
			{
				if (!handleCombinedJccResize(lastCondition))
				{
					return 0;
				}
				
				lastCondition->combinedJccIndexes[combinationCount] = i;
				lastCondition->combinedJccsLogicType = AND_LT;
				combinationCount++;

				lastCondition->numOfCombinedJccs = combinationCount;
			}
			else if (lastCondition && lastCondition->dstIndex - 1 == i && !stopCombination)
			{
				if (!handleCombinedJccResize(lastCondition))
				{
					return 0;
				}
				
				lastCondition->combinedJccIndexes[combinationCount] = i;
				lastCondition->combinedJccsLogicType = OR_LT;
				lastCondition->dstIndex = dstIndex;
				lastCondition->exitIndex = exitIndex;
				combinationCount++;

				lastCondition->numOfCombinedJccs = combinationCount;
			}
			else
			{
				if (!handleConditionsResize(params))
				{
					return 0;
				}

				struct Condition* currentCondition = &params->currentFunc->conditions[params->currentFunc->numOfConditions];
				currentCondition->jccIndex = i;
				currentCondition->dstIndex = dstIndex;
				currentCondition->exitIndex = exitIndex;
				currentCondition->connectedConditionIndex = -1;

				// setting the type
				if (dstIndex < i)
				{
					currentCondition->conditionType = DO_WHILE_CT;
					currentCondition->firstBodyIndex = dstIndex;
					currentCondition->lastBodyIndex = i - 1;
				}
				else if (doesInstructionLeadStraightToReturn(params, dstJmpChainIndex))
				{
					currentCondition->conditionType = CONDITIONAL_RETURN_CT;
					currentCondition->dstIndex = dstJmpChainIndex;
					currentCondition->firstBodyIndex = -1;
					currentCondition->lastBodyIndex = -1;
				}
				else if (exitIndex != -1 && exitIndex == i - 1) // checks if the exitIndex is to the instruction before the Jcc, which is assumed to be the comparisson instruction
				{
					currentCondition->conditionType = LOOP_CT;
					currentCondition->firstBodyIndex = i + 1;
					currentCondition->lastBodyIndex = dstIndex - 1;
				}
				else
				{
					currentCondition->conditionType = IF_CT;
					currentCondition->firstBodyIndex = i + 1;
					currentCondition->lastBodyIndex = dstIndex - 1;
				}

				combinationCount = 0;
				stopCombination = 0;
				params->currentFunc->numOfConditions++;
			}
		}
		else if(params->currentFunc->numOfConditions > 0 && !isOpcodeCmp(instruction->opcode) && instruction->opcode != TEST) // the Jccs cant be combined into one condition if there is other code that runs between them.
		{
			stopCombination = 1;
		}
	}
	
	// handling else ifs and elses
	int ogNumOfConditions = params->currentFunc->numOfConditions;
	for (int i = 0; i < ogNumOfConditions; i++) 
	{
		struct Condition* cond1 = &params->currentFunc->conditions[i];
		if (cond1->connectedConditionIndex != -1) 
		{
			continue;
		}

		if ((cond1->conditionType == IF_CT || cond1->conditionType == ELSE_IF_CT) && cond1->exitIndex > cond1->dstIndex)
		{
			unsigned char foundElseIf = 0;
			for (int j = i + 1; j < ogNumOfConditions; j++)
			{
				struct Condition* cond2 = &params->currentFunc->conditions[j];
				if (cond2->conditionType == IF_CT &&
					cond1->exitIndex == cond2->exitIndex &&
					cond1->dstIndex + 1 == cond2->jccIndex)
				{
					cond1->connectedConditionIndex = j;
					cond2->conditionType = ELSE_IF_CT;
					foundElseIf = 1;
					break;
				}
			}

			// adding else condition
			if (!foundElseIf)
			{
				if (!doesInstructionLeadStraightToReturn(params, cond1->exitIndex))
				{
					if (!handleConditionsResize(params))
					{
						return 0;
					}

					cond1 = &params->currentFunc->conditions[i]; // needs to be updated due to reallocation

					struct Condition* elseCond = &params->currentFunc->conditions[params->currentFunc->numOfConditions];
					cond1->connectedConditionIndex = params->currentFunc->numOfConditions;
					params->currentFunc->numOfConditions++;

					elseCond->conditionType = ELSE_CT;
					elseCond->jccIndex = cond1->dstIndex - 1;
					elseCond->dstIndex = cond1->exitIndex;
					elseCond->firstBodyIndex = cond1->dstIndex;
					elseCond->lastBodyIndex = cond1->exitIndex - 1;
					elseCond->connectedConditionIndex = -1;
				}
			}
		}
	}

	// checking for overlapping conditions which need to be handled as go to
	struct Condition* conditionToMakeGoTo = 0;
	do
	{
		// the conditions that overlap with the most conditions are set to gotos first because this minimizes the total amount of conditional gotos.
		conditionToMakeGoTo = 0;
		int maxNumOfOverlapping = 0;
		for (int i = 0; i < params->currentFunc->numOfConditions; i++)
		{
			struct Condition* cond = &params->currentFunc->conditions[i];
			if (cond->conditionType == CONDITIONAL_RETURN_CT || cond->conditionType == CONDITIONAL_GOTO_CT || cond->conditionType == ELSE_IF_CT || cond->conditionType == ELSE_CT)
			{
				continue;
			}

			int numOfOverlappingConditions = getNumOfOverlappingConditions(params, cond); // this is how many conditions cond overlaps with
			if (numOfOverlappingConditions > maxNumOfOverlapping)
			{
				maxNumOfOverlapping = numOfOverlappingConditions;
				conditionToMakeGoTo = cond;
			}
		}

		if (conditionToMakeGoTo)
		{
			conditionToMakeGoTo->conditionType = CONDITIONAL_GOTO_CT;
			if (conditionToMakeGoTo->connectedConditionIndex != -1) // connected else needs to be removed
			{
				struct Condition* connectedCond = &params->currentFunc->conditions[conditionToMakeGoTo->connectedConditionIndex];
				if (connectedCond->conditionType == ELSE_CT)
				{
					removeCondition(params, conditionToMakeGoTo->connectedConditionIndex);
				}
				else if (connectedCond->conditionType == ELSE_IF_CT)
				{
					connectedCond->conditionType = IF_CT;
				}

				conditionToMakeGoTo->connectedConditionIndex = -1;
			}
		}
	} while (conditionToMakeGoTo);
	
	return 1;
}

static int getNumOfOverlappingConditions(struct DecompilationParameters* params, struct Condition* cond1)
{
	int result = 0;
	for (int i = 0; i < params->currentFunc->numOfConditions; i++) 
	{
		struct Condition* cond2 = &params->currentFunc->conditions[i];
		if (cond1 == cond2 || cond1->connectedConditionIndex == i || cond2->conditionType == CONDITIONAL_RETURN_CT || cond2->conditionType == CONDITIONAL_GOTO_CT)
		{
			continue;
		}

		if ((cond1->jccIndex < cond2->firstBodyIndex || cond1->jccIndex > cond2->lastBodyIndex) &&
			cond1->dstIndex >= cond2->firstBodyIndex && cond1->dstIndex <= cond2->lastBodyIndex) 
		{
			result++;
		}
		else if ((cond1->dstIndex < cond2->firstBodyIndex || cond1->dstIndex > cond2->lastBodyIndex) &&
			cond1->jccIndex >= cond2->firstBodyIndex && cond1->jccIndex <= cond2->lastBodyIndex)
		{
			result++;
		}
	}
	
	return result;
}

static unsigned char handleConditionsResize(struct DecompilationParameters* params)
{
	if (params->currentFunc->numOfConditions % 5 == 0)
	{
		struct Condition* newConditions = (struct Condition*)realloc(params->currentFunc->conditions, (params->currentFunc->numOfConditions + 5) * sizeof(struct Condition));
		if (!newConditions)
		{
			return 0;
			
		}

		params->currentFunc->conditions = newConditions;
		memset(params->currentFunc->conditions + params->currentFunc->numOfConditions, 0, sizeof(struct Condition) * 5);
	}

	return 1;
}

static unsigned char removeCondition(struct DecompilationParameters* params, int conditionIndex)
{
	if (conditionIndex < 0 || conditionIndex >= params->currentFunc->numOfConditions) 
	{
		return 0;
	}

	for(int i = 0; i < params->currentFunc->numOfConditions; i++)
	{
		struct Condition* cond = &params->currentFunc->conditions[i];
		if (cond->connectedConditionIndex > conditionIndex) 
		{
			cond->connectedConditionIndex--;
		}
		else if (cond->connectedConditionIndex == conditionIndex) 
		{
			cond->connectedConditionIndex = -1;
		}
	}

	if (conditionIndex + 1 < params->currentFunc->numOfConditions)
	{
		struct Condition* cond = &params->currentFunc->conditions[conditionIndex];
		struct Condition* nextCond = &params->currentFunc->conditions[conditionIndex + 1];
		memcpy(cond, nextCond, (params->currentFunc->numOfConditions - (conditionIndex + 1)) * sizeof(struct Condition));
	}

	params->currentFunc->numOfConditions--;

	struct Condition* newConditions = (struct Condition*)realloc(params->currentFunc->conditions, params->currentFunc->numOfConditions * sizeof(struct Condition));
	if (!newConditions)
	{
		return 0;
	}

	params->currentFunc->conditions = newConditions;
	return 1;
}

static unsigned char handleCombinedJccResize(struct Condition* condition) 
{
	if (condition->numOfCombinedJccs % 5 == 0)
	{
		int* newCombinedJccIndexes = (int*)realloc(condition->combinedJccIndexes, (condition->numOfCombinedJccs + 5) * sizeof(int));
		if (!newCombinedJccIndexes)
		{
			return 0;
		}

		condition->combinedJccIndexes = newCombinedJccIndexes;
	}

	return 1;
}

unsigned char decompileConditionEnds(struct DecompilationParameters* params, int instructionIndex, unsigned char* isInUnreachableStateRef, struct JdcStr* result)
{
	for (int i = 0; i < params->currentFunc->numOfConditions; i++)
	{
		struct Condition* condition = &params->currentFunc->conditions[i];
		if (!isConditionRegular(condition) || condition->indentLevel != params->numOfIndents)
		{
			continue;
		}

		if (instructionIndex == condition->lastBodyIndex)
		{
			if (!decompileCondition(params, i, 0, result))
			{
				return 0;
			}

			if (isInUnreachableStateRef) { *isInUnreachableStateRef = 0; }

			i = -1; // the loop needs to restart in order to recheck condition->indentLevel
		}
	}

	return 1;
}

unsigned char decompileConditionStarts(struct DecompilationParameters* params, int instructionIndex, struct JdcStr* result)
{
	for (int i = 0; i < params->currentFunc->numOfConditions; i++)
	{
		struct Condition* condition = &params->currentFunc->conditions[i];
		if (condition->conditionType == CONDITIONAL_GOTO_CT || condition->conditionType == CONDITIONAL_RETURN_CT)
		{
			if (instructionIndex == condition->jccIndex) 
			{
				if (!decompileCondition(params, i, 1, result))
				{
					return 0;
				}

				condition->indentLevel = params->numOfIndents;
			}
		}
		else if (instructionIndex == condition->firstBodyIndex)
		{
			if (!decompileCondition(params, i, 1, result))
			{
				return 0;
			}

			condition->indentLevel = params->numOfIndents;
		}
	}

	return 1;
}

static unsigned char decompileCondition(struct DecompilationParameters* params, int conditionIndex, unsigned char decompileStart, struct JdcStr* result)
{
	struct Condition* condition = &params->currentFunc->conditions[conditionIndex];

	if (decompileStart) 
	{
		if (condition->conditionType == DO_WHILE_CT)
		{
			addIndents(result, params->numOfIndents);
			strcatJdc(result, "do\n");
			addAssociatedInstruction(params->currentFunc, condition->firstBodyIndex);
			params->currentFunc->numOfLines++;

			addIndents(result, params->numOfIndents);
			strcatJdc(result, "{\n");
			addAssociatedInstruction(params->currentFunc, condition->firstBodyIndex);
			params->currentFunc->numOfLines++;

			params->numOfIndents++;
			return 1;
		}
		else if (condition->conditionType == ELSE_CT)
		{
			addIndents(result, params->numOfIndents);
			strcatJdc(result, "else\n");
			addAssociatedInstruction(params->currentFunc, condition->jccIndex);
			params->currentFunc->numOfLines++;

			addIndents(result, params->numOfIndents);
			strcatJdc(result, "{\n");
			addAssociatedInstruction(params->currentFunc, condition->jccIndex);
			params->currentFunc->numOfLines++;

			params->numOfIndents++;
			return 1;
		}
	}
	else if (condition->conditionType != DO_WHILE_CT)
	{
		params->numOfIndents--;
		addIndents(result, params->numOfIndents);
		strcatJdc(result, "}\n");
		addAssociatedInstruction(params->currentFunc, condition->lastBodyIndex);
		params->currentFunc->numOfLines++;
		return 1;
	}

	unsigned char invertCondition = condition->conditionType == CONDITIONAL_RETURN_CT || condition->conditionType == CONDITIONAL_GOTO_CT || condition->conditionType == DO_WHILE_CT;

	struct JdcStr conditionExpression = initializeJdcStr();
	if (condition->combinedJccsLogicType == OR_LT)
	{
		if (!decompileComparison(params, condition->jccIndex, invertCondition, &conditionExpression))
		{
			freeJdcStr(&conditionExpression);
			return 0;
		}

		for (int i = 0; i < condition->numOfCombinedJccs; i++)
		{
			unsigned char invertOperator = i == (condition->numOfCombinedJccs - 1);
			if (invertCondition)
			{
				invertOperator = !invertOperator;
			}

			struct JdcStr currentConditionExpression = initializeJdcStr();
			if (!decompileComparison(params, condition->combinedJccIndexes[i], invertOperator, &currentConditionExpression))
			{
				freeJdcStr(&currentConditionExpression);
				freeJdcStr(&conditionExpression);
				return 0;
			}

			addAssociatedInstruction(params->currentFunc, condition->combinedJccIndexes[i]);

			strcatJdc(&conditionExpression, !invertCondition ? " || " : " && ");
			strcatJdc(&conditionExpression, currentConditionExpression.buffer);
			freeJdcStr(&currentConditionExpression);
		}
	}
	else
	{
		if (!decompileComparison(params, condition->jccIndex, !invertCondition, &conditionExpression)) // this needs to run if combinedJccsLogicType is either AND_LT or NONE_LT. if it is NONE_LT, the loop wont run because numOfCombinedJccs will be 0 
		{
			freeJdcStr(&conditionExpression);
			return 0;
		}

		for (int i = 0; i < condition->numOfCombinedJccs; i++)
		{
			struct JdcStr currentConditionExpression = initializeJdcStr();
			if (!decompileComparison(params, condition->combinedJccIndexes[i], !invertCondition, &currentConditionExpression))
			{
				freeJdcStr(&conditionExpression);
				freeJdcStr(&currentConditionExpression);
				return 0;
			}

			addAssociatedInstruction(params->currentFunc, condition->combinedJccIndexes[i]);

			strcatJdc(&conditionExpression, !invertCondition ? " && " : " || ");
			strcatJdc(&conditionExpression, currentConditionExpression.buffer);
			freeJdcStr(&currentConditionExpression);
		}
	}

	if (condition->conditionType == LOOP_CT)
	{
		// check for for loop
		if (params->instructions[condition->exitIndex - 1].opcode == JMP_SHORT)
		{
			struct JdcStr assignmentExpression = initializeJdcStr();
			for (int i = condition->exitIndex; i < condition->jccIndex; i++)
			{
				if (checkForAssignment(params, i))
				{
					if (decompileAssignments(params, i, &assignmentExpression))
					{
						break;
					}
					else
					{
						freeJdcStr(&conditionExpression);
						freeJdcStr(&assignmentExpression);
						return 0;
					}
				}
			}

			addIndents(result, params->numOfIndents);
			sprintfJdc(result, 1, "for (; %s; %s)\n", conditionExpression.buffer, assignmentExpression.buffer);
			addAssociatedInstruction(params->currentFunc, condition->jccIndex);
			params->currentFunc->numOfLines++;

			freeJdcStr(&assignmentExpression);
		}
		else
		{
			addIndents(result, params->numOfIndents);
			sprintfJdc(result, 1, "while (%s)\n", conditionExpression.buffer);
			addAssociatedInstruction(params->currentFunc, condition->jccIndex);
			params->currentFunc->numOfLines++;
		}
	}
	else if (condition->conditionType == IF_CT)
	{
		addIndents(result, params->numOfIndents);
		sprintfJdc(result, 1, "if (%s)\n", conditionExpression.buffer);
		addAssociatedInstruction(params->currentFunc, condition->jccIndex);
		params->currentFunc->numOfLines++;
	}
	else if (condition->conditionType == CONDITIONAL_RETURN_CT)
	{
		addIndents(result, params->numOfIndents);
		sprintfJdc(result, 1, "if (%s)\n", conditionExpression.buffer);
		addAssociatedInstruction(params->currentFunc, condition->jccIndex);
		params->currentFunc->numOfLines++;

		addIndents(result, params->numOfIndents);
		strcatJdc(result, "{\n");
		addAssociatedInstruction(params->currentFunc, condition->dstIndex);
		params->currentFunc->numOfLines++;
		
		params->numOfIndents++;
		if (!decompileReturnStatement(params, condition->jccIndex, 0, result))
		{
			return 0;
		}
		params->numOfIndents--;

		addIndents(result, params->numOfIndents);
		strcatJdc(result, "}\n");
		addAssociatedInstruction(params->currentFunc, condition->dstIndex);
		params->currentFunc->numOfLines++;

		return freeJdcStr(&conditionExpression);
	}
	else if (condition->conditionType == CONDITIONAL_GOTO_CT)
	{
		addIndents(result, params->numOfIndents);
		sprintfJdc(result, 1, "if (%s)\n", conditionExpression.buffer);
		addAssociatedInstruction(params->currentFunc, condition->jccIndex);
		params->currentFunc->numOfLines++;

		addIndents(result, params->numOfIndents);
		strcatJdc(result, "{\n");
		addAssociatedInstruction(params->currentFunc, condition->jccIndex);
		params->currentFunc->numOfLines++;

		addIndents(result, params->numOfIndents + 1);
		sprintfJdc(result, 1, "goto label_%llX;\n", params->instructions[condition->dstIndex].address - params->imageBase);
		addAssociatedInstruction(params->currentFunc, condition->jccIndex);
		params->currentFunc->numOfLines++;

		addIndents(result, params->numOfIndents);
		strcatJdc(result, "}\n");
		addAssociatedInstruction(params->currentFunc, condition->jccIndex);
		params->currentFunc->numOfLines++;

		return freeJdcStr(&conditionExpression);
	}
	else if(condition->conditionType == ELSE_IF_CT)
	{
		addIndents(result, params->numOfIndents);
		sprintfJdc(result, 1, "else if (%s)\n", conditionExpression.buffer);
		addAssociatedInstruction(params->currentFunc, condition->jccIndex);
		params->currentFunc->numOfLines++;
	}
	else if (condition->conditionType == DO_WHILE_CT)
	{
		params->numOfIndents--;
		addIndents(result, params->numOfIndents);
		sprintfJdc(result, 1, "} while (%s);\n", conditionExpression.buffer);
		addAssociatedInstruction(params->currentFunc, condition->jccIndex);
		params->currentFunc->numOfLines++;

		return freeJdcStr(&conditionExpression);
	}

	
	addIndents(result, params->numOfIndents);
	strcatJdc(result, "{\n");
	addAssociatedInstruction(params->currentFunc, condition->jccIndex);
	params->currentFunc->numOfLines++;

	params->numOfIndents++;

	return freeJdcStr(&conditionExpression);
}

unsigned char isConditionRegular(struct Condition* condition) 
{
	return condition->conditionType != CONDITIONAL_GOTO_CT && condition->conditionType != CONDITIONAL_RETURN_CT;
}

int getConditionFromFirstBodyInstruction(struct DecompilationParameters* params, int instructionIndex)
{
	for (int i = 0; i < params->currentFunc->numOfConditions; i++)
	{
		if (instructionIndex == params->currentFunc->conditions[i].firstBodyIndex)
		{
			return i;
		}
	}

	return -1;
}

int getConditionFromLastBodyInstruction(struct DecompilationParameters* params, int instructionIndex)
{
	for (int i = 0; i < params->currentFunc->numOfConditions; i++)
	{
		if (instructionIndex == params->currentFunc->conditions[i].lastBodyIndex)
		{
			return i;
		}
	}

	return -1;
}