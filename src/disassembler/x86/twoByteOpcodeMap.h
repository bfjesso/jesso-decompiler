#pragma once
#include "disassemblyStructs.h"

// Intel 64 and IA-32 Architectures Software Developer’s Manuals:
//	intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html

// Appendix A: A.3 table A-3
const struct Opcode twoByteOpcodeMap[0x100][4] = // [byte][prefix] 0 - no prefix; 1 - 0x66; 2 - 0xF3; 3 - 0xF2  (not all opcodes have different definitions for each prefix or any of them)
{
	{ EXTENDED_GROUP6_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },  // 0x00
	{ EXTENDED_GROUP7_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },  // 0x01
	{ LAR_X86, REG_WDQ_X86_OC, RM_W_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                       // 0x02
	{ LSL_X86, REG_WDQ_X86_OC, RM_W_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                       // 0x03
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x04; not defined
	{ SYSCALL_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, ONLY_64_MODE_OP },                    // 0x05
	{ CLTS_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },            // 0x06
	{ SYSRET_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, ONLY_64_MODE_OP },                     // 0x07
	{ INVD_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },            // 0x08
	{ WBINVD_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },          // 0x09
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x0A; not defined
	{ UD2_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },             // 0x0B; 2-byte illegal opcodes UD2
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x0C; not defined
	{ PREFETCHW_X86, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                    // 0x0D
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x0E; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x0F; not defined
	{                                                                                                            // 0x10
		{ VMOVUPS_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VMOVUPD_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VMOVSS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, R_X_M_D_X86_OC, NONE_OC, NONE_OP },
		{ VMOVSD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, R_X_M_Q_X86_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x11
		{ VMOVUPS_X86, RM_XYZ_X86_OC, REG_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VMOVUPD_X86, RM_XYZ_X86_OC, REG_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VMOVSS_X86, R_X_M_D_X86_OC, VVVV_XYZ_X86_OC, REG_X_X86_OC, NONE_OC, NONE_OP },
		{ VMOVSD_X86, R_X_M_Q_X86_OC, VVVV_XYZ_X86_OC, REG_X_X86_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x12
		{ VMOVLPS_X86, REG_REQ_X_X86_OC, VVVV_REQ_X_X86_OC, M_Q_X86_OC, NONE_OC, NONE_OP }, // vmovhlps if ModRM.mod = 0b11
		{ VMOVLPD_X86, REG_REQ_X_X86_OC, VVVV_REQ_X_X86_OC, M_Q_X86_OC, NONE_OC, NONE_OP },
		{ VMOVSLDUP_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VMOVDDUP_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x13
		{ VMOVLPS_X86, M_Q_X86_OC, REG_REQ_X_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VMOVLPD_X86, M_Q_X86_OC, REG_REQ_X_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x14
		{ VUNPCKLPS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ VUNPCKLPD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x15
		{ VUNPCKHPS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ VUNPCKHPD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x16
		{ VMOVHPS_X86, REG_X_X86_OC, VVVV_REQ_X_X86_OC, M_Q_X86_OC, NONE_OC, NONE_OP }, // vmovlhps if ModRM.mod = 0b11
		{ VMOVHPD_X86, REG_X_X86_OC, VVVV_REQ_X_X86_OC, M_Q_X86_OC, NONE_OC, NONE_OP },
		{ VMOVSHDUP_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x17
		{ VMOVHPS_X86, M_Q_X86_OC, REG_REQ_X_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VMOVHPD_X86, M_Q_X86_OC, REG_REQ_X_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{ EXTENDED_GROUP16_X86, M_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },              // 0x18; prefetch
	{ NOP_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },             // 0x19; reserved nop
	{                                                                                                            // 0x1A
		{ BNDLDX_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ BNDMOV_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ BNDCL_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ BNDCU_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x1B
		{ BNDSTX_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ BNDMOV_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ BNDMK_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ BNDCN_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{ NOP_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },             // 0x1C; reserved nop
	{ NOP_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },             // 0x1D; reserved nop
	{                                                                                                            // 0x1E
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ ENDBR32_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },         // this is ENDBR32 if 0xFB is used after the opcode bytes, and ENDBR64 if 0xFA is used
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{ NOP_X86, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                          // 0x1F
	{ MOV_X86, RM_REG_64MODE_X86_OC, REG_CTRL_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                       // 0x20
	{ MOV_X86, RM_REG_64MODE_X86_OC, REG_DEBUG_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                       // 0x21
	{ MOV_X86, REG_CTRL_X86_OC, RM_REG_64MODE_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                       // 0x22
	{ MOV_X86, REG_DEBUG_X86_OC, RM_REG_64MODE_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                       // 0x23
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x24; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x25; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x26; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x27; not defined
	{                                                                                                            // 0x28
		{ VMOVAPS_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VMOVAPD_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x29
		{ VMOVAPS_X86, RM_XYZ_X86_OC, REG_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VMOVAPD_X86, RM_XYZ_X86_OC, REG_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x2A
		{ CVTPI2PS_X86, REG_XYZ_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ CVTPI2PD_X86, REG_XYZ_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VCVTSI2SS_X86, REG_X_X86_OC, VVVV_X_X86_OC, RM_DQ_X86_OC, NONE_OC, NONE_OP },
		{ VCVTSI2SD_X86, REG_X_X86_OC, VVVV_X_X86_OC, RM_DQ_X86_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x2B
		{ VMOVNTPS_X86, M_XYZ_X86_OC, REG_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VMOVNTPD_X86, M_XYZ_X86_OC, REG_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x2C
		{ CVTTPS2PI_X86, REG_MMX_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ CVTTPD2PI_X86, REG_MMX_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VCVTTSS2SI_X86, REG_DQ_X86_OC, R_X_M_D_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VCVTTSD2SI_X86, REG_DQ_X86_OC, R_X_M_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x2D
		{ CVTPS2PI_X86, REG_MMX_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ CVTPD2PI_X86, RM_MMX_Q_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VCVTSS2SI_X86, REG_DQ_X86_OC, R_X_M_D_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VCVTSD2SI_X86, REG_DQ_X86_OC, R_X_M_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x2E
		{ VUCOMISS_X86, REG_X_X86_OC, R_X_M_D_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VUCOMISD_X86, REG_X_X86_OC, R_X_M_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x2F
		{ VCOMISS_X86, REG_X_X86_OC, R_X_M_D_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VCOMISD_X86, REG_X_X86_OC, R_X_M_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{ WRMSR_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },           // 0x30
	{ RDTSC_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },           // 0x31
	{ RDMSR_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },           // 0x32
	{ RDPMC_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },           // 0x33
	{ SYSENTER_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },        // 0x34
	{ SYSEXIT_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },         // 0x35
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x36; not defined
	{ GETSEC_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },          // 0x37
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x38; 3-byte escape
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x39; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x3A; 3-byte escape
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x3B; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x3C; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x3D; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x3E; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x3F; not defined
	{ CMOVO_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                     // 0x40
	{ CMOVNO_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                    // 0x41
	{ CMOVB_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                     // 0x42
	{ CMOVNB_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                    // 0x43
	{ CMOVZ_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                     // 0x44
	{ CMOVNZ_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                    // 0x45
	{ CMOVBE_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                    // 0x46
	{ CMOVA_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                     // 0x47
	{ CMOVS_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                     // 0x48
	{ CMOVNS_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                    // 0x49
	{ CMOVP_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                     // 0x4A
	{ CMOVNP_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                    // 0x4B
	{ CMOVL_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                     // 0x4C
	{ CMOVGE_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                    // 0x4D
	{ CMOVLE_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                    // 0x4E
	{ CMOVG_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                     // 0x4F
	{                                                                                                            // 0x50
		{ VMOVMSKPS_X86, REG_DQ_X86_OC, RM_MOD11_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP},
		{ VMOVMSKPD_X86, REG_DQ_X86_OC, RM_MOD11_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x51
		{ VSQRTPS_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP},
		{ VSQRTPD_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VSQRTSS_X86, REG_X_X86_OC, VVVV_X_X86_OC, R_X_M_D_X86_OC, NONE_OC, NONE_OP },
		{ VSQRTSD_X86, REG_X_X86_OC, VVVV_X_X86_OC, R_X_M_Q_X86_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x52
		{ VRSQRTPS_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP},
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VRSQRTSS_X86, REG_X_X86_OC, VVVV_X_X86_OC, R_X_M_D_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x53
		{ VRCPPS_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP},
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VRCPSS_X86, REG_X_X86_OC, VVVV_X_X86_OC, R_X_M_D_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x54
		{ VANDPS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP},
		{ VANDPD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x55
		{ VANDNPS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP},
		{ VANDNPD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x56
		{ VORPS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP},
		{ VORPD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x57
		{ VXORPS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ VXORPD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x58
		{ VADDPS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ VADDPD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ VADDSS_X86, REG_X_X86_OC, VVVV_X_X86_OC, R_X_M_D_X86_OC, NONE_OC, NONE_OP },
		{ VADDSD_X86, REG_X_X86_OC, VVVV_X_X86_OC, R_X_M_Q_X86_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x59
		{ VMULPS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ VMULPD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ VMULSS_X86, REG_X_X86_OC, VVVV_X_X86_OC, R_X_M_D_X86_OC, NONE_OC, NONE_OP },
		{ VMULSD_X86, REG_X_X86_OC, VVVV_X_X86_OC, R_X_M_Q_X86_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x5A
		{ VCVTPS2PD_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VCVTPD2PS_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VCVTSS2SD_X86, REG_X_X86_OC, VVVV_XYZ_X86_OC, R_X_M_D_X86_OC, NONE_OC, NONE_OP },
		{ VCVTSD2SS_X86, REG_X_X86_OC, VVVV_XYZ_X86_OC, R_X_M_Q_X86_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x5B
		{ VCVTDQ2PS_X86, REG_XYZ_X86_OC, RM_REQ_X_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VCVTPS2DQ_X86, REG_X_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VCVTTPS2DQ_X86, REG_X_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x5C
		{ VSUBPS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ VSUBPD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ VSUBSS_X86, REG_X_X86_OC, VVVV_X_X86_OC, R_X_M_D_X86_OC, NONE_OC, NONE_OP },
		{ VSUBSD_X86, REG_X_X86_OC, VVVV_X_X86_OC, R_X_M_Q_X86_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x5D
		{ VMINPS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ VMINPD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ VMINSS_X86, REG_X_X86_OC, VVVV_X_X86_OC, R_X_M_D_X86_OC, NONE_OC, NONE_OP },
		{ VMINSD_X86, REG_X_X86_OC, VVVV_X_X86_OC, R_X_M_Q_X86_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x5E
		{ VDIVPS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ VDIVPD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ VDIVSS_X86, REG_X_X86_OC, VVVV_X_X86_OC, R_X_M_D_X86_OC, NONE_OC, NONE_OP },
		{ VDIVSD_X86, REG_X_X86_OC, VVVV_X_X86_OC, R_X_M_Q_X86_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x5F
		{ VMAXPS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ VMAXPD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ VMAXSS_X86, REG_X_X86_OC, VVVV_X_X86_OC, R_X_M_D_X86_OC, NONE_OC, NONE_OP },
		{ VMAXSD_X86, REG_X_X86_OC, VVVV_X_X86_OC, R_X_M_Q_X86_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x60
		{ PUNPCKLBW_X86, REG_MMX_X86_OC, RM_MMX_D_X86_OC, NONE_OC, NONE_OC, NONE_OP},
		{ VPUNPCKLBW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x61
		{ PUNPCKLWD_X86, REG_MMX_X86_OC, RM_MMX_D_X86_OC, NONE_OC, NONE_OC, NONE_OP},
		{ VPUNPCKLWD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x62
		{ PUNPCKLDQ_X86, REG_MMX_X86_OC, RM_MMX_D_X86_OC, NONE_OC, NONE_OC, NONE_OP},
		{ VPUNPCKLDQ_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x63
		{ PACKSSWB_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP},
		{ VPACKSSWB_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x64
		{ PCMPGTB_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP},
		{ VPCMPGTB_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x65
		{ PCMPGTW_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP},
		{ VPCMPGTW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x66
		{ PCMPGTD_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP},
		{ VPCMPGTD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x67
		{ PACKUSWB_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP},
		{ VPACKUSWB_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x68
		{ PUNPCKHBW_X86, REG_MMX_X86_OC, RM_MMX_D_X86_OC, NONE_OC, NONE_OC, NONE_OP},
		{ VPUNPCKHBW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x69
		{ PUNPCKHWD_X86, REG_MMX_X86_OC, RM_MMX_D_X86_OC, NONE_OC, NONE_OC, NONE_OP},
		{ VPUNPCKHWD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x6A
		{ PUNPCKHDQ_X86, REG_MMX_X86_OC, RM_MMX_D_X86_OC, NONE_OC, NONE_OC, NONE_OP},
		{ VPUNPCKHDQ_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x6B
		{ PACKSSDW_X86, REG_MMX_X86_OC, RM_MMX_D_X86_OC, NONE_OC, NONE_OC, NONE_OP},
		{ VPACKSSDW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x6C
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPUNPCKLQDQ_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x6D
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPUNPCKHQDQ_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x6E
		{ MOVD_X86, REG_MMX_X86_OC, RM_DQ_X86_OC, NONE_OC, NONE_OC, NONE_OP }, // MOVQ if REX.w or E/VEX.W1
		{ VMOVD_X86, REG_REQ_X_X86_OC, RM_DQ_X86_OC, NONE_OC, NONE_OC, NONE_OP }, // VMOVQ if REX.w or E/VEX.W1
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x6F
		{ MOVQ_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VMOVDQA_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VMOVDQU_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP }, // VMOVDQU32 if EVEX.W = 0, VMOVDQU64 if EVEX.W = 1
		{ VMOVDQU8_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, FULL_MEM_TT },  // AVX-512, VMOVDQU16 if EVEX.W = 1
	},
	{                                                                                                            // 0x70
		{ PSHUFW_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
		{ VPSHUFD_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
		{ VPSHUFHW_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
		{ VPSHUFLW_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
	},
	{ EXTENDED_GROUP12_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }, // 0x71
	{ EXTENDED_GROUP13_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }, // 0x72
	{ EXTENDED_GROUP14_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }, // 0x73
	{                                                                                                            // 0x74
		{ PCMPEQB_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP},
		{ VPCMPEQB_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x75
		{ PCMPEQW_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP},
		{ VPCMPEQW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x76
		{ PCMPEQD_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP},
		{ VPCMPEQD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{ EMMS_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },            // 0x77; vzeroupper if VEX.L = 0, vzeroall if VEX.L = 1
	{ VMREAD_X86, RM_64MODE_X86_OC, REG_64MODE_X86_OC, NONE_OC, NONE_OC, NONE_OP },               // 0x78
	{ VMWRITE_X86, REG_64MODE_X86_OC, RM_64MODE_X86_OC, NONE_OC, NONE_OC, NONE_OP },              // 0x79
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x7A; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0x7B; not defined
	{                                                                                                            // 0x7C
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VHADDPD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VHADDPS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x7D
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VHSUBPD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VHSUBPS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x7E
		{ MOVD_X86, RM_DQ_X86_OC, REG_MMX_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VMOVD_X86, RM_DQ_X86_OC, REG_REQ_X_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VMOVQ_X86, REG_REQ_X_X86_OC, RM_REQ_X_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0x7F
		{ MOVQ_X86, RM_MMX_Q_X86_OC, REG_MMX_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VMOVDQA_X86, RM_XYZ_X86_OC, REG_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VMOVDQU_X86, RM_XYZ_X86_OC, REG_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP }, // VMOVDQU32 if EVEX.W = 0, VMOVDQU64 if EVEX.W = 1
		{ VMOVDQU8_X86, RM_XYZ_X86_OC, REG_XYZ_X86_OC, NONE_OC, NONE_OC, FULL_MEM_TT }, // AVX-512, VMOVDQU16 if EVEX.W = 1
	},
	{ JO_SHORT_X86, IMM_WD_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                     // 0x80
	{ JNO_SHORT_X86, IMM_WD_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                    // 0x81
	{ JB_SHORT_X86, IMM_WD_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                     // 0x82
	{ JNB_SHORT_X86, IMM_WD_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                    // 0x83
	{ JZ_SHORT_X86, IMM_WD_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                     // 0x84
	{ JNZ_SHORT_X86, IMM_WD_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                    // 0x85
	{ JBE_SHORT_X86, IMM_WD_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                    // 0x86
	{ JA_SHORT_X86, IMM_WD_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                     // 0x87
	{ JS_SHORT_X86, IMM_WD_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                     // 0x88
	{ JNS_SHORT_X86, IMM_WD_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                    // 0x89
	{ JP_SHORT_X86, IMM_WD_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                     // 0x8A
	{ JNP_SHORT_X86, IMM_WD_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                    // 0x8B
	{ JL_SHORT_X86, IMM_WD_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                     // 0x8C
	{ JGE_SHORT_X86, IMM_WD_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                    // 0x8D
	{ JLE_SHORT_X86, IMM_WD_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                    // 0x8E
	{ JG_SHORT_X86, IMM_WD_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                     // 0x8F
	{ SETO_X86, RM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                         // 0x90
	{ SETNO_X86, RM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                        // 0x91
	{ SETB_X86, RM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                         // 0x92
	{ SETNB_X86, RM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                        // 0x93
	{ SETZ_X86, RM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                         // 0x94
	{ SETNZ_X86, RM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                        // 0x95
	{ SETBE_X86, RM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                        // 0x96
	{ SETA_X86, RM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                         // 0x97
	{ SETS_X86, RM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                         // 0x98
	{ SETNS_X86, RM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                        // 0x99
	{ SETP_X86, RM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                         // 0x9A
	{ SETNP_X86, RM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                        // 0x9B
	{ SETL_X86, RM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                         // 0x9C
	{ SETNL_X86, RM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                        // 0x9D
	{ SETLE_X86, RM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                        // 0x9E
	{ SETG_X86, RM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                         // 0x9F
	{ PUSH_X86, FS_X86_OC, NONE_OC, NONE_OC, NONE_OC, DEFAULT_64_SIZE_OP },                               // 0xA0
	{ POP_X86, FS_X86_OC, NONE_OC, NONE_OC, NONE_OC, DEFAULT_64_SIZE_OP },                                // 0xA1
	{ CPUID_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },           // 0xA2
	{ BT_X86, RM_WDQ_X86_OC, REG_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                        // 0xA3
	{ SHLD_X86, RM_WDQ_X86_OC, REG_WDQ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },                                                   // 0xA4
	{ SHLD_X86, RM_WDQ_X86_OC, REG_WDQ_X86_OC, CL_X86_OC, NONE_OC, NONE_OP },                                              // 0xA5
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xA6; not defined
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },     // 0xA7; not defined
	{ PUSH_X86, GS_X86_OC, NONE_OC, NONE_OC, NONE_OC, DEFAULT_64_SIZE_OP },                               // 0xA8
	{ POP_X86, GS_X86_OC, NONE_OC, NONE_OC, NONE_OC, DEFAULT_64_SIZE_OP },                                // 0xA9
	{ RSM_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },             // 0xAA
	{ BTS_X86, RM_WDQ_X86_OC, REG_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                       // 0xAB
	{ SHRD_X86, RM_WDQ_X86_OC, REG_WDQ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },                                                   // 0xAC
	{ SHRD_X86, RM_WDQ_X86_OC, REG_WDQ_X86_OC, CL_X86_OC, NONE_OC, NONE_OP },                                              // 0xAD
	{ EXTENDED_GROUP15_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }, // 0xAE
	{ IMUL_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                      // 0xAF
	{ CMPXCHG_X86, RM_B_X86_OC, REG_B_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                   // 0xB0
	{ CMPXCHG_X86, RM_WDQ_X86_OC, REG_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                   // 0xB1
	{ LSS_X86, REG_WDQ_X86_OC, M_DF_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                       // 0xB2
	{ BTR_X86, RM_WDQ_X86_OC, REG_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                       // 0xB3
	{ LFS_X86, REG_WDQ_X86_OC, M_DF_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                       // 0xB4
	{ LGS_X86, REG_WDQ_X86_OC, M_DF_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                       // 0xB5
	{ MOVZX_X86, REG_WDQ_X86_OC, RM_B_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                     // 0xB6
	{ MOVZX_X86, REG_WDQ_X86_OC, RM_W_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                     // 0xB7
	{                                                                                                            // 0xB8
		{ JMPE_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ POPCNT_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{ EXTENDED_GROUP10_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }, // 0xB9; UD1
	{ EXTENDED_GROUP8_X86, RM_WDQ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OP },                            // 0xBA
	{ BTC_X86, RM_WDQ_X86_OC, REG_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                       // 0xBB
	{                                                                                                            // 0xBC
		{ BSF_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ TZCNT_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xBD
		{ BSR_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ LZCNT_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{ MOVSX_X86, REG_WDQ_X86_OC, RM_B_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                     // 0xBE
	{ MOVSX_X86, REG_WDQ_X86_OC, RM_W_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                     // 0xBF
	{ XADD_X86, RM_B_X86_OC, REG_B_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                      // 0xC0
	{ XADD_X86, RM_WDQ_X86_OC, REG_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                      // 0xC1
	{                                                                                                            // 0xC2
		{ VCMPPS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, IMM_B_X86_OC, NONE_OP },
		{ VCMPPD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, IMM_B_X86_OC, NONE_OP },
		{ VCMPSS_X86, REG_X_X86_OC, VVVV_X_X86_OC, R_X_M_D_X86_OC, IMM_B_X86_OC, NONE_OP },
		{ VCMPSD_X86, REG_X_X86_OC, VVVV_X_X86_OC, R_X_M_Q_X86_OC, IMM_B_X86_OC, NONE_OP },
	},
	{ MOVNTI_X86, M_DQ_X86_OC, REG_DQ_X86_OC, NONE_OC, NONE_OC, NONE_OP },                                    // 0xC3
	{                                                                                                            // 0xC4
		{ PINSRW_X86, REG_MMX_X86_OC, RM_DQ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP }, // second operand is Ry/Mw, leaving Ey
		{ VPINSRW_X86, REG_X_X86_OC, VVVV_X_X86_OC, RM_DQ_X86_OC, IMM_B_X86_OC, NONE_OP }, // third operand is Ry/Mw, leaving Ey
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xC5
		{ PEXTRW_X86, REG_D_X86_OC, RM_MOD11_MMX_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
		{ VPEXTRW_X86, REG_D_X86_OC, RM_MOD11_REQ_X_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xC6
		{ VSHUFPS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, IMM_B_X86_OC, NONE_OP },
		{ VSHUFPD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, IMM_B_X86_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{ EXTENDED_GROUP9_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },  // 0xC7
	{ BSWAP_X86, AX_R8_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                    // 0xC8
	{ BSWAP_X86, CX_R9_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                    // 0xC9
	{ BSWAP_X86, DX_R10_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                   // 0xCA
	{ BSWAP_X86, BX_R11_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                   // 0xCB
	{ BSWAP_X86, SP_R12_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                   // 0xCC
	{ BSWAP_X86, BP_R13_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                   // 0xCD
	{ BSWAP_X86, SI_R14_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                   // 0xCE
	{ BSWAP_X86, DI_R15_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },                   // 0xCF
	{                                                                                                            // 0xD0
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VADDSUBPD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VADDSUBPS_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xD1
		{ PSRLW_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPSRLW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xD2
		{ PSRLD_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPSRLD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xD3
		{ PSRLQ_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPSRLQ_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xD4
		{ PADDQ_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPADDQ_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xD5
		{ PMULLW_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMULLW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xD6
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VMOVQ_X86, RM_REQ_X_X86_OC, REG_REQ_X_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ MOVQ2DQ_X86, REG_X_X86_OC, RM_MOD11_MMX_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ MOVDQ2Q_X86, REG_MMX_X86_OC, RM_MOD11_X_X86_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xD7
		{ PMOVMSKB_X86, REG_D_X86_OC, RM_MOD11_MMX_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMOVMSKB_X86, REG_D_X86_OC, RM_MOD11_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xD8
		{ PSUBUSB_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPSUBUSB_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xD9
		{ PSUBUSW_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPSUBUSW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xDA
		{ PMINUB_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMINUB_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xDB
		{ PAND_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPAND_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xDC
		{ PADDUSB_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPADDUSB_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xDD
		{ PADDUSW_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPADDUSW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xDE
		{ PMAXUB_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ PMAXUB_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xDF
		{ PANDN_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPANDN_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xE0
		{ PAVGB_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPAVGB_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xE1
		{ PSRAW_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPSRAW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xE2
		{ PSRAD_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPSRAD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xE3
		{ PAVGW_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPAVGW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xE4
		{ PMULHUW_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMULHUW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xE5
		{ PMULHW_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMULHW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xE6
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VCVTTPD2DQ_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VCVTDQ2PD_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VCVTPD2DQ_X86, REG_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xE7
		{ MOVNTQ_X86, M_Q_X86_OC, REG_MMX_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VMOVNTDQ_X86, M_XYZ_X86_OC, REG_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xE8
		{ PSUBSB_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPSUBSB_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xE9
		{ PSUBSW_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPSUBSW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xEA
		{ PMINSW_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMINSW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xEB
		{ POR_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPOR_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xEC
		{ PADDSB_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP},
		{ VPADDSB_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xED
		{ PADDSW_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP},
		{ VPADDSW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xEE
		{ PMAXSW_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP},
		{ VPMAXSW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xEF
		{ PXOR_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP},
		{ VPXOR_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xF0
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VLDDQU_X86, REG_XYZ_X86_OC, M_XYZ_X86_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xF1
		{ PSLLW_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPSLLW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xF2
		{ PSLLD_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPSLLD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xF3
		{ PSLLQ_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPSLLQ_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xF4
		{ PMULUDQ_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMULUDQ_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xF5
		{ PMADDWD_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPMADDWD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xF6
		{ PSADBW_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPSADBW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xF7
		{ MASKMOVQ_X86, REG_MMX_X86_OC, RM_MOD11_MMX_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VMASKMOVDQU_X86, REG_X_X86_OC, RM_MOD11_REQ_X_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xF8
		{ PSUBB_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPSUBB_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xF9
		{ PSUBW_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPSUBW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xFA
		{ PSUBD_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPSUBD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xFB
		{ PSUBQ_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPSUBQ_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xFC
		{ PADDB_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPADDB_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xFD
		{ PADDW_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPADDW_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{                                                                                                            // 0xFE
		{ PADDD_X86, REG_MMX_X86_OC, RM_MMX_Q_X86_OC, NONE_OC, NONE_OC, NONE_OP },
		{ VPADDD_X86, REG_XYZ_X86_OC, VVVV_XYZ_X86_OC, RM_XYZ_X86_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
		{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP },
	},
	{ UD0_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP }              // 0xFF; UD0
};
const struct Opcode alternateX12 = { VMOVHLPS_X86, REG_REQ_X_X86_OC, VVVV_REQ_X_X86_OC, RM_MOD11_X_X86_OC, NONE_OC, NONE_OP };                                       // no prefix 0x12 when ModRM.mod == 0b11
const struct Opcode alternateX16 = { VMOVLHPS_X86, REG_X_X86_OC, VVVV_REQ_X_X86_OC, RM_MOD11_X_X86_OC, NONE_OC, NONE_OP };                                      // no prefix 0x16 when ModRM.mod == 0b11
const struct Opcode alternateX1E = { RDSSPD_X86, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NONE_OP };               // if ModRM.mod == 0b11. RDSSPQ if REX.W
