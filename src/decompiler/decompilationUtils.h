#pragma once
#include "decompilationStructs.h"

#define NUM_OF_KEYWORDS 11

#ifdef __cplusplus
extern "C"
{
#endif

	extern const char* keywordStrs[];

	unsigned long long getJmpDst(struct DecompilationParameters* params, int startInstructionIndex);

	unsigned long long resolveJmpChain(struct DecompilationParameters* params, int startInstructionIndex);

	unsigned char getJumpTable(struct DecompilationParameters* params, int instructionIndex, struct JumpTable* result);

	int findAddressInArr(unsigned long long* addresses, int numOfAddresses, unsigned long long address);

	int findInstructionByAddress(struct DisassembledInstruction* instructions, int numOfInstructions, unsigned long long address);

	int findInstructionByAddressInclusive(struct DisassembledInstruction* instructions, int numOfInstructions, unsigned long long address);

	int findInstructionInsertPoint(struct DisassembledInstruction* instructions, int numOfInstructions, unsigned long long address);

	int findJumpTableByAddress(struct JumpTable* jumpTables, int numOfJumpTables, unsigned long long address, unsigned char* foundIndirectTable);

	unsigned char validateName(struct DecompilationParameters* params, const char* name);

#ifdef __cplusplus
}
#endif

void addIndents(struct JdcStr* result, int numOfIndents);

static unsigned char operandToValue(struct DecompilationParameters* params, int startInstructionIndex, struct Operand* operand, unsigned long long* result);

static unsigned char regToValue(struct DecompilationParameters* params, int startInstructionIndex, enum Register reg, unsigned long long* result);

unsigned char checkForAddressInArrInRange(unsigned long long* addresses, int numOfAddresses, unsigned long long minAddress, unsigned long long maxAddress);

unsigned char doesInstructionModifyOperand(struct DecompilationParameters* params, int instructionIndex, unsigned char operandNum, unsigned char* overwrites);

unsigned char doesInstructionAccessRegister(struct DecompilationParameters* params, int instructionIndex, enum Register reg, unsigned char checkUnknownCalls, enum Register* specificReg); // this will return 0 if the instruction only writes to the reg without reading its value

unsigned char doesInstructionModifyRegister(struct DecompilationParameters* params, int instructionIndex, enum Register reg, enum Register* specificReg, unsigned char* overwrites);

unsigned char doesInstructionDoNothing(struct DisassembledInstruction* instruction);

unsigned char doesInstructionGenerateInterruptOrException(struct DisassembledInstruction* instruction);

unsigned char isImmediateAllOnes(struct Immediate* immediate);

unsigned char compareOperands(struct Operand* op1, struct Operand* op2);

unsigned char getSizeOfOperand(struct Operand* operand);

unsigned char checkRegVarScope(struct DecompilationParameters* params, struct RegisterVariable* regVar, int instructionIndex);