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

	unsigned char checkForControlFlowJump(struct DisassembledInstruction* instruction, unsigned long long* jmpDst, unsigned char* stop);

	unsigned char getJumpTable(struct DisassembledInstruction* instructions, int instructionIndex, struct JumpTable* result);

#ifdef __cplusplus
}
#endif

static unsigned char memAddressToStr(struct MemoryAddress* memAddr, struct JdcStr* result);