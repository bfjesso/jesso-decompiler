#pragma once
#include "disassemblyStructs.h"

// Intel 64 and IA-32 Architectures Software Developer’s Manuals:
//	intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html

// Appendix A: A.5.2 table A-7
// opcode: 0xD8
const struct Opcode escapeD8OpcodeMapBits[8] = // [ModR/M bits 3-5]
{
	{ FADD_X86, -1, M_D_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FMUL_X86, -1, M_D_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FCOM_X86, -1, M_D_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FCOMP_X86, -1, M_D_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FSUB_X86, -1, M_D_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FSUBR_X86, -1, M_D_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FDIV_X86, -1, M_D_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FDIVR_X86, -1, M_D_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT }
};

// Appendix A: A.5.2 table A-8
// opcode: 0xD8
const struct Opcode escapeD8OpcodeMapByte[0x40] = // [ModR/M byte - 0xC0]
{
	{ FADD_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xC0
	{ FADD_X86, -1, ST0_X86_OC, ST1_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xC1
	{ FADD_X86, -1, ST0_X86_OC, ST2_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xC2
	{ FADD_X86, -1, ST0_X86_OC, ST3_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xC3
	{ FADD_X86, -1, ST0_X86_OC, ST4_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xC4
	{ FADD_X86, -1, ST0_X86_OC, ST5_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xC5
	{ FADD_X86, -1, ST0_X86_OC, ST6_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xC6
	{ FADD_X86, -1, ST0_X86_OC, ST7_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xC7
	{ FMUL_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xC8
	{ FMUL_X86, -1, ST0_X86_OC, ST1_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xC9
	{ FMUL_X86, -1, ST0_X86_OC, ST2_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xCA
	{ FMUL_X86, -1, ST0_X86_OC, ST3_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xCB
	{ FMUL_X86, -1, ST0_X86_OC, ST4_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xCC
	{ FMUL_X86, -1, ST0_X86_OC, ST5_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xCD
	{ FMUL_X86, -1, ST0_X86_OC, ST6_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xCE
	{ FMUL_X86, -1, ST0_X86_OC, ST7_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xCF
	{ FCOM_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xD0
	{ FCOM_X86, -1, ST0_X86_OC, ST1_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xD1
	{ FCOM_X86, -1, ST0_X86_OC, ST2_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xD2
	{ FCOM_X86, -1, ST0_X86_OC, ST3_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xD3
	{ FCOM_X86, -1, ST0_X86_OC, ST4_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xD4
	{ FCOM_X86, -1, ST0_X86_OC, ST5_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xD5
	{ FCOM_X86, -1, ST0_X86_OC, ST6_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xD6
	{ FCOM_X86, -1, ST0_X86_OC, ST7_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xD7
	{ FCOMP_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xD8
	{ FCOMP_X86, -1, ST0_X86_OC, ST1_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xD9
	{ FCOMP_X86, -1, ST0_X86_OC, ST2_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xDA
	{ FCOMP_X86, -1, ST0_X86_OC, ST3_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xDB
	{ FCOMP_X86, -1, ST0_X86_OC, ST4_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xDC
	{ FCOMP_X86, -1, ST0_X86_OC, ST5_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xDD
	{ FCOMP_X86, -1, ST0_X86_OC, ST6_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xDE
	{ FCOMP_X86, -1, ST0_X86_OC, ST7_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xDF
	{ FSUB_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xE0
	{ FSUB_X86, -1, ST0_X86_OC, ST1_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xE1
	{ FSUB_X86, -1, ST0_X86_OC, ST2_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xE2
	{ FSUB_X86, -1, ST0_X86_OC, ST3_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xE3
	{ FSUB_X86, -1, ST0_X86_OC, ST4_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xE4
	{ FSUB_X86, -1, ST0_X86_OC, ST5_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xE5
	{ FSUB_X86, -1, ST0_X86_OC, ST6_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xE6
	{ FSUB_X86, -1, ST0_X86_OC, ST7_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xE7
	{ FSUBR_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xE8
	{ FSUBR_X86, -1, ST0_X86_OC, ST1_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xE9
	{ FSUBR_X86, -1, ST0_X86_OC, ST2_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xEA
	{ FSUBR_X86, -1, ST0_X86_OC, ST3_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xEB
	{ FSUBR_X86, -1, ST0_X86_OC, ST4_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xEC
	{ FSUBR_X86, -1, ST0_X86_OC, ST5_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xED
	{ FSUBR_X86, -1, ST0_X86_OC, ST6_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xEE
	{ FSUBR_X86, -1, ST0_X86_OC, ST7_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xEF
	{ FDIV_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xF0
	{ FDIV_X86, -1, ST0_X86_OC, ST1_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xF1
	{ FDIV_X86, -1, ST0_X86_OC, ST2_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xF2
	{ FDIV_X86, -1, ST0_X86_OC, ST3_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xF3
	{ FDIV_X86, -1, ST0_X86_OC, ST4_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xF4
	{ FDIV_X86, -1, ST0_X86_OC, ST5_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xF5
	{ FDIV_X86, -1, ST0_X86_OC, ST6_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xF6
	{ FDIV_X86, -1, ST0_X86_OC, ST7_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xF7
	{ FDIVR_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xF8
	{ FDIVR_X86, -1, ST0_X86_OC, ST1_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xF9
	{ FDIVR_X86, -1, ST0_X86_OC, ST2_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xFA
	{ FDIVR_X86, -1, ST0_X86_OC, ST3_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xFB
	{ FDIVR_X86, -1, ST0_X86_OC, ST4_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xFC
	{ FDIVR_X86, -1, ST0_X86_OC, ST5_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xFD
	{ FDIVR_X86, -1, ST0_X86_OC, ST6_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xFE
	{ FDIVR_X86, -1, ST0_X86_OC, ST7_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT }                         // 0xFF
};

// Appendix A: A.5.2 table A-9
// opcode: 0xD9
const struct Opcode escapeD9OpcodeMapBits[8] = // [ModR/M bits 3-5]
{
	{ FLD_X86, -1, M_D_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },    // not an opcode: reserved
	{ FST_X86, -1, M_D_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FSTP_X86, -1, M_D_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FLDENV_X86, -1, M_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FLDCW_X86, -1, M_W_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FSTENV_X86, -1, M_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FSTCW_X86, -1, M_W_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT }
};

// Appendix A: A.5.2 table A-10
// opcode: 0xD9
const struct Opcode escapeD9OpcodeMapByte[0x40] = // [ModR/M byte - 0xC0]
{
	{ FLD_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xC0
	{ FLD_X86, -1, ST0_X86_OC, ST1_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xC1
	{ FLD_X86, -1, ST0_X86_OC, ST2_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xC2
	{ FLD_X86, -1, ST0_X86_OC, ST3_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xC3
	{ FLD_X86, -1, ST0_X86_OC, ST4_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xC4
	{ FLD_X86, -1, ST0_X86_OC, ST5_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xC5
	{ FLD_X86, -1, ST0_X86_OC, ST6_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xC6
	{ FLD_X86, -1, ST0_X86_OC, ST7_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xC7
	{ FXCH_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xC8
	{ FXCH_X86, -1, ST0_X86_OC, ST1_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xC9
	{ FXCH_X86, -1, ST0_X86_OC, ST2_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xCA
	{ FXCH_X86, -1, ST0_X86_OC, ST3_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xCB
	{ FXCH_X86, -1, ST0_X86_OC, ST4_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xCC
	{ FXCH_X86, -1, ST0_X86_OC, ST5_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xCD
	{ FXCH_X86, -1, ST0_X86_OC, ST6_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xCE
	{ FXCH_X86, -1, ST0_X86_OC, ST7_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xCF
	{ FNOP_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },           // 0xD0
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },    // 0xD1; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },    // 0xD2; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },    // 0xD3; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },    // 0xD4; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },    // 0xD5; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },    // 0xD6; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },    // 0xD7; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },    // 0xD8; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },    // 0xD9; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },    // 0xDA; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },    // 0xDB; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },    // 0xDC; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },    // 0xDD; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },    // 0xDE; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },    // 0xDF; not an opcode: reserved
	{ FCHS_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },           // 0xE0
	{ FABS_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },           // 0xE1
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },    // 0xE2; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },    // 0xE3; not an opcode: reserved
	{ FTST_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },           // 0xE4
	{ FXAM_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },           // 0xE5
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },    // 0xE6; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },    // 0xE7; not an opcode: reserved
	{ FLD1_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },           // 0xE8
	{ FLDL2T_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },         // 0xE9
	{ FLDL2E_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },         // 0xEA
	{ FLDPI_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },          // 0xEB
	{ FLDLG2_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },         // 0xEC
	{ FLDLN2_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },         // 0xED
	{ FLDZ_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },           // 0xEE
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },    // 0xEF; not an opcode: reserved
	{ F2XM1_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },          // 0xF0
	{ FYL2X_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },          // 0xF1
	{ FPTAN_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },          // 0xF2
	{ FPATAN_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },		    // 0xF3
	{ FXTRACT_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },		// 0xF4
	{ FPREM1_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },	        // 0xF5
	{ FDECSTP_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },	    // 0xF6
	{ FINCSTP_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },	    // 0xF7
	{ FPREM_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },		    // 0xF8
	{ FYL2XP1_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },		// 0xF9
	{ FSQRT_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },	        // 0xFA
	{ FSINCOS_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },	    // 0xFB
	{ FRNDINT_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },		// 0xFC
	{ FSCALE_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },		    // 0xFD
	{ FSIN_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },		    // 0xFE
	{ FCOS_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT }		    // 0xFF
};

// Appendix A: A.5.2 table A-11
// opcode: 0xDA
const struct Opcode escapeDAOpcodeMapBits[8] = // [ModR/M bits 3-5]
{
	{ FIADD_X86, -1, M_D_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FIMUL_X86, -1, M_D_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FICOM_X86, -1, M_D_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FICOMP_X86, -1, M_D_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FISUB_X86, -1, M_D_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FISUBR_X86, -1, M_D_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FIDIV_X86, -1, M_D_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FIDIVR_X86, -1, M_D_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT }
};

// Appendix A: A.5.2 table A-12
// opcode: 0xDA
const struct Opcode escapeDAOpcodeMapByte[0x40] = // [ModR/M byte - 0xC0]
{
	{ FCMOVB_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xC0
	{ FCMOVB_X86, -1, ST0_X86_OC, ST1_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xC1
	{ FCMOVB_X86, -1, ST0_X86_OC, ST2_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xC2
	{ FCMOVB_X86, -1, ST0_X86_OC, ST3_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xC3
	{ FCMOVB_X86, -1, ST0_X86_OC, ST4_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xC4
	{ FCMOVB_X86, -1, ST0_X86_OC, ST5_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xC5
	{ FCMOVB_X86, -1, ST0_X86_OC, ST6_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xC6
	{ FCMOVB_X86, -1, ST0_X86_OC, ST7_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xC7
	{ FCMOVE_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xC8
	{ FCMOVE_X86, -1, ST0_X86_OC, ST1_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xC9
	{ FCMOVE_X86, -1, ST0_X86_OC, ST2_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xCA
	{ FCMOVE_X86, -1, ST0_X86_OC, ST3_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xCB
	{ FCMOVE_X86, -1, ST0_X86_OC, ST4_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xCC
	{ FCMOVE_X86, -1, ST0_X86_OC, ST5_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xCD
	{ FCMOVE_X86, -1, ST0_X86_OC, ST6_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xCE
	{ FCMOVE_X86, -1, ST0_X86_OC, ST7_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xCF
	{ FCMOVBE_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xD0
	{ FCMOVBE_X86, -1, ST0_X86_OC, ST1_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xD1
	{ FCMOVBE_X86, -1, ST0_X86_OC, ST2_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xD2
	{ FCMOVBE_X86, -1, ST0_X86_OC, ST3_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xD3
	{ FCMOVBE_X86, -1, ST0_X86_OC, ST4_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xD4
	{ FCMOVBE_X86, -1, ST0_X86_OC, ST5_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xD5
	{ FCMOVBE_X86, -1, ST0_X86_OC, ST6_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xD6
	{ FCMOVBE_X86, -1, ST0_X86_OC, ST7_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xD7
	{ FCMOVU_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xD8
	{ FCMOVU_X86, -1, ST0_X86_OC, ST1_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xD9
	{ FCMOVU_X86, -1, ST0_X86_OC, ST2_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xDA
	{ FCMOVU_X86, -1, ST0_X86_OC, ST3_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xDB
	{ FCMOVU_X86, -1, ST0_X86_OC, ST4_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xDC
	{ FCMOVU_X86, -1, ST0_X86_OC, ST5_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xDD
	{ FCMOVU_X86, -1, ST0_X86_OC, ST6_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xDE
	{ FCMOVU_X86, -1, ST0_X86_OC, ST7_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xDF
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xE0; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xE1; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xE2; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xE3; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xE4; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xE5; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xE6; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xE7; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xE8; not an opcode: reserved
	{ FUCOMPP_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },	     // 0xE9
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xEA; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xEB; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xEC; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xED; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xEE; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xEF; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xF0; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xF1; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xF2; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xF3; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xF4; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xF5; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xF6; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xF7; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xF8; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xF9; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xFA; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xFB; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xFC; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xFD; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xFE; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT }      // 0xFF; not an opcode: reserved
};

// Appendix A: A.5.2 table A-13
// opcode: 0xDB
const struct Opcode escapeDBOpcodeMapBits[8] = // [ModR/M bits 3-5]
{
	{ FILD_X86, -1, M_D_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FISTTP_X86, -1, M_D_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FIST_X86, -1, M_D_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FISTP_X86, -1, M_D_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // not an opcode: reserved
	{ FLD_X86, -1, M_T_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // not an opcode: reserved
	{ FSTP_X86, -1, M_T_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT }
};

// Appendix A: A.5.2 table A-14
// opcode: 0xDB
const struct Opcode escapeDBOpcodeMapByte[0x40] = // [ModR/M byte - 0xC0]
{
	{ FCMOVNB_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xC0
	{ FCMOVNB_X86, -1, ST0_X86_OC, ST1_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xC1
	{ FCMOVNB_X86, -1, ST0_X86_OC, ST2_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xC2
	{ FCMOVNB_X86, -1, ST0_X86_OC, ST3_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xC3
	{ FCMOVNB_X86, -1, ST0_X86_OC, ST4_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xC4
	{ FCMOVNB_X86, -1, ST0_X86_OC, ST5_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xC5
	{ FCMOVNB_X86, -1, ST0_X86_OC, ST6_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xC6
	{ FCMOVNB_X86, -1, ST0_X86_OC, ST7_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xC7
	{ FCMOVNE_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xC8
	{ FCMOVNE_X86, -1, ST0_X86_OC, ST1_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xC9
	{ FCMOVNE_X86, -1, ST0_X86_OC, ST2_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xCA
	{ FCMOVNE_X86, -1, ST0_X86_OC, ST3_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xCB
	{ FCMOVNE_X86, -1, ST0_X86_OC, ST4_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xCC
	{ FCMOVNE_X86, -1, ST0_X86_OC, ST5_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xCD
	{ FCMOVNE_X86, -1, ST0_X86_OC, ST6_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xCE
	{ FCMOVNE_X86, -1, ST0_X86_OC, ST7_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xCF
	{ FCMOVNBE_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                      // 0xD0
	{ FCMOVNBE_X86, -1, ST0_X86_OC, ST1_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                      // 0xD1
	{ FCMOVNBE_X86, -1, ST0_X86_OC, ST2_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                      // 0xD2
	{ FCMOVNBE_X86, -1, ST0_X86_OC, ST3_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                      // 0xD3
	{ FCMOVNBE_X86, -1, ST0_X86_OC, ST4_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                      // 0xD4
	{ FCMOVNBE_X86, -1, ST0_X86_OC, ST5_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                      // 0xD5
	{ FCMOVNBE_X86, -1, ST0_X86_OC, ST6_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                      // 0xD6
	{ FCMOVNBE_X86, -1, ST0_X86_OC, ST7_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                      // 0xD7
	{ FCMOVNU_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xD8
	{ FCMOVNU_X86, -1, ST0_X86_OC, ST1_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xD9
	{ FCMOVNU_X86, -1, ST0_X86_OC, ST2_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xDA
	{ FCMOVNU_X86, -1, ST0_X86_OC, ST3_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xDB
	{ FCMOVNU_X86, -1, ST0_X86_OC, ST4_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xDC
	{ FCMOVNU_X86, -1, ST0_X86_OC, ST5_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xDD
	{ FCMOVNU_X86, -1, ST0_X86_OC, ST6_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xDE
	{ FCMOVNU_X86, -1, ST0_X86_OC, ST7_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xDF
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xE0; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xE1; not an opcode: reserved
	{ FCLEX_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },           // 0xE2; not an opcode: reserved
	{ FINIT_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },           // 0xE3; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xE4; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xE5; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xE6; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xE7; not an opcode: reserved
	{ FUCOMI_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xE8
	{ FUCOMI_X86, -1, ST0_X86_OC, ST1_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xE9
	{ FUCOMI_X86, -1, ST0_X86_OC, ST2_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xEA
	{ FUCOMI_X86, -1, ST0_X86_OC, ST3_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xEB
	{ FUCOMI_X86, -1, ST0_X86_OC, ST4_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xEC
	{ FUCOMI_X86, -1, ST0_X86_OC, ST5_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xED
	{ FUCOMI_X86, -1, ST0_X86_OC, ST6_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xEE
	{ FUCOMI_X86, -1, ST0_X86_OC, ST7_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xEF
	{ FCOMI_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xF0
	{ FCOMI_X86, -1, ST0_X86_OC, ST1_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xF1
	{ FCOMI_X86, -1, ST0_X86_OC, ST2_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xF2
	{ FCOMI_X86, -1, ST0_X86_OC, ST3_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xF3
	{ FCOMI_X86, -1, ST0_X86_OC, ST4_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xF4
	{ FCOMI_X86, -1, ST0_X86_OC, ST5_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xF5
	{ FCOMI_X86, -1, ST0_X86_OC, ST6_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xF6
	{ FCOMI_X86, -1, ST0_X86_OC, ST7_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xF7
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xF8; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xF9; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xFA; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xFB; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xFC; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xFD; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xFE; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT }      // 0xFF; not an opcode: reserved
};

// Appendix A: A.5.2 table A-15
// opcode: 0xDC
const struct Opcode escapeDCOpcodeMapBits[8] = // [ModR/M bits 3-5]
{
	{ FADD_X86, -1, M_Q_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FMUL_X86, -1, M_Q_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FCOM_X86, -1, M_Q_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FCOMP_X86, -1, M_Q_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FSUB_X86, -1, M_Q_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FSUBR_X86, -1, M_Q_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FDIV_X86, -1, M_Q_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FDIVR_X86, -1, M_Q_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT }
};

// Appendix A: A.5.2 table A-16
// opcode: 0xDC
const struct Opcode escapeDCOpcodeMapByte[0x40] = // [ModR/M byte - 0xC0]
{
	{ FADD_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xC0
	{ FADD_X86, -1, ST1_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xC1
	{ FADD_X86, -1, ST2_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xC2
	{ FADD_X86, -1, ST3_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xC3
	{ FADD_X86, -1, ST4_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xC4
	{ FADD_X86, -1, ST5_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xC5
	{ FADD_X86, -1, ST6_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xC6
	{ FADD_X86, -1, ST7_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xC7
	{ FMUL_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xC8
	{ FMUL_X86, -1, ST1_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xC9
	{ FMUL_X86, -1, ST2_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xCA
	{ FMUL_X86, -1, ST3_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xCB
	{ FMUL_X86, -1, ST4_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xCC
	{ FMUL_X86, -1, ST5_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xCD
	{ FMUL_X86, -1, ST6_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xCE
	{ FMUL_X86, -1, ST7_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xCF
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD0; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD1; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD2; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD3; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD4; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD5; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD6; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD7; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD8; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD9; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xDA; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xDB; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xDC; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xDD; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xDE; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xDF; not an opcode: reserved
	{ FSUBR_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xE0
	{ FSUBR_X86, -1, ST1_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xE1
	{ FSUBR_X86, -1, ST2_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xE2
	{ FSUBR_X86, -1, ST3_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xE3
	{ FSUBR_X86, -1, ST4_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xE4
	{ FSUBR_X86, -1, ST5_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xE5
	{ FSUBR_X86, -1, ST6_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xE6
	{ FSUBR_X86, -1, ST7_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xE7
	{ FSUB_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xE8
	{ FSUB_X86, -1, ST1_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xE9
	{ FSUB_X86, -1, ST2_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xEA
	{ FSUB_X86, -1, ST3_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xEB
	{ FSUB_X86, -1, ST4_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xEC
	{ FSUB_X86, -1, ST5_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xED
	{ FSUB_X86, -1, ST6_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xEE
	{ FSUB_X86, -1, ST7_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xEF
	{ FDIVR_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xF0
	{ FDIVR_X86, -1, ST1_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xF1
	{ FDIVR_X86, -1, ST2_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xF2
	{ FDIVR_X86, -1, ST3_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xF3
	{ FDIVR_X86, -1, ST4_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xF4
	{ FDIVR_X86, -1, ST5_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xF5
	{ FDIVR_X86, -1, ST6_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xF6
	{ FDIVR_X86, -1, ST7_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xF7
	{ FDIV_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xF8
	{ FDIV_X86, -1, ST1_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xF9
	{ FDIV_X86, -1, ST2_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xFA
	{ FDIV_X86, -1, ST3_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xFB
	{ FDIV_X86, -1, ST4_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xFC
	{ FDIV_X86, -1, ST5_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xFD
	{ FDIV_X86, -1, ST6_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xFE
	{ FDIV_X86, -1, ST7_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT }                           // 0xFF
};

// Appendix A: A.5.2 table A-17
// opcode: 0xDD
const struct Opcode escapeDDOpcodeMapBits[8] = // [ModR/M bits 3-5]
{
	{ FLD_X86, -1, M_Q_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FISTTP_X86, -1, M_Q_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FST_X86, -1, M_Q_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FSTP_X86, -1, M_Q_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FRSTOR_X86, -1, M_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FSAVE_X86, -1, M_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FSTSW_X86, -1, M_W_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT }
};

// Appendix A: A.5.2 table A-18
// opcode: 0xDD
const struct Opcode escapeDDOpcodeMapByte[0x40] = // [ModR/M byte - 0xC0]
{
	{ FFREE_X86, -1, ST0_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                  // 0xC0
	{ FFREE_X86, -1, ST1_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                  // 0xC1
	{ FFREE_X86, -1, ST2_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                  // 0xC2
	{ FFREE_X86, -1, ST3_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                  // 0xC3
	{ FFREE_X86, -1, ST4_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                  // 0xC4
	{ FFREE_X86, -1, ST5_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                  // 0xC5
	{ FFREE_X86, -1, ST6_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                  // 0xC6
	{ FFREE_X86, -1, ST7_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                  // 0xC7
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xC8; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xC9; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xCA; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xCB; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xCC; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xCD; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xCE; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xCF; not an opcode: reserved
	{ FST_X86, -1, ST0_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                    // 0xD0
	{ FST_X86, -1, ST1_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                    // 0xD1
	{ FST_X86, -1, ST2_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                    // 0xD2
	{ FST_X86, -1, ST3_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                    // 0xD3
	{ FST_X86, -1, ST4_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                    // 0xD4
	{ FST_X86, -1, ST5_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                    // 0xD5
	{ FST_X86, -1, ST6_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                    // 0xD6
	{ FST_X86, -1, ST7_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                    // 0xD7
	{ FSTP_X86, -1, ST0_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                   // 0xD8
	{ FSTP_X86, -1, ST1_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                   // 0xD9
	{ FSTP_X86, -1, ST2_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                   // 0xDA
	{ FSTP_X86, -1, ST3_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                   // 0xDB
	{ FSTP_X86, -1, ST4_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                   // 0xDC
	{ FSTP_X86, -1, ST5_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                   // 0xDD
	{ FSTP_X86, -1, ST6_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                   // 0xDE
	{ FSTP_X86, -1, ST7_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                   // 0xDF
	{ FUCOM_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xE0
	{ FUCOM_X86, -1, ST1_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xE1
	{ FUCOM_X86, -1, ST2_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xE2
	{ FUCOM_X86, -1, ST3_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xE3
	{ FUCOM_X86, -1, ST4_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xE4
	{ FUCOM_X86, -1, ST5_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xE5
	{ FUCOM_X86, -1, ST6_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xE6
	{ FUCOM_X86, -1, ST7_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xE7
	{ FUCOMP_X86, -1, ST0_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },			     // 0xE8
	{ FUCOMP_X86, -1, ST1_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },			     // 0xE9
	{ FUCOMP_X86, -1, ST2_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },			     // 0xEA
	{ FUCOMP_X86, -1, ST3_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },			     // 0xEB
	{ FUCOMP_X86, -1, ST4_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },			     // 0xEC
	{ FUCOMP_X86, -1, ST5_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },			     // 0xED
	{ FUCOMP_X86, -1, ST6_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },			     // 0xEE
	{ FUCOMP_X86, -1, ST7_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },			     // 0xEF
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xF0; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xF1; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xF2; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xF3; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xF4; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xF5; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xF6; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xF7; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xF8; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xF9; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xFA; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xFB; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xFC; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xFD; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xFE; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT }      // 0xFF; not an opcode: reserved
};

// Appendix A: A.5.2 table A-19
// opcode: 0xDE
const struct Opcode escapeDEOpcodeMapBits[8] = // [ModR/M bits 3-5]
{
	{ FIADD_X86, -1, M_W_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FIMUL_X86, -1, M_W_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FICOM_X86, -1, M_W_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FICOMP_X86, -1, M_W_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FISUB_X86, -1, M_W_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FISUBR_X86, -1, M_W_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FIDIV_X86, -1, M_W_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FIDIVR_X86, -1, M_W_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT }
};

// Appendix A: A.5.2 table A-20
// opcode: 0xDE
const struct Opcode escapeDEOpcodeMapByte[0x40] = // [ModR/M byte - 0xC0]
{
	{ FADDP_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xC0
	{ FADDP_X86, -1, ST1_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xC1
	{ FADDP_X86, -1, ST2_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xC2
	{ FADDP_X86, -1, ST3_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xC3
	{ FADDP_X86, -1, ST4_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xC4
	{ FADDP_X86, -1, ST5_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xC5
	{ FADDP_X86, -1, ST6_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xC6
	{ FADDP_X86, -1, ST7_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xC7
	{ FMULP_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xC8
	{ FMULP_X86, -1, ST1_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xC9
	{ FMULP_X86, -1, ST2_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xCA
	{ FMULP_X86, -1, ST3_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xCB
	{ FMULP_X86, -1, ST4_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xCC
	{ FMULP_X86, -1, ST5_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xCD
	{ FMULP_X86, -1, ST6_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xCE
	{ FMULP_X86, -1, ST7_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xCF
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD0; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD1; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD2; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD3; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD4; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD5; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD6; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD7; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD8; not an opcode: reserved
	{ FCOMPP_X86, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },	         // 0xD9; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xDA; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xDB; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xDC; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xDD; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xDE; not an opcode: reserved
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xDF; not an opcode: reserved
	{ FSUBRP_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xE0
	{ FSUBRP_X86, -1, ST1_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xE1
	{ FSUBRP_X86, -1, ST2_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xE2
	{ FSUBRP_X86, -1, ST3_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xE3
	{ FSUBRP_X86, -1, ST4_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xE4
	{ FSUBRP_X86, -1, ST5_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xE5
	{ FSUBRP_X86, -1, ST6_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xE6
	{ FSUBRP_X86, -1, ST7_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xE7
	{ FSUBP_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xE8
	{ FSUBP_X86, -1, ST1_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xE9
	{ FSUBP_X86, -1, ST2_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xEA
	{ FSUBP_X86, -1, ST3_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xEB
	{ FSUBP_X86, -1, ST4_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xEC
	{ FSUBP_X86, -1, ST5_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xED
	{ FSUBP_X86, -1, ST6_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xEE
	{ FSUBP_X86, -1, ST7_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xEF
	{ FDIVRP_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xF0
	{ FDIVRP_X86, -1, ST1_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xF1
	{ FDIVRP_X86, -1, ST2_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xF2
	{ FDIVRP_X86, -1, ST3_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xF3
	{ FDIVRP_X86, -1, ST4_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xF4
	{ FDIVRP_X86, -1, ST5_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xF5
	{ FDIVRP_X86, -1, ST6_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xF6
	{ FDIVRP_X86, -1, ST7_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xF7
	{ FDIVP_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xF8
	{ FDIVP_X86, -1, ST1_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xF9
	{ FDIVP_X86, -1, ST2_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xFA
	{ FDIVP_X86, -1, ST3_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xFB
	{ FDIVP_X86, -1, ST4_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xFC
	{ FDIVP_X86, -1, ST5_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xFD
	{ FDIVP_X86, -1, ST6_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xFE
	{ FDIVP_X86, -1, ST7_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xFF
};

// Appendix A: A.5.2 table A-21
// opcode: 0xDF
const struct Opcode escapeDFOpcodeMapBits[8] = // [ModR/M bits 3-5]
{
	{ FILD_X86, -1, M_W_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FISTTP_X86, -1, M_W_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FIST_X86, -1, M_W_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FISTP_X86, -1, M_W_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FBLD_X86, -1, M_T_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FILD_X86, -1, M_Q_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FBSTP_X86, -1, M_T_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },
	{ FISTP_X86, -1, M_Q_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT }
};

// Appendix A: A.5.2 table A-22
// opcode: 0xDF
const struct Opcode escapeDFOpcodeMapByte[0x40] = // [ModR/M byte - 0xC0]
{
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xC0
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xC1
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xC2
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xC3
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xC4
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xC5
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xC6
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xC7
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xC8
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xC9
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xCA
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xCB
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xCC
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xCD
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xCE
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xCF
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD0
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD1
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD2
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD3
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD4
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD5
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD6
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD7
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD8
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xD9
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xDA
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xDB
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xDC
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xDD
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xDE
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xDF
	{ FSTSW_X86, -1, AX_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                   // 0xE0
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xE1
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xE2
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xE3
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xE4
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xE5
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xE6
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xE7
	{ FUCOMIP_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT},                        // 0xE8
	{ FUCOMIP_X86, -1, ST0_X86_OC, ST1_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xE9
	{ FUCOMIP_X86, -1, ST0_X86_OC, ST2_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xEA
	{ FUCOMIP_X86, -1, ST0_X86_OC, ST3_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xEB
	{ FUCOMIP_X86, -1, ST0_X86_OC, ST4_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xEC
	{ FUCOMIP_X86, -1, ST0_X86_OC, ST5_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xED
	{ FUCOMIP_X86, -1, ST0_X86_OC, ST6_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xEE
	{ FUCOMIP_X86, -1, ST0_X86_OC, ST7_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                       // 0xEF
	{ FCOMIP_X86, -1, ST0_X86_OC, ST0_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT},                         // 0xF0
	{ FCOMIP_X86, -1, ST0_X86_OC, ST1_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xF1
	{ FCOMIP_X86, -1, ST0_X86_OC, ST2_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xF2
	{ FCOMIP_X86, -1, ST0_X86_OC, ST3_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xF3
	{ FCOMIP_X86, -1, ST0_X86_OC, ST4_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xF4
	{ FCOMIP_X86, -1, ST0_X86_OC, ST5_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xF5
	{ FCOMIP_X86, -1, ST0_X86_OC, ST6_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xF6
	{ FCOMIP_X86, -1, ST0_X86_OC, ST7_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                        // 0xF7
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xF8
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xF9
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xFA
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xFB
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xFC
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xFD
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },     // 0xFE
	{ NO_OPCODE, -1, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT }      // 0xFF
};