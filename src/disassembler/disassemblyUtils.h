#pragma once
#include "disassemblyStructs.h"

unsigned char doesInstructionModifyOperand(struct DisassembledInstruction* instruction, unsigned char operandNum, unsigned char* overwrites);

unsigned char doesInstructionModifyZF(struct DisassembledInstruction* instruction);

unsigned char doesInstructionDoNothing(struct DisassembledInstruction* instruction);

unsigned char doesInstructionGenerateInterruptOrException(struct DisassembledInstruction* instruction);

unsigned char isImmediateAllOnes(struct Immediate* immediate);