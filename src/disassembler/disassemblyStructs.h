#pragma once
#include "../jdcTypes.h"
#include "mnemonics.h"
#include "registers.h"

struct DisassemblerOptions
{
	bool is64BitMode;
};

enum LegacyPrefix
{
	NO_PREFIX,
	
	// group 1
	LOCK,      // 0xF0
	REPNZ_BND, // 0xF2
	REPZ,      // 0xF3
	// group 2
	CSO_BNT,   // 0x2E
	SSO,       // 0x36
	DSO_BT,    // 0x3E
	ESO,       // 0x26
	FSO,       // 0x64
	GSO,       // 0x65
	// group 3
	OSO,       // 0x66
	// group 4
	ASO        // 0x67
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
	bool isValidREX;

	uint8_t W; // 64 bit operand size if 1
	uint8_t R; // extension of the ModR/M reg field
	uint8_t X; // extension of the SIB index field
	uint8_t B; // extension of the ModR/M r/m field, SIB base field, or opcode reg field
};

struct VEXPrefix
{
	bool isValidVEX;

	uint8_t R; // inverted REX.R

	// only in 3 byte VEX
	uint8_t X; // inverted REX.X
	uint8_t B; // inverted REX.B
	uint8_t m_mmmm; // implied opcode map
	uint8_t W; // same as REX.W

	uint8_t vvvv; // register specifier encoded in 1's compliment (inverted)
	uint8_t L; // vector length
	uint8_t pp; // implied legacy prefix for opcode extension
};

struct EVEXPrefix
{
	bool isValidEVEX;

	// P0
	uint8_t R; // combine with ModR/M.reg
	uint8_t X; // combine with EVEX.B and ModR/M.rm, when SIB/VSIB absent
	uint8_t B; // combine with ModR/M.rm
	uint8_t R_prime; // high 16 register specifier modifier
	uint8_t mmm; // implied opcode map

	// P1
	uint8_t W; // operand size promotion/opcode extension
	uint8_t vvvv; // same as VEX.vvvv
	uint8_t pp; // same as VEX.pp

	// P2
	uint8_t z; // zeroing/merging
	uint8_t LL; // vector length/RC
	uint8_t b; // broadcast/RC/SAE context
	uint8_t V_prime; // high 16 VVVV/VIDX register specifier
	uint8_t aaa; // embedded opmask register specifier
};

// Appendix A: A.2
// "Operands are identified by a two-character code of the form Zz. The first character, an uppercase letter, specifies
//	the addressing method; the second character, a lowercase letter, specifies the type of operand."
enum OperandCode
{
	NO_OPERAND_CODE,
	
	ONE,
	AL_CODE, CL_CODE, AX_CODE, DX_CODE,
	ES_CODE, CS_CODE, SS_CODE, DS_CODE, FS_CODE, GS_CODE,
	rAX, rCX, rDX, rBX, rSP, rBP, rSI, rDI,
	rAX_r8, rCX_r9, rDX_r10, rBX_r11, rSP_r12, rBP_r13, rSI_r14, rDI_r15,
	AL_R8B, CL_R9B, DL_R10B, BL_R11B, AH_R12B, CH_R13B, DH_R14B, BH_R15B,
	ST0_CODE, ST1_CODE, ST2_CODE, ST3_CODE, ST4_CODE, ST5_CODE, ST6_CODE, ST7_CODE,
	Eb, Ed, Ev, Ew, Ep, Ey,
	Gb, Gd, Gv, Gz, Gw, Gy,
	By,
	M, Mb, Mw, Md, Mv, Mp, Ma, Mq, Mt, Mps, Mpd, Mdq, My, Mx,
	Ib, Iv, Iz, Iw,
	Yb, Yv, Yz,
	Xb, Xv, Xz,
	Jb, Jz,
	Nq,
	Ob, Ov,
	Rd, Ry, Rv,
	Cd,
	Dd,
	Sw,
	Ap,
	Pd, Ppi, Pq,
	Qd, Qpi, Qq,
	Ups, Upd, Udq, Uq, Ux,
	Vps, Vpd, Vx, Vss, Vsd, Vdq, Vqq, Vy, Vq,
	Wps, Wpd, Wss, Wsd, Wdq, Wqq, Wd, Wx, Wq,
	Hps, Hpd, Hx, Hss, Hsd, Hdq, Hqq, Hq,
	Lx,

	EVEXvvvv
};

enum OpcodeSuperscript
{
	NO_SUPERSCRIPT,
	
	i64,  // invalid in 64-bit mode
	o64,  // only available in 64-bit mode
	d64,  // operand size defaults to 64-bit size
	f64,  // operand size forced to 64-bit size

	// these are tuple types, which are attributes of EVEX encoded instructions. theyre not superscripts in the opcode tables, but im using this field to store it anyway.
	// the number before "TT" is the input element size. this is used for disp8*N. see Table 2-34 and 2-35.
	// i still need to go back and fill in the tuple types for most instructions in the maps.
	FULL_32_TT, FULL_64_TT,
	HALF_32_TT,
	FULL_MEM_TT,
	TUPLE1_SCALAR_8_TT, TUPLE1_SCALAR_16_TT, TUPLE1_SCALAR_32_TT, TUPLE1_SCALAR_64_TT,
	TUPLE1_FIXED_32_TT, TUPLE1_FIXED_64_TT,
	TUPLE2_32_TT, TUPLE2_64_TT,
	TUPLE4_32_TT, TUPLE4_64_TT,
	TUPLE8_32_TT,
	HALF_MEM_TT,
	QUARTER_MEM_TT,
	EIGHTH_MEM_TT,
	MEM128_TT,
	MOVDDUP_TT,
};

struct Opcode
{
	enum Mnemonic mnemonic;
	int8_t extensionGroup; // -1 if the opcode is not an extended one. 0 = Group 1; 1 = Group 1A; from there this number corresponds to the actual group number
	enum OperandCode operands[4];
	enum OpcodeSuperscript opcodeSuperscript;
};

enum OperandType
{
	NO_OPERAND,

	SEGMENT,
	REGISTER,
	MEM_ADDRESS,
	IMMEDIATE
};

struct ModRM
{
	bool hasGotModRM;

	uint8_t mod;
	uint8_t reg;
	uint8_t rm;
};

struct MemoryAddress
{
	uint8_t ptrSize;
	enum Segment segment;
	uint16_t constSegment;

	enum Register reg;
	uint8_t scale; // if SIB byte
	enum Register regDisplacement;
	int64_t constDisplacement;
};

struct Immediate 
{
	int64_t value;
	uint8_t size;
};

struct Operand
{
	union
	{
		enum Segment segment;
		enum Register reg;
		struct MemoryAddress memoryAddress;
		struct Immediate immediate;
	};

	enum OperandType type;
};

struct DisassemblyParameters 
{
	uint8_t* bytes;
	uint8_t* maxBytesAddr;
	uint8_t* startBytePtr;
	
	bool is64BitMode;
	
	struct LegacyPrefixes legPrefixes;
	struct REXPrefix rexPrefix;
	struct VEXPrefix vexPrefix;
	struct EVEXPrefix evexPrefix;
	struct Opcode opcode;
	struct ModRM modRM;
};

struct DisassembledInstruction
{
	enum LegacyPrefix group1Prefix;
	enum Mnemonic opcode;

	struct Operand* operands;
	uint8_t numOfOperands;
	uint8_t operandSizeAttribute; // this is only used when decompiling PUSH/POP instructions to determine by how much the SP is changed
	
	uint8_t numOfBytes;
	bool isInvalid;
	bool isCalled;
	bool isJmpDst;

	uint64_t address;
};

struct JumpTable 
{
	uint64_t jmpTableAddress;
	uint64_t indirectTableAddress;
	uint64_t jmpInstructionAddress;
	uint8_t addressSize;
};