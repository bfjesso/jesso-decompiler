#pragma once
#include "opcodes.h"
#include "registers.h"

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
	enum LegacyPrefix group1;
	enum LegacyPrefix group2;
	enum LegacyPrefix group3;
	enum LegacyPrefix group4;
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
	enum Segment segment;
	unsigned short constSegment;

	enum Register reg;
	unsigned char scale; // if SIB byte
	enum Register regDisplacement;
	long long constDisplacement;
};

struct Operand
{
	union
	{
		enum Segment segment;
		enum Register reg;
		struct MemoryAddress memoryAddress;
		long long immediate;
	};

	enum OperandType type;
};

struct DisassemblerOptions
{
	char is64BitMode;
};

struct DisassembledInstruction
{
	enum Mnemonic opcode;
	struct Operand operands[3];
	enum LegacyPrefix group1Prefix;

	unsigned char numOfBytes;
};

#ifdef __cplusplus
extern "C"
{
#endif

	extern const char* ptrSizeStrs[];

	unsigned char disassembleInstruction(unsigned char* bytes, unsigned char* maxBytesAddr, struct DisassemblerOptions* disassemblerOptions, struct DisassembledInstruction* result);
	
	unsigned char instructionToStr(struct DisassembledInstruction* instruction, char* buffer, unsigned char bufferSize);

#ifdef __cplusplus
}
#endif

static unsigned char memAddressToStr(struct MemoryAddress* memAddr, char* buffer, unsigned char bufferSize);

static unsigned char handleLegacyPrefixes(unsigned char** bytesPtr, unsigned char* maxBytesAddr, struct LegacyPrefixes* result);

static unsigned char handleREXPrefix(unsigned char** bytesPtr, unsigned char* maxBytesAddr, struct REXPrefix* result);

static unsigned char handleVEXPrefix(unsigned char** bytesPtr, unsigned char* maxBytesAddr, struct VEXPrefix* result);

static unsigned char handleOpcode(unsigned char** bytesPtr, unsigned char* maxBytesAddr, char* hasGotModRMRef, unsigned char* modRMByteRef, struct DisassemblerOptions* disassemblerOptions, struct LegacyPrefixes* legPrefixes, struct Opcode* result);

static unsigned char handleOperands(unsigned char** bytesPtr, unsigned char* maxBytesAddr, unsigned char* startBytePtr, char hasGotModRM, unsigned char* modRMByteRef, unsigned char is64BitMode, struct Opcode* opcode, struct LegacyPrefixes* legPrefixes, struct REXPrefix* rexPrefix, struct VEXPrefix* vexPrefix, struct Operand* result);

static unsigned char handleModRM(unsigned char** bytesPtr, unsigned char* maxBytesAddr, char hasGotModRM, unsigned char* modRMByteRef, char getRegOrSeg, unsigned char operandSize, char addressSizeOverride, unsigned char is64bitMode, struct Operand* result);

static unsigned char handleSIB(unsigned char** bytesPtr, unsigned char mod, struct Operand* result);

static unsigned long long getUIntFromBytes(unsigned char** bytesPtr, unsigned char resultSize);

unsigned char getLastOperand(struct DisassembledInstruction* instruction);

unsigned char isOperandStackArgument(struct Operand* operand);

unsigned char isOperandLocalVariable(struct Operand* operand);

unsigned char doesInstructionModifyOperand(struct DisassembledInstruction* instruction, unsigned char operandNum, unsigned char* overwrites);

unsigned char doesInstructionAccessRegister(struct DisassembledInstruction* instruction, enum Register reg, unsigned char* operandNum);

unsigned char doesInstructionModifyRegister(struct DisassembledInstruction* instruction, enum Register reg);

unsigned char areOperandsEqual(struct Operand* op1, struct Operand* op2);

unsigned char operandToValue(struct DisassembledInstruction* instructions, unsigned long long* addresses, int startInstructionIndex, struct Operand* operand, unsigned long long* result);