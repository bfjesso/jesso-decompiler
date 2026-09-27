#pragma once
#include "disassemblyStructs.h"

enum JdcStatus handleOperands(struct DisassemblyParameters* params, struct DisassembledInstruction* result);

uint64_t getUIntFromBytes(uint8_t** bytesPtr, uint8_t resultSize);

uint8_t getVectorLength(struct DisassemblyParameters* params);