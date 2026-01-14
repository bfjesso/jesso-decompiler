#pragma once
#include "decompilationUtils.h"

unsigned char decompileOperand(struct DecompilationParameters params, struct Operand* operand, enum PrimitiveType type, char* resultBuffer, unsigned char resultBufferSize);

static unsigned char getValueFromDataSection(struct DecompilationParameters params, enum PrimitiveType type, unsigned long long address, char* resultBuffer);

static unsigned char decompileRegister(struct DecompilationParameters params, enum Register targetReg, enum PrimitiveType type, char* resultBuffer, unsigned char resultBufferSize);

unsigned char decompileComparison(struct DecompilationParameters params, char* resultBuffer, unsigned char invertOperator);

unsigned char getOperationStr(enum Mnemonic opcode, unsigned char getAssignment, char* resultBuffer);

// this is for when the instruction is not of the format where the first operand is being modified using the second operand, so getOperationStr cant be used
unsigned char handleOtherOperationStr(struct DecompilationParameters params, struct DisassembledInstruction* instruction, char* resultBuffer, unsigned char* isOtherOpcode);