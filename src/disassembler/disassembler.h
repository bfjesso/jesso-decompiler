#pragma once
#include "disassemblyStructs.h"
#include "../jdc-str/jdcStr.h"

#ifdef __cplusplus
extern "C"
{
#endif

	enum JdcStatus disassembleInstruction(uint8_t* bytes, uint8_t* maxBytesAddr, struct DisassemblerOptions* disassemblerOptions, struct DisassembledInstruction* result);
	
	enum JdcStatus instructionToStr(struct DisassembledInstruction* instruction, struct JdcStr* result);

	const char* getPtrSizeStr(int32_t ptrSize);

	const char* getGroup1PrefixStr(struct DisassembledInstruction* instruction);

#ifdef __cplusplus
}
#endif

static enum JdcStatus memAddressToStr(struct MemoryAddress* memAddr, struct JdcStr* result);