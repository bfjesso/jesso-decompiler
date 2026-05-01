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

unsigned char doesInstructionModifyOperand(struct DisassembledInstruction* instruction, unsigned char operandNum, unsigned char* srcOperandNum, unsigned char* overwrites);

unsigned char doesInstructionAccessRegister(struct DisassembledInstruction* instruction, enum Register reg, unsigned char* regOperandNum);

unsigned char doesInstructionModifyRegister(struct DisassembledInstruction* instruction, enum Register reg, unsigned char* regOperandNum, unsigned char* srcOperandNum, unsigned char* overwrites);

unsigned char doesInstructionModifyZF(struct DisassembledInstruction* instruction);

unsigned char isImmediateAllOnes(struct Immediate* immediate);