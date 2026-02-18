#pragma once
#include "../disassembler/disassembler.h"
#include "../fileStructs.h"
#include "dataTypes.h"

struct RegisterVariable
{
	enum PrimitiveType type;
	enum Register reg;
	struct JdcStr name;
};

struct StackVariable
{
	enum PrimitiveType type;
	int stackOffset;
	struct JdcStr name;
};

struct ReturnedVariable // variables that contain the reuturn value of another function call
{
	enum PrimitiveType type;
	char callNum;
	unsigned long long callAddr;
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

struct Function
{
	enum PrimitiveType returnType;
	unsigned long long addressOfReturnFunction; // if the function's return value depends on another function, this will be the address of that function
	unsigned long long addressOfFirstFuncCall; // if the function has arguments that it only uses to pass to this function call
	int indexOfFirstFuncCall;

	enum CallingConvention callingConvention;

	struct JdcStr name;

	struct RegisterVariable* regArgs;
	unsigned char numOfRegArgs;
	struct StackVariable* stackArgs;
	unsigned char numOfStackArgs;

	unsigned char hasGottenLocalVars;

	struct StackVariable* localVars;
	unsigned char numOfLocalVars;
	struct ReturnedVariable* returnedVars;
	unsigned char numOfReturnedVars;
	struct RegisterVariable* regVars;
	unsigned char numOfRegVars;

	struct DisassembledInstruction* instructions;
	unsigned short numOfInstructions;
};

struct DecompilationParameters
{
	struct Function* functions;
	int numOfFunctions;

	struct ImportedFunction* imports;
	int numOfImports;

	struct Function* currentFunc; // function being decompiled
	int startInstructionIndex; // index of instruction to start decompiling from relative to function
	
	// these are used when a condition has requiresJumpInDecomp
	int skipUpperBound;
	int skipLowerBound;

	int stackFrameSize;

	struct DisassembledInstruction* allInstructions;
	int totalNumOfInstructions;

	unsigned long long imageBase;
	struct FileSection* dataSections;
	int numOfDataSections;
	unsigned char* dataSectionByte;

	unsigned char is64Bit;
};
