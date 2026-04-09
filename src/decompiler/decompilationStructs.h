#pragma once
#include "../disassembler/disassembler.h"
#include "../fileStructs.h"
#include "dataTypes.h"

struct RegisterVariable
{
	struct VarType type;
	enum Register reg;
	struct JdcStr name;
};

struct StackVariable
{
	struct VarType type;
	int stackOffset;
	struct JdcStr name;
};

struct ReturnedVariable // variables that contain the reuturn value of another function call
{
	struct VarType type;
	unsigned long long calleeAddress;
	unsigned long long callInstructionAddress;
	enum Register returnReg;
	struct JdcStr name;
};

enum CallingConvention
{
	__CDECL,
	__STDCALL,
	__FASTCALL,
	__THISCALL
};

static const char* callingConventionStrs[] =
{
	"__cdecl",
	"__stdcall",
	"__fastcall",
	"__thiscall"
};

enum ConditionType
{
	IF_CT,
	ELSE_IF_CT,
	ELSE_CT,
	CONDITIONAL_GOTO_CT,
	CONDITIONAL_RETURN_CT,
	LOOP_CT, // for or while loop
	DO_WHILE_CT,
	SWITCH_CASE_CT
};

static const char* conditionTypeStrs[] =
{
	"IF_CT",
	"ELSE_IF_CT",
	"ELSE_CT",
	"CONDITIONAL_GOTO_CT",
	"CONDITIONAL_RETURN_CT",
	"LOOP_CT",
	"DO_WHILE_CT",
	"SWITCH_CASE_CT"
};

enum LogicalType
{
	NONE_LT,
	AND_LT,
	OR_LT
};

static const char* logicalTypeStrs[] =
{
	"NONE_LT",
	"AND_LT",
	"OR_LT"
};

struct Condition
{
	int jccIndex;
	int dstIndex; // the index of the instruction jumped to by the jcc
	int exitIndex; // if the instruction before dstIndex is a jmp, this is the index of the instruction jumped to by that jmp
	int startIndex;
	int endIndex;
	enum ConditionType conditionType;

	int* combinedJccIndexes; // these will be either all connected by && or ||
	int numOfCombinedJccs;
	enum LogicalType combinedJccsLogicType;

	int combinedConditionIndex; // this will be the index of the combined condition within the conditions buffer
	enum LogicalType combinationLogicType;
	unsigned char isCombinedByOther; // is this Condition referenced in another one by combinedConditionIndex

	struct DisassembledInstruction* cmpInstruction;
	unsigned char isFirstSwitchCase;

	int indentLevel; // used to check if the condition was entered at all, and to make sure the conditions are ended in the right order in the case where multiple end at the same address. the order only matters for conditions like do while, where the do and } while(); need to match
};

enum DirectJmpType
{
	NONE_DJT,
	GO_TO_DJT,
	BREAK_DJT,
	CONTINUE_DJT
};

static const char* directJmpTypeStrs[] =
{
	"NONE_DJT",
	"GO_TO_DJT",
	"BREAK_DJT",
	"CONTINUE_DJT"
};

struct DirectJmp
{
	int jmpIndex;
	int dstIndex;
	enum DirectJmpType type;
};

struct Function
{
	struct VarType returnType;
	enum Register returnReg;
	unsigned long long addressOfReturnFunction; // if the function's return value depends on another function, this will be the address of that function
	unsigned long long addressOfFirstFuncCall; // if the function has arguments that it only uses to pass to this function call
	int indexOfFirstFuncCall;

	enum CallingConvention callingConvention;

	struct JdcStr name;

	struct RegisterVariable* regArgs;
	unsigned char numOfRegArgs;
	struct StackVariable* stackArgs;
	unsigned char numOfStackArgs;

	struct StackVariable* stackVars;
	unsigned char numOfStackVars;
	struct ReturnedVariable* returnedVars;
	unsigned char numOfReturnedVars;
	struct RegisterVariable* regVars;
	unsigned char numOfRegVars;

	int firstInstructionIndex;
	int lastInstructionIndex;

	struct Condition* conditions;
	int numOfConditions;
	struct DirectJmp* directJmps;
	int numOfDirectJmps;

	unsigned char hasDoneInitialAnalysis;
};

struct DecompilationParameters
{
	struct Function* functions;
	int numOfFunctions;

	struct ImportedFunction* imports;
	int numOfImports;

	struct Function* currentFunc; // function being decompiled
	int startInstructionIndex; // index of instruction to start decompiling from relative to function
	unsigned char numOfIndents;

	struct DisassembledInstruction* instructions;
	int numOfInstructions;

	unsigned long long imageBase;
	struct FileSection* dataSections;
	int numOfDataSections;
	unsigned char* dataSectionByte;

	unsigned char is64Bit;
};
