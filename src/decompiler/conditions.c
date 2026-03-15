#include "conditions.h"
#include "decompilationUtils.h"
#include "returnStatements.h"
#include "expressions.h"
#include "assignment.h"

int getAllConditions(struct DecompilationParameters params, struct Condition* conditions, int conditionsLength)
{
	int numOfConditions = 0;
	int combinationCount = 0;
	int lastDstIndex = -1;
	unsigned char stopCombination = 0;
	for (int i = 0; i < params.currentFunc->numOfInstructions; i++)
	{
		struct DisassembledInstruction* instruction = &(params.currentFunc->instructions[i]);

		if (isOpcodeJcc(instruction->opcode))
		{
			// getting index of instruction jumped to by jcc
			unsigned long long jccDst = params.currentFunc->instructions[i].address + instruction->operands[0].immediate.value;
			int dstIndex = findInstructionByAddress(params.currentFunc->instructions, 0, params.currentFunc->numOfInstructions - 1, jccDst);

			// if the conditions ends with a jmp, this will get the index of the instruction jumped to by that jmp
			int exitIndex = -1;
			if (params.currentFunc->instructions[dstIndex - 1].opcode == JMP_SHORT)
			{
				// sometimes compiler puts multiple jmps next to each other at the end?
				int firstJmpIndex = dstIndex - 1;
				for (int j = dstIndex - 2; j > i; j--)
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

				unsigned long long jmpDst = params.currentFunc->instructions[firstJmpIndex].address + params.currentFunc->instructions[firstJmpIndex].operands[0].immediate.value;
				exitIndex = findInstructionByAddress(params.currentFunc->instructions, 0, params.currentFunc->numOfInstructions - 1, jmpDst);
			}

			// a series of Jcc instructions that have the same destination are combined with a logical AND
			// if the series ends with a Jcc that does not have the same destination, then if the instruction immediatly before the destination of the previous Jcc is the this Jcc, they are all combined with a logical OR

			if (numOfConditions > 0 && dstIndex == conditions[numOfConditions - 1].dstIndex && !stopCombination)
			{
				conditions[numOfConditions - 1].combinedJccIndexes[combinationCount] = i;
				conditions[numOfConditions - 1].combinedJccsLogicType = AND_LT;
				combinationCount++;

				conditions[numOfConditions - 1].numOfCombinedJccs = combinationCount;
			}
			else if (numOfConditions > 0 && lastDstIndex - 1 == i && !stopCombination)
			{
				conditions[numOfConditions - 1].combinedJccIndexes[combinationCount] = i;
				conditions[numOfConditions - 1].combinedJccsLogicType = OR_LT;
				conditions[numOfConditions - 1].dstIndex = dstIndex;
				conditions[numOfConditions - 1].exitIndex = exitIndex;
				combinationCount++;

				conditions[numOfConditions - 1].numOfCombinedJccs = combinationCount;
			}
			else
			{
				if (numOfConditions >= conditionsLength)
				{
					return 0;
				}
				
				// setting the type
				if (exitIndex != -1 && exitIndex < i)
				{
					conditions[numOfConditions].conditionType = LOOP_CT;
				}
				else if (dstIndex < i)
				{
					conditions[numOfConditions].conditionType = DO_WHILE_CT;
				}
				else if (numOfConditions > 0 && exitIndex != -1 &&
					conditions[numOfConditions - 1].exitIndex == exitIndex && // check for else if
					dstIndex > conditions[numOfConditions - 1].dstIndex) // also have to check that its not nested
				{
					conditions[numOfConditions].conditionType = ELSE_IF_CT;
				}
				else
				{
					// checking if last condition has an else and add it if so
					if (numOfConditions > 0 && conditions[numOfConditions - 1].conditionType != LOOP_CT && conditions[numOfConditions - 1].exitIndex != -1)
					{
						params.startInstructionIndex = conditions[numOfConditions - 1].dstIndex - 1;
						if (!checkForReturnStatement(params)) // if the jmp functions as a return, it doesnt need to be handled as an ELSE
						{
							conditions[numOfConditions].jccIndex = conditions[numOfConditions - 1].dstIndex;
							conditions[numOfConditions].dstIndex = conditions[numOfConditions - 1].exitIndex;
							conditions[numOfConditions].conditionType = ELSE_CT;
							numOfConditions++;
						}
					}

					conditions[numOfConditions].conditionType = IF_CT;
				}

				conditions[numOfConditions].jccIndex = i;
				conditions[numOfConditions].dstIndex = dstIndex;
				conditions[numOfConditions].exitIndex = exitIndex;

				combinationCount = 0;
				stopCombination = 0;
				numOfConditions++;
			}

			lastDstIndex = dstIndex;
		}
		else if(numOfConditions > 0 && !isOpcodeCmp(instruction->opcode) && instruction->opcode != TEST) // the Jccs cant be combined into one condition if there is other code that runs between them.
		{
			stopCombination = 1;
		}
	}
	if (numOfConditions > 0 && conditions[numOfConditions - 1].conditionType != LOOP_CT && conditions[numOfConditions - 1].exitIndex != -1)
	{
		params.startInstructionIndex = conditions[numOfConditions - 1].dstIndex - 1;
		if (!checkForReturnStatement(params)) 
		{
			conditions[numOfConditions].jccIndex = conditions[numOfConditions - 1].dstIndex;
			conditions[numOfConditions].dstIndex = conditions[numOfConditions - 1].exitIndex;
			conditions[numOfConditions].conditionType = ELSE_CT;

			numOfConditions++;
			if (numOfConditions >= conditionsLength)
			{
				return 0;
			}
		}
	}

	//// combining conditions
	//for (int i = 0; i < numOfConditions - 2; i++)
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

	for (int i = 0; i < numOfConditions; i++)
	{
		// checking for an if statement ends before another if statment that it encolses
		if (conditions[i].conditionType == IF_CT)
		{
			for (int j = i + 1; j < numOfConditions; j++)
			{
				if (conditions[j].conditionType == IF_CT)
				{
					if (conditions[i].dstIndex > conditions[j].jccIndex && conditions[i].dstIndex < conditions[j].dstIndex)
					{
						conditions[i].requiresJumpInDecomp = 1;
						break;
					}
				}
			}
		}

		// checking for ELSEs with same dstIndex
		if (conditions[i].conditionType == ELSE_CT)
		{
			for (int j = 0; j < numOfConditions; j++)
			{
				if (j == i)
				{
					continue;
				}

				if (conditions[j].conditionType == ELSE_CT)
				{
					if (conditions[i].dstIndex == conditions[j].dstIndex && conditions[i].jccIndex < conditions[j].jccIndex)
					{
						conditions[i].dstIndex = conditions[j].jccIndex;
						break;
					}
				}
			}
		}
	}

	return numOfConditions;
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

unsigned char decompileCondition(struct DecompilationParameters params, struct Condition* conditions, int conditionIndex, struct JdcStr* result)
{
	if (conditions[conditionIndex].conditionType == ELSE_CT)
	{
		return strcatJdc(result, "else");
	}

	unsigned char invertCondition = conditions[conditionIndex].requiresJumpInDecomp || conditions[conditionIndex].conditionType == DO_WHILE_CT;

	struct JdcStr conditionExpression = initializeJdcStr();
	if (conditions[conditionIndex].combinedJccsLogicType == OR_LT)
	{
		if (!decompileComparison(params, invertCondition, &conditionExpression))
		{
			freeJdcStr(&conditionExpression);
			return 0;
		}

		for (int i = 0; i < conditions[conditionIndex].numOfCombinedJccs; i++)
		{
			unsigned char invertOperator = i == (conditions[conditionIndex].numOfCombinedJccs - 1);
			if (invertCondition)
			{
				invertOperator = !invertOperator;
			}

			struct JdcStr currentConditionExpression = initializeJdcStr();
			params.startInstructionIndex = conditions[conditionIndex].combinedJccIndexes[i];
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

		for (int i = 0; i < conditions[conditionIndex].numOfCombinedJccs; i++)
		{
			struct JdcStr currentConditionExpression = initializeJdcStr();
			params.startInstructionIndex = conditions[conditionIndex].combinedJccIndexes[i];
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
	if (conditions[conditionIndex].combinedConditionIndex)
	{
		params.startInstructionIndex = conditions[conditions[conditionIndex].combinedConditionIndex].jccIndex;
		if (decompileCondition(params, conditions, conditions[conditionIndex].combinedConditionIndex, &combinedConditionExpression))
		{
			if (!wrapJdcStrInParentheses(&conditionExpression)) 
			{
				freeJdcStr(&conditionExpression);
				freeJdcStr(&combinedConditionExpression);
				return 0;
			}

			if (conditions[conditionIndex].combinationLogicType == AND_LT)
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

	if (conditions[conditionIndex].isCombinedByOther)
	{
		strcatJdc(result, conditionExpression.buffer);
		freeJdcStr(&conditionExpression);
		return 1;
	}

	if (conditions[conditionIndex].conditionType == LOOP_CT)
	{
		// check for for loop
		if (params.currentFunc->instructions[conditions[conditionIndex].exitIndex - 1].opcode == JMP_SHORT)
		{
			struct JdcStr assignmentExpression = initializeJdcStr();
			for (int i = conditions[conditionIndex].exitIndex; i < conditions[conditionIndex].jccIndex; i++)
			{
				params.startInstructionIndex = i;
				if (checkForAssignment(params))
				{
					if (decompileAssignment(params, &assignmentExpression))
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
			
			sprintfJdc(result, 1, "for (; %s; %s)", conditionExpression.buffer, assignmentExpression.buffer);
			freeJdcStr(&assignmentExpression);
		}
		else
		{
			sprintfJdc(result, 1, "while (%s)", conditionExpression.buffer);
		}
	}
	else if (conditions[conditionIndex].conditionType == DO_WHILE_CT) 
	{
		sprintfJdc(result, 1, "} while (%s);", conditionExpression.buffer);
	}
	else if (conditions[conditionIndex].conditionType == IF_CT)
	{
		sprintfJdc(result, 1, "if (%s)", conditionExpression.buffer);
	}
	else
	{
		sprintfJdc(result, 1, "else if (%s)", conditionExpression.buffer);
	}

	return freeJdcStr(&conditionExpression);
}
