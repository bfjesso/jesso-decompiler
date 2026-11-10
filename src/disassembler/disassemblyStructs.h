#pragma once
#include "../jdcTypes.h"
#include "opcodeMnemonics.h"
#include "registers.h"

enum InstructionSetArchitecture
{
	X86_ISA,
	LC3_ISA
};

struct DisassemblerOptions
{
	bool is64BitMode;
};

enum LegacyPrefix
{
	NONE_LP,

	LOCK_LP,      // 0xF0, group 1, ensures exclusive use of shared memory in a multiprocessor environment
	REPNZ_BND_LP, // 0xF2, group 1, reapat if ZF is 0 or bound
	REPZ_LP,      // 0xF3, group 1, repeat if ZF is 1

	CSO_BNT_LP,   // 0x2E, group 2, CS segment override or branch not taken
	SSO_LP,       // 0x36, group 2, SS segment overide
	DSO_BT_LP,    // 0x3E, group 2, DS segment override or branch taken
	ESO_LP,       // 0x26, group 2, ES segment override
	FSO_LP,       // 0x64, group 2, FS segment override
	GSO_LP,       // 0x65, group 2, GS segment override

	OSO_LP,       // 0x66, group 3, operand-size override

	ASO_LP        // 0x67, group 4, address-size override
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

enum OperandCode
{
	NONE_OC,

	ONE_OC, // implied integer value of 1

	AL_X86_OC, // implied AL register
	CL_X86_OC, // implied CL register
	AX_X86_OC, // implied AX register
	DX_X86_OC, // implied DX register

	ES_X86_OC, // implied ES register
	CS_X86_OC, // implied CS register
	SS_X86_OC, // implied SS register
	DS_X86_OC, // implied DS register
	FS_X86_OC, // implied FS register
	GS_X86_OC, // implied GS register

	AX_WDQ_X86_OC, // implied AX, EAX, or RAX register, depending on operand size
	CX_WDQ_X86_OC, // implied CX, ECX, or RCX register, depending on operand size
	DX_WDQ_X86_OC, // implied DX, EDX, or RDX register, depending on operand size
	BX_WDQ_X86_OC, // implied BX, EBX, or RBX register, depending on operand size
	SP_WDQ_X86_OC, // implied SP, ESP, or RSP register, depending on operand size
	BP_WDQ_X86_OC, // implied BP, EBP, or RBP register, depending on operand size
	SI_WDQ_X86_OC, // implied SI, ESI, or RSI register, depending on operand size
	DI_WDQ_X86_OC, // implied DI, EDI, or RDI register, depending on operand size

	AX_R8_WDQ_X86_OC, // implied AX, EAX, RAX, R8D, or R8 register, depending on operand size and REX.B
	CX_R9_WDQ_X86_OC, // implied CX, ECX, RCX, R9D, or R9 register, depending on operand size and REX.B
	DX_R10_WDQ_X86_OC, // implied DX, EDX, RDX, R10D, or R10 register, depending on operand size and REX.B
	BX_R11_WDQ_X86_OC, // implied BX, EBX, RBX, R11D, or R11 register, depending on operand size and REX.B
	SP_R12_WDQ_X86_OC, // implied SP, ESP, RSP, R12D, or R12 register, depending on operand size and REX.B
	BP_R13_WDQ_X86_OC, // implied BP, EBP, RBP, R13D, or R13 register, depending on operand size and REX.B
	SI_R14_WDQ_X86_OC, // implied SI, ESI, RSI, R14D, or R14 register, depending on operand size and REX.B
	DI_R15_WDQ_X86_OC, // implied DI, EDI, RDI, R15D, or R15 register, depending on operand size and REX.B

	AL_R8B_X86_OC, // implied AL or R8B register, depending on REX.B
	CL_R9B_X86_OC, // implied CL or R9B register, depending on REX.B
	DL_R10B_X86_OC, // implied DL or R10B register, depending on REX.B
	BL_R11B_X86_OC, // implied BL or R11B register, depending on REX.B
	AH_R12B_X86_OC, // implied AH or R12B register, depending on REX.B
	CH_R13B_X86_OC, // implied CH or R13B register, depending on REX.B
	DH_R14B_X86_OC, // implied DH or R14B register, depending on REX.B
	BH_R15B_X86_OC, // implied BH or R15B register, depending on REX.B

	ST0_X86_OC, // implied ST(0) register
	ST1_X86_OC, // implied ST(1) register
	ST2_X86_OC, // implied ST(2) register
	ST3_X86_OC, // implied ST(3) register
	ST4_X86_OC, // implied ST(4) register
	ST5_X86_OC, // implied ST(5) register
	ST6_X86_OC, // implied ST(6) register
	ST7_X86_OC, // implied ST(7) register

	RM_B_X86_OC, // ModRM:r/m encodes an 8-bit general register or memory address
	RM_D_X86_OC, // ModRM:r/m encodes a 32-bit general register or memory address
	RM_WDQ_X86_OC, // ModRM:r/m encodes a 16, 32, or 64-bit general register or memory address, depending on operand size
	RM_W_X86_OC, // ModRM:r/m encodes a 16-bit general register or memory address
	RM_DF_X86_OC, // ModRM:r/m encodes a 32 or 48-bit general register or memory address, depending on operand size
	RM_DQ_X86_OC, // ModRM:r/m encodes a 32 or 64-bit general register or memory address, depending on operand size
	RM_64MODE_X86_OC, // ModRM:r/m encodes a 32 or 64-bit general register or memory address, depending on 64-bit mode

	REG_B_X86_OC, // ModRM:reg encodes an 8-bit general register
	REG_D_X86_OC, // ModRM:reg encodes a 32-bit general register
	REG_WDQ_X86_OC, // ModRM:reg encodes a 16, 32, or 64-bit general register, depending on operand size
	REG_WD_X86_OC, // ModRM:reg encodes a 16 or 32-bit general register, depending on operand size
	REG_W_X86_OC, // ModRM:reg encodes a 16-bit general register
	REG_DQ_X86_OC, // ModRM:reg encodes a 32 or 64-bit general register, depending on operand size
	REG_64MODE_X86_OC, // ModRM:reg encodes a 32 or 64-bit general register, depending on 64-bit mode

	VEX_VVVV_DQ_X86_OC, // VEX.vvvv encodes a 32 or 64-bit general register, depending on operand size

	M_B_X86_OC, // ModRM:r/m can only encode an 8-bit memory address
	M_W_X86_OC, // ModRM:r/m can only encode a 16-bit memory address
	M_D_X86_OC, // ModRM:r/m can only encode a 32-bit memory address
	M_WDQ_X86_OC, // ModRM:r/m can only encode a 16, 32, or 64-bit memory address, depending on operand size
	M_OSO_DQ_X86_OC, // ModRM:r/m can only encode a 32 or 64-bit memory address, depending on if operand size override is used
	M_DF_X86_OC, // ModRM:r/m can only encode a 32 or 48-bit memory address, depending on operand size
	M_Q_X86_OC, // ModRM:r/m can only encode a 64-bit memory address
	M_T_X86_OC, // ModRM:r/m can only encode a 80-bit memory address
	M_XYZ_X86_OC, // ModRM:r/m can only encode a 128, 256, or 512-bit memory address, depending on vector length
	M_X_X86_OC, // ModRM:r/m can only encode a 128-bit memory address
	M_DQ_X86_OC, // ModRM:r/m can only encode a 32 or 64-bit memory address, depending on operand size

	IMM_B_X86_OC, // 8-bit immediate
	IMM_WDQ_X86_OC, // 16, 32, or 64-bit immediate, depending on operand size
	IMM_WD_X86_OC, // 16 or 32-bit immediate, depending on operand size
	IMM_W_X86_OC, // 16-bit immediate

	ES_MEM_DI_B_X86_OC, // 8-bit memory address, ES:[rDI]
	ES_MEM_DI_WDQ_X86_OC, // 16, 32, or 64-bit memory address, ES:[rDI], depending on operand size
	ES_MEM_DI_WD_X86_OC, // 16 or 32-bit memory address, ES:[rDI], depending on operand size

	DS_MEM_SI_B_X86_OC, // 8-bit memory address, DS:[rSI]
	DS_MEM_SI_WDQ_X86_OC, // 16, 32, or 64-bit memory address, DS:[rSI], depending on operand size
	DS_MEM_SI_WD_X86_OC, // 16 or 32-bit memory address, DS:[rSI], depending on operand size

	CONST_MEM_B_X86_OC, // 8-bit memory address, where the address is only a constant displacement. No ModR/M byte.
	CONST_MEM_WDQ_X86_OC, // 16, 32, or 64-bit memory address, depending on operand size, where the address is only a constant displacement. No ModR/M byte

	RM_REG_64MODE_X86_OC, // ModRM:r/m encodes a 32 or 64-bit general register, depending on 64-bit mode, regardless of ModRM.mod
	RM_REG_DQ_X86_OC, // ModRM:r/m encodes a 32 or 64-bit general register, depending on operand size, regardless of ModRM.mod
	RM_REG_WDQ_X86_OC, // ModRM:r/m encodes a 16, 32, or 64-bit general register, depending on operand size, regardless of ModRM.mod

	REG_CTRL_X86_OC, // ModRM:reg encodes a control register
	REG_DEBUG_X86_OC, // ModRM:reg encodes a debug register

	R_WDQ_M_W_X86_OC, // ModRM:r/m encodes a 16, 32, or 64-bit general register depending on operand size, or a 16-bit memory address

	REG_SEGMENT_X86_OC, // ModRM:reg encodes a segment register

	CONST_SEG_MEM_X86_OC, // direct memory address where the segment is a constant and the address is only a constant displacement. No ModR/M byte

	REG_MMX_X86_OC, // ModRM:reg encodes an MMX register

	RM_MMX_D_X86_OC, // ModRM:r/m encodes an MMX register or a 32-bit memory address
	RM_MMX_Q_X86_OC, // ModRM:r/m encodes an MMX register or a 64-bit memory address

	RM_MOD11_MMX_X86_OC, // ModRM:r/m encodes an MMX register. ModRM.mod must be 0b11

	RM_MOD11_XYZ_X86_OC, // ModRM:r/m encodes an XMM, YMM, or ZMM register, depending on vector length. ModRM.mod must be 0b11
	RM_MOD11_X_X86_OC, // ModRM:r/m encodes an XMM register. ModRM.mod must be 0b11
	RM_MOD11_REQ_X_X86_OC, // ModRM:r/m encodes an XMM register. ModRM.mod must be 0b11 and vector length must be 128-bit


	REG_XYZ_X86_OC, // ModRM:reg encodes an XMM, YMM, or ZMM register, depending on vector length
	REG_REQ_X_X86_OC, // ModRM:reg encodes an XMM register. Vector length must be 128-bit
	REG_X_X86_OC, // ModRM:reg encodes an XMM register
	REG_REQ_YZ_X86_OC, // ModRM:reg encodes a YMM or ZMM register. Vector length cannot be 128-bit


	RM_XYZ_X86_OC, // ModRM:r/m encodes an XMM, YMM, or ZMM register, or a 128, 256, or 512-bit memory address, depending on vector length
	RM_REQ_X_X86_OC, // ModRM:r/m encodes an XMM register or a 128-bit memory address. Vector length must be 128-bit
	R_X_M_D_X86_OC, // ModRM:r/m encodes an XMM register or a 32-bit memory address
	R_X_M_Q_X86_OC, // ModRM:r/m encodes an XMM register or a 64-bit memory address
	RM_REQ_YZ_X86_OC, // ModRM:r/m encodes a YMM or ZMM register, or a 256 or 512-bit memory address. Vector length cannot be 128-bit

	VVVV_XYZ_X86_OC, // VEX.vvvv or EVEX.vvvv encodes an XMM, YMM, or ZMM register, depending on vector length
	VVVV_REQ_X_X86_OC, // VEX.vvvv or EVEX.vvvv encodes an XMM register. Vector length must be 128-bit
	VVVV_X_X86_OC, // VEX.vvvv or EVEX.vvvv encodes an XMM register
	VVVV_REQ_YZ_X86_OC, // VEX.vvvv or EVEX.vvvv encodes a YMM or ZMM register. Vector length cannot be 128-bit

	U4_XY_X86_OC, // upper 4 bits of an 8-bit immediate encodes an XMM or YMM register, depending on vector length

	EVEX_VVVV_XYZ_X86_OC, // EVEX.vvvv encodes an XMM, YMM, or ZMM register, depending on vector length

	// LC3
	DRSR_LC3_OC, // bits 11-9 encode a destination or source register
	SR1_LC3_OC, // bits 8-6 encode a first source register
	SR2_IMM5_LC3_OC, // bits 5-0 encode either a second source register or a 5-bit immediate, depending on bit 5. if bit 5 is 0, bits 4-3 must be 0
	PCOFFSET9_LC3_OC, // bits 8-0 encode a 9-bit offset from PC
	BASER_PCOFFSET11_LC3_OC, // bits 10-0 encode an 11-bit offset from PC. bit 11 must be 1, otherwise BASER_LC3_OC
	BASER_LC3_OC, // bits 8-6 encode a base register. bits 11-9 and 5-0 must be 0
	BASER_OFFSET6_LC3_OC, // bits 8-6 encode a base register and bits 5-0 encode a 6-bit offset
	TRAPVECT8_LC3_OC, // bits 7-0 encodes the trapvector. bits 10-8 must be 0
};

enum OpcodeProperty
{
	NONE_OP,

	INVALID_64_MODE_OP,  // invalid in 64-bit mode
	ONLY_64_MODE_OP,  // only available in 64-bit mode
	DEFAULT_64_SIZE_OP,  // operand size defaults to 64-bit size
	FORCE_64_SIZE_OP,  // operand size forced to 64-bit size

	// these are tuple types, which are attributes of EVEX encoded instructions
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
	enum OpcodeMnemonic mnemonic;
	enum OperandCode operandCodes[4];
	enum OpcodeProperty opcodeProperty;
};

enum OperandType
{
	NO_OPERAND,

	REGISTER_OPERAND,
	MEMORY_OPERAND,
	IMMEDIATE_OPERAND
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
	int64_t constDisplacement;
	enum Register regSegment;
	enum Register reg;
	enum Register regDisplacement;
	uint16_t constSegment;
	uint8_t ptrSize;
	uint8_t scale; // if SIB byte
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
	enum OpcodeMnemonic opcode;

	struct Operand* operands;
	enum Register opmask;
	bool zeroMasking; // EVEX.z

	uint8_t numOfOperands;
	uint8_t operandSizeAttribute; // this is only used when decompiling PUSH/POP instructions to determine by how much the SP is changed

	uint8_t numOfBytes;
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