#include "conditions.h"

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

			// getting index of instruction jumped to by jcc (minus one)
			unsigned long long jccDst = params.currentFunc->addresses[i] + instruction->operands[0].immediate;
			int jccDstIndex = findInstructionByAddress(params.currentFunc, 0, params.currentFunc->numOfInstructions - 1, jccDst);
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
				int jmpDstIndex = findInstructionByAddress(params.currentFunc, 0, params.currentFunc->numOfInstructions - 1, jmpDst);
				allJccs[jccCount].exitIndex = jmpDstIndex;
			}
			else
			{
				allJccs[jccCount].exitIndex = -1;
			}

			jccCount++;
		}
	}

	struct Condition condencedConditions[20] = { 0 };
	int numOfConditions = condenceConditions(allJccs, jccCount, condencedConditions);

	combineConditions(condencedConditions, numOfConditions);

	// set types
	int conditionsIndex = 0; // this has its own index and buffer because ELSEs might be added to the array
	for (int i = 0; i < numOfConditions; i++)
	{
		if (condencedConditions[i].isCombinedByOther) { continue; }

		// check for else if
		if (i > 0 && condencedConditions[i].exitIndex != -1 &&
			condencedConditions[i - 1].exitIndex == condencedConditions[i].exitIndex &&
			condencedConditions[i].dstIndex > condencedConditions[i - 1].dstIndex) // also have to check that its not nested
		{
			memcpy(&conditionsBuffer[conditionsIndex], &condencedConditions[i], sizeof(struct Condition));
			conditionsBuffer[conditionsIndex].conditionType = ELSE_IF_CT;
		}
		else
		{
			// check if last condition has an else and add it if so
			if (i > 0 && condencedConditions[i - 1].exitIndex != -1)
			{
				conditionsBuffer[conditionsIndex].jccIndex = condencedConditions[i - 1].dstIndex;
				conditionsBuffer[conditionsIndex].dstIndex = condencedConditions[i - 1].exitIndex;
				conditionsBuffer[conditionsIndex].conditionType = ELSE_CT;

				conditionsIndex++;
			}

			memcpy(&conditionsBuffer[conditionsIndex], &condencedConditions[i], sizeof(struct Condition));
			conditionsBuffer[conditionsIndex].conditionType = IF_CT;
		}

		conditionsIndex++;
	}
	// check if last condition has an else and add it if so
	if (numOfConditions > 0 && condencedConditions[numOfConditions - 1].exitIndex != -1)
	{
		conditionsBuffer[conditionsIndex].jccIndex = condencedConditions[numOfConditions - 1].dstIndex;
		conditionsBuffer[conditionsIndex].dstIndex = condencedConditions[numOfConditions - 1].exitIndex;
		conditionsBuffer[conditionsIndex].conditionType = ELSE_CT;

		conditionsIndex++;
	}

	return conditionsIndex;
}

int condenceConditions(struct Condition* conditions, int numOfConditions, struct Condition* newConditionsBuffer)
{
	int newConditionsIndex = 0;

	for (int i = 0; i < numOfConditions; i++)
	{
		memcpy(&newConditionsBuffer[newConditionsIndex], &conditions[i], sizeof(struct Condition));

		// check for ||. a group of ORs is a series of Jccs that all go to the same destination (like ANDs), but the last one in the series has a Jcc as the instruction right before its destination
		int orCount = 0;
		unsigned char reachedEnd = 0;
		for (int j = i + 1; j < numOfConditions; j++)
		{
			if (conditions[i].dstIndex == conditions[j].dstIndex)
			{
				newConditionsBuffer[newConditionsIndex].otherJccIndexes[orCount] = conditions[j].jccIndex; // this could be part of an AND, but orCount will be set to 0 if its not so this will be ignored or overwritten
				orCount++;
			}
			else if (conditions[j - 1].dstIndex - 1 == conditions[j].jccIndex)
			{
				newConditionsBuffer[newConditionsIndex].otherJccIndexes[orCount] = conditions[j].jccIndex;
				newConditionsBuffer[newConditionsIndex].dstIndex = conditions[j].dstIndex;
				newConditionsBuffer[newConditionsIndex].exitIndex = conditions[j].exitIndex;
				orCount++;

				newConditionsBuffer[newConditionsIndex].otherJccsLogicType = OR_LT;
				newConditionsBuffer[newConditionsIndex].numOfOtherJccs = orCount;
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
				if (conditions[i].dstIndex == conditions[j].dstIndex)
				{
					newConditionsBuffer[newConditionsIndex].otherJccsLogicType = AND_LT;
					newConditionsBuffer[newConditionsIndex].otherJccIndexes[andCount] = conditions[j].jccIndex;
					andCount++;
				}
				else
				{
					break;
				}
			}
			newConditionsBuffer[newConditionsIndex].numOfOtherJccs = andCount;
			i += andCount;
		}

		newConditionsIndex++;
	}

	return newConditionsIndex;
}

void combineConditions(struct Condition* conditions, int numOfConditions)
{
	for (int i = 0; i < numOfConditions - 1; i++)
	{
		if (conditions[i].dstIndex == conditions[i + 1].dstIndex)
		{
			conditions[i].dstIndex = conditions[i + 1].dstIndex;
			conditions[i].exitIndex = conditions[i + 1].exitIndex;
			conditions[i].combinedCondition = &conditions[i + 1];
			conditions[i].combinationLogicType = AND_LT;
			conditions[i + 1].isCombinedByOther = 1;
			i++;
		}
		else if (conditions[i].dstIndex - 1 == conditions[i + 1].jccIndex)
		{
			conditions[i].dstIndex = conditions[i + 1].dstIndex;
			conditions[i].exitIndex = conditions[i + 1].exitIndex;
			conditions[i].combinedCondition = &conditions[i + 1];
			conditions[i].combinationLogicType = OR_LT;
			conditions[i + 1].isCombinedByOther = 1;
			i++;
		}
	}
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