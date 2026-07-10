#pragma once
#include "../disassembler/registers.h"
#include "decompilationStructs.h"

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned char findNextFunction(struct DecompilationParameters* params, unsigned long long currentSectionEndAddress, unsigned long long* calledAddresses, int numOfCalledAddresses, struct Function* result, int* instructionIndex);
	
	void getAllFunctionReturnTypes(struct DecompilationParameters* params);

	unsigned char getAllFunctionConditionsAndArguments(struct DecompilationParameters* params);

	void freeFunction(struct Function* function);

	int findFunctionByAddress(struct DecompilationParameters* params, unsigned long long address);

	int findFunctionByAddressInclusive(struct DecompilationParameters* params, unsigned long long address);

#ifdef __cplusplus
}
#endif

static unsigned char getFunctionRegArgsAndStackVars(struct DecompilationParameters* params);

static unsigned char isRegInitialized(struct DecompilationParameters* params, int startInstructionIndex, int minInstructionIndex, enum Register reg, enum Register* specificReg, struct DataType* dataType);

static unsigned char fixAllFunctionArgs(struct DecompilationParameters* params);

static void setStackVarTypes(struct Function* function, unsigned char is64Bit);

static long long getStackFrameChange(struct DisassembledInstruction* instruction);

long long getStackFrameSizeAtInstruction(struct DecompilationParameters* params, int instructionIndex);

unsigned char isMemAddressStackVar(struct DecompilationParameters* params, int instructionIndex, struct MemoryAddress* memAddress, long long* stackOffset);

struct StackVariable* getStackVarByOffset(struct Function* function, long long stackOffset);

int getNumOfStackArgs(struct Function* function);

int getNumOfRegArgs(struct Function* function);

struct RegisterVariable* getRegArgByReg(struct Function* function, enum Register reg);

struct RegisterVariable* getLocalRegVarByReg(struct Function* function, enum Register reg);

struct ReturnedVariable* findReturnedVar(struct Function* function, unsigned long long callInstructionAddress);

unsigned char addStackVar(struct Function* function, struct DataType dataType, unsigned char isArgument, long long stackOffset);

unsigned char addRegVar(struct Function* function, struct DataType dataType, unsigned char isArgument, enum Register reg);

unsigned char addReturnedVar(struct Function* function, struct DataType dataType, unsigned long long calleeAddress, unsigned long long callInstructionAddress, enum Register returnReg, const char* calleeName);

unsigned char addAssociatedInstruction(struct Function* function, int instructionIndex);