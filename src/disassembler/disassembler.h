#pragma once
#include "disassemblyStructs.h"
#include "../jdc-str/jdcStr.h"

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned char disassembleInstruction(unsigned char* bytes, unsigned char* maxBytesAddr, struct DisassemblerOptions* disassemblerOptions, struct DisassembledInstruction* result);
	
	unsigned char instructionToStr(struct DisassembledInstruction* instruction, struct JdcStr* result);

	const char* getPtrSizeStr(int ptrSize);

	const char* getGroup1PrefixStr(struct DisassembledInstruction* instruction);

	unsigned char checkForControlFlowJump(struct DisassembledInstruction* instructions, int instructionIndex, unsigned long long* jmpDst, unsigned char* stop);

	unsigned long long getJumpTableAddress(struct DisassembledInstruction* instructions, int numOfInstructions, unsigned char* size);

	unsigned long long getIndirectTableAddress(struct DisassembledInstruction* instructions, int numOfInstructions);

#ifdef __cplusplus
}
#endif

static unsigned char memAddressToStr(struct MemoryAddress* memAddr, struct JdcStr* result);