#pragma once
#include "decompiler.h"

enum ConditionType
{
	IF_CT,
	ELSE_IF_CT,
	ELSE_CT
};

enum LogicalType
{
	NONE_LT,
	AND_LT,
	OR_LT
};

struct Condition // a Condition struct will be a series of conditions all connected by either &&s or ||s. if combinedCondition is not null
{
	int jccIndex;
	int dstIndex; // the index of the instruction jumped to by the jcc
	int exitIndex; // if the last instruction of the condition (the one before dstIndex) is a jmp, this is the index of the instruction jumped to by that jmp
	unsigned char conditionType;

	int otherJccIndexes[3];
	unsigned char numOfOtherJccs;
	unsigned char otherJccsLogicType;

	struct Condition* combinedCondition;
	unsigned char combinationLogicType;
	unsigned char isCombinedByOther; // is this struct referenced in another one by combinedCondition
};

int getAllConditions(struct DecompilationParameters params, struct Condition* conditionsBuffer);

// this will combine a list of all Jccs into groups of ORs or ANDs
int condenceConditions(struct Condition* conditions, int numOfConditions, struct Condition* newConditionsBuffer);

// this will go through the conditions and them into single complex conditions if applicable
void combineConditions(struct Condition* conditions, int numOfConditions);

int checkForCondition(int instructionIndex, struct Condition* conditions, int numOfConditions);