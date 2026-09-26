#pragma once
#include "../disassembler/registers.h"
#include "decompilationStructs.h"

#ifdef __cplusplus
extern "C"
{
#endif

	enum JdcStatus findNextFunction(struct DecompilationParameters* params, struct Function* result, int* instructionIndex);

	enum JdcStatus analyzeAllFunctions(struct DecompilationParameters* params);

	void freeFunction(struct Function* function);

	int findFunctionByAddress(struct DecompilationParameters* params, unsigned long long address);

	int findFunctionByAddressInclusive(struct DecompilationParameters* params, unsigned long long address);

#ifdef __cplusplus
}
#endif

static enum JdcStatus getAllFunctionReturnTypesAndConditions(struct DecompilationParameters* params);

static enum JdcStatus getAllFunctionRegArgsAndStackVars(struct DecompilationParameters* params);

static unsigned char isRegInitialized(struct DecompilationParameters* params, int startInstructionIndex, int minInstructionIndex, enum Register reg, enum Register* specificReg, struct DataType* dataType);

static enum JdcStatus fixAllFunctionArgs(struct DecompilationParameters* params);

unsigned char getStackArgInitializer(struct DecompilationParameters* params, int callInstructionIndex, long long stackArgOffset, struct StackVariable** stackVarRef, int* pushInstructionRef, long long* stackFrameSizeRef);

static enum JdcStatus setAllStackVarTypes(struct DecompilationParameters* params);

static long long getStackFrameChange(struct DisassembledInstruction* instruction);

long long getStackFrameSizeAtInstruction(struct DecompilationParameters* params, int instructionIndex);

unsigned char isMemAddressStackVar(struct DecompilationParameters* params, int instructionIndex, struct MemoryAddress* memAddress, long long* offsetFromInitSP);

struct StackVariable* getStackVarByOffset(struct Function* function, long long offsetFromInitSP);

int getNumOfStackArgs(struct Function* function);

int getNumOfRegArgs(struct Function* function);

struct RegisterVariable* getRegArgByReg(struct Function* function, enum Register reg);

struct RegisterVariable* getLocalRegVarByReg(struct Function* function, enum Register reg);

struct ReturnedVariable* findReturnedVar(struct Function* function, unsigned long long callInstructionAddress);

static enum JdcStatus addStackVar(struct Function* function, long long offsetFromInitSP, struct DataType* dataTypeRef);

enum JdcStatus addRegVar(struct DecompilationParameters* params, struct DataType* dataTypeRef, unsigned char isArgument, enum Register reg);

enum JdcStatus addRegVarScope(struct RegisterVariable* regVar, int startIndex, int endIndex);

static void setRegVarDataType(struct DecompilationParameters* params, struct RegisterVariable* regVar);

enum JdcStatus addReturnedVar(struct Function* function, struct DataType dataType, unsigned long long calleeAddress, unsigned long long callInstructionAddress, enum Register returnReg, const char* calleeName);

enum JdcStatus addAssociatedInstruction(struct Function* function, int instructionIndex);