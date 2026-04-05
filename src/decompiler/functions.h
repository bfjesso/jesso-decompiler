#pragma once
#include "../disassembler/registers.h"
#include "decompilationStructs.h"

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned char findNextFunction(struct DecompilationParameters* params, unsigned long long currentSectionEndAddress, unsigned long long* calledAddresses, int numOfCalledAddresses, struct Function* result, int* instructionIndex);
	
	unsigned char fixAllFunctionReturnTypes(struct DecompilationParameters* params);

	unsigned char fixAllFunctionArgs(struct DecompilationParameters* params);

	void freeFunction(struct Function* function);

	int findFunctionByAddress(struct DecompilationParameters* params, int low, int high, unsigned long long address);

#ifdef __cplusplus
}
#endif

static int getStackFrameChange(struct DisassembledInstruction* instruction);

int getStackFrameSizeAtInstruction(struct DecompilationParameters* params);

struct StackVariable* getStackArgByOffset(struct Function* function, int stackOffset);

struct StackVariable* getStackVarByOffset(struct Function* function, int stackOffset);

struct RegisterVariable* getRegArgByReg(struct Function* function, enum Register reg);

struct RegisterVariable* getRegVarByReg(struct Function* function, enum Register reg);

struct ReturnedVariable* findReturnedVar(struct Function* function, char callNum, unsigned long long callAddr);

unsigned char addStackArg(struct Function* function, struct VarType type, int stackOffset);

unsigned char addStackVar(struct Function* function, struct VarType type, int stackOffset);

unsigned char addRegArg(struct Function* function, struct VarType type, enum Register reg);

unsigned char addRegVar(struct Function* function, struct VarType type, enum Register reg);

unsigned char addReturnedVar(struct Function* function, struct VarType type, char callNum, unsigned long long callAddr, enum Register returnReg, const char* calleeName);

static void sortFunctionArguments(struct Function* function);
