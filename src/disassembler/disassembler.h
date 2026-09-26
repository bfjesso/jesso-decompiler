#pragma once
#include "disassemblyStructs.h"
#include "../jdc-str/jdcStr.h"

#ifdef __cplusplus
extern "C"
{
#endif

	enum JdcStatus disassembleInstruction(unsigned char* bytes, unsigned char* maxBytesAddr, struct DisassemblerOptions* disassemblerOptions, struct DisassembledInstruction* result);
	
	enum JdcStatus instructionToStr(struct DisassembledInstruction* instruction, struct JdcStr* result);

	const char* getPtrSizeStr(int ptrSize);

	const char* getGroup1PrefixStr(struct DisassembledInstruction* instruction);

#ifdef __cplusplus
}
#endif

static enum JdcStatus memAddressToStr(struct MemoryAddress* memAddr, struct JdcStr* result);