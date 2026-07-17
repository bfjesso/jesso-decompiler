#pragma once
#include "../disassembler/disassemblyUtils.h"
#include "../fileStructs.h"
#include "dataTypes.h"

struct RegisterVariable
{
	struct DataType dataType;
	enum Register reg;
	unsigned char isArgument;
	int scopeStartIndex; // instruction that initialzes the reg
	int scopeEndIndex;// instruction that overwrites the reg
	struct JdcStr name;
};

struct StackVariable
{
	struct DataType dataType;
	unsigned char isArgument;
	long long stackOffset;
	struct JdcStr name;
};

struct ReturnedVariable // variables that contain the reuturn value of another function call
{
	struct DataType dataType;
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
	LOOP_CT, // for or while loop
	DO_WHILE_CT
};

static const char* conditionTypeStrs[] =
{
	"IF_CT",
	"ELSE_IF_CT",
	"ELSE_CT",
	"CONDITIONAL_GOTO_CT",
	"CONDITIONAL_RETURN_CT",
	"LOOP_CT",
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
	int jccIndex;
	int dstIndex;
	int exitIndex; // if the instruction before dstIndex is a jmp, this is the index of the instruction jumped to by that jmp
	int firstBodyIndex; // first instruction in the body of the condition
	int lastBodyIndex;  // last instruction in the body of the condition
	enum ConditionType conditionType;

	int* combinedJccIndexes; // these will be either all connected by && or ||
	int numOfCombinedJccs;
	enum LogicalType combinedJccsLogicType;

	int connectedUpperConditionIndex; // this would be an if or else if condition
	int connectedLowerConditionIndex; // this would be an else if or else condition

	int indentLevel; // used to check if the condition was entered at all, and to make sure the conditions are ended in the right order in the case where multiple end at the same address. the order only matters for conditions like do while, where the do and } while(); need to match
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
	int jmpIndex;
	int dstIndex;
	enum DirectJmpType type;
};

struct AssociatedInstructions
{
	int* indexes;
	int numOfIndexes;
};

struct Function
{
	struct DataType returnType;
	enum Register returnReg;

	enum CallingConvention callingConvention;

	struct JdcStr name;

	struct RegisterVariable* regVars;
	unsigned char numOfRegVars;

	struct StackVariable* stackVars;
	unsigned char numOfStackVars;

	struct ReturnedVariable* returnedVars;
	unsigned char numOfReturnedVars;
	

	int firstInstructionIndex;
	int lastInstructionIndex;

	struct Condition* conditions;
	int numOfConditions;
	struct DirectJmp* directJmps;
	int numOfDirectJmps;

	struct AssociatedInstructions* associatedInstructions; // these are a list of instructions indexes that correspond to each line of the decompilation
	int numOfLines;
	int associatedInstructionsBufferLen;

	unsigned char hasDoneInitialAnalysis;
};

struct DecompilationParameters
{
	struct Function* functions;
	int numOfFunctions;

	struct ImportedFunction* imports;
	int numOfImports;

	struct Function* currentFunc; // function being decompiled
	unsigned char numOfIndents;

	struct DisassembledInstruction* instructions;
	int numOfInstructions;

	unsigned long long imageBase;
	struct FileSection* sections;
	int numOfSections;

	unsigned char* fileBytes;
	unsigned long long numOfFileBytes;

	unsigned char is64Bit;
};
