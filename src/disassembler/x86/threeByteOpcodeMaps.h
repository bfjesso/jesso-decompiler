#pragma once
#include "../disassemblyStructs.h"

// Intel 64 and IA-32 Architectures Software Developer’s Manuals:
//	intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html

// Appendix A: A.3 table A-4
const struct Opcode threeByteOpcodeMap38[0x100][4] = // [byte][prefix] 0 - no prefix; 1 - 0x66; 2 - 0xF3; 3 - 0xF2  (not all opcodes have different definitions for each prefix or any of them)
{
	{                                                                                                            // 0x00
		{ PSHUFB_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPSHUFB_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x01
		{ PHADDW_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPHADDW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x02
		{ PHADDD_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPHADDD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x03
		{ PHADDSW_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPHADDSW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x04
		{ PMADDUBSW_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMADDUBSW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x05
		{ PHSUBW_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPHSUBW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x06
		{ PHSUBD_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPHSUBD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x07
		{ PHSUBSW_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPHSUBSW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x08
		{ PSIGNB_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPSIGNB_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x09
		{ PSIGNW_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPSIGNW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x0A
		{ PSIGND_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPSIGND_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x0B
		{ PMULHRSW_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMULHRSW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x0C
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPERMILPS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x0D
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPERMILPD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x0E
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VTESTPS_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x0F
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VTESTPD_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x10
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ PBLENDVB_X86, REG_X_X86_OC, RM_REQ_X_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x11; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x12; not defined
	{                                                                                                            // 0x13
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VCVTPH2PS_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x14
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ BLENDVPS_X86, REG_X_X86_OC, RM_REQ_X_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x15
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ BLENDVPD_X86, REG_X_X86_OC, RM_REQ_X_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x16
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPERMPS_X86, REG_REQ_YZ_X86_OC, VVVV_REQ_YZ_X86_OC, RM_REQ_YZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x17
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPTEST_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x18; 0x66 prefix isn't defined for 1 in this row ?
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VBROADCASTSS_X86, REG_XYZ_X86_OC, R_X_M_D_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x19
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VBROADCASTSD_X86, REG_REQ_YZ_X86_OC, RM_REQ_X_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x1A
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VBROADCASTF128_X86, REG_REQ_YZ_X86_OC, M_X_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x1B; not defined
	{                                                                                                            // 0x1C
		{ PABSB_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPABSB_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x1D
		{ PABSW_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPABSW_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x1E
		{ PABSD_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPABSD_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x1F; not defined
	{                                                                                                            // 0x20
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMOVSXBW_X86, REG_XYZ_X86_OC, RM_MOD11_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP }, // second operand is Ux/Mq ?
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x21
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMOVSXBD_X86, REG_XYZ_X86_OC, RM_MOD11_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP }, // second operand is Ux/Md ?
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x22
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMOVSXBQ_X86, REG_XYZ_X86_OC, RM_MOD11_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP }, // second operand is Ux/Mw ?
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x23
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMOVSXWD_X86, REG_XYZ_X86_OC, RM_MOD11_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP }, // second operand is Ux/Mq ?
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x24
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMOVSXWQ_X86, REG_XYZ_X86_OC, RM_MOD11_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP }, // second operand is Ux/Md ?
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x25
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMOVSXDQ_X86, REG_XYZ_X86_OC, RM_MOD11_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP }, // second operand is Ux/Mq ?
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x26; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x27; not defined
	{                                                                                                            // 0x28
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMULDQ_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x29
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPCMPEQQ_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x2A
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VMOVNTDQA_X86, REG_XYZ_X86_OC, M_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x2B
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPACKUSDW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x2C
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VMASKMOVPS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, M_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x2D
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VMASKMOVPD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, M_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x2E
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VMASKMOVPS_X86, M_XYZ_X86_OC, VVVV_XYZ_X86_OC, REG_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x2F
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VMASKMOVPD_X86, M_XYZ_X86_OC, VVVV_XYZ_X86_OC, REG_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x30
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMOVZXBW_X86, REG_XYZ_X86_OC, RM_MOD11_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP }, // second operand is Ux/Mq ?
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x31
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMOVZXBD_X86, REG_XYZ_X86_OC, RM_MOD11_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP }, // second operand is Ux/Md ?
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x32
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMOVZXBQ_X86, REG_XYZ_X86_OC, RM_MOD11_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP }, // second operand is Ux/Mw ?
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x33
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMOVZXWD_X86, REG_XYZ_X86_OC, RM_MOD11_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP }, // second operand is Ux/Mq ?
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x34
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMOVZXWQ_X86, REG_XYZ_X86_OC, RM_MOD11_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP }, // second operand is Ux/Md ?
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x35
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMOVZXDQ_X86, REG_XYZ_X86_OC, RM_MOD11_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP }, // second operand is Ux/Mq ?
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x36
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPERMD_X86, REG_REQ_YZ_X86_OC, VVVV_REQ_YZ_X86_OC, RM_REQ_YZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x37
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPCMPGTQ_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x38
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMINSB_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x39
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMINSD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x3A
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMINUW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x3B
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMINUD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x3C
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMAXSB_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x3D
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMAXSD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x3E
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMAXUW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x3F
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMAXUD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x40
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMULLD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x41
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPHMINPOSUW_X86, REG_X_X86_OC, RM_REQ_X_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x42; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x43; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x44; not defined
	{                                                                                                            // 0x45
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPSRLVD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VPSRLVQ if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x46
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPSRAVD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x47
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPSLLVD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VPSLLVQ if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x48; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x49; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x4A; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x4B; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x4C; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x4D; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x4E; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x4F; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x50; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x51; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x52; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x53; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x54; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x55; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x56; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x57; not defined
	{                                                                                                            // 0x58
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPBROADCASTD_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x59
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPBROADCASTQ_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x5A
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VBROADCASTI128_X86, REG_REQ_YZ_X86_OC, M_X_X86_OC, NONE_OC, NONE_OC, NONE_OP }, // VBROADCASTI64X2 if EVEX.W = 1, VBROADCASTI32X4 if EVEX.W = 0
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x5B; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x5C; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x5D; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x5E; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x5F; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x60; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x61; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x62; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x63; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x64; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x65; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x66; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x67; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x68; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x69; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x6A; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x6B; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x6C; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x6D; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x6E; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x6F; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x70; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x71; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x72; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x73; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x74; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x75; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x76; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x77; not defined
	{                                                                                                            // 0x78
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPBROADCASTB_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x79
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPBROADCASTW_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x7A; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x7B; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x7C; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x7D; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x7E; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x7F; not defined
	{                                                                                                            // 0x80
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ INVEPT_X86, REG_64MODE_X86_OC, M_X_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x81
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ INVVPID_X86, REG_64MODE_X86_OC, M_X_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x82
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ INVPCID_X86, REG_DQ_X86_OC, M_X_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x83; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x84; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x85; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x86; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x87; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x88; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x89; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x8A; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x8B; not defined
	{                                                                                                            // 0x8C
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMASKMOVD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, M_XYZ_X86_OC, NONE_OC, NONE_OP }, // VPMASKMOVQ if VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x8D; not defined
	{                                                                                                            // 0x8E
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMASKMOVD_X86, M_XYZ_X86_OC, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, NONE_OC, NONE_OP }, // VPMASKMOVQ if VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x8F; not defined
	{                                                                                                            // 0x90
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VGATHERDD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // undocumented? assuming alternate mnemonic is VGATHERDQ when EVEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x91
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VGATHERQD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // undocumented? assuming alternate mnemonic is VGATHERQQ when EVEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x92
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VGATHERDPS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VGATHERDPD if EVEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x93
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VGATHERQPS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VGATHERQPD if EVEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x94; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x95; not defined
	{                                                                                                            // 0x96
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFMADDSUB132PS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFMADDSUB132PD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x97
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFMSUBADD132PS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFMSUBADD132PD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x98
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFMADD132PS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFMADD132PD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x99
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFMADD132SS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFMADD132SD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x9A
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFMSUB132PS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFMSUB132PD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x9B
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFMSUB132SS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFMSUB132SD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x9C
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFNMADD132PS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFNMADD132PD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x9D
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFNMADD132SS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFNMADD132SD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x9E
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFNMSUB132PS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFNMSUB132PD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x9F
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFNMSUB132SS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFNMSUB132SD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xA0; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xA1; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xA2; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xA3; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xA4; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xA5; not defined
	{                                                                                                            // 0xA6
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFMADDSUB213PS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFMADDSUB213PD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0xA7
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFMSUBADD213PS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFMSUBADD213PD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0xA8
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFMADD213PS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFMADD213PD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0xA9
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFMADD213SS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFMADD213SD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0xAA
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFMSUB213PS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFMSUB213PD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0xAB
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFMSUB213SS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFMSUB213SD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0xAC
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFNMADD213PS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFNMADD213PD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0xAD
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFNMADD213SS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFNMADD213SD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0xAE
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFNMSUB213PS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFNMSUB213PD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0xAF
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFNMSUB213SS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFNMSUB213SD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xB0; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xB1; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xB2; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xB3; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xB4; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xB5; not defined
	{                                                                                                            // 0xB6
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFMADDSUB231PS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFMADDSUB231PD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0xB7
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFMSUBADD231PS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFMSUBADD231PD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0xB8
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFMADD231PS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFMADD231PD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0xB9
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFMADD231SS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFMADD231SD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0xBA
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFMSUB231PS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFMSUB231PD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0xBB
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFMSUB231SS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFMSUB231SD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0xBC
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFNMADD231PS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFNMADD231PD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0xBD
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFNMADD231SS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFNMADD231SD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0xBE
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFNMSUB231PS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFNMSUB231PD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0xBF
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VFNMSUB231SS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP }, // VFNMSUB231SD if (E)VEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xC0; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xC1; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xC2; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xC3; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xC4; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xC5; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xC6; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xC7; not defined
	{                                                                                                            // 0xC8
		{ SHA1NEXTE_X86, REG_X_X86_OC, RM_REQ_X_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0xC9
		{ SHA1MSG1_X86, REG_X_X86_OC, RM_REQ_X_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0xCA
		{ SHA1MSG2_X86, REG_X_X86_OC, RM_REQ_X_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0xCB
		{ SHA256RNDS2_X86, REG_X_X86_OC, RM_REQ_X_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0xCC
		{ SHA256MSG1_X86, REG_X_X86_OC, RM_REQ_X_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0xCD
		{ SHA256MSG2_X86, REG_X_X86_OC, RM_REQ_X_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xCE; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xCF; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xD0; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xD1; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xD2; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xD3; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xD4; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xD5; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xD6; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xD7; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xD8; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xD9; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xDA; not defined
	{                                                                                                            // 0xDB
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VAESIMC_X86, REG_X_X86_OC, RM_REQ_X_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0xDC
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VAESENC_X86, REG_X_X86_OC, VVVV_X_X86_OC, RM_REQ_X_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0xDD
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VAESENCLAST_X86, REG_X_X86_OC, VVVV_X_X86_OC, RM_REQ_X_X86_OC, NONE_OC, NONE_OP },
		{ AESDEC128KL_X86, REG_X_X86_OC, RM_REQ_X_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0xDE
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VAESDEC_X86, REG_X_X86_OC, VVVV_X_X86_OC, RM_REQ_X_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0xDF
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VAESDECLAST_X86, REG_X_X86_OC, VVVV_X_X86_OC, RM_REQ_X_X86_OC, NONE_OC, NONE_OP },
		{ AESDEC256KL_X86, REG_X_X86_OC, RM_REQ_X_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xE0; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xE1; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xE2; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xE3; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xE4; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xE5; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xE6; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xE7; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xE8; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xE9; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xEA; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xEB; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xEC; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xED; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xEE; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xEF; not defined
	{                                                                                                            // 0xF0
		{ MOVBE_X86, REG_WDQ_X86_OC, M_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ CRC32_X86, REG_D_X86_OC, RM_B_X86_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xF1
		{ MOVBE_X86, M_WDQ_X86_OC, REG_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ CRC32_X86, REG_D_X86_OC, RM_DQ_X86_OC, NONE_OC, NONE_OC, NONE_OP }, // second operand is Ew if both 0x66 and 0xF2
	},
	{                                                                                                            // 0xF2
		{ ANDN_X86, REG_DQ_X86_OC, VEX_VVVV_DQ_X86_OC, RM_DQ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{ EXTENDED_GROUP17_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }, // 0xF3
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xF4; not defined
	{                                                                                                            // 0xF5
		{ BZHI_X86, REG_DQ_X86_OC, RM_DQ_X86_OC, VEX_VVVV_DQ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ PEXT_X86, REG_DQ_X86_OC, VEX_VVVV_DQ_X86_OC, RM_DQ_X86_OC, NONE_OC, NONE_OP },
		{ PDEP_X86, REG_DQ_X86_OC, VEX_VVVV_DQ_X86_OC, RM_DQ_X86_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xF6
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ ADCX_X86, REG_DQ_X86_OC, RM_DQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ ADOX_X86, REG_DQ_X86_OC, RM_DQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ MULX_X86, VEX_VVVV_DQ_X86_OC, REG_DQ_X86_OC, DX_WDQ_X86_OC, RM_DQ_X86_OC, NONE_OP },
	},
	{                                                                                                            // 0xF7
		{ BEXTR_X86, REG_DQ_X86_OC, RM_DQ_X86_OC, VEX_VVVV_DQ_X86_OC, NONE_OC, NONE_OP },
		{ SHLX_X86, REG_DQ_X86_OC, RM_DQ_X86_OC, VEX_VVVV_DQ_X86_OC, NONE_OC, NONE_OP },
		{ SARX_X86, REG_DQ_X86_OC, RM_DQ_X86_OC, VEX_VVVV_DQ_X86_OC, NONE_OC, NONE_OP },
		{ SHRX_X86, REG_DQ_X86_OC, RM_DQ_X86_OC, VEX_VVVV_DQ_X86_OC, NONE_OC, NONE_OP },
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xF8; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xF9; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xFA; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xFB; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xFC; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xFD; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xFE; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xFF; not defined
};

// Appendix A: A.3 table A-5
const struct Opcode threeByteOpcodeMap3A[0x100][4] = // [byte][prefix] 0 - no prefix; 1 - 0x66; 2 - 0xF3; 3 - 0xF2  (not all opcodes have different definitions for each prefix or any of them)
{
	{                                                                                                            // 0x00
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPERMQ_X86, REG_REQ_YZ_X86_OC, RM_REQ_YZ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x01
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPERMPD_X86, REG_REQ_YZ_X86_OC, RM_REQ_YZ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x02
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPBLENDD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, IMM_B_X86_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x03; not defined
	{                                                                                                            // 0x04
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPERMILPS_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x05
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPERMILPD_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x06
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPERM2F128_X86, REG_REQ_YZ_X86_OC, VVVV_REQ_YZ_X86_OC, RM_REQ_YZ_X86_OC, IMM_B_X86_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x07; not defined
	{                                                                                                            // 0x08
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VROUNDPS_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x09
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VROUNDPD_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x0A
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VROUNDSS_X86, REG_X_X86_OC, R_X_M_D_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x0B
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VROUNDSD_X86, REG_X_X86_OC, R_X_M_Q_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x0C
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VBLENDPS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, IMM_B_X86_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x0D
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VBLENDPD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, IMM_B_X86_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x0E
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPBLENDW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, IMM_B_X86_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x0F
		{ PALIGNR_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
		{ VPALIGNR_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, IMM_B_X86_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x10; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x11; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x12; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x13; not defined
	{                                                                                                            // 0x14
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPEXTRB_X86, RM_B_X86_OC, REG_X_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP }, // first operand is Rd/Mb? leaving Eb
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x15
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPEXTRW_X86, RM_W_X86_OC, REG_X_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP }, // first operand is Rd/Mb? leaving Ew
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x16
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPEXTRD_X86, RM_DQ_X86_OC, REG_X_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP }, // VPEXTRQ if (E)VEX.W = 1 or PEXTRQ if REX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x17
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VEXTRACTPS_X86, RM_D_X86_OC, REG_X_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x18
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VINSERTF128_X86, REG_REQ_YZ_X86_OC, VVVV_REQ_YZ_X86_OC, RM_REQ_YZ_X86_OC, IMM_B_X86_OC, NONE_OP }, // VINSERTF32X4 if EVEX.W = 0, VINSERTF64X2 if EVEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x19
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VEXTRACTF128_X86, RM_REQ_X_X86_OC, REG_REQ_YZ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP }, // VEXTRACTF32X4 if EVEX.W = 0, VEXTRACTF64X2 if EVEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x1A; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x1B; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x1C; not defined
	{                                                                                                            // 0x1D
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VCVTPS2PH_X86, RM_XYZ_X86_OC, REG_XYZ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x1E; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x1F; not defined
	{                                                                                                            // 0x20
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPINSRB_X86, REG_X_X86_OC, VVVV_X_X86_OC, RM_B_X86_OC, IMM_B_X86_OC, NONE_OP }, // third operand is Ry/Mb? leaving Eb
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x21
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VINSERTPS_X86, REG_X_X86_OC, VVVV_X_X86_OC, RM_MOD11_REQ_X_X86_OC, IMM_B_X86_OC, NONE_OP}, // third operand is Udq/Md ?
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x22
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPINSRD_X86, REG_X_X86_OC, VVVV_X_X86_OC, RM_DQ_X86_OC, IMM_B_X86_OC, NONE_OP}, // VPINSRQ if (E)VEX.W = 1 or PINSRQ if REX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x23; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x24; not defined
	{                                                                                                            // 0x25; AVX-512
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPTERNLOGD_X86, REG_X_X86_OC, EVEX_VVVV_XYZ_X86_OC, M_XYZ_X86_OC, IMM_B_X86_OC, NONE_OP}, // VPTERNLOGQ if EVEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x26; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x27; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x28; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x29; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x2A; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x2B; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x2C; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x2D; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x2E; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x2F; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x30; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x31; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x32; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x33; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x34; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x35; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x36; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x37; not defined
	{                                                                                                            // 0x38
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VINSERTI128_X86, REG_REQ_YZ_X86_OC, VVVV_REQ_YZ_X86_OC, RM_REQ_YZ_X86_OC, IMM_B_X86_OC, NONE_OP }, // VINSERTI32X4 if EVEX.W = 0, VINSERTI64X2 if EVEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x39
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VEXTRACTI128_X86, RM_REQ_X_X86_OC, REG_REQ_YZ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP }, // VEXTRACTI32X4 if EVEX.W = 0, VEXTRACTI64X2 if EVEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x3A; AVX-512
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VINSERTI32X8_X86, REG_REQ_YZ_X86_OC, EVEX_VVVV_XYZ_X86_OC, RM_REQ_YZ_X86_OC, IMM_B_X86_OC, NONE_OP }, // VINSERTI64X4 if EVEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x3B; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x3C; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x3D; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x3E; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x3F; not defined
	{                                                                                                            // 0x40
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VDPPS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, IMM_B_X86_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x41
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VDPPD_X86, REG_X_X86_OC, VVVV_X_X86_OC, RM_REQ_X_X86_OC, IMM_B_X86_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x42
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VMPSADBW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, IMM_B_X86_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x43; AVX-512
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VSHUFI32X4_X86, REG_REQ_YZ_X86_OC, EVEX_VVVV_XYZ_X86_OC, RM_REQ_YZ_X86_OC, IMM_B_X86_OC, NONE_OP }, // VSHUFI64X2 if EVEX.W = 1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x44
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPCLMULQDQ_X86, REG_X_X86_OC, VVVV_X_X86_OC, RM_REQ_X_X86_OC, IMM_B_X86_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x45; not defined
	{                                                                                                            // 0x46
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPERM2I128_X86, REG_REQ_YZ_X86_OC, VVVV_REQ_YZ_X86_OC, RM_REQ_YZ_X86_OC, IMM_B_X86_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x47; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x48; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x49; not defined
	{                                                                                                            // 0x4A
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VBLENDVPS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, U4_XY_X86_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x4B
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VBLENDVPD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, U4_XY_X86_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x4C
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPBLENDVB_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, U4_XY_X86_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x4D; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x4E; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x4F; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x50; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x51; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x52; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x53; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x54; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x55; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x56; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x57; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x58; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x59; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x5A; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x5B; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x5C; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x5D; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x5E; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x5F; not defined
	{                                                                                                            // 0x60
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPCMPESTRM_X86, REG_X_X86_OC, RM_REQ_X_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x61
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPCMPESTRI_X86, REG_X_X86_OC, RM_REQ_X_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x62
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPCMPISTRM_X86, REG_X_X86_OC, RM_REQ_X_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{                                                                                                            // 0x63
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPCMPISTRI_X86, REG_X_X86_OC, RM_REQ_X_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x64; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x65; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x66; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x67; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x68; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x69; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x6A; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x6B; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x6C; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x6D; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x6E; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x6F; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x70; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x71; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x72; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x73; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x74; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x75; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x76; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x77; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x78; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x79; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x7A; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x7B; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x7C; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x7D; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x7E; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x7F; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x80; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x81; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x82; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x83; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x84; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x85; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x86; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x87; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x88; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x89; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x8A; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x8B; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x8C; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x8D; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x8E; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x8F; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x90; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x91; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x92; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x93; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x94; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x95; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x96; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x97; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x98; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x99; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x9A; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x9B; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x9C; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x9D; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x9E; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x9F; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xA0; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xA1; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xA2; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xA3; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xA4; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xA5; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xA6; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xA7; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xA8; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xA9; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xAA; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xAB; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xAC; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xAD; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xAE; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xAF; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xB0; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xB1; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xB2; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xB3; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xB4; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xB5; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xB6; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xB7; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xB8; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xB9; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xBA; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xBB; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xBC; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xBD; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xBE; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xBF; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xC0; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xC1; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xC2; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xC3; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xC4; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xC5; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xC6; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xC7; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xC8; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xC9; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xCA; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xCB; not defined
	{                                                                                                            // 0xCC
		{ SHA1RNDS4_X86, REG_X_X86_OC, RM_REQ_X_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xCD; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xCE; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xCF; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xD0; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xD1; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xD2; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xD3; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xD4; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xD5; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xD6; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xD7; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xD8; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xD9; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xDA; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xDB; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xDC; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xDD; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xDE; not defined
	{                                                                                                            // 0xDF
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VAESKEYGENASSIST_X86, REG_X_X86_OC, RM_REQ_X_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xE0; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xE1; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xE2; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xE3; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xE4; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xE5; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xE6; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xE7; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xE8; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xE9; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xEA; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xEB; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xEC; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xED; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xEE; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xEF; not defined
	{                                                                                                            // 0xF0
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ RORX_X86, REG_DQ_X86_OC, RM_DQ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
	},
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xF1; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xF2; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xF3; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xF4; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xF5; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xF6; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xF7; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xF8; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xF9; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xFA; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xFB; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xFC; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xFD; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xFE; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xFF; not defined
};
