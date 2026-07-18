#pragma once
#include "../disassembler/registers.h"
#include "decompilationStructs.h"

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned char findNextFunction(struct DecompilationParameters* params, unsigned long long currentSectionEndAddress, unsigned long long* calledAddresses, int numOfCalledAddresses, struct Function* result, int* instructionIndex);

	unsigned char analyzeAllFunctions(struct DecompilationParameters* params);

	void freeFunction(struct Function* function);

	int findFunctionByAddress(struct DecompilationParameters* params, unsigned long long address);

	int findFunctionByAddressInclusive(struct DecompilationParameters* params, unsigned long long address);

#ifdef __cplusplus
}
#endif

static unsigned char getAllFunctionReturnTypesAndConditions(struct DecompilationParameters* params);

static unsigned char getAllFunctionRegArgsAndStackVars(struct DecompilationParameters* params);

static unsigned char isRegInitialized(struct DecompilationParameters* params, int startInstructionIndex, int minInstructionIndex, enum Register reg, enum Register* specificReg, struct DataType* dataType);

static unsigned char fixAllFunctionArgs(struct DecompilationParameters* params);

unsigned char getStackArgInitializer(struct DecompilationParameters* params, int callInstructionIndex, long long stackArgOffset, struct StackVariable** stackVarRef, int* pushInstructionRef, long long* stackFrameSizeRef);

static unsigned char setAllStackVarTypes(struct DecompilationParameters* params);

static long long getStackFrameChange(struct DisassembledInstruction* instruction);

long long getStackFrameSizeAtInstruction(struct DecompilationParameters* params, int instructionIndex);

unsigned char isMemAddressStackVar(struct DecompilationParameters* params, int instructionIndex, struct MemoryAddress* memAddress, long long* offsetFromInitSP);

struct StackVariable* getStackVarByOffset(struct Function* function, long long offsetFromInitSP);

int getNumOfStackArgs(struct Function* function);

int getNumOfRegArgs(struct Function* function);

struct RegisterVariable* getRegArgByReg(struct Function* function, enum Register reg);

struct RegisterVariable* getLocalRegVarByReg(struct Function* function, enum Register reg);

struct ReturnedVariable* findReturnedVar(struct Function* function, unsigned long long callInstructionAddress);

static unsigned char addStackVar(struct Function* function, long long offsetFromInitSP, struct DataType* dataTypeRef);

unsigned char addRegVar(struct DecompilationParameters* params, struct DataType* dataTypeRef, unsigned char isArgument, enum Register reg);

static void setRegVarDataType(struct DecompilationParameters* params, struct RegisterVariable* regVar);

unsigned char addReturnedVar(struct Function* function, struct DataType dataType, unsigned long long calleeAddress, unsigned long long callInstructionAddress, enum Register returnReg, const char* calleeName);

unsigned char addAssociatedInstruction(struct Function* function, int instructionIndex);