#pragma once
#include "decompilationStructs.h"

#ifdef __cplusplus
extern "C"
{
#endif

	int findAddressInArr(unsigned long long* addresses, int low, int high, unsigned long long address);

	int findInstructionByAddress(struct DisassembledInstruction* instructions, int low, int high, unsigned long long address);

	unsigned long long resolveJmpChain(struct DecompilationParameters* params);

#ifdef __cplusplus
}
#endif

void addIndents(struct JdcStr* result, int numOfIndents);

unsigned char checkForAddressInArrInRange(unsigned long long* addresses, int low, int high, unsigned long long minAddress, unsigned long long maxAddress);
