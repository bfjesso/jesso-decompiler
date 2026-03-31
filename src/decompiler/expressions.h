#pragma once
#include "decompilationStructs.h"

unsigned char decompileOperand(struct DecompilationParameters* params, struct Operand* operand, struct VarType type, struct JdcStr* result);

static unsigned char getValueFromDataSection(struct DecompilationParameters* params, struct VarType type, unsigned long long address, struct JdcStr* result);

static unsigned char getStringFromDataSection(struct DecompilationParameters* params, unsigned long long address, struct JdcStr* result);

unsigned char decompileRegister(struct DecompilationParameters* params, enum Register targetReg, struct VarType type, struct JdcStr* result, struct RegisterVariable** regArgVarRef);

unsigned char decompileComparison(struct DecompilationParameters* params, unsigned char invertOperator, struct JdcStr* result);

unsigned char decompileOperation(struct DecompilationParameters* params, struct VarType type, enum Register targetReg, unsigned char getAssignment, struct JdcStr* result);