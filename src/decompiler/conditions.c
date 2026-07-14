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
	int lastDstIndex = -1;
	int firstDstSwitchCaseIndex = -1;
	unsigned char stopCombination = 0;
	struct DisassembledInstruction* lastCmpInstruction = 0;
	struct DisassembledInstruction* currentCmpInstruction = 0;
	for (int i = params->currentFunc->firstInstructionIndex; i <= params->currentFunc->lastInstructionIndex; i++)
	{
		struct DisassembledInstruction* instruction = &(params->instructions[i]);
		if (isOpcodeJcc(instruction->opcode))
		{
			unsigned long long dstAddress = instruction->address + instruction->numOfBytes + instruction->operands[0].immediate.value;
			int dstIndex = findInstructionByAddress(params->instructions, params->numOfInstructions, dstAddress);

			// the jmp chain result should only be used for conditional gotos and conditional returns
			int dstJmpChainIndex = findInstructionByAddress(params->instructions, params->numOfInstructions, resolveJmpChain(params, i));

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
			else if (lastCondition && lastDstIndex - 1 == i && !stopCombination)
			{
				if (!handleCombinedJccResize(lastCondition))
				{
					return 0;
				}
				
				lastCondition->combinedJccIndexes[combinationCount] = i;
				lastCondition->combinedJccsLogicType = OR_LT;
				lastCondition->dstIndex = dstIndex;
				lastCondition->lastBodyIndex = dstIndex - 1;
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

				// setting the type
				if (lastCondition && exitIndex != -1 && exitIndex == lastCondition->exitIndex &&
					i == lastCondition->jccIndex + 2 && // the Jccs need to be right next to eachother with only comparisson instructions between them
					instruction->opcode == JZ_SHORT && params->instructions[lastCondition->jccIndex].opcode == JZ_SHORT &&
					lastCmpInstruction && currentCmpInstruction && compareOperands(&lastCmpInstruction->operands[0], &currentCmpInstruction->operands[0]) &&
					combinationCount == 0 && lastCondition->numOfCombinedJccs == 0)
				{
					if (lastCondition->conditionType != SWITCH_CASE_CT) 
					{
						lastCondition->cmpInstruction = lastCmpInstruction;
						lastCondition->conditionType = SWITCH_CASE_CT;
						lastCondition->isFirstSwitchCase = 1;
						firstDstSwitchCaseIndex = params->currentFunc->numOfConditions - 1;
					}
					
					currentCondition->cmpInstruction = currentCmpInstruction;
					currentCondition->conditionType = SWITCH_CASE_CT;

					if (dstIndex < params->currentFunc->conditions[firstDstSwitchCaseIndex].dstIndex) 
					{
						params->currentFunc->conditions[firstDstSwitchCaseIndex].isFirstSwitchCase = 0;
						currentCondition->isFirstSwitchCase = 1;
					}
				}
				else if (doesInstructionLeadStraightToReturn(params, dstJmpChainIndex))
				{
					currentCondition->conditionType = CONDITIONAL_RETURN_CT;
				}
				else if (exitIndex != -1 && exitIndex == i - 1) // checks if the exitIndex is to the instruction before the Jcc, which is assumed to be the comparisson instruction
				{
					currentCondition->conditionType = LOOP_CT;
				}
				else if (dstIndex < i)
				{
					currentCondition->conditionType = DO_WHILE_CT;
				}
				else if (lastCondition &&
					(lastCondition->conditionType == IF_CT || lastCondition->conditionType == ELSE_IF_CT) &&
					i == lastCondition->dstIndex + 1 && // assuming again that the previous Jcc jumps directly to this one's comparisson instruction
					exitIndex != -1 && lastCondition->exitIndex == exitIndex && // check for else if
					dstIndex > lastCondition->dstIndex) // also have to check that its not nested
				{
					currentCondition->conditionType = ELSE_IF_CT;
				}
				else
				{
					currentCondition->conditionType = IF_CT;
				}

				currentCondition->jccIndex = i;
				currentCondition->dstIndex = dstIndex;
				currentCondition->exitIndex = exitIndex;

				int firstBodyIndex = i + 1;
				int lastBodyIndex = dstIndex - 1;
				if (currentCondition->conditionType == SWITCH_CASE_CT)
				{
					firstBodyIndex = dstIndex;

					// only one of the switch cases needs an exit index set
					if (currentCondition->isFirstSwitchCase)
					{
						lastBodyIndex = exitIndex;
					}
					else 
					{
						exitIndex = 0;
					}
				}
				else if(currentCondition->conditionType != CONDITIONAL_RETURN_CT)
				{
					if (firstBodyIndex > lastBodyIndex)
					{
						firstBodyIndex = dstIndex;
						lastBodyIndex = i - 1;
					}
				}
				else 
				{
					currentCondition->dstIndex = dstJmpChainIndex;
					firstBodyIndex = -1;
					lastBodyIndex = -1;
				}

				currentCondition->firstBodyIndex = firstBodyIndex;
				currentCondition->lastBodyIndex = lastBodyIndex;

				combinationCount = 0;
				stopCombination = 0;
				params->currentFunc->numOfConditions++;
			}

			lastDstIndex = dstIndex;
		}
		else if(params->currentFunc->numOfConditions > 0 && !isOpcodeCmp(instruction->opcode) && instruction->opcode != TEST) // the Jccs cant be combined into one condition if there is other code that runs between them.
		{
			stopCombination = 1;
		}
		else if (isOpcodeCmp(instruction->opcode)) 
		{
			lastCmpInstruction = currentCmpInstruction;
			currentCmpInstruction = instruction;
		}
	}
	
	// addings ELSEs
	int ogNumOfConditions = params->currentFunc->numOfConditions;
	for (int i = 0; i < ogNumOfConditions; i++) 
	{
		struct Condition* cond = &params->currentFunc->conditions[i];
		if ((cond->conditionType == IF_CT || cond->conditionType == ELSE_IF_CT) && 
			cond->exitIndex > cond->dstIndex &&
			(i == ogNumOfConditions - 1 || params->currentFunc->conditions[i + 1].conditionType != ELSE_IF_CT))
		{
			if (!doesInstructionLeadStraightToReturn(params, cond->exitIndex))
			{
				if (!handleConditionsResize(params))
				{
					return 0;
				}

				struct Condition* elseCond = &params->currentFunc->conditions[params->currentFunc->numOfConditions];
				params->currentFunc->numOfConditions++;

				elseCond->conditionType = ELSE_CT;
				elseCond->jccIndex = cond->dstIndex - 1;
				elseCond->dstIndex = cond->exitIndex;
				elseCond->firstBodyIndex = cond->dstIndex;
				elseCond->lastBodyIndex = cond->exitIndex - 1;
			}
		}
	}

	//// combining conditions
	//for (int i = 0; i < params->currentFunc->numOfConditions - 2; i++)
	//{
	//	if (conditions[i].dstIndex == conditions[i + 1].dstIndex)
	//	{
	//		conditions[i].dstIndex = conditions[i + 1].dstIndex;
	//		conditions[i].exitIndex = conditions[i + 1].exitIndex;
	//		conditions[i].combinedConditionIndex = i + 1;
	//		conditions[i].combinationLogicType = AND_LT;
	//		conditions[i + 1].isCombinedByOther = 1;
	//		i++;
	//	}
	//	else if (conditions[i].dstIndex - 1 == conditions[i + 1].jccIndex)
	//	{
	//		conditions[i].dstIndex = conditions[i + 1].dstIndex;
	//		conditions[i].exitIndex = conditions[i + 1].exitIndex;
	//		conditions[i].combinedConditionIndex = i + 1;
	//		conditions[i].combinationLogicType = OR_LT;
	//		conditions[i + 1].isCombinedByOther = 1;
	//		i++;
	//	}
	//}

	// checking for overlapping conditions which need to be handled as go to
	for (int i = 0; i < params->currentFunc->numOfConditions; i++)
	{
		struct Condition* cond1 = &params->currentFunc->conditions[i];
		if (cond1->conditionType == CONDITIONAL_RETURN_CT || cond1->conditionType == CONDITIONAL_GOTO_CT || cond1->conditionType == SWITCH_CASE_CT || cond1->conditionType == ELSE_CT)
		{
			continue;
		}

		struct Condition* cond2 = doesConditionOverlapWithAnother(params, cond1);
		if (cond2)
		{
			// the loops check is arbitrary, but it looks better to preserve them. an else can't be a conditional goto because it is not a jcc
			if (cond2->conditionType == DO_WHILE_CT || cond2->conditionType == LOOP_CT || cond2->conditionType == ELSE_CT)
			{
				cond1->conditionType = CONDITIONAL_GOTO_CT;
				cond1->dstIndex = findInstructionByAddress(params->instructions, params->numOfInstructions, resolveJmpChain(params, cond1->jccIndex));
				cond1->firstBodyIndex = -1;
				cond1->lastBodyIndex = -1;
			}
			else 
			{
				cond2->conditionType = CONDITIONAL_GOTO_CT;
				cond2->dstIndex = findInstructionByAddress(params->instructions, params->numOfInstructions, resolveJmpChain(params, cond2->jccIndex));
				cond2->firstBodyIndex = -1;
				cond2->lastBodyIndex = -1;
			}
		}
	}

	// checking for ELSE conditions that are associated with a conditional goto, which need to be removed
	unsigned char reallocate = 0;
	for (int i = 0; i < params->currentFunc->numOfConditions; i++)
	{
		struct Condition* gotoCond = &params->currentFunc->conditions[i];
		if (gotoCond->conditionType == CONDITIONAL_GOTO_CT)
		{
			for (int j = i + 1; j < params->currentFunc->numOfConditions; j++)
			{
				struct Condition* elseCond = &params->currentFunc->conditions[j];
				if (elseCond->conditionType == ELSE_CT && gotoCond->dstIndex == elseCond->firstBodyIndex)
				{
					if (j + 1 < params->currentFunc->numOfConditions) 
					{
						struct Condition* nextCond = &params->currentFunc->conditions[j + 1];
						memcpy(elseCond, nextCond, (params->currentFunc->numOfConditions - (j + 1)) * sizeof(struct Condition));
					}
					
					params->currentFunc->numOfConditions--;
					reallocate = 1;
					break;
				}
			}
		}
	}

	if (reallocate) 
	{
		struct Condition* newConditions = (struct Condition*)realloc(params->currentFunc->conditions, params->currentFunc->numOfConditions * sizeof(struct Condition));
		if (!newConditions)
		{
			return 0;
		}

		params->currentFunc->conditions = newConditions;
	}

	return 1;
}

static struct Condition* doesConditionOverlapWithAnother(struct DecompilationParameters* params, struct Condition* cond1)
{
	for (int i = 0; i < params->currentFunc->numOfConditions; i++) 
	{
		struct Condition* cond2 = &params->currentFunc->conditions[i];
		if (cond1 == cond2 || cond2->conditionType == CONDITIONAL_RETURN_CT || cond2->conditionType == CONDITIONAL_GOTO_CT || cond2->conditionType == SWITCH_CASE_CT)
		{
			continue;
		}

		if ((cond1->firstBodyIndex < cond2->firstBodyIndex && cond1->lastBodyIndex > cond2->firstBodyIndex && cond1->lastBodyIndex < cond2->lastBodyIndex) ||
			(cond1->firstBodyIndex > cond2->firstBodyIndex && cond1->firstBodyIndex < cond2->lastBodyIndex && cond1->lastBodyIndex > cond2->lastBodyIndex))
		{
			return cond2;
		}
	}
	
	return 0;
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
		if (condition->isCombinedByOther)
		{
			continue;
		}

		if (instructionIndex == condition->firstBodyIndex || 
			(instructionIndex == condition->jccIndex && (condition->conditionType == CONDITIONAL_GOTO_CT || condition->conditionType == CONDITIONAL_RETURN_CT)))
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
		else if (condition->conditionType == SWITCH_CASE_CT)
		{
			if (condition->isFirstSwitchCase)
			{
				struct JdcStr switchVar = initializeJdcStr();
				if (!decompileOperand(params, condition->firstBodyIndex, &condition->cmpInstruction->operands[0], 1, &switchVar))
				{
					freeJdcStr(&switchVar);
					return 0;
				}

				addIndents(result, params->numOfIndents);
				sprintfJdc(result, 1, "switch(%s)\n", switchVar.buffer);
				addAssociatedInstruction(params->currentFunc, condition->jccIndex);
				params->currentFunc->numOfLines++;

				addIndents(result, params->numOfIndents);
				strcatJdc(result, "{\n");
				addAssociatedInstruction(params->currentFunc, condition->jccIndex);
				params->currentFunc->numOfLines++;

				params->numOfIndents++;

				freeJdcStr(&switchVar);
			}
			else
			{
				addIndents(result, params->numOfIndents);
				strcatJdc(result, "break;\n");
				addAssociatedInstruction(params->currentFunc, condition->jccIndex);
				params->currentFunc->numOfLines++;
			}

			struct JdcStr value = initializeJdcStr();
			if (!decompileOperand(params, condition->firstBodyIndex, &condition->cmpInstruction->operands[1], 1, &value))
			{
				freeJdcStr(&value);
				return 0;
			}

			addIndents(result, params->numOfIndents - 1);
			sprintfJdc(result, 1, "case %s:\n", value.buffer);
			addAssociatedInstruction(params->currentFunc, condition->jccIndex);
			params->currentFunc->numOfLines++;

			freeJdcStr(&value);
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
		if (condition->isFirstSwitchCase)
		{
			addIndents(result, params->numOfIndents);
			strcatJdc(result, "break;\n");
			addAssociatedInstruction(params->currentFunc, condition->jccIndex);
			params->currentFunc->numOfLines++;
		}

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

	struct JdcStr combinedConditionExpression = initializeJdcStr();
	if (condition->combinedConditionIndex)
	{
		if (decompileCondition(params, condition->combinedConditionIndex, 1, &combinedConditionExpression))
		{
			if (!wrapJdcStrInParentheses(&conditionExpression))
			{
				freeJdcStr(&conditionExpression);
				freeJdcStr(&combinedConditionExpression);
				return 0;
			}

			if (condition->combinationLogicType == AND_LT)
			{
				strcatJdc(&conditionExpression, !invertCondition ? " && " : " || ");
			}
			else
			{
				strcatJdc(&conditionExpression, !invertCondition ? " || " : " && ");
			}

			strcatJdc(&conditionExpression, combinedConditionExpression.buffer);
		}
		else
		{
			freeJdcStr(&conditionExpression);
			freeJdcStr(&combinedConditionExpression);
			return 0;
		}
	}

	freeJdcStr(&combinedConditionExpression);

	if (condition->isCombinedByOther)
	{
		strcatJdc(result, conditionExpression.buffer);
		freeJdcStr(&conditionExpression);
		return 1;
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
	return condition->conditionType != CONDITIONAL_GOTO_CT && condition->conditionType != CONDITIONAL_RETURN_CT && !condition->isCombinedByOther;
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