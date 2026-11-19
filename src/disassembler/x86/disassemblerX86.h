#pragma once
#include "../disassemblyStructs.h"
#include "../../jdc-str/jdcStr.h"

enum JdcStatus disassembleX86(uint8_t* bytes, uint8_t* maxBytesAddr, bool is64BitMode, struct DisassembledInstruction* result);

enum JdcStatus generateInstructionInfoStrX86(uint8_t* bytes, uint8_t numOfBytes, bool is64BitMode, struct JdcStr* result);