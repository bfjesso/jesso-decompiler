#pragma once
#include "disassemblyUtils.h"
#include "modRM.h"
#include "registers.h"

#ifdef __cplusplus
extern "C"
{
#endif

	extern const char* ptrSizeStrs[];

	unsigned char disassembleInstruction(unsigned char* bytes, unsigned char* maxBytesAddr, struct DisassemblerOptions* disassemblerOptions, struct DisassembledInstruction* result, unsigned char* isOpcodeInvalid);
	
	unsigned char instructionToStr(struct DisassembledInstruction* instruction, char* buffer, unsigned char bufferSize);

#ifdef __cplusplus
}
#endif

static unsigned char memAddressToStr(struct MemoryAddress* memAddr, char* buffer, unsigned char bufferSize);

unsigned char doesInstructionModifyOperand(struct DisassembledInstruction* instruction, unsigned char operandNum, unsigned char* overwrites);

unsigned char doesInstructionAccessRegister(struct DisassembledInstruction* instruction, enum Register reg, unsigned char* operandNum);

unsigned char doesInstructionModifyRegister(struct DisassembledInstruction* instruction, enum Register reg, unsigned char* operandNum, unsigned char* overwrites);