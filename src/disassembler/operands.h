#pragma once
#include "disassemblyStructs.h"

enum JdcStatus handleOperands(struct DisassemblyParameters* params, struct DisassembledInstruction* result);

static enum JdcStatus getImmediate(struct DisassemblyParameters* params, uint8_t immediateSize, struct Operand* result);

enum JdcStatus readIntFromBytes(struct DisassemblyParameters* params, uint8_t sizeOfInt, int64_t* result);

uint8_t getVectorLength(struct DisassemblyParameters* params);