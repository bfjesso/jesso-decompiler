#pragma once

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

enum OperandType
{
	NO_OPERAND,
	SEGMENT,
	REGISTER,
	MEM_ADDRESS,
	IMMEDIATE
};

struct LegacyPrefixes
{
	unsigned char group1;
	unsigned char group2;
	unsigned char group3;
	unsigned char group4;
};

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

struct VEXPrefix
{
	unsigned char isValidVEX;

	unsigned char w;
	unsigned char r;
	unsigned char x;
	unsigned char b;

	unsigned char mmmmm;

	unsigned char vvvv;
	unsigned char l;
	unsigned char pp;
};

struct MemoryAddress
{
	unsigned char ptrSize;
	unsigned char segment;
	unsigned short constSegment;

	unsigned char reg;
	unsigned char scale; // if SIB byte
	unsigned char regDisplacement;
	int constDisplacement;
};

struct Operand
{
	union
	{
		unsigned char segment;
		unsigned char reg;
		struct MemoryAddress memoryAddress;
		unsigned long long immediate;
	};

	unsigned char type;
};

struct DisassemblerOptions
{
	char is64BitMode;
};

struct DisassembledInstruction
{
	unsigned char opcode;
	struct Operand operands[3];
	unsigned char group1Prefix;

	unsigned char numOfBytes;
	unsigned char hasBeenDecompiled;
};

static const char* group1PrefixStrs[] =
{
	"LOCK",
	"REPNZ",
	"REPZ"
};

static const char* ptrSizeStrs[] =
{
	"BYTE PTR",
	"WORD PTR",
	"DWORD PTR",
	"FWORD PTR",
	"QWORD PTR",
	"TBYTE PTR"
};

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned char disassembleInstruction(unsigned char* bytes, unsigned char* maxBytesAddr, struct DisassemblerOptions* disassemblerOptions, struct DisassembledInstruction* result);
	unsigned char instructionToStr(struct DisassembledInstruction* instruction, char* buffer, unsigned char bufferSize);

#ifdef __cplusplus
}
#endif

static unsigned char memAddressToStr(struct MemoryAddress* memAddr, char* buffer, unsigned char bufferSize, unsigned char* resultSize);

static unsigned char handleLegacyPrefixes(unsigned char** bytesPtr, unsigned char* maxBytesAddr, struct LegacyPrefixes* result);

static unsigned char handleREXPrefix(unsigned char** bytesPtr, unsigned char* maxBytesAddr, struct REXPrefix* result);

static unsigned char handleVEXPrefix(unsigned char** bytesPtr, unsigned char* maxBytesAddr, struct VEXPrefix* result);

static unsigned char handleOpcode(unsigned char** bytesPtr, unsigned char* maxBytesAddr, char* hasGotModRMRef, unsigned char* modRMByteRef, struct DisassemblerOptions* disassemblerOptions, struct LegacyPrefixes* legPrefixes, struct Opcode* result);

static unsigned char handleOperands(unsigned char** bytesPtr, unsigned char* maxBytesAddr, unsigned char* startBytePtr, char hasGotModRM, unsigned char* modRMByteRef, unsigned char is64BitMode, struct Opcode* opcode, struct LegacyPrefixes* legPrefixes, struct REXPrefix* rexPrefix, struct VEXPrefix* vexPrefix, struct Operand* result);

static unsigned char handleModRM(unsigned char** bytesPtr, unsigned char* maxBytesAddr, char hasGotModRM, unsigned char* modRMByteRef, char getRegOrSeg, unsigned char operandSize, char addressSizeOverride, unsigned char is64bitMode, struct Operand* result);

static unsigned char* handleSIB(unsigned char** bytesPtr, struct Operand* result);

static unsigned long long getUIntFromBytes(unsigned char** bytesPtr, unsigned char resultSize);