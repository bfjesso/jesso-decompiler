#pragma once
#include "../disassembler/registers.h"
#include "decompilationStructs.h"

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned char findNextFunction(struct DecompilationParameters* params, unsigned long long currentSectionEndAddress, unsigned long long* calledAddresses, int numOfCalledAddresses, struct Function* result, int* instructionIndex);
	
	void getAllFunctionReturnTypes(struct DecompilationParameters* params);

	unsigned char fixAllFunctionReturnTypes(struct DecompilationParameters* params);

	unsigned char getAllFunctionArguments(struct DecompilationParameters* params);

	unsigned char fixAllFunctionArgs(struct DecompilationParameters* params);

	void freeFunction(struct Function* function);

	int findFunctionByAddress(struct DecompilationParameters* params, int low, int high, unsigned long long address);

#ifdef __cplusplus
}
#endif

static unsigned char getFunctionArguments(struct DecompilationParameters* params, int startInstructionIndex, int endInstructionIndex, long long stackFrameSize, unsigned char* initializedRegs, int callNum);

static void sortFunctionArguments(struct Function* function);

static long long getStackFrameChange(struct DisassembledInstruction* instruction);

long long getStackFrameSizeAtInstruction(struct DecompilationParameters* params, int instructionIndex);

struct StackVariable* getStackArgByOffset(struct Function* function, long long stackOffset);

struct StackVariable* getStackVarByOffset(struct Function* function, long long stackOffset);

struct RegisterVariable* getRegArgByReg(struct Function* function, enum Register reg);

struct RegisterVariable* getRegVarByReg(struct Function* function, enum Register reg);

struct ReturnedVariable* findReturnedVar(struct Function* function, unsigned long long callInstructionAddress);

unsigned char addStackArg(struct Function* function, struct DataType dataType, long long stackOffset);

unsigned char addStackVar(struct Function* function, struct DataType dataType, long long stackOffset);

unsigned char addRegArg(struct Function* function, struct DataType dataType, enum Register reg);

unsigned char addRegVar(struct Function* function, struct DataType dataType, enum Register reg);

unsigned char addReturnedVar(struct Function* function, struct DataType dataType, unsigned long long calleeAddress, unsigned long long callInstructionAddress, enum Register returnReg, const char* calleeName);
