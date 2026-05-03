#pragma once
#include "disassemblyStructs.h"

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned char doesInstructionDoNothing(struct DisassembledInstruction* instruction);

#ifdef __cplusplus
}
#endif

unsigned char operandToValue(struct DisassembledInstruction* instructions, int startInstructionIndex, int minInstructionIndex, struct Operand* operand, unsigned long long* result);

unsigned char regToValue(struct DisassembledInstruction* instructions, int startInstructionIndex, int minInstructionIndex, enum Register reg, unsigned long long* result);

unsigned char doesInstructionModifyOperand(struct DisassembledInstruction* instruction, unsigned char operandNum, unsigned char* srcOperandNum, unsigned char* overwrites);

unsigned char doesInstructionAccessRegister(struct DisassembledInstruction* instruction, enum Register reg, unsigned char* regOperandNum);

unsigned char doesInstructionModifyRegister(struct DisassembledInstruction* instruction, enum Register reg, unsigned char* regOperandNum, unsigned char* srcOperandNum, unsigned char* overwrites);

unsigned char doesInstructionModifyZF(struct DisassembledInstruction* instruction);

unsigned char isImmediateAllOnes(struct Immediate* immediate);