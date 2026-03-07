#pragma once
#include "../disassembler/registers.h"
#include "decompilationStructs.h"

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned char findNextFunction(struct DecompilationParameters params, unsigned long long nextSectionStartAddress, struct Function* result, int* instructionIndex);
	
	unsigned char fixAllFunctionReturnTypes(struct Function* functions, unsigned short numOfFunctions, unsigned char is64Bit);

	unsigned char fixAllFunctionArgs(struct Function* functions, unsigned short numOfFunctions);

	void freeFunction(struct Function* function);

	int findFunctionByAddress(struct Function* functions, int low, int high, unsigned long long address);

#ifdef __cplusplus
}
#endif

static int getStackFrameChange(struct DisassembledInstruction* instruction);

int getStackFrameSizeAtInstruction(struct Function* function, int instructionIndex);

struct StackVariable* getLocalVarByOffset(struct Function* function, int stackOffset);

struct StackVariable* getStackArgByOffset(struct Function* function, int stackOffset);

struct RegisterVariable* getRegArgByReg(struct Function* function, enum Register reg);

struct FuncReturnVariable* findReturnedVar(struct Function* function, char callNum, unsigned long long callAddr);

static void sortFunctionArguments(struct Function* function);
