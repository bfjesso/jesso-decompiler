#pragma once

// Legacy Prefixes

enum LegacyPrefix
{
	// group 2
	// segment overrides
	CSO_BNT, // branch not taken if Jcc (conditional jump) instruction
	SSO,
	DSO_BT, // branch taken if Jcc (conditional jump) instruction
	ESO,
	FSO,
	GSO,
	
	// group 1
	LOCK,
	REPNZ, // repeat not zero
	REPZ, // repeat zero

	// group 3
	OSO, // operand-size override

	// group 4
	ASO, // address-size override

	NO_PREFIX
};

struct LegacyPrefixes
{
	enum LegacyPrefix group1;
	enum LegacyPrefix group2;
	enum LegacyPrefix group3;
	enum LegacyPrefix group4;
	unsigned char numOfPrefixes;
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

static struct REXPrefix handleREXPrefix(unsigned char* bytes, char bytesLen);


// Opcode

static struct Opcode* handleOpcode(unsigned char* bytes, char bytesLen, struct DisassemblerOptions* disassemblerOptions);


// Operands

enum Segment 
{
	CS,
	SS,
	DS,
	ES,
	FS,
	GS,

	NO_SEGMENT
};

enum Register
{
	AL, CL, DL, BL, AH, CH, DH, BH,
	R8B, R9B, R10B, R11B, R12B, R13B, R14B, R15B,
	AX, CX, DX, BX, SP, BP, SI, DI, 
	EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI,
	RAX, RCX, RDX, RBX, RSP, RBP, RSI, RDI,
	R8, R9, R10, R11, R12, R13, R14, R15,

	NO_REG
};

struct MemoryAddress 
{
	union 
	{
		enum Segment segment;
		unsigned short constSegment;
	};
	
	enum Register reg;
	enum Register regDisplacement;
	unsigned int constDisplacement;
	unsigned char scale; // if SIB byte
};

union Operand
{
	enum Segement segment;
	enum Register reg;
	struct MemoryAddress memoryAddress;
	unsigned long long immediate;
};

struct OperandsResult
{
	union Operand operand1;
	union Operand operand2;
	union Operand operand3;
};

static struct OperandsResult handleOperands(unsigned char* bytes, char bytesLen, unsigned char is64BitMode, struct Opcode* opcode, struct LegacyPrefixes* legPrefixes, struct REXPrefix* rexPrefix);

// ModR/M

static union Operand handleModRM(unsigned char* bytes, char bytesLen, unsigned char modRMByte, char getRegOrSeg, unsigned char operandSize, char addressSizeOverride);

// SIB

static union Operand handleSIB(unsigned char* bytes, char bytesLen);


static unsigned long long getUIntFromBytes(unsigned char* bytes, char bytesLen, unsigned char size);


// Result


struct DisassemblerOptions
{
	char is64BitMode;
};

struct DisassembledInstruction
{
	struct LegacyPrefixes legacyPrefixes;
	enum Mnemonic opcode;
	struct OperandsResult operands;

	const char* str;
	unsigned char numOfBytes;
};

const char* instructionToStr(struct DisassembledInstruction* instruction);

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned char disassembleInstruction(unsigned char* bytes, char bytesLen, struct DisassemblerOptions* disassemblerOptions, struct DisassembledInstruction* result);

#ifdef __cplusplus
}
#endif