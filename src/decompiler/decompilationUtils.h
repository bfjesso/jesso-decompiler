#pragma once
#include "decompilationStructs.h"

#define NUM_OF_KEYWORDS 11

#ifdef __cplusplus
extern "C"
{
#endif

	extern const char* keywordStrs[];

	int findAddressInArr(unsigned long long* addresses, int numOfAddresses, unsigned long long address);

	int findInstructionByAddress(struct DisassembledInstruction* instructions, int numOfInstructions, unsigned long long address);

	int findInstructionByAddressInclusive(struct DisassembledInstruction* instructions, int numOfInstructions, unsigned long long address);

	int findInstructionInsertPoint(struct DisassembledInstruction* instructions, int numOfInstructions, unsigned long long address);

	int findJumpTableByAddress(struct JumpTable* jumpTables, int numOfJumpTables, unsigned long long address, unsigned char* foundIndirectTable);

	unsigned long long resolveJmpChain(struct DecompilationParameters* params, int startInstructionIndex);

	unsigned char validateName(struct DecompilationParameters* params, const char* name);

#ifdef __cplusplus
}
#endif

void addIndents(struct JdcStr* result, int numOfIndents);

unsigned char checkForAddressInArrInRange(unsigned long long* addresses, int numOfAddresses, unsigned long long minAddress, unsigned long long maxAddress);

unsigned char doesInstructionAccessRegister(struct DecompilationParameters* params, int instructionIndex, enum Register reg, unsigned char checkUnknownCalls, enum Register* specificReg); // this will return 0 if the instruction only writes to the reg without reading its value

unsigned char doesInstructionModifyRegister(struct DecompilationParameters* params, int instructionIndex, enum Register reg, enum Register* specificReg, unsigned char* overwrites);