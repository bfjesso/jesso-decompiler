#pragma once
#include "decompilationUtils.h"

unsigned char decompileOperand(struct DecompilationParameters params, struct Operand* operand, enum PrimitiveType type, struct JdcStr* result);

static unsigned char getValueFromDataSection(struct DecompilationParameters params, enum PrimitiveType type, unsigned long long address, struct JdcStr* result);

static unsigned char decompileRegister(struct DecompilationParameters params, enum Register targetReg, enum PrimitiveType type, struct JdcStr* result);

unsigned char decompileComparison(struct DecompilationParameters params, unsigned char invertOperator, struct JdcStr* result);

unsigned char decompileOperation(struct DecompilationParameters params, enum PrimitiveType type, unsigned char getAssignment, struct JdcStr* result);