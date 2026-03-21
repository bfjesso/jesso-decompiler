#pragma once
#include "decompilationStructs.h"

#ifdef __cplusplus
extern "C"
{
#endif

	int findAddressInArr(unsigned long long* addresses, int low, int high, unsigned long long address);

#ifdef __cplusplus
}
#endif


int findInstructionByAddress(struct DisassembledInstruction* instructions, int low, int high, unsigned long long address);

unsigned long long resolveJmpChain(struct DecompilationParameters params, int startInstructionIndex);

static unsigned char operandToValue(struct DecompilationParameters params, int startInstructionIndex, struct Operand* operand, unsigned long long* result);

static unsigned char getNumFromData(struct DecompilationParameters params, unsigned long long address, unsigned long long* result);