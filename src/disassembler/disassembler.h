#pragma once
#include "disassemblyStructs.h"

#ifdef __cplusplus
extern "C"
{
#endif

	enum JdcStatus disassembleInstruction(enum InstructionSetArchitecture isa, uint8_t* bytes, uint8_t* maxBytesAddr, struct DisassemblerOptions* disassemblerOptions, struct DisassembledInstruction* result);

	enum JdcStatus instructionToStr(struct DisassembledInstruction* instruction, bool evaluateIP, uint8_t addressability, struct JdcStr* result);

	const char* getGroup1PrefixStr(struct DisassembledInstruction* instruction);

	const char* getPtrSizeStr(int32_t ptrSize);

	enum JdcStatus generateDataInstruction(struct DisassembledInstruction* instruction, uint64_t address, uint64_t data, uint8_t dataSize);

#ifdef __cplusplus
}
#endif

static enum JdcStatus memAddressToStr(struct DisassembledInstruction* instruction, struct MemoryAddress* memAddr, bool evaluateIP, uint8_t addressability, struct JdcStr* result);