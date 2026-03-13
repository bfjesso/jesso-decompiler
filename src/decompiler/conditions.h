#pragma once
#include "decompilationStructs.h"

enum ConditionType
{
	IF_CT,
	ELSE_IF_CT,
	ELSE_CT,
	LOOP_CT, // for or while loop
	DO_WHILE_CT
};

enum LogicalType
{
	NONE_LT,
	AND_LT,
	OR_LT
};

struct Condition
{
	int jccIndex;
	int dstIndex; // the index of the instruction jumped to by the jcc
	int exitIndex; // if the last instruction of the condition (the one before dstIndex) is a jmp, this is the index of the instruction jumped to by that jmp. this field is only used while getting all conditions
	enum ConditionType conditionType;
	unsigned char requiresJumpInDecomp; // when the if statement cuts into another if statement

	int otherJccIndexes[3]; // these will be either all connected by && or ||
	unsigned char numOfOtherJccs;
	enum LogicalType otherJccsLogicType;

	int combinedConditionIndex; // this will be the index of the combined condition within the conditions buffer
	enum LogicalType combinationLogicType; // OR or AND
	unsigned char isCombinedByOther; // is this Condition referenced in another one by combinedConditionIndex
};

int getAllConditions(struct DecompilationParameters params, struct Condition* conditions, int conditionsLength);

int checkForCondition(int instructionIndex, struct Condition* conditions, int numOfConditions);

unsigned char decompileCondition(struct DecompilationParameters params, struct Condition* conditions, int conditionIndex, struct JdcStr* result);
