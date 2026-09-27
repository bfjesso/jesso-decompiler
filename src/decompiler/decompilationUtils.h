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

	enum JdcStatus getJumpTable(struct DecompilationParameters* params, int instructionIndex, struct JumpTable* result);

	int findAddressInArr(unsigned long long* addresses, int numOfAddresses, unsigned long long address);

	int findInstructionByAddress(struct DisassembledInstruction* instructions, int numOfInstructions, unsigned long long address);

	int findInstructionByAddressInclusive(struct DisassembledInstruction* instructions, int numOfInstructions, unsigned long long address);

	int findInstructionInsertPoint(struct DisassembledInstruction* instructions, int numOfInstructions, unsigned long long address);

	int findJumpTableByAddress(struct JumpTable* jumpTables, int numOfJumpTables, unsigned long long address, bool* foundIndirectTable);

	bool validateName(struct DecompilationParameters* params, const char* name);

#ifdef __cplusplus
}
#endif

enum JdcStatus addDecompiledLine(struct DecompilationParameters* params, struct JdcStr* decompiledFunction, int associatedInstruction, const char* format, ...);

static enum JdcStatus operandToValue(struct DecompilationParameters* params, int startInstructionIndex, struct Operand* operand, unsigned long long* result);

static enum JdcStatus regToValue(struct DecompilationParameters* params, int startInstructionIndex, enum Register reg, unsigned long long* result);

bool checkForAddressInArrInRange(unsigned long long* addresses, int numOfAddresses, unsigned long long minAddress, unsigned long long maxAddress);

bool doesInstructionModifyOperand(struct DecompilationParameters* params, int instructionIndex, unsigned char operandNum, bool* overwrites);

bool doesInstructionAccessRegister(struct DecompilationParameters* params, int instructionIndex, enum Register reg, bool checkUnknownCalls, enum Register* specificReg); // this will return 0 if the instruction only writes to the reg without reading its value

bool doesInstructionModifyRegister(struct DecompilationParameters* params, int instructionIndex, enum Register reg, enum Register* specificReg, bool* overwrites);

bool doesInstructionConditionallyModifyRegister(struct DecompilationParameters* params, int instructionIndex, enum Register reg);

bool doesInstructionDoNothing(struct DisassembledInstruction* instruction);

bool doesInstructionGenerateInterruptOrException(struct DisassembledInstruction* instruction);

bool isImmediateAllOnes(struct Immediate* immediate);

bool compareOperands(struct Operand* op1, struct Operand* op2);

unsigned char getSizeOfOperand(struct Operand* operand);

bool checkRegVarScope(struct DecompilationParameters* params, struct RegisterVariable* regVar, int instructionIndex);