#pragma once
#include "../disassemblyStructs.h"

// Intel 64 and IA-32 Architectures Software Developer’s Manuals:
//	intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html

// Appendix A: A.4.2 table A-6

// any NONE_OC means that the operand code will be what is in the regular opcode map

// this is only up to group 6. These groups only depend on ModR/M bits 3-5
const struct Opcode extendedOpcodeMapThroughGroupSix[7][8] = // [group index][ModR/M bits 3-5]
{
	{	// group 1
		{ ADD_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ OR_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ ADC_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ SBB_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ AND_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ SUB_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ XOR_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ CMP_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{	// group 1A
		{ POP_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE }
	},
	{	// group 2
		{ ROL_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ ROR_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ RCL_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ RCR_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ SHL_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ SHR_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE },
		{ SAR_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{	// group 3
		{ TEST_X86, NONE_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OP }, // second operands is Iz if opcode is 0xF7
		{ NO_OPCODE },
		{ NOT_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NEG_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ MUL_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ IMUL_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP},
		{ DIV_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ IDIV_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{	// group 4
		{ INC_X86, RM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ DEC_X86, RM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE }
	},
	{	// group 5
		{ INC_X86, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ DEC_X86, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ CALL_NEAR_X86, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, FORCE_64_SIZE_OP },
		{ CALL_FAR_X86, RM_DF_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ JMP_NEAR_X86, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, FORCE_64_SIZE_OP },
		{ JMP_FAR_X86, M_DF_X86_OC, NONE_OC, NONE_OC, NONE_OC, DEFAULT_64_SIZE_OP },
		{ PUSH_X86, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE }
	},
	{	// group 6
		{ SLDT_X86, RM_W_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }, // first operand is Rv/Mw, leaving Ew
		{ STR_X86, RM_W_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }, // first operand is Rv/Mw, leaving Ew
		{ LLDT_X86, RM_W_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ LTR_X86, RM_W_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VERR_X86, RM_W_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VERW_X86, RM_W_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE }
	}
};

// group 7 where Mod 7,6 is not 11B
const struct Opcode extendedOpcodeMapGroup7[8] = // [ModR/M bits 3-5]
{
	{ SGDT_X86, M_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }, // first operand is Ms, leaving Mv
	{ SIDT_X86, M_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }, // first operand is Ms, leaving Mv
	{ LGDT_X86, M_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }, // first operand is Ms, leaving Mv
	{ LIDT_X86, M_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }, // first operand is Ms, leaving Mv
	{ SMSW_X86, RM_W_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }, // first operand is Mw/Rv, leaving Ew
	{ NO_OPCODE },
	{ LMSW_X86, RM_W_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	{ INVLPG_X86, M_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
};

// group 7 where Mod 7,6 is 11B
const struct Opcode extendedOpcodeMapGroup711B[8][8] =  // [ModR/M bits 3-5][ModR/M bits 0-2]
{
	{
		{ NO_OPCODE },
		{ VMCALL_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VMLAUNCH_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VMRESUME_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VMXOFF_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE }
	},
	{
		{ MONITOR_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ MWAIT_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ CLAC_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ STAC_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ ENCLS_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{
		{ XGETBV_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ XSETBV_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ VMFUNC_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ XEND_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ XTEST_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ ENCLU_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE }
	},
	{
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE }
	},
	{
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE }
	},
	{
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE }
	},
	{
		{ SWAPGS_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, ONLY_64_MODE_OP },
		{ RDTSCP_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
		{ NO_OPCODE },
	}
};

// group 8
const struct Opcode extendedOpcodeMapGroup8[8] = // [ModR/M bits 3-5]
{
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ BT_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	{ BTS_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	{ BTR_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	{ BTC_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
};

// group 9 with no prefix and Mod 7,6 is not 11B
const struct Opcode extendedOpcodeMapGroup9[8] = // [ModR/M bits 3-5]
{
	{ NO_OPCODE },
	{ CMPXCH8B_X86, M_Q_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }, // CMPXCHG16B and Mdq if REX.w
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ VMPTRLD_X86, M_Q_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	{ VMPTRST_X86, M_Q_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
};
const struct Opcode alternateCMPXCH = { CMPXCHG16B_X86, M_X_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP };

// group 9 with 0x66 prefix and Mod 7,6 is not 11B
const struct Opcode extendedOpcodeMapGroup966[8] = // [ModR/M bits 3-5]
{
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ VMCLEAR_X86, M_Q_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	{ NO_OPCODE }
};

// group 9 with 0xF3 prefix and Mod 7,6 is not 11B
const struct Opcode extendedOpcodeMapGroup9F3[8] = // [ModR/M bits 3-5]
{
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ VMXON_X86, M_Q_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	{ NO_OPCODE }
};

// group 9 with no prefix and Mod 7,6 is 11B
const struct Opcode extendedOpcodeMapGroup911B[8] = // [ModR/M bits 3-5]
{
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ RDRAND_X86, RM_REG_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	{ RDSEED_X86, RM_REG_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
};

// group 9 with 0xF3 prefix and Mod 7,6 is 11B
const struct Opcode extendedOpcodeMapGroup9F311B[8] = // [ModR/M bits 3-5]
{
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ RDPID_X86, RM_REG_64MODE_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }, // first operand is Rd/q
};

// group 10 is UD1
const struct Opcode extendedOpcodeGroup10 = { UD1_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP };

// group 11 for opcode 0xC6
const struct Opcode extendedOpcodeMapGroup11C6[8] = // [ModR/M bits 3-5]
{
	{ MOV_X86, RM_B_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OP },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ XABORT_X86, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
};

// group 11 for opcode 0xC7
const struct Opcode extendedOpcodeMapGroup11C7[8] = // [ModR/M bits 3-5]
{
	{ MOV_X86, RM_WDQ_X86_OC, IMM_WD_X86_OC, NONE_OC, NONE_OC, NONE_OP },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ XBEGIN_X86, IMM_WD_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
};

// group 12 with no prefix and Mod 7,6 is 11B
const struct Opcode extendedOpcodeMapGroup1211B[8] = // [ModR/M bits 3-5]
{
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ PSRLW_X86, RM_MOD11_MMX_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OP },
	{ NO_OPCODE },
	{ PSRAW_X86, RM_MOD11_MMX_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OP },
	{ NO_OPCODE },
	{ PSLLW_X86, RM_MOD11_MMX_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OP },
	{ NO_OPCODE },
};

// group 12 with 0x66 prefix and Mod 7,6 is 11B
const struct Opcode extendedOpcodeMapGroup126611B[8] = // [ModR/M bits 3-5]
{
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ VPSRLW_X86, VVVV_XYZ_X86_OC, RM_MOD11_XYZ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
	{ NO_OPCODE },
	{ VPSRAW_X86, VVVV_XYZ_X86_OC, RM_MOD11_XYZ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
	{ NO_OPCODE },
	{ VPSLLW_X86, VVVV_XYZ_X86_OC, RM_MOD11_XYZ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
	{ NO_OPCODE },
};

// group 13 with no prefix and Mod 7,6 is 11B
const struct Opcode extendedOpcodeMapGroup1311B[8] = // [ModR/M bits 3-5]
{
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ PSRLD_X86, RM_MOD11_MMX_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OP },
	{ NO_OPCODE },
	{ PSRAD_X86, RM_MOD11_MMX_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OP },
	{ NO_OPCODE },
	{ PSLLD_X86, RM_MOD11_MMX_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OP },
	{ NO_OPCODE },
};

// group 13 with 0x66 prefix and Mod 7,6 is 11B
const struct Opcode extendedOpcodeMapGroup136611B[8] = // [ModR/M bits 3-5]
{
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ VPSRLD_X86, VVVV_XYZ_X86_OC, RM_MOD11_XYZ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
	{ NO_OPCODE },
	{ VPSRAD_X86, VVVV_XYZ_X86_OC, RM_MOD11_XYZ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
	{ NO_OPCODE },
	{ VPSLLD_X86, VVVV_XYZ_X86_OC, RM_MOD11_XYZ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
	{ NO_OPCODE },
};

// group 14 with no prefix and Mod 7,6 is 11B
const struct Opcode extendedOpcodeMapGroup1411B[8] = // [ModR/M bits 3-5]
{
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ PSRLQ_X86, RM_MOD11_MMX_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OP },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ PSLLQ_X86, RM_MOD11_MMX_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OP },
	{ NO_OPCODE },
};

// group 14 with 0x66 prefix and Mod 7,6 is 11B
const struct Opcode extendedOpcodeMapGroup146611B[8] = // [ModR/M bits 3-5]
{
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ VPSRLQ_X86, VVVV_XYZ_X86_OC, RM_MOD11_XYZ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
	{ VPSRLDQ_X86, VVVV_XYZ_X86_OC, RM_MOD11_XYZ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ VPSLLQ_X86, VVVV_XYZ_X86_OC, RM_MOD11_XYZ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
	{ VPSLLDQ_X86, VVVV_XYZ_X86_OC, RM_MOD11_XYZ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
};

// group 15 with no prefix and Mod 7,6 is not 11B
const struct Opcode extendedOpcodeMapGroup15[8] = // [ModR/M bits 3-5]
{
	{ FXSAVE_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	{ FXRSTOR_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	{ LDMXCSR_X86, M_D_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	{ STMXCSR_X86, M_D_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	{ XSAVE_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	{ XRSTOR_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	{ XSAVEOPT_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	{ CLFLUSH_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
};

// group 15 with no prefix and Mod 7,6 is 11B
const struct Opcode extendedOpcodeMapGroup1511B[8] = // [ModR/M bits 3-5]
{
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ LFENCE_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	{ MFENCE_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	{ SFENCE_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
};

// group 15 with 0xF3 prefix and Mod 7,6 is 11B
const struct Opcode extendedOpcodeMapGroup15F311B[8] = // [ModR/M bits 3-5]
{
	{ RDFSBASE_X86, RM_REG_DQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	{ RDGSBASE_X86, RM_REG_DQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	{ WRFSBASE_X86, RM_REG_DQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	{ WRGSBASE_X86, RM_REG_DQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE }
};

// group 16
const struct Opcode extendedOpcodeMapGroup16[8] = // [ModR/M bits 3-5]
{
	{ PREFETCHNTA_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	{ PREFETCHT0_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	{ PREFETCHT1_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	{ PREFETCHT2_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE }
};

// group 17
const struct Opcode extendedOpcodeMapGroup17[8] = // [ModR/M bits 3-5]
{
	{ NO_OPCODE },
	{ BLSR_X86, VEX_VVVV_DQ_X86_OC, RM_DQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
	{ BLSMSK_X86, VEX_VVVV_DQ_X86_OC, RM_DQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
	{ BLSI_X86, VEX_VVVV_DQ_X86_OC, RM_DQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
	{ NO_OPCODE },
};