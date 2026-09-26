#pragma once
#include "disassemblyStructs.h"

enum JdcStatus handleOperands(struct DisassemblyParameters* params, struct DisassembledInstruction* result);

unsigned long long getUIntFromBytes(unsigned char** bytesPtr, unsigned char resultSize);

unsigned char getVectorLength(struct DisassemblyParameters* params);