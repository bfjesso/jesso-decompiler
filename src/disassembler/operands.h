#pragma once
#include "disassemblyStructs.h"

unsigned char handleOperands(struct DisassemblyParameters* params, struct Operand* result);

unsigned long long getUIntFromBytes(unsigned char** bytesPtr, unsigned char resultSize);