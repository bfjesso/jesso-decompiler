#pragma once
#include "../disassembler/registers.h"
#include "../disassembler/disassembler.h"

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
	char name[20];
};

struct StackVariable 
{
	enum PrimitiveType type;
	int stackOffset;
	char name[20];
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
};

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned char findNextFunction(struct DisassembledInstruction* instructions, unsigned long long* addresses, unsigned short numOfInstructions, struct Function* result, int* instructionIndex);
	
	unsigned char fixAllFunctionReturnTypes(struct Function* functions, unsigned short numOfFunctions);

#ifdef __cplusplus
}
#endif

int findFunctionByAddress(struct Function* functions, int low, int high, unsigned long long address);

int findInstructionByAddress(unsigned long long* addresses, int low, int high, unsigned long long address);

struct StackVariable* getLocalVarByOffset(struct Function* function, int stackOffset);

struct StackVariable* getStackArgByOffset(struct Function* function, int stackOffset);

struct RegisterVariable* getRegArgByReg(struct Function* function, enum Register reg);

unsigned char getTypeOfOperand(enum Mnemonic opcode, struct Operand* operand);

static void initializeFunctionVarNames(struct Function* function);

static void sortFunctionArguments(struct Function* function);