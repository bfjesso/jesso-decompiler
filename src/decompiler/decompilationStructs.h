#pragma once
#include "../jdcTypes.h"
#include "../fileStructs.h"
#include "dataTypes.h"

struct RegVarScope
{
	int32_t startIndex; // instruction that initialzes the reg
	int32_t endIndex; // last instruction that accesses the reg before it is initialized again
};

struct RegisterVariable
{
	struct DataType dataType;
	enum Register reg;
	bool isArgument;
	uint16_t numOfScopes;
	struct RegVarScope* scopes;
	struct JdcStr name;
};

struct StackVariable
{
	struct DataType dataType;
	bool isArgument;
	int64_t offsetFromInitSP; // this is the offset from the initial value of the stack pointer, which may not be the same as the base pointer
	struct JdcStr name;
};

struct ReturnedVariable // variables that contain the reuturn value of another function call
{
	struct DataType dataType;
	uint64_t calleeAddress;
	uint64_t callInstructionAddress;
	enum Register returnReg;
	struct JdcStr name;
};

enum CallingConvention
{
	__CDECL,
	__STDCALL,
	__FASTCALL,
	__THISCALL,
	__UNKNOWNCALL
};
static const char* callingConventionStrs[] =
{
	"__cdecl",
	"__stdcall",
	"__fastcall",
	"__thiscall",
	"__unknowncall"
};
#define NUM_OF_CALLING_CONVENTIONS 5

enum ConditionType
{
	IF_CT,
	ELSE_IF_CT,
	ELSE_CT,
	CONDITIONAL_GOTO_CT,
	CONDITIONAL_RETURN_CT,
	WHILE_CT,
	DO_WHILE_CT
};

static const char* conditionTypeStrs[] =
{
	"IF_CT",
	"ELSE_IF_CT",
	"ELSE_CT",
	"CONDITIONAL_GOTO_CT",
	"CONDITIONAL_RETURN_CT",
	"WHILE_CT",
	"DO_WHILE_CT"
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
	int32_t jccIndex;
	int32_t dstIndex;
	int32_t exitIndex; // if the instruction before dstIndex is a jmp, this is the index of the instruction jumped to by that jmp
	int32_t firstBodyIndex; // first instruction in the body of the condition
	int32_t lastBodyIndex;  // last instruction in the body of the condition
	enum ConditionType conditionType;

	int32_t* combinedJccIndexes; // these will be either all connected by && or ||
	int32_t numOfCombinedJccs;
	enum LogicalType combinedJccsLogicType;

	int32_t connectedUpperConditionIndex; // this would be an if or else if condition
	int32_t connectedLowerConditionIndex; // this would be an else if or else condition

	int32_t indentLevel; // used to check if the condition was entered at all, and to make sure the conditions are ended in the right order in the case where multiple end at the same address. the order only matters for conditions like do while, where the do and } while(); need to match
};

enum DirectJmpType
{
	NONE_DJT,
	GO_TO_DJT,
	BREAK_DJT,
	CONTINUE_DJT,
	JUMP_TO_DJT
};

static const char* directJmpTypeStrs[] =
{
	"NONE_DJT",
	"GO_TO_DJT",
	"BREAK_DJT",
	"CONTINUE_DJT",
	"JUMP_TO_DJT"
};

struct DirectJmp
{
	int32_t jmpIndex;
	int32_t dstIndex;
	enum DirectJmpType type;
};

struct AssociatedInstructions
{
	int32_t* indexes;
	int32_t numOfIndexes;
};

struct Function
{
	struct DataType returnType;
	enum Register returnReg;

	enum CallingConvention callingConvention;

	struct JdcStr name;

	struct StackVariable* stackVars;
	struct ReturnedVariable* returnedVars;
	struct RegisterVariable* regVars;
	uint16_t numOfStackVars;
	uint16_t numOfReturnedVars;
	uint16_t numOfRegVars;

	bool hasDoneInitialAnalysis;

	int32_t firstInstructionIndex;
	int32_t lastInstructionIndex;

	struct Condition* conditions;
	struct DirectJmp* directJmps;
	int32_t numOfConditions;
	int32_t numOfDirectJmps;

	struct AssociatedInstructions* associatedInstructions; // these are a list of instructions indexes that correspond to each line of the decompilation
	int32_t numOfLines;
	int32_t associatedInstructionsBufferLen;
};

struct DecompilationParameters
{
	struct Function* functions;
	struct ImportedFunction* imports;
	int32_t numOfFunctions;
	int32_t numOfImports;

	struct Function* currentFunc; // function being decompiled

	struct DisassembledInstruction* instructions;
	struct FileSection* sections;
	int32_t numOfInstructions;
	int32_t numOfSections;

	uint64_t imageBase;
	
	uint8_t* fileBytes;
	uint64_t numOfFileBytes;

	uint8_t numOfIndents;
	bool is64Bit;
	enum FileFormat fileFormat;
};
