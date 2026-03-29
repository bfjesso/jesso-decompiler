#pragma once
#include "decompilationStructs.h"

#ifdef __cplusplus
extern "C"
{
#endif

	int findAddressInArr(unsigned long long* addresses, int low, int high, unsigned long long address);

	int findInstructionByAddress(struct DisassembledInstruction* instructions, int low, int high, unsigned long long address);

	unsigned long long resolveJmpChain(struct DecompilationParameters params, int startInstructionIndex);

#ifdef __cplusplus
}
#endif

unsigned char checkForAddressInArrInRange(unsigned long long* addresses, int low, int high, unsigned long long minAddress, unsigned long long maxAddress);

static unsigned char operandToValue(struct DecompilationParameters params, int startInstructionIndex, struct Operand* operand, unsigned long long* result);

static unsigned char getNumFromData(struct DecompilationParameters params, unsigned long long address, unsigned long long* result);
