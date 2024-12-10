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

	NO_PREFIX
};

struct LegacyPrefixes
{
	enum LegacyPrefix group1;
	enum LegacyPrefix group2;
	enum LegacyPrefix group3;
	enum LegacyPrefix group4;
};

static struct LegacyPrefixes handleLegacyPrefixes(unsigned char* bytes);


// REX Prefix

struct REXPrefix
{
	unsigned char isValidREX;
	
	// fixed bits: 0100
	unsigned char w; // bit position 3
	unsigned char r; // bit position 2
	unsigned char x; // bit position 1
	unsigned char b; // bit position 0
	// REX byte: 0100WRXB
};

static struct REXPrefix handleREXPrefix(unsigned char* bytes);


// Opcode

static struct Opcode* handleOpcode(unsigned char* bytes, struct DisassemblerOptions* disassemblerOptions);


// Operands

enum Register
{
	AL,
	CL,
	DL,
	BL,
	AH,
	CH,
	DH,
	BH,
	R8B,
	R9B,
	R10B,
	R11B,
	R12B,
	R13B,
	R14B,
	R15B,
	AX,
	CX,
	DX,
	BX,
	SP,
	BP,
	SI,
	DI,
	EAX,
	ECX,
	EDX,
	EBX,
	ESP,
	EBP,
	ESI,
	EDI,
	RAX,
	RCX,
	RDX,
	RBX,
	RSP,
	RBP,
	RSI,
	RDI,
	R8,
	R9,
	R10,
	R11,
	R12,
	R13,
	R14,
	R15,

	NO_REG
};

struct MemoryAddress 
{
	enum Register reg;
	enum Register regDisplacement;
	unsigned int constDisplacement;
};

union Operand
{
	enum Register reg;
	struct MemoryAddress memoryAddress;
	int immediate;
};

struct OperandsResult
{
	union Operand operand1;
	union Operand operand2;
	union Operand operand3;
};

static struct OperandsResult handleOperands(unsigned char* bytes, unsigned char is64BitMode, struct OpcodeResult* opcode, struct LegacyPrefixes* legPrefixes, struct REXPrefix* rexPrefix);

// ModR/M

const char* handleModRM(unsigned char* bytes, unsigned char is64BitMode, struct OpcodeResult* opcode, struct LegacyPrefixes* legPrefixes, struct REXPrefix* rexPrefix);