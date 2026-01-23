#pragma once
#include "disassemblyStructs.h"
#include "opcodes.h"
#include "prefixes.h"
#include "registers.h"

unsigned char handleOperands(unsigned char** bytesPtr, unsigned char* maxBytesAddr, unsigned char* startBytePtr, char hasGotModRM, unsigned char* modRMByteRef, unsigned char is64BitMode, struct Opcode* opcode, struct LegacyPrefixes* legPrefixes, struct REXPrefix* rexPrefix, struct VEXPrefix* vexPrefix, struct Operand* result);

unsigned long long getUIntFromBytes(unsigned char** bytesPtr, unsigned char resultSize);