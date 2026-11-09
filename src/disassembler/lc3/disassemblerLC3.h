#pragma once
#include "../disassemblyStructs.h"

enum JdcStatus disassembleLC3(uint8_t* bytes, uint8_t* maxBytesAddr, struct DisassembledInstruction* result);

static enum JdcStatus readSignedIntFromLowerBits(uint16_t instructionData, uint8_t numOfBits, int64_t* result);