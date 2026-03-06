#pragma once
#include "decompilationStructs.h"

int findInstructionByAddress(struct DisassembledInstruction* instructions, int low, int high, unsigned long long address);

unsigned long long resolveJmpChain(struct DecompilationParameters params, int startInstructionIndex);

static unsigned char operandToValue(struct DecompilationParameters params, int startInstructionIndex, struct Operand* operand, unsigned long long* result);

static unsigned char getNumFromData(struct DecompilationParameters params, unsigned long long address, unsigned long long* result);