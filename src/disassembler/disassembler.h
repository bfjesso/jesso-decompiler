#pragma once
#include "disassemblyStructs.h"

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned char disassembleInstruction(unsigned char* bytes, unsigned char* maxBytesAddr, struct DisassemblerOptions* disassemblerOptions, struct DisassembledInstruction* result, unsigned char* numOfBytes);
	
	unsigned char instructionToStr(struct DisassembledInstruction* instruction, char* buffer, unsigned char bufferSize);

	const char* getPtrSizeStr(int ptrSize);

	const char* getGroup1PrefixStr(enum LegacyPrefix prefix);

	unsigned long long getJumpTableAddress(struct DisassembledInstruction* instructions, int numOfInstructions, unsigned char* size);

	unsigned long long getIndirectTableAddress(struct DisassembledInstruction* instructions, int numOfInstructions);

#ifdef __cplusplus
}
#endif

static unsigned char memAddressToStr(struct MemoryAddress* memAddr, char* buffer, unsigned char bufferSize);