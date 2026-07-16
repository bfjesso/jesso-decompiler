#pragma once
#include "decompilationStructs.h"

struct Expression
{
	struct JdcStr jdcStr;
	unsigned char placeOperatorInfront;
};

unsigned char decompileOperand(struct DecompilationParameters* params, int instructionIndex, unsigned char operandNum, unsigned char defaultToReg, struct JdcStr* result);

static unsigned char decompileMemoryAddress(struct DecompilationParameters* params, int instructionIndex, struct MemoryAddress* memAddress, struct JdcStr* result);

static unsigned char decompileStackVar(struct DecompilationParameters* params, int instructionIndex, struct MemoryAddress* memAddress, long long stackOffset, struct JdcStr* result);

unsigned char decompileRegister(struct DecompilationParameters* params, int instructionIndex, enum Register targetReg, unsigned char defaultToReg, struct JdcStr* result, struct RegisterVariable** regVarRef);

unsigned char decompileComparison(struct DecompilationParameters* params, int jccIndex, unsigned char invertOperator, struct JdcStr* result);

static unsigned char getValueFromDataSection(struct DecompilationParameters* params, struct DataType dataType, unsigned long long address, struct JdcStr* result);

static unsigned char getStringFromDataSection(struct DecompilationParameters* params, unsigned long long address, struct JdcStr* result);