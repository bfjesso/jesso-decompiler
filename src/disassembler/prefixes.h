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

struct EVEXPrefix
{
	unsigned char isValidEVEX;

	// P0
	unsigned char rxb;
	unsigned char r;
	unsigned char mmm;

	// P1
	unsigned char w;
	unsigned char vvvv;
	unsigned char pp;

	// P2
	unsigned char z;
	unsigned char ll;
	unsigned char b;
	unsigned char v;
	unsigned char aaa;
};

unsigned char handleLegacyPrefixes(unsigned char** bytesPtr, unsigned char* maxBytesAddr, struct LegacyPrefixes* result);

unsigned char handleREXPrefix(unsigned char** bytesPtr, unsigned char* maxBytesAddr, struct REXPrefix* result);

unsigned char handleVEXPrefix(unsigned char** bytesPtr, unsigned char* maxBytesAddr, struct LegacyPrefixes* legPrefixes, struct VEXPrefix* result);

unsigned char handleEVEXPrefix(unsigned char** bytesPtr, unsigned char* maxBytesAddr, struct LegacyPrefixes* legPrefixes, struct EVEXPrefix* result);