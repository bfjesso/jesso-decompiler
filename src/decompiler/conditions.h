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
	int exitIndex; // if the instruction before dstIndex is a jmp, this is the index of the instruction jumped to by that jmp
	enum ConditionType conditionType;
	unsigned char requiresJumpInDecomp; // when the if statement cuts into another if statement

	int combinedJccIndexes[3]; // these will be either all connected by && or ||
	unsigned char numOfCombinedJccs;
	enum LogicalType combinedJccsLogicType;

	int combinedConditionIndex; // this will be the index of the combined condition within the conditions buffer
	enum LogicalType combinationLogicType; // OR or AND
	unsigned char isCombinedByOther; // is this Condition referenced in another one by combinedConditionIndex

	unsigned char hasEnteredCondition; // this is checked when looking for ends of conditions. it is possible that a condition will never be entered if there is a direct jmp that skips it
};

int getAllConditions(struct DecompilationParameters params, struct Condition** conditionsRef, int conditionsBufferSize);

int checkForCondition(int instructionIndex, struct Condition* conditions, int numOfConditions);

unsigned char decompileCondition(struct DecompilationParameters params, struct Condition* conditions, int conditionIndex, struct JdcStr* result);