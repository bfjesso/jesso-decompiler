#pragma once
#include "../disassembler/registers.h"
#include "../disassembler/disassembler.h"
#include "../fileStructs.h"
#include "dataTypes.h"

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

struct Function
{
	struct DisassembledInstruction* instructions;
	unsigned short numOfInstructions;

	struct JdcStr name;

	enum PrimitiveType returnType;
	unsigned long long addressOfReturnFunction; // if the function's return value depends on another function, this will be the address of that function

	enum CallingConvention callingConvention;

	int stackFrameSize;

	struct RegisterVariable regArgs[ST0 - RAX];
	unsigned char numOfRegArgs;

	struct StackVariable stackArgs[6];
	unsigned char numOfStackArgs;

	unsigned char hasGottenLocalVars; // returnedVars count for this too
	struct StackVariable localVars[100];
	unsigned char numOfLocalVars;
	struct ReturnedVariable returnedVars[100];
	unsigned char numOfReturnedVars;
};

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned char findNextFunction(struct DisassembledInstruction* instructions, int startInstructionIndex, int numOfInstructions, unsigned long long nextSectionStartAddress, struct Function* result, int* instructionIndex, unsigned char is64Bit);
	
	unsigned char fixAllFunctionReturnTypes(struct Function* functions, unsigned short numOfFunctions, unsigned char is64Bit);

#ifdef __cplusplus
}
#endif

int findFunctionByAddress(struct Function* functions, int low, int high, unsigned long long address);

int findInstructionByAddress(struct DisassembledInstruction* instructions, int low, int high, unsigned long long address);

unsigned long long resolveJmpChain(struct DisassembledInstruction* instructions, int numOfInstructions, int startInstructionIndex);

struct StackVariable* getLocalVarByOffset(struct Function* function, int stackOffset);

struct StackVariable* getStackArgByOffset(struct Function* function, int stackOffset);

struct RegisterVariable* getRegArgByReg(struct Function* function, enum Register reg);

struct FuncReturnVariable* findReturnedVar(struct Function* function, char callNum, unsigned long long callAddr);

enum PrimitiveType getTypeOfOperand(enum Mnemonic opcode, struct Operand* operand, unsigned char is64Bit);

unsigned char operandToValue(struct DisassembledInstruction* instructions, int startInstructionIndex, struct Operand* operand, unsigned long long* result);

static void sortFunctionArguments(struct Function* function);
