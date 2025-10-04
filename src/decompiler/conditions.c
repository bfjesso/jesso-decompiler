#include "conditions.h"
#include "expressions.h"
#include "assignment.h"

#include <string.h>

int getAllConditions(struct DecompilationParameters params, struct Condition* conditionsBuffer)
{
	// find all conditional jumps
	struct Condition allJccs[20] = { 0 };
	int jccCount = 0;
	for (int i = 0; i < params.currentFunc->numOfInstructions; i++)
	{
		struct DisassembledInstruction* instruction = &(params.currentFunc->instructions[i]);

		if (instruction->opcode >= JA_SHORT && instruction->opcode < JMP_SHORT)
		{
			allJccs[jccCount].jccIndex = i;

			// getting index of instruction jumped to by jcc
			unsigned long long jccDst = params.currentFunc->addresses[i] + instruction->operands[0].immediate;
			int jccDstIndex = findInstructionByAddress(params.currentFunc->addresses, 0, params.currentFunc->numOfInstructions - 1, jccDst);
			allJccs[jccCount].dstIndex = jccDstIndex;

			// if the conditions ends with a jmp, this will get the index of the instruction jumped to by that jmp
			if (params.currentFunc->instructions[jccDstIndex - 1].opcode == JMP_SHORT)
			{
				// sometimes compiler puts multiple jmps next to each other at the end?
				int firstJmpIndex = jccDstIndex - 1;
				for (int j = jccDstIndex - 2; j > i; j--)
				{
					if (params.currentFunc->instructions[j].opcode == JMP_SHORT)
					{
						firstJmpIndex = j;
					}
					else
					{
						break;
					}
				}

				unsigned long long jmpDst = params.currentFunc->addresses[firstJmpIndex] + params.currentFunc->instructions[firstJmpIndex].operands[0].immediate;
				int jmpDstIndex = findInstructionByAddress(params.currentFunc->addresses, 0, params.currentFunc->numOfInstructions - 1, jmpDst);
				allJccs[jccCount].exitIndex = jmpDstIndex;
			}
			else
			{
				allJccs[jccCount].exitIndex = -1;
			}

			jccCount++;
		}
	}

	struct Condition conditions[20] = { 0 };
	int numOfConditions = getAndsAndOrs(allJccs, jccCount, conditions);

	combineConditions(conditions, numOfConditions);

	return setConditionTypes(conditions, numOfConditions, conditionsBuffer);
}

static int getAndsAndOrs(struct Condition* allJccs, int numOfConditions, struct Condition* conditionsBuffer)
{
	int newConditionsIndex = 0;

	for (int i = 0; i < numOfConditions; i++)
	{
		memcpy(&conditionsBuffer[newConditionsIndex], &allJccs[i], sizeof(struct Condition));

		// check for ||. a group of ORs is a series of Jccs that all go to the same destination (like ANDs), but the last one in the series has a Jcc as the instruction right before its destination
		int orCount = 0;
		unsigned char reachedEnd = 0;
		for (int j = i + 1; j < numOfConditions; j++)
		{
			if (allJccs[i].dstIndex == allJccs[j].dstIndex)
			{
				conditionsBuffer[newConditionsIndex].otherJccIndexes[orCount] = allJccs[j].jccIndex; // this could be part of an AND, but orCount will be set to 0 if its not so this will be ignored or overwritten
				orCount++;
			}
			else if (allJccs[j - 1].dstIndex - 1 == allJccs[j].jccIndex)
			{
				conditionsBuffer[newConditionsIndex].otherJccIndexes[orCount] = allJccs[j].jccIndex;
				conditionsBuffer[newConditionsIndex].dstIndex = allJccs[j].dstIndex;
				conditionsBuffer[newConditionsIndex].exitIndex = allJccs[j].exitIndex;
				orCount++;

				conditionsBuffer[newConditionsIndex].otherJccsLogicType = OR_LT;
				conditionsBuffer[newConditionsIndex].numOfOtherJccs = orCount;
				i += orCount;

				break;
			}
			else
			{
				orCount = 0;
				break;
			}

			if (j == numOfConditions - 1)
			{
				orCount = 0;
			}
		}

		if (orCount == 0)
		{
			// check for &&. a group of ANDs is a series of Jccs that all go to the same destination
			int andCount = 0;
			for (int j = i + 1; j < numOfConditions; j++)
			{
				if (allJccs[i].dstIndex == allJccs[j].dstIndex)
				{
					conditionsBuffer[newConditionsIndex].otherJccsLogicType = AND_LT;
					conditionsBuffer[newConditionsIndex].otherJccIndexes[andCount] = allJccs[j].jccIndex;
					andCount++;
				}
				else
				{
					break;
				}
			}
			conditionsBuffer[newConditionsIndex].numOfOtherJccs = andCount;
			i += andCount;
		}

		newConditionsIndex++;
	}

	return newConditionsIndex;
}

static void combineConditions(struct Condition* conditions, int numOfConditions)
{
	for (int i = 0; i < numOfConditions - 2; i++)
	{
		if (conditions[i].dstIndex == conditions[i + 1].dstIndex)
		{
			conditions[i].dstIndex = conditions[i + 1].dstIndex;
			conditions[i].exitIndex = conditions[i + 1].exitIndex;
			conditions[i].combinedConditionIndex = i + 1;
			conditions[i].combinationLogicType = AND_LT;
			conditions[i + 1].isCombinedByOther = 1;
			i++;
		}
		else if (conditions[i].dstIndex - 1 == conditions[i + 1].jccIndex)
		{
			conditions[i].dstIndex = conditions[i + 1].dstIndex;
			conditions[i].exitIndex = conditions[i + 1].exitIndex;
			conditions[i].combinedConditionIndex = i + 1;
			conditions[i].combinationLogicType = OR_LT;
			conditions[i + 1].isCombinedByOther = 1;
			i++;
		}
	}
}

static int setConditionTypes(struct Condition* conditions, int numOfConditions, struct Condition* conditionsBuffer)
{
	int conditionsIndex = 0; // this has its own index and buffer because ELSEs might be added to the array of conditions
	for (int i = 0; i < numOfConditions; i++)
	{
		if (conditions[i].isCombinedByOther)
		{
			memcpy(&conditionsBuffer[conditionsIndex], &conditions[i], sizeof(struct Condition));
			conditionsIndex++;
			continue;
		}

		// check for loops
		if (conditions[i].exitIndex != -1 && conditions[i].exitIndex < conditions[i].jccIndex)
		{
			memcpy(&conditionsBuffer[conditionsIndex], &conditions[i], sizeof(struct Condition));
			conditionsBuffer[conditionsIndex].conditionType = LOOP_CT;
		}
		else if (i > 0 && conditions[i].exitIndex != -1 &&
			conditions[i - 1].exitIndex == conditions[i].exitIndex && // check for else if
			conditions[i].dstIndex > conditions[i - 1].dstIndex) // also have to check that its not nested
		{
			memcpy(&conditionsBuffer[conditionsIndex], &conditions[i], sizeof(struct Condition));
			conditionsBuffer[conditionsIndex].conditionType = ELSE_IF_CT;
		}
		else
		{
			// check if last if condition has an else and add it if so
			if (i > 0 && conditionsBuffer[conditionsIndex - 1].conditionType != LOOP_CT && conditions[i - 1].exitIndex != -1)
			{
				conditionsBuffer[conditionsIndex].jccIndex = conditions[i - 1].dstIndex;
				conditionsBuffer[conditionsIndex].dstIndex = conditions[i - 1].exitIndex;
				conditionsBuffer[conditionsIndex].conditionType = ELSE_CT;

				conditionsIndex++;
			}

			memcpy(&conditionsBuffer[conditionsIndex], &conditions[i], sizeof(struct Condition));
			conditionsBuffer[conditionsIndex].conditionType = IF_CT;
		}

		conditionsIndex++;
	}
	if (numOfConditions > 0 && conditionsBuffer[conditionsIndex - 1].conditionType != LOOP_CT && conditions[numOfConditions - 1].exitIndex != -1)
	{
		conditionsBuffer[conditionsIndex].jccIndex = conditions[numOfConditions - 1].dstIndex;
		conditionsBuffer[conditionsIndex].dstIndex = conditions[numOfConditions - 1].exitIndex;
		conditionsBuffer[conditionsIndex].conditionType = ELSE_CT;

		conditionsIndex++;
	}

	return conditionsIndex;
}

int checkForCondition(int instructionIndex, struct Condition* conditions, int numOfConditions)
{
	for (int i = 0; i < numOfConditions; i++)
	{
		if (!conditions[i].isCombinedByOther && conditions[i].jccIndex == instructionIndex)
		{
			return i;
		}
	}

	return -1;
}

unsigned char decompileCondition(struct DecompilationParameters params, struct Condition* conditions, int conditionIndex, struct LineOfC* result)
{
	if (conditions[conditionIndex].conditionType == ELSE_CT)
	{
		strcpy(result->line, "else");
		return 1;
	}

	char conditionExpression[100] = { 0 };

	if (conditions[conditionIndex].otherJccsLogicType == OR_LT)
	{
		if (!decompileComparison(params, conditionExpression, 0))
		{
			return 0;
		}

		for (int i = 0; i < conditions[conditionIndex].numOfOtherJccs; i++)
		{
			char currentConditionExpression[100] = { 0 };
			params.startInstructionIndex = conditions[conditionIndex].otherJccIndexes[i];
			if (!decompileComparison(params, currentConditionExpression, i == conditions[conditionIndex].numOfOtherJccs - 1))
			{
				return 0;
			}

			strcat(conditionExpression, " || ");
			strcat(conditionExpression, currentConditionExpression);
		}
	}
	else
	{
		if (!decompileComparison(params, conditionExpression, 1)) // this needs to run if otherJccsLogicType is either AND_LT or NONE_LT. if it is NONE_LT, the loop wont run because numOfOtherJccs will be 0 
		{
			return 0;
		}

		for (int i = 0; i < conditions[conditionIndex].numOfOtherJccs; i++)
		{
			char currentConditionExpression[100] = { 0 };
			params.startInstructionIndex = conditions[conditionIndex].otherJccIndexes[i];
			if (!decompileComparison(params, currentConditionExpression, 1))
			{
				return 0;
			}

			strcat(conditionExpression, " && ");
			strcat(conditionExpression, currentConditionExpression);
		}
	}

	struct LineOfC combinedConditionExpression = { 0 };
	if (conditions[conditionIndex].combinedConditionIndex)
	{
		params.startInstructionIndex = conditions[conditions[conditionIndex].combinedConditionIndex].jccIndex;
		if (decompileCondition(params, conditions, conditions[conditionIndex].combinedConditionIndex, &combinedConditionExpression))
		{
			wrapStrInParentheses(conditionExpression);

			if (conditions[conditionIndex].combinationLogicType == AND_LT)
			{
				strcat(conditionExpression, " && ");
			}
			else
			{
				strcat(conditionExpression, " || ");
			}

			strcat(conditionExpression, combinedConditionExpression.line);
		}
		else
		{
			return 0;
		}
	}

	if (conditions[conditionIndex].isCombinedByOther)
	{
		strcpy(result->line, conditionExpression);
		return 1;
	}

	if (conditions[conditionIndex].conditionType == LOOP_CT)
	{
		// check for for loop
		if (params.currentFunc->instructions[conditions[conditionIndex].exitIndex - 1].opcode == JMP_SHORT)
		{
			struct LineOfC assignmentExpression = { 0 };
			for (int i = conditions[conditionIndex].exitIndex; i < conditions[conditionIndex].jccIndex; i++)
			{
				struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[i]);
				if (checkForAssignment(currentInstruction))
				{
					params.startInstructionIndex = i;
					if (decompileAssignment(params, &assignmentExpression))
					{
						break;
					}
					else
					{
						return 0;
					}
				}
			}
			int len = (int)(strlen(assignmentExpression.line));
			if (len > 0)
			{
				assignmentExpression.line[len - 1] = 0; // remove ;
			}

			sprintf(result->line, "for (; %s; %s)", conditionExpression, assignmentExpression.line);
		}
		else
		{
			sprintf(result->line, "while (%s)", conditionExpression);
		}
	}
	else if (conditions[conditionIndex].conditionType == IF_CT)
	{
		sprintf(result->line, "if (%s)", conditionExpression);
	}
	else
	{
		sprintf(result->line, "else if (%s)", conditionExpression);
	}

	return 1;
}
