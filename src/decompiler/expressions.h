#pragma once
#include "decompilationStructs.h"

unsigned char decompileOperand(struct DecompilationParameters* params, int instructionIndex, struct Operand* operand, unsigned char defaultToReg, struct JdcStr* result);

static unsigned char decompileMemoryAddress(struct DecompilationParameters* params, int instructionIndex, struct MemoryAddress* memAddress, struct JdcStr* result);

unsigned char decompileRegister(struct DecompilationParameters* params, int instructionIndex, enum Register targetReg, unsigned char defaultToReg, struct JdcStr* result, struct RegisterVariable** regArgVarRef);

unsigned char decompileComparison(struct DecompilationParameters* params, int jccIndex, unsigned char invertOperator, struct JdcStr* result);

static unsigned char getValueFromDataSection(struct DecompilationParameters* params, struct DataType dataType, unsigned long long address, struct JdcStr* result);

static unsigned char getStringFromDataSection(struct DecompilationParameters* params, unsigned long long address, struct JdcStr* result);