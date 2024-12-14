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
};

static unsigned char handleLegacyPrefixes(unsigned char** bytesPtr, unsigned char* maxBytesAddr, struct LegacyPrefixes* result);


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

static unsigned char handleREXPrefix(unsigned char** bytesPtr, unsigned char* maxBytesAddr, struct REXPrefix* result);


// Opcode

static unsigned char handleOpcode(unsigned char** bytesPtr, unsigned char* maxBytesAddr, char* hasGotModRMRef, unsigned char* modRMByteRef, struct DisassemblerOptions* disassemblerOptions, struct Opcode* result);


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

const char* segmentStrs[] = 
{
	"CS",
	"SS",
	"DS",
	"ES",
	"FS",
	"GS"
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


const char* registerStrs[] = 
{
	"AL", "CL", "DL", "BL", "AH", "CH", "DH", "BH",
	"R8B", "R9B", "R10B", "R11B", "R12B", "R13B", "R14B", "R15B",
	"AX", "CX", "DX", "BX", "SP", "BP", "SI", "DI",
	"EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI",
	"RAX", "RCX", "RDX", "RBX", "RSP", "RBP", "RSI", "RDI",
	"R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15"
};

const char* ptrSizeStrs[] =
{
	"BYTE PTR",
	"WORD PTR",
	"DWORD PTR",
	"FWORD PTR",
	"QWORD PTR"
};


struct MemoryAddress 
{
	unsigned char ptrSize;
	enum Segment segment;
	unsigned short constSegment;

	enum Register reg;
	unsigned char scale; // if SIB byte
	enum Register regDisplacement;
	int constDisplacement;
};

enum OperandType 
{
	NO_OPERAND,
	SEGMENT,
	REGISTER,
	MEM_ADDRESS,
	IMMEDIATE
};

struct Operand
{
	union
	{
		enum Segement segment;
		enum Register reg;
		struct MemoryAddress memoryAddress;
		unsigned long long immediate;
	};

	enum OperandType type;
};

static unsigned char handleOperands(unsigned char** bytesPtr, unsigned char* maxBytesAddr, char hasGotModRM, unsigned char* modRMByteRef, unsigned char is64BitMode, struct Opcode* opcode, struct LegacyPrefixes* legPrefixes, struct REXPrefix* rexPrefix, struct Operand* result);

// ModR/M

static unsigned char handleModRM(unsigned char** bytesPtr, unsigned char* maxBytesAddr, char hasGotModRM, unsigned char* modRMByteRef, char getRegOrSeg, unsigned char operandSize, char addressSizeOverride, unsigned char is64bitMode, struct Operand* result);

// SIB

static unsigned char* handleSIB(unsigned char** bytesPtr, struct Operand* result);


static unsigned long long getUIntFromBytes(unsigned char** bytesPtr, unsigned char resultSize);

// Result


struct DisassemblerOptions
{
	char is64BitMode;
};

struct DisassembledInstruction
{
	struct LegacyPrefixes legacyPrefixes;
	enum Mnemonic opcode;
	struct Operand operands[3];

	unsigned char numOfBytes;
};

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned char disassembleInstruction(unsigned char* bytes, unsigned char* maxBytesAddr, struct DisassemblerOptions* disassemblerOptions, struct DisassembledInstruction* result);
	void instructionToStr(struct DisassembledInstruction* instruction, char* buffer, unsigned char bufferSize);

#ifdef __cplusplus
}
#endif

static void memAddressToStr(struct MemoryAddress* memAddr, char* buffer, unsigned char bufferSize, unsigned char* resultSize);