#pragma once
#include "disassemblyStructs.h"

unsigned char handleOperands(struct DisassemblyParameters* params, struct DisassembledInstruction* result);

unsigned long long getUIntFromBytes(unsigned char** bytesPtr, unsigned char resultSize);

unsigned char getVectorLength(struct DisassemblyParameters* params);