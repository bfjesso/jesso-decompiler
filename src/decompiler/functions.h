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
	char name[50];
};

struct StackVariable 
{
	enum PrimitiveType type;
	int stackOffset;
	char name[50];
};

struct FuncReturnVariable // variables that contain the reuturn value of another function call
{
	enum PrimitiveType type;
	char callNum;
	unsigned long long callAddr;
	char name[50];
};

struct Function
{
	unsigned long long* addresses;
	struct DisassembledInstruction* instructions;
	unsigned short numOfInstructions;

	char name[50];

	enum PrimitiveType returnType;
	unsigned long long addressOfReturnFunction; // if the function's return value is that of another function, this will be the address of that function

	enum CallingConvention callingConvention;

	struct RegisterVariable regArgs[NO_REG - RAX];
	unsigned char numOfRegArgs;

	struct StackVariable stackArgs[6];
	unsigned char numOfStackArgs;

	struct StackVariable localVars[20];
	unsigned char numOfLocalVars;

	struct FuncReturnVariable returnVars[20];
	unsigned char numOfReturnVars;
};

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned char findNextFunction(struct DisassembledInstruction* instructions, unsigned long long* addresses, unsigned short numOfInstructions, struct Function* result, int* instructionIndex);
	
	unsigned char fixAllFunctionReturnTypes(struct Function* functions, unsigned short numOfFunctions);

	unsigned char getAllFuncReturnVars(struct Function* functions, int numOfFunctions, struct DisassembledInstruction* instructions, unsigned long long* addresses, int numOfInstructions, struct ImportedFunction* imports, int numOfImports);

#ifdef __cplusplus
}
#endif

int findFunctionByAddress(struct Function* functions, int low, int high, unsigned long long address);

int findInstructionByAddress(unsigned long long* addresses, int low, int high, unsigned long long address);

unsigned long long resolveJmpChain(struct DisassembledInstruction* instructions, unsigned long long* addresses, int numOfInstructions, int startInstructionIndex);

struct StackVariable* getLocalVarByOffset(struct Function* function, int stackOffset);

struct StackVariable* getStackArgByOffset(struct Function* function, int stackOffset);

struct RegisterVariable* getRegArgByReg(struct Function* function, enum Register reg);

struct FuncReturnVariable* findReturnVar(struct Function* function, char callNum, unsigned long long callAddr);

unsigned char getTypeOfOperand(enum Mnemonic opcode, struct Operand* operand);

static void initializeFunctionVarNames(struct Function* function);

static void sortFunctionArguments(struct Function* function);
