#pragma once
#include "disassemblyStructs.h"

unsigned char handleOperands(struct DisassemblyParameters* params, struct DisassembledInstruction* result);

unsigned long long getUIntFromBytes(unsigned char** bytesPtr, unsigned char resultSize);

unsigned char getSizeOfOperand(struct Operand* operand);

unsigned char compareOperands(struct Operand* op1, struct Operand* op2);