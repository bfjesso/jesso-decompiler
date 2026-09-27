#pragma once
#include "decompilationStructs.h"

struct Expression
{
	struct JdcStr jdcStr;
	bool placeOperatorInfront;
};

enum JdcStatus decompileOperand(struct DecompilationParameters* params, int32_t instructionIndex, uint8_t operandNum, bool defaultToReg, struct JdcStr* result);

static enum JdcStatus decompileMemoryAddress(struct DecompilationParameters* params, int32_t instructionIndex, uint8_t operandNum, struct JdcStr* result);

static enum JdcStatus decompileStackVar(struct DecompilationParameters* params, int32_t instructionIndex, uint8_t operandNum, int64_t offsetFromInitSP, struct JdcStr* result);

enum JdcStatus decompileRegister(struct DecompilationParameters* params, int32_t instructionIndex, uint8_t operandNum, enum Register targetReg, bool defaultToReg, bool notStatusFlag, struct JdcStr* result, struct RegisterVariable** regVarRef);

enum JdcStatus decompileComparison(struct DecompilationParameters* params, int32_t jccIndex, bool invertOperator, struct JdcStr* result);

static enum JdcStatus getValueFromDataSection(struct DecompilationParameters* params, struct DataType dataType, uint64_t address, struct JdcStr* result);

static enum JdcStatus getStringFromDataSection(struct DecompilationParameters* params, uint64_t address, struct JdcStr* result);