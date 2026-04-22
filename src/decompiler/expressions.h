#pragma once
#include "decompilationStructs.h"

unsigned char decompileOperand(struct DecompilationParameters* params, struct Operand* operand, unsigned char defaultToReg, struct JdcStr* result);

static unsigned char decompileMemoryAddress(struct DecompilationParameters* params, struct MemoryAddress* memAddress, struct JdcStr* result);

unsigned char decompileRegister(struct DecompilationParameters* params, enum Register targetReg, unsigned char defaultToReg, struct JdcStr* result, struct RegisterVariable** regArgVarRef);

unsigned char decompileComparison(struct DecompilationParameters* params, unsigned char invertOperator, struct JdcStr* result);

static unsigned char getValueFromDataSection(struct DecompilationParameters* params, struct VarType type, unsigned long long address, struct JdcStr* result);

static unsigned char getStringFromDataSection(struct DecompilationParameters* params, unsigned long long address, struct JdcStr* result);