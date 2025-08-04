#pragma once
#include "decompiler.h"

enum ConditionType
{
	IF_CT,
	ELSE_IF_CT,
	ELSE_CT,
	LOOP_CT
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
	unsigned char conditionType;

	int otherJccIndexes[3]; // these will be either all connected by && or ||
	unsigned char numOfOtherJccs;
	unsigned char otherJccsLogicType;

	int combinedConditionIndex; // this will be the index of the combined condition within the conditions buffer
	unsigned char combinationLogicType; // OR or AND
	unsigned char isCombinedByOther; // is this Condition referenced in another one by combinedConditionIndex
};

int getAllConditions(struct DecompilationParameters params, struct Condition* conditionsBuffer);

// this function sets the otherJccIndexes by finding series of &&s and ||s
static int getAndsAndOrs(struct Condition* allJccs, int numOfConditions, struct Condition* newConditionsBuffer);

// this will go through the conditions and combine them into single conditions if applicable
static void combineConditions(struct Condition* conditions, int numOfConditions);

// set types as if or else if. This function will also find and add elses to the conditions buffer
static int setConditionTypes(struct Condition* conditions, int numOfConditions, struct Condition* conditionsBuffer);

int checkForCondition(int instructionIndex, struct Condition* conditions, int numOfConditions);