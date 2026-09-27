#pragma once
#include "decompilationStructs.h"

#define NUM_OF_KEYWORDS 11

#ifdef __cplusplus
extern "C"
{
#endif

	extern const char* keywordStrs[];

	uint64_t getJmpDst(struct DecompilationParameters* params, int32_t startInstructionIndex);

	uint64_t resolveJmpChain(struct DecompilationParameters* params, int32_t startInstructionIndex);

	enum JdcStatus getJumpTable(struct DecompilationParameters* params, int32_t instructionIndex, struct JumpTable* result);

	int32_t findAddressInArr(uint64_t* addresses, int32_t numOfAddresses, uint64_t address);

	int32_t findInstructionByAddress(struct DisassembledInstruction* instructions, int32_t numOfInstructions, uint64_t address);

	int32_t findInstructionByAddressInclusive(struct DisassembledInstruction* instructions, int32_t numOfInstructions, uint64_t address);

	int32_t findInstructionInsertPoint(struct DisassembledInstruction* instructions, int32_t numOfInstructions, uint64_t address);

	int32_t findJumpTableByAddress(struct JumpTable* jumpTables, int32_t numOfJumpTables, uint64_t address, bool* foundIndirectTable);

	bool validateName(struct DecompilationParameters* params, const char* name);

#ifdef __cplusplus
}
#endif

enum JdcStatus addDecompiledLine(struct DecompilationParameters* params, struct JdcStr* decompiledFunction, int32_t associatedInstruction, const char* format, ...);

static enum JdcStatus operandToValue(struct DecompilationParameters* params, int32_t startInstructionIndex, struct Operand* operand, uint64_t* result);

static enum JdcStatus regToValue(struct DecompilationParameters* params, int32_t startInstructionIndex, enum Register reg, uint64_t* result);

bool checkForAddressInArrInRange(uint64_t* addresses, int32_t numOfAddresses, uint64_t minAddress, uint64_t maxAddress);

bool doesInstructionModifyOperand(struct DecompilationParameters* params, int32_t instructionIndex, uint8_t operandNum, bool* overwrites);

bool doesInstructionAccessRegister(struct DecompilationParameters* params, int32_t instructionIndex, enum Register reg, bool checkUnknownCalls, enum Register* specificReg); // this will return 0 if the instruction only writes to the reg without reading its value

bool doesInstructionModifyRegister(struct DecompilationParameters* params, int32_t instructionIndex, enum Register reg, enum Register* specificReg, bool* overwrites);

bool doesInstructionConditionallyModifyRegister(struct DecompilationParameters* params, int32_t instructionIndex, enum Register reg);

bool doesInstructionDoNothing(struct DisassembledInstruction* instruction);

bool doesInstructionGenerateInterruptOrException(struct DisassembledInstruction* instruction);

bool isImmediateAllOnes(struct Immediate* immediate);

bool compareOperands(struct Operand* op1, struct Operand* op2);

uint8_t getSizeOfOperand(struct Operand* operand);

bool checkRegVarScope(struct DecompilationParameters* params, struct RegisterVariable* regVar, int32_t instructionIndex);