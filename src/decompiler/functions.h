#pragma once
#include "../disassembler/registers.h"
#include "decompilationStructs.h"

#ifdef __cplusplus
extern "C"
{
#endif

	enum JdcStatus findNextFunction(struct DecompilationParameters* params, struct Function* result, int32_t* instructionIndex);

	enum JdcStatus analyzeAllFunctions(struct DecompilationParameters* params);

	void freeFunction(struct Function* function);

	int32_t findFunctionByAddress(struct DecompilationParameters* params, uint64_t address);

	int32_t findFunctionByAddressInclusive(struct DecompilationParameters* params, uint64_t address);

#ifdef __cplusplus
}
#endif

static enum JdcStatus getAllFunctionReturnTypesAndConditions(struct DecompilationParameters* params);

static enum JdcStatus getAllFunctionRegArgsAndStackVars(struct DecompilationParameters* params);

static bool isRegInitialized(struct DecompilationParameters* params, int32_t startInstructionIndex, int32_t minInstructionIndex, enum Register reg, enum Register* specificReg, struct DataType* dataType);

static bool isStackVarInitialized(struct DecompilationParameters* params, int32_t startInstructionIndex, int32_t minInstructionIndex, int64_t offsetFromInitSP);

static enum JdcStatus fixAllFunctionArgs(struct DecompilationParameters* params);

bool getStackArgInitializer(struct DecompilationParameters* params, int32_t callInstructionIndex, int64_t stackArgOffset, struct StackVariable** stackVarRef, int32_t* pushInstructionRef, int64_t* stackFrameSizeRef);

static enum JdcStatus setAllStackVarTypes(struct DecompilationParameters* params);

static int64_t getStackFrameChange(struct DisassembledInstruction* instruction);

int64_t getStackFrameSizeAtInstruction(struct DecompilationParameters* params, int32_t instructionIndex);

bool isMemAddressStackVar(struct DecompilationParameters* params, int32_t instructionIndex, struct MemoryAddress* memAddress, int64_t* offsetFromInitSP);

struct StackVariable* getStackVarByOffset(struct Function* function, int64_t offsetFromInitSP);

int32_t getNumOfStackArgs(struct Function* function);

int32_t getNumOfRegArgs(struct Function* function);

struct RegisterVariable* getRegArgByReg(struct Function* function, enum Register reg);

struct RegisterVariable* getLocalRegVarByReg(struct Function* function, enum Register reg);

struct ReturnedVariable* findReturnedVar(struct Function* function, uint64_t callInstructionAddress);

static enum JdcStatus addStackVar(struct Function* function, int64_t offsetFromInitSP, bool isArgument, struct DataType* dataTypeRef);

enum JdcStatus addRegVar(struct DecompilationParameters* params, struct DataType* dataTypeRef, bool isArgument, enum Register reg);

enum JdcStatus addRegVarScope(struct RegisterVariable* regVar, int32_t startIndex, int32_t endIndex);

static void setRegVarDataType(struct DecompilationParameters* params, struct RegisterVariable* regVar);

enum JdcStatus addReturnedVar(struct Function* function, struct DataType dataType, uint64_t calleeAddress, uint64_t callInstructionAddress, enum Register returnReg, const char* calleeName);

enum JdcStatus addAssociatedInstruction(struct Function* function, int32_t instructionIndex);