#pragma once
#include "decompilationStructs.h"

#ifdef __cplusplus
extern "C"
{
#endif

	int findAddressInArr(unsigned long long* addresses, int numOfAddresses, unsigned long long address);

	int findInstructionByAddress(struct DisassembledInstruction* instructions, int numOfInstructions, unsigned long long address);

	int findInstructionInsertPoint(struct DisassembledInstruction* instructions, int numOfInstructions, unsigned long long address);

	unsigned long long resolveJmpChain(struct DecompilationParameters* params, int startInstructionIndex);

#ifdef __cplusplus
}
#endif

void addIndents(struct JdcStr* result, int numOfIndents);

unsigned char checkForAddressInArrInRange(unsigned long long* addresses, int numOfAddresses, unsigned long long minAddress, unsigned long long maxAddress);

unsigned char doesInstructionAccessRegister(struct DecompilationParameters* params, int instructionIndex, enum Register reg, enum Register* specificReg, struct DataType* dataTypeRef); // this will return 0 if the instruction only writes to the reg without reading its value

unsigned char doesInstructionModifyRegister(struct DecompilationParameters* params, int instructionIndex, enum Register reg, enum Register* specificReg, unsigned char* overwrites);