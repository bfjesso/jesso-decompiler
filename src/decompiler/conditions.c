#include "conditions.h"
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

		params->startInstructionIndex = i;

		if (isOpcodeJcc(instruction->opcode))
		{
			unsigned long long jccDstAddr = resolveJmpChain(params);
			int dstIndex = findInstructionByAddress(params->instructions, 0, params->numOfInstructions - 1, jccDstAddr);

			// if the conditions ends with a jmp, this will get the index of the instruction jumped to by that jmp
			int exitIndex = -1;
			if (dstIndex > 0 && isOpcodeJmp(params->instructions[dstIndex - 1].opcode))
			{
				// sometimes compiler puts multiple jmps next to each other at the end?
				int firstJmpIndex = dstIndex - 1;
				for (int j = dstIndex - 2; j > i; j--)
				{
					if (isOpcodeJmp(params->instructions[j].opcode))
					{
						firstJmpIndex = j;
					}
					else
					{
						break;
					}
				}

				params->startInstructionIndex = firstJmpIndex;
				unsigned long long jmpDstAddr = resolveJmpChain(params);
				exitIndex = findInstructionByAddress(params->instructions, 0, params->numOfInstructions - 1, jmpDstAddr);
			}

			params->startInstructionIndex = i;

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
				lastCondition->endIndex = dstIndex;
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
				else if (checkForJumpToReturnStatement(params))
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

				int startIndex = i;
				int endIndex = dstIndex;
				if (currentCondition->conditionType == SWITCH_CASE_CT)
				{
					startIndex = dstIndex;

					// only one of the switch cases needs an exit index set
					if (currentCondition->isFirstSwitchCase)
					{
						endIndex = exitIndex;
					}
					else 
					{
						exitIndex = 0;
					}
				}
				else if(currentCondition->conditionType != CONDITIONAL_RETURN_CT)
				{
					if (startIndex > endIndex)
					{
						startIndex = dstIndex;
						endIndex = i;
					}
				}

				currentCondition->startIndex = startIndex;
				currentCondition->endIndex = endIndex;

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
		if ((params->currentFunc->conditions[i].conditionType == IF_CT || params->currentFunc->conditions[i].conditionType == ELSE_IF_CT) && 
			params->currentFunc->conditions[i].exitIndex > params->currentFunc->conditions[i].dstIndex &&
			(i == ogNumOfConditions - 1 || params->currentFunc->conditions[i + 1].conditionType != ELSE_IF_CT))
		{
			params->startInstructionIndex = params->currentFunc->conditions[i].dstIndex - 1;
			if (!checkForReturnStatement(params))
			{
				if (!handleConditionsResize(params))
				{
					return 0;
				}

				params->currentFunc->conditions[params->currentFunc->numOfConditions].jccIndex = params->currentFunc->conditions[i].dstIndex;
				params->currentFunc->conditions[params->currentFunc->numOfConditions].dstIndex = params->currentFunc->conditions[i].exitIndex;
				params->currentFunc->conditions[params->currentFunc->numOfConditions].conditionType = ELSE_CT;
				params->currentFunc->numOfConditions++;
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
		if (cond1->conditionType == CONDITIONAL_RETURN_CT || cond1->conditionType == SWITCH_CASE_CT || cond1->conditionType == ELSE_CT)
		{
			continue;
		}

		for (int j = 0; j < params->currentFunc->numOfConditions; j++)
		{
			struct Condition* cond2 = &params->currentFunc->conditions[j];
			if (i == j || cond2->conditionType == CONDITIONAL_RETURN_CT || cond2->conditionType == CONDITIONAL_GOTO_CT || cond2->conditionType == SWITCH_CASE_CT)
			{
				continue;
			}

			if ((cond1->startIndex < cond2->startIndex && cond1->endIndex > cond2->startIndex && cond1->endIndex < cond2->endIndex) || (cond1->startIndex > cond2->startIndex && cond1->startIndex < cond2->endIndex && cond1->endIndex > cond2->endIndex) || (cond1->startIndex == cond2->startIndex && cond1->endIndex > cond2->endIndex)) // last check is for do while loops
			{
				cond1->conditionType = CONDITIONAL_GOTO_CT;
				cond1->startIndex = cond1->jccIndex;
				cond1->endIndex = cond1->dstIndex;
				break;
			}
		}
	}

	return 1;
}

static unsigned char handleConditionsResize(struct DecompilationParameters* params)
{
	if (params->currentFunc->numOfConditions % 5 == 0)
	{
		struct Condition* newConditions = (struct Condition*)realloc(params->currentFunc->conditions, (params->currentFunc->numOfConditions + 5) * sizeof(struct Condition));
		if (newConditions)
		{
			params->currentFunc->conditions = newConditions;
			memset(params->currentFunc->conditions + params->currentFunc->numOfConditions, 0, sizeof(struct Condition) * 5);
		}
		else
		{
			return 0;
		}
	}

	return 1;
}

static unsigned char handleCombinedJccResize(struct Condition* condition) 
{
	if (condition->numOfCombinedJccs % 5 == 0)
	{
		int* newCombinedJccIndexes = (int*)realloc(condition->combinedJccIndexes, (condition->numOfCombinedJccs + 5) * sizeof(int));
		if (newCombinedJccIndexes)
		{
			condition->combinedJccIndexes = newCombinedJccIndexes;
		}
		else
		{
			return 0;
		}
	}

	return 1;
}

unsigned char decompileConditions(struct DecompilationParameters* params, struct JdcStr* result) 
{
	// handling the ends of conditions
	for (int i = 0; i < params->currentFunc->numOfConditions; i++)
	{
		struct Condition* condition = &params->currentFunc->conditions[i];
		if (!isConditionRegular(condition) || condition->indentLevel != params->numOfIndents)
		{
			continue;
		}

		if (params->startInstructionIndex == condition->endIndex)
		{
			if (!decompileCondition(params, i, 0, result))
			{
				return 0;
			}

			i = -1; // the loop needs to restart in order to recheck condition->indentLevel
		}
	}

	// setting the labels for conditional gotos
	for (int i = 0; i < params->currentFunc->numOfConditions; i++)
	{
		struct Condition* condition = &params->currentFunc->conditions[i];
		if (condition->conditionType == CONDITIONAL_GOTO_CT && params->startInstructionIndex == condition->dstIndex)
		{
			addIndents(result, params->numOfIndents - 1);
			sprintfJdc(result, 1, "label_%llX:\n", params->instructions[condition->dstIndex].address - params->imageBase);
			break;
		}
	}

	// handling the starts of conditions
	for (int i = 0; i < params->currentFunc->numOfConditions; i++)
	{
		struct Condition* condition = &params->currentFunc->conditions[i];
		if (condition->isCombinedByOther)
		{
			continue;
		}

		if (params->startInstructionIndex == condition->startIndex)
		{
			if (!decompileCondition(params, i, 1, result))
			{
				return 0;
			}

			condition->indentLevel = params->numOfIndents;

			break;
		}
	}

	return 1;
}

static unsigned char decompileCondition(struct DecompilationParameters* params, int conditionIndex, unsigned char decompileStart, struct JdcStr* result)
{
	struct Condition* condition = &params->currentFunc->conditions[conditionIndex];
	int ogStartInstructionIndex = params->startInstructionIndex;

	if (decompileStart) 
	{
		if (condition->conditionType == DO_WHILE_CT)
		{
			addIndents(result, params->numOfIndents);
			strcatJdc(result, "do\n");
			addIndents(result, params->numOfIndents);
			strcatJdc(result, "{\n");
			params->numOfIndents++;
			return 1;
		}
		else if (condition->conditionType == LOOP_CT)
		{
			params->startInstructionIndex = condition->jccIndex;
		}
		else if (condition->conditionType == SWITCH_CASE_CT)
		{
			if (condition->isFirstSwitchCase)
			{
				struct JdcStr switchVar = initializeJdcStr();
				if (!decompileOperand(params, &condition->cmpInstruction->operands[0], &switchVar))
				{
					freeJdcStr(&switchVar);
					return 0;
				}

				addIndents(result, params->numOfIndents);
				sprintfJdc(result, 1, "switch(%s)\n", switchVar.buffer);
				addIndents(result, params->numOfIndents);
				strcatJdc(result, "{\n");
				params->numOfIndents++;

				freeJdcStr(&switchVar);
			}
			else
			{
				addIndents(result, params->numOfIndents);
				strcatJdc(result, "break;\n");
			}

			struct JdcStr value = initializeJdcStr();
			if (!decompileOperand(params, &condition->cmpInstruction->operands[1], &value))
			{
				freeJdcStr(&value);
				return 0;
			}

			addIndents(result, params->numOfIndents - 1);
			sprintfJdc(result, 1, "case %s:\n", value.buffer);
			freeJdcStr(&value);
			return 1;
		}
		else if (condition->conditionType == ELSE_CT)
		{
			addIndents(result, params->numOfIndents);
			strcatJdc(result, "else\n");
			addIndents(result, params->numOfIndents);
			strcatJdc(result, "{\n");
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
		}

		params->numOfIndents--;
		addIndents(result, params->numOfIndents);
		strcatJdc(result, "}\n");
		return 1;
	}

	unsigned char invertCondition = condition->conditionType == CONDITIONAL_RETURN_CT || condition->conditionType == CONDITIONAL_GOTO_CT || condition->conditionType == DO_WHILE_CT;

	struct JdcStr conditionExpression = initializeJdcStr();
	if (condition->combinedJccsLogicType == OR_LT)
	{
		if (!decompileComparison(params, invertCondition, &conditionExpression))
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
			params->startInstructionIndex = condition->combinedJccIndexes[i];
			if (!decompileComparison(params, invertOperator, &currentConditionExpression))
			{
				freeJdcStr(&currentConditionExpression);
				freeJdcStr(&conditionExpression);
				return 0;
			}

			strcatJdc(&conditionExpression, !invertCondition ? " || " : " && ");
			strcatJdc(&conditionExpression, currentConditionExpression.buffer);
			freeJdcStr(&currentConditionExpression);
		}
	}
	else
	{
		if (!decompileComparison(params, !invertCondition, &conditionExpression)) // this needs to run if combinedJccsLogicType is either AND_LT or NONE_LT. if it is NONE_LT, the loop wont run because numOfCombinedJccs will be 0 
		{
			freeJdcStr(&conditionExpression);
			return 0;
		}

		for (int i = 0; i < condition->numOfCombinedJccs; i++)
		{
			struct JdcStr currentConditionExpression = initializeJdcStr();
			params->startInstructionIndex = condition->combinedJccIndexes[i];
			if (!decompileComparison(params, !invertCondition, &currentConditionExpression))
			{
				freeJdcStr(&conditionExpression);
				freeJdcStr(&currentConditionExpression);
				return 0;
			}

			strcatJdc(&conditionExpression, !invertCondition ? " && " : " || ");
			strcatJdc(&conditionExpression, currentConditionExpression.buffer);
			freeJdcStr(&currentConditionExpression);
		}
	}

	struct JdcStr combinedConditionExpression = initializeJdcStr();
	if (condition->combinedConditionIndex)
	{
		params->startInstructionIndex = params->currentFunc->conditions[condition->combinedConditionIndex].jccIndex;
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
				params->startInstructionIndex = i;
				if (checkForAssignment(params))
				{
					if (decompileAssignments(params, &assignmentExpression))
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
			freeJdcStr(&assignmentExpression);
		}
		else
		{
			addIndents(result, params->numOfIndents);
			sprintfJdc(result, 1, "while (%s)\n", conditionExpression.buffer);
		}
	}
	else if (condition->conditionType == IF_CT)
	{
		addIndents(result, params->numOfIndents);
		sprintfJdc(result, 1, "if (%s)\n", conditionExpression.buffer);
	}
	else if (condition->conditionType == CONDITIONAL_RETURN_CT)
	{
		addIndents(result, params->numOfIndents);
		sprintfJdc(result, 1, "if (%s)\n", conditionExpression.buffer);
		addIndents(result, params->numOfIndents);
		strcatJdc(result, "{\n");
		
		params->numOfIndents++;
		if (!decompileReturnStatement(params, 0, result))
		{
			return 0;
		}
		params->numOfIndents--;

		addIndents(result, params->numOfIndents);
		strcatJdc(result, "}\n");

		params->startInstructionIndex = ogStartInstructionIndex;
		return freeJdcStr(&conditionExpression);
	}
	else if (condition->conditionType == CONDITIONAL_GOTO_CT)
	{
		addIndents(result, params->numOfIndents);
		sprintfJdc(result, 1, "if (%s)\n", conditionExpression.buffer);
		addIndents(result, params->numOfIndents);
		strcatJdc(result, "{\n");
		addIndents(result, params->numOfIndents + 1);
		sprintfJdc(result, 1, "goto label_%llX;\n", params->instructions[condition->dstIndex].address - params->imageBase);
		addIndents(result, params->numOfIndents);
		strcatJdc(result, "}\n");

		params->startInstructionIndex = ogStartInstructionIndex;
		return freeJdcStr(&conditionExpression);
	}
	else if(condition->conditionType == ELSE_IF_CT)
	{
		addIndents(result, params->numOfIndents);
		sprintfJdc(result, 1, "else if (%s)\n", conditionExpression.buffer);
	}
	else if (condition->conditionType == DO_WHILE_CT)
	{
		params->numOfIndents--;
		addIndents(result, params->numOfIndents);
		sprintfJdc(result, 1, "} while (%s);\n", conditionExpression.buffer);
		params->startInstructionIndex = ogStartInstructionIndex;
		return freeJdcStr(&conditionExpression);
	}

	
	addIndents(result, params->numOfIndents);
	strcatJdc(result, "{\n");
	params->numOfIndents++;

	params->startInstructionIndex = ogStartInstructionIndex;
	return freeJdcStr(&conditionExpression);
}

unsigned char isConditionRegular(struct Condition* condition) 
{
	return condition->conditionType != CONDITIONAL_GOTO_CT && condition->conditionType != CONDITIONAL_RETURN_CT && !condition->isCombinedByOther;
}

int checkForConditionStart(struct DecompilationParameters* params)
{
	for (int i = 0; i < params->currentFunc->numOfConditions; i++)
	{
		if (params->startInstructionIndex == params->currentFunc->conditions[i].startIndex)
		{
			return i;
		}
	}

	return -1;
}

int checkForConditionEnd(struct DecompilationParameters* params)
{
	for (int i = 0; i < params->currentFunc->numOfConditions; i++)
	{
		if (params->startInstructionIndex == params->currentFunc->conditions[i].endIndex && isConditionRegular(&params->currentFunc->conditions[i]))
		{
			return i;
		}
	}

	return -1;
}

unsigned char checkForConditionDst(struct DecompilationParameters* params)
{
	for (int i = 0; i < params->currentFunc->numOfConditions; i++)
	{
		if (params->startInstructionIndex == params->currentFunc->conditions[i].dstIndex)
		{
			return 1;
		}
	}

	return 0;
}

