#pragma once
#include "decompilationUtils.h"

unsigned char decompileOperand(struct DecompilationParameters params, struct Operand* operand, enum PrimitiveType type, char* resultBuffer, unsigned char resultBufferSize);

static unsigned char decompileExpression(struct DecompilationParameters params, enum Register targetReg, enum PrimitiveType type, char* resultBuffer, unsigned char resultBufferSize);

unsigned char decompileComparison(struct DecompilationParameters params, char* resultBuffer, unsigned char invertOperator);