#include "conditions.h"
#include "decompilationUtils.h"
#include "returnStatements.h"
#include "expressions.h"
#include "assignment.h"

int getAllConditions(struct DecompilationParameters params, int conditionsBufferSize)
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

			if (numOfConditions > 0 && dstIndex == params.currentFunc->conditions[numOfConditions - 1].dstIndex && !stopCombination)
			{
				params.currentFunc->conditions[numOfConditions - 1].combinedJccIndexes[combinationCount] = i;
				params.currentFunc->conditions[numOfConditions - 1].combinedJccsLogicType = AND_LT;
				combinationCount++;

				params.currentFunc->conditions[numOfConditions - 1].numOfCombinedJccs = combinationCount;
			}
			else if (numOfConditions > 0 && lastDstIndex - 1 == i && !stopCombination)
			{
				params.currentFunc->conditions[numOfConditions - 1].combinedJccIndexes[combinationCount] = i;
				params.currentFunc->conditions[numOfConditions - 1].combinedJccsLogicType = OR_LT;
				params.currentFunc->conditions[numOfConditions - 1].dstIndex = dstIndex;
				params.currentFunc->conditions[numOfConditions - 1].exitIndex = exitIndex;
				combinationCount++;

				params.currentFunc->conditions[numOfConditions - 1].numOfCombinedJccs = combinationCount;
			}
			else
			{
				if (numOfConditions >= conditionsBufferSize)
				{
					conditionsBufferSize += 5;
					struct Condition* newConditions = (struct Condition*)realloc(params.currentFunc->conditions, conditionsBufferSize * sizeof(struct Condition));
					if (newConditions) 
					{
						params.currentFunc->conditions = newConditions;
						memset(params.currentFunc->conditions + conditionsBufferSize - 5, 0, sizeof(struct Condition) * 5);
					}
					else 
					{
						return -1;
					}
				}
				
				// setting the type
				if (exitIndex != -1 && exitIndex < i)
				{
					params.currentFunc->conditions[numOfConditions].conditionType = LOOP_CT;
				}
				else if (dstIndex < i)
				{
					params.currentFunc->conditions[numOfConditions].conditionType = DO_WHILE_CT;
				}
				else if (numOfConditions > 0 && exitIndex != -1 &&
					params.currentFunc->conditions[numOfConditions - 1].exitIndex == exitIndex && // check for else if
					dstIndex > params.currentFunc->conditions[numOfConditions - 1].dstIndex) // also have to check that its not nested
				{
					params.currentFunc->conditions[numOfConditions].conditionType = ELSE_IF_CT;
				}
				else
				{
					// checking if last condition has an else and add it if so
					if (numOfConditions > 0 && params.currentFunc->conditions[numOfConditions - 1].conditionType != LOOP_CT && params.currentFunc->conditions[numOfConditions - 1].exitIndex != -1)
					{
						if (!checkForReturnStatement(params.currentFunc->conditions[numOfConditions - 1].dstIndex - 1, params.currentFunc->instructions, params.currentFunc->numOfInstructions)) // if the jmp functions as a return, it doesnt need to be handled as an ELSE
						{
							params.currentFunc->conditions[numOfConditions].jccIndex = params.currentFunc->conditions[numOfConditions - 1].dstIndex;
							params.currentFunc->conditions[numOfConditions].dstIndex = params.currentFunc->conditions[numOfConditions - 1].exitIndex;
							params.currentFunc->conditions[numOfConditions].conditionType = ELSE_CT;
							numOfConditions++;
						}
					}

					params.currentFunc->conditions[numOfConditions].conditionType = IF_CT;
				}

				params.currentFunc->conditions[numOfConditions].jccIndex = i;
				params.currentFunc->conditions[numOfConditions].dstIndex = dstIndex;
				params.currentFunc->conditions[numOfConditions].exitIndex = exitIndex;

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
	if (numOfConditions > 0 && params.currentFunc->conditions[numOfConditions - 1].conditionType != LOOP_CT && params.currentFunc->conditions[numOfConditions - 1].exitIndex != -1)
	{
		if (!checkForReturnStatement(params.currentFunc->conditions[numOfConditions - 1].dstIndex - 1, params.currentFunc->instructions, params.currentFunc->numOfInstructions))
		{
			params.currentFunc->conditions[numOfConditions].jccIndex = params.currentFunc->conditions[numOfConditions - 1].dstIndex;
			params.currentFunc->conditions[numOfConditions].dstIndex = params.currentFunc->conditions[numOfConditions - 1].exitIndex;
			params.currentFunc->conditions[numOfConditions].conditionType = ELSE_CT;

			numOfConditions++;
			if (numOfConditions >= conditionsBufferSize)
			{
				conditionsBufferSize += 5;
				struct Condition* newConditions = (struct Condition*)realloc(params.currentFunc->conditions, conditionsBufferSize * sizeof(struct Condition));
				if (newConditions)
				{
					params.currentFunc->conditions = newConditions;
					memset(params.currentFunc->conditions + conditionsBufferSize - 5, 0, sizeof(struct Condition) * 5);
				}
				else
				{
					return -1;
				}
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
		if (params.currentFunc->conditions[i].conditionType == IF_CT)
		{
			for (int j = i + 1; j < numOfConditions; j++)
			{
				if (params.currentFunc->conditions[j].conditionType == IF_CT)
				{
					if (params.currentFunc->conditions[i].dstIndex > params.currentFunc->conditions[j].jccIndex && params.currentFunc->conditions[i].dstIndex < params.currentFunc->conditions[j].dstIndex)
					{
						params.currentFunc->conditions[i].requiresJumpInDecomp = 1;
						break;
					}
				}
			}
		}

		// checking for ELSEs with same dstIndex
		if (params.currentFunc->conditions[i].conditionType == ELSE_CT)
		{
			for (int j = 0; j < numOfConditions; j++)
			{
				if (j == i)
				{
					continue;
				}

				if (params.currentFunc->conditions[j].conditionType == ELSE_CT)
				{
					if (params.currentFunc->conditions[i].dstIndex == params.currentFunc->conditions[j].dstIndex && params.currentFunc->conditions[i].jccIndex < params.currentFunc->conditions[j].jccIndex)
					{
						params.currentFunc->conditions[i].dstIndex = params.currentFunc->conditions[j].jccIndex;
						break;
					}
				}
			}
		}
	}

	return numOfConditions;
}

unsigned char decompileCondition(struct DecompilationParameters params, int conditionIndex, struct JdcStr* result)
{
	if (params.currentFunc->conditions[conditionIndex].conditionType == ELSE_CT)
	{
		return strcatJdc(result, "else");
	}

	unsigned char invertCondition = params.currentFunc->conditions[conditionIndex].requiresJumpInDecomp || params.currentFunc->conditions[conditionIndex].conditionType == DO_WHILE_CT;

	struct JdcStr conditionExpression = initializeJdcStr();
	if (params.currentFunc->conditions[conditionIndex].combinedJccsLogicType == OR_LT)
	{
		if (!decompileComparison(params, invertCondition, &conditionExpression))
		{
			freeJdcStr(&conditionExpression);
			return 0;
		}

		for (int i = 0; i < params.currentFunc->conditions[conditionIndex].numOfCombinedJccs; i++)
		{
			unsigned char invertOperator = i == (params.currentFunc->conditions[conditionIndex].numOfCombinedJccs - 1);
			if (invertCondition)
			{
				invertOperator = !invertOperator;
			}

			struct JdcStr currentConditionExpression = initializeJdcStr();
			params.startInstructionIndex = params.currentFunc->conditions[conditionIndex].combinedJccIndexes[i];
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

		for (int i = 0; i < params.currentFunc->conditions[conditionIndex].numOfCombinedJccs; i++)
		{
			struct JdcStr currentConditionExpression = initializeJdcStr();
			params.startInstructionIndex = params.currentFunc->conditions[conditionIndex].combinedJccIndexes[i];
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
	if (params.currentFunc->conditions[conditionIndex].combinedConditionIndex)
	{
		params.startInstructionIndex = params.currentFunc->conditions[params.currentFunc->conditions[conditionIndex].combinedConditionIndex].jccIndex;
		if (decompileCondition(params, params.currentFunc->conditions[conditionIndex].combinedConditionIndex, &combinedConditionExpression))
		{
			if (!wrapJdcStrInParentheses(&conditionExpression)) 
			{
				freeJdcStr(&conditionExpression);
				freeJdcStr(&combinedConditionExpression);
				return 0;
			}

			if (params.currentFunc->conditions[conditionIndex].combinationLogicType == AND_LT)
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

	if (params.currentFunc->conditions[conditionIndex].isCombinedByOther)
	{
		strcatJdc(result, conditionExpression.buffer);
		freeJdcStr(&conditionExpression);
		return 1;
	}

	if (params.currentFunc->conditions[conditionIndex].conditionType == LOOP_CT)
	{
		// check for for loop
		if (params.currentFunc->instructions[params.currentFunc->conditions[conditionIndex].exitIndex - 1].opcode == JMP_SHORT)
		{
			struct JdcStr assignmentExpression = initializeJdcStr();
			for (int i = params.currentFunc->conditions[conditionIndex].exitIndex; i < params.currentFunc->conditions[conditionIndex].jccIndex; i++)
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
	else if (params.currentFunc->conditions[conditionIndex].conditionType == DO_WHILE_CT)
	{
		sprintfJdc(result, 1, "} while (%s);", conditionExpression.buffer);
	}
	else if (params.currentFunc->conditions[conditionIndex].conditionType == IF_CT)
	{
		sprintfJdc(result, 1, "if (%s)", conditionExpression.buffer);
	}
	else
	{
		sprintfJdc(result, 1, "else if (%s)", conditionExpression.buffer);
	}

	return freeJdcStr(&conditionExpression);
}

int checkForConditionStart(struct DecompilationParameters params)
{
	for (int i = 0; i < params.currentFunc->numOfConditions; i++)
	{
		if (params.currentFunc->conditions[i].conditionType == DO_WHILE_CT && params.currentFunc->conditions[i].dstIndex == params.startInstructionIndex)
		{
			return i;
		}
		else if (!params.currentFunc->conditions[i].isCombinedByOther && params.currentFunc->conditions[i].jccIndex == params.startInstructionIndex)
		{
			return i;
		}
	}

	return -1;
}