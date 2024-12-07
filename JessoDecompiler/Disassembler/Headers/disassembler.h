#pragma once

struct DisassemblerOptions
{
	char is64BitMode;
};

struct DisassembledInstruction
{
	const char* temp;
	unsigned char numOfBytes;
};

#ifdef __cplusplus
extern "C"
{
#endif

	struct DisassembledInstruction disassembleInstruction(unsigned char* bytes, struct DisassemblerOptions* disassemblerOptions);

#ifdef __cplusplus
}
#endif


// Legacy Prefixes

enum LegacyPrefix
{
	NO_PREFIX,

	// group 1
	LOCK = 0xF0,
	REPNZ = 0xF2, // repeat not zero
	REPZ = 0xF3, // repeat zero

	// group 2
	// segment overrides
	CS_BNT = 0x2E, // branch not taken if Jcc (conditional jump) instruction
	SS = 0x36,
	DS_BT = 0x3E, // branch taken if Jcc (conditional jump) instruction
	ES = 0x26,
	FS = 0x64,
	GS = 0x65,

	// group 3
	OSO = 0x66, // operand-size override

	// group 4
	ASO = 0x67, // address-size override
};

struct LegacyPrefixesResult
{
	enum prefix group1;
	enum prefix group2;
	enum prefix group3;
	enum prefix group4;
	unsigned char numOfPrefixes;
};

static struct LegacyPrefixesResult handleLegacyPrefixes(unsigned char* bytes);


// REX Prefix

struct REXPrefixResult 
{
	unsigned char isValidREX;
	
	// fixed bits: 0100
	unsigned char w; // bit position 3
	unsigned char r; // bit position 2
	unsigned char x; // bit position 1
	unsigned char b; // bit position 0
	// REX byte: 0100WRXB
};

static struct REXPrefixResult handleREXPrefix(unsigned char* bytes);


// Opcode

enum OpcodeEscape
{
	NO_ESCAPE, // one byte opcode
	x0F, // two byte opcode
	x0Fx38, // three byte opcode
	x0Fx3A // three byte opcode
};

struct OpcodeResult 
{
	struct Opcode opcode;
	unsigned char numOfBytes;
};

static struct OpcodeResult handleOpcode(unsigned char* bytes);
