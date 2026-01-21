#pragma once
#include "disassemblyUtils.h"
#include "registers.h"
#include "prefixes.h"
#include "mnemonics.h"

enum OpcodeSuperscript
{
	i64,  // invalid in 64-bit mode
	o64,  // only available in 64-bit mode
	d64,  // operand size defaults to 64-bit size
	f64,  // operand size forced to 64-bit size
	f256, // operand size forced to 256-bit size

	NO_SUPERSCRIPT
};

struct Opcode
{
	enum Mnemonic mnemonic;
	char extensionGroup; // -1 if the opcode is not an extended one. 0 = Group 1; 1 = Group 1A; from there this number corresponds to the actual group number
	enum OperandCode operands[4];
	enum OpcodeSuperscript opcodeSuperscript;
};

unsigned char handleOpcode(unsigned char** bytesPtr, unsigned char* maxBytesAddr, char* hasGotModRMRef, unsigned char* modRMByteRef, struct DisassemblerOptions* disassemblerOptions, struct LegacyPrefixes* legPrefixes, struct REXPrefix* rexPrefix, struct Opcode* result);