#pragma once

enum LegacyPrefix
{
	// group 1
	LOCK,    // 0xF0
	REPNZ,   // 0xF2
	REPZ,    // 0xF3
	// group 2
	CSO_BNT, // 0x2E
	SSO,     // 0x36
	DSO_BT,  // 0x3E
	ESO,     // 0x26
	FSO,     // 0x64
	GSO,     // 0x65
	// group 3
	OSO,     // 0x66
	// group 4
	ASO,     // 0x67

	NO_PREFIX
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

	unsigned char W; // 64 bit operand size if 1
	unsigned char R; // extension of the ModR/M reg field
	unsigned char X; // extension of the SIB index field
	unsigned char B; // extension of the ModR/M r/m field, SIB base field, or opcode reg field
};

struct VEXPrefix
{
	unsigned char isValidVEX;
	
	unsigned char R; // inverted REX.R

	// only in 3 byte VEX
	unsigned char X; // inverted REX.X
	unsigned char B; // inverted REX.B
	unsigned char m_mmmm; // implied opcode map
	unsigned char W; // same as REX.W

	unsigned char vvvv; // register specifier encoded in 1's compliment (inverted)
	unsigned char L; // vector length
	unsigned char pp; // implied legacy prefix for opcode extension
};

struct EVEXPrefix
{
	unsigned char isValidEVEX;

	// P0
	unsigned char R; // combine with ModR/M.reg
	unsigned char X; // combine with EVEX.B and ModR/M.rm, when SIB/VSIB absent
	unsigned char B; // combine with ModR/M.rm
	unsigned char R_prime; // high 16 register specifier modifier
	unsigned char mmm; // implied opcode map

	// P1
	unsigned char W; // operand size promotion/opcode extension
	unsigned char vvvv; // same as VEX.vvvv
	unsigned char pp; // same as VEX.pp

	// P2
	unsigned char z; // zeroing/merging
	unsigned char LL; // vector length/RC
	unsigned char b; // broadcast/RC/SAE context
	unsigned char V_prime; // high 16 VVVV/VIDX register specifier
	unsigned char aaa; // embedded opmask register specifier
};

unsigned char handleLegacyPrefixes(unsigned char** bytesPtr, unsigned char* maxBytesAddr, struct LegacyPrefixes* result);

unsigned char handleREXPrefix(unsigned char** bytesPtr, unsigned char* maxBytesAddr, struct REXPrefix* result);

unsigned char handleVEXPrefix(unsigned char** bytesPtr, unsigned char* maxBytesAddr, struct LegacyPrefixes* legPrefixes, struct VEXPrefix* result);

unsigned char handleEVEXPrefix(unsigned char** bytesPtr, unsigned char* maxBytesAddr, struct LegacyPrefixes* legPrefixes, struct EVEXPrefix* result);