#pragma once
#include "decompilationStructs.h"

struct Expression
{
	struct JdcStr jdcStr;
	unsigned char placeOperatorInfront;
};

enum JdcStatus decompileOperand(struct DecompilationParameters* params, int instructionIndex, unsigned char operandNum, unsigned char defaultToReg, struct JdcStr* result);

static enum JdcStatus decompileMemoryAddress(struct DecompilationParameters* params, int instructionIndex, unsigned char operandNum, struct JdcStr* result);

static enum JdcStatus decompileStackVar(struct DecompilationParameters* params, int instructionIndex, unsigned char operandNum, long long offsetFromInitSP, struct JdcStr* result);

enum JdcStatus decompileRegister(struct DecompilationParameters* params, int instructionIndex, unsigned char operandNum, enum Register targetReg, unsigned char defaultToReg, unsigned char notStatusFlag, struct JdcStr* result, struct RegisterVariable** regVarRef);

enum JdcStatus decompileComparison(struct DecompilationParameters* params, int jccIndex, unsigned char invertOperator, struct JdcStr* result);

static enum JdcStatus getValueFromDataSection(struct DecompilationParameters* params, struct DataType dataType, unsigned long long address, struct JdcStr* result);

static enum JdcStatus getStringFromDataSection(struct DecompilationParameters* params, unsigned long long address, struct JdcStr* result);