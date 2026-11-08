#pragma once
#include "disassemblyStructs.h"

// Intel 64 and IA-32 Architectures Software Developer’s Manuals:
//	intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html

// Appendix A: A.3 table A-2
const struct Opcode oneByteOpcodeMap[0x100] = // [byte]
{
	{ ADD_X86, RM_B_X86_OC, REG_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x00
	{ ADD_X86, RM_WDQ_X86_OC, REG_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x01
	{ ADD_X86, REG_B_X86_OC, RM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x02
	{ ADD_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x03
	{ ADD_X86, AL_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                 // 0x04
	{ ADD_X86, AX_WDQ_X86_OC, IMM_WD_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                     // 0x05
	{ PUSH_X86, ES_X86_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                              // 0x06
	{ POP_X86, ES_X86_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                               // 0x07
	{ OR_X86, RM_B_X86_OC, REG_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                       // 0x08
	{ OR_X86, RM_WDQ_X86_OC, REG_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                       // 0x09
	{ OR_X86, REG_B_X86_OC, RM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                       // 0x0A
	{ OR_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                       // 0x0B
	{ OR_X86, AL_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                  // 0x0C
	{ OR_X86, AX_WDQ_X86_OC, IMM_WD_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x0D
	{ PUSH_X86, CS_X86_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                              // 0x0E
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },          // 0x0F; not an opcode: 2-byte escape
	{ ADC_X86, RM_B_X86_OC, REG_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x10
	{ ADC_X86, RM_WDQ_X86_OC, REG_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x11
	{ ADC_X86, REG_B_X86_OC, RM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x12
	{ ADC_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x13
	{ ADC_X86, AL_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                 // 0x14
	{ ADC_X86, AX_WDQ_X86_OC, IMM_WD_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                     // 0x15
	{ PUSH_X86, SS_X86_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                              // 0x16
	{ POP_X86, SS_X86_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                               // 0x17
	{ SBB_X86, RM_B_X86_OC, REG_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x18
	{ SBB_X86, RM_WDQ_X86_OC, REG_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x19
	{ SBB_X86, REG_B_X86_OC, RM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x1A
	{ SBB_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x1B
	{ SBB_X86, AL_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                 // 0x1C
	{ SBB_X86, AX_WDQ_X86_OC, IMM_WD_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                     // 0x1D
	{ PUSH_X86, DS_X86_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                              // 0x1E
	{ POP_X86, DS_X86_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                               // 0x1F
	{ AND_X86, RM_B_X86_OC, REG_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x20
	{ AND_X86, RM_WDQ_X86_OC, REG_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x21
	{ AND_X86, REG_B_X86_OC, RM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x22
	{ AND_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x23
	{ AND_X86, AL_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                 // 0x24
	{ AND_X86, AX_WDQ_X86_OC, IMM_WD_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                     // 0x25
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },          // 0x26; not an opcode: ES segment prefix
	{ DAA_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                       // 0x27
	{ SUB_X86, RM_B_X86_OC, REG_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x28
	{ SUB_X86, RM_WDQ_X86_OC, REG_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x29
	{ SUB_X86, REG_B_X86_OC, RM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x2A
	{ SUB_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x2B
	{ SUB_X86, AL_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                 // 0x2C
	{ SUB_X86, AX_WDQ_X86_OC, IMM_WD_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                     // 0x2D
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },          // 0x2E; not an opcode: CS segment prefix
	{ DAS_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                       // 0x2F
	{ XOR_X86, RM_B_X86_OC, REG_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x30
	{ XOR_X86, RM_WDQ_X86_OC, REG_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x31
	{ XOR_X86, REG_B_X86_OC, RM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x32
	{ XOR_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x33
	{ XOR_X86, AL_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                 // 0x34
	{ XOR_X86, AX_WDQ_X86_OC, IMM_WD_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                     // 0x35
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },          // 0x36; not an opcode: SS segment prefix
	{ AAA_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                       // 0x37
	{ CMP_X86, RM_B_X86_OC, REG_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x38
	{ CMP_X86, RM_WDQ_X86_OC, REG_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x39
	{ CMP_X86, REG_B_X86_OC, RM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x3A
	{ CMP_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x3B
	{ CMP_X86, AL_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                 // 0x3C
	{ CMP_X86, AX_WDQ_X86_OC, IMM_WD_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                     // 0x3D
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },          // 0x3E; not an opcode: DS segment prefix
	{ AAS_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                       // 0x3F
	{ INC_X86, AX_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                                   // 0x40; not an opcode if in 64-bit mode: REX
	{ INC_X86, CX_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                                   // 0x41; not an opcode if in 64-bit mode: REX.B
	{ INC_X86, DX_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                                   // 0x42; not an opcode if in 64-bit mode: REX.X
	{ INC_X86, BX_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                                   // 0x43; not an opcode if in 64-bit mode: REX.XB
	{ INC_X86, SP_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                                   // 0x44; not an opcode if in 64-bit mode: REX.R
	{ INC_X86, BP_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                                   // 0x45; not an opcode if in 64-bit mode: REX.RB
	{ INC_X86, SI_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                                   // 0x46; not an opcode if in 64-bit mode: REX.RX
	{ INC_X86, DI_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                                   // 0x47; not an opcode if in 64-bit mode: REX.RXB
	{ DEC_X86, AX_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                                   // 0x48; not an opcode if in 64-bit mode: REX.W
	{ DEC_X86, CX_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                                   // 0x49; not an opcode if in 64-bit mode: REX.WB
	{ DEC_X86, DX_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                                   // 0x4A; not an opcode if in 64-bit mode: REX.WX
	{ DEC_X86, BX_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                                   // 0x4B; not an opcode if in 64-bit mode: REX.WXB
	{ DEC_X86, SP_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                                   // 0x4C; not an opcode if in 64-bit mode: REX.WR
	{ DEC_X86, BP_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                                   // 0x4D; not an opcode if in 64-bit mode: REX.WRB
	{ DEC_X86, SI_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                                   // 0x4E; not an opcode if in 64-bit mode: REX.WRX
	{ DEC_X86, DI_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                                   // 0x4F; not an opcode if in 64-bit mode: REX.WRXB
	{ PUSH_X86, AX_R8_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, d64 },                               // 0x50
	{ PUSH_X86, CX_R9_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, d64 },                               // 0x51
	{ PUSH_X86, DX_R10_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, d64 },                              // 0x52
	{ PUSH_X86, BX_R11_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, d64 },                              // 0x53
	{ PUSH_X86, SP_R12_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, d64 },                              // 0x54
	{ PUSH_X86, BP_R13_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, d64 },                              // 0x55
	{ PUSH_X86, SI_R14_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, d64 },                              // 0x56
	{ PUSH_X86, DI_R15_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, d64 },                              // 0x57
	{ POP_X86, AX_R8_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, d64 },                                // 0x58
	{ POP_X86, CX_R9_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, d64 },                                // 0x59
	{ POP_X86, DX_R10_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, d64 },                               // 0x5A
	{ POP_X86, BX_R11_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, d64 },                               // 0x5B
	{ POP_X86, SP_R12_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, d64 },                               // 0x5C
	{ POP_X86, BP_R13_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, d64 },                               // 0x5D
	{ POP_X86, SI_R14_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, d64 },                               // 0x5E
	{ POP_X86, DI_R15_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, d64 },                               // 0x5F
	{ PUSHAD_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                    // 0x60; PUSHA if operand size is 16-bit
	{ POPAD_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                     // 0x61; POPA if operand size is 16-bit
	{ BOUND_X86, REG_WDQ_X86_OC, M_OSO_DQ_X86_OC, NONE_OC, NONE_OC, i64 },                                               // 0x62
	{ ARPL_X86, RM_W_X86_OC, REG_W_X86_OC, NONE_OC, NONE_OC, i64 },                                                // 0x63; { MOVSXD_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, o64 } if 64-bit mode
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },          // 0x64; not an opcode: FS segment prefix
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },          // 0x65; not an opcode: GS segment prefix
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },          // 0x66; not an opcode: Operand size prefix
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },          // 0x67; not an opcode: Address size prefix
	{ PUSH_X86, IMM_WD_X86_OC, NONE_OC, NONE_OC, NONE_OC, d64 },                                   // 0x68
	{ IMUL_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, IMM_WD_X86_OC, NONE_OC, NO_SUPERSCRIPT },                                                  // 0x69
	{ PUSH_X86, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, d64 },                                   // 0x6A
	{ IMUL_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, IMM_B_X86_OC, NONE_OC, NO_SUPERSCRIPT },                                                  // 0x6B
	{ INS_X86, ES_MEM_DI_B_X86_OC, DX_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                 // 0x6C; alternate mnemonics: INSB
	{ INS_X86, ES_MEM_DI_WD_X86_OC, DX_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                 // 0x6D; alternate mnemonics: INSW, INSD
	{ OUTS_X86, DX_X86_OC, DS_MEM_SI_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                // 0x6E; alternate mnemonics: OUTSB
	{ OUTS_X86, DX_X86_OC, DS_MEM_SI_WD_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                // 0x6F; alternate mnemonics: OUTSW, OUTSD
	{ JO_SHORT_X86, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, f64 },                               // 0x70
	{ JNO_SHORT_X86, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, f64 },                              // 0x71
	{ JB_SHORT_X86, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, f64 },                               // 0x72; alternate mnemonics: JNAE, JC
	{ JNB_SHORT_X86, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, f64 },                              // 0x73; alternate mnemonics: JAE, JNC
	{ JZ_SHORT_X86, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, f64 },                               // 0x74; alternate mnemonics: JE
	{ JNZ_SHORT_X86, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, f64 },                              // 0x75; alternate mnemonics: JNE
	{ JBE_SHORT_X86, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, f64 },                              // 0x76; alternate mnemonics: JNA
	{ JA_SHORT_X86, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, f64 },                               // 0x77; alternate mnemonics: JNBE
	{ JS_SHORT_X86, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, f64 },                               // 0x78
	{ JNS_SHORT_X86, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, f64 },                              // 0x79
	{ JP_SHORT_X86, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, f64 },                               // 0x7A; alternate mnemonics: JPE
	{ JNP_SHORT_X86, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, f64 },                              // 0x7B; alternate mnemonics: JPO
	{ JL_SHORT_X86, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, f64 },                               // 0x7C; alternate mnemonics: JNGE
	{ JGE_SHORT_X86, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, f64 },                              // 0x7D; alternate mnemonics: JNL
	{ JLE_SHORT_X86, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, f64 },                              // 0x7E; alternate mnemonics: JNG
	{ JG_SHORT_X86, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, f64 },                               // 0x7F; alternate mnemonics: JNLE
	{ EXTENDED_GROUP1_X86, RM_B_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                           // 0x80; an extended opcode
	{ EXTENDED_GROUP1_X86, RM_WDQ_X86_OC, IMM_WD_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                           // 0x81; an extended opcode
	{ EXTENDED_GROUP1_X86, RM_B_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, i64 },                                      // 0x82; an extended opcode
	{ EXTENDED_GROUP1_X86, RM_WDQ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                           // 0x83; an extended opcode
	{ TEST_X86, RM_B_X86_OC, REG_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                     // 0x84
	{ TEST_X86, RM_WDQ_X86_OC, REG_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                     // 0x85
	{ XCHG_X86, RM_B_X86_OC, REG_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                     // 0x86
	{ XCHG_X86, RM_WDQ_X86_OC, REG_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                     // 0x87
	{ MOV_X86, RM_B_X86_OC, REG_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x88
	{ MOV_X86, RM_WDQ_X86_OC, REG_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x89
	{ MOV_X86, REG_B_X86_OC, RM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x8A
	{ MOV_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                      // 0x8B
	{ MOV_X86, R_WDQ_M_W_X86_OC, REG_SEGMENT_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                              // 0x8C
	{ LEA_X86, REG_WDQ_X86_OC, M_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                       // 0x8D
	{ MOV_X86, REG_SEGMENT_X86_OC, R_WDQ_M_W_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                              // 0x8E
	{ EXTENDED_GROUP1A_X86, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, d64 },                         // 0x8F; an extended opcode
	{ NOP_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },            // 0x90; this instruction is technically XCHG rAX, rAX, but it functions as NOP due to not chaning state
	{ XCHG_X86, CX_R9_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                    // 0x91
	{ XCHG_X86, DX_R10_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                   // 0x92
	{ XCHG_X86, BX_R11_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                   // 0x93
	{ XCHG_X86, SP_R12_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                   // 0x94
	{ XCHG_X86, BP_R13_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                   // 0x95
	{ XCHG_X86, SI_R14_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                   // 0x96
	{ XCHG_X86, DI_R15_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                   // 0x97
	{ CWDE_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },           // 0x98; CBW if operand size is 16-bit, CDQE if operand size is 64-bit
	{ CDQ_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },            // 0x99; CWD if operand size is 16-bit, CQO if operand size is 64-bit
	{ CALL_FAR_X86, CONST_SEG_MEM_X86_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                               // 0x9A
	{ WAIT_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },           // 0x9B; alternate mnemonics: FWAIT
	{ PUSHF_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, d64 },                     // 0x9C
	{ POPF_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, d64 },                      // 0x9D
	{ SAHF_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },           // 0x9E
	{ LAHF_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },           // 0x9F
	{ MOV_X86, AL_X86_OC, CONST_MEM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                 // 0xA0
	{ MOV_X86, AX_WDQ_X86_OC, CONST_MEM_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                     // 0xA1
	{ MOV_X86, CONST_MEM_B_X86_OC, AL_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                 // 0xA2
	{ MOV_X86, CONST_MEM_WDQ_X86_OC, AX_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                     // 0xA3
	{ MOVS_X86, ES_MEM_DI_B_X86_OC, DS_MEM_SI_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                     // 0xA4; alternate mnemonics: MOVSB
	{ MOVS_X86, ES_MEM_DI_WDQ_X86_OC, DS_MEM_SI_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                     // 0xA5; alternate mnemonics: MOVSW, MOVSD, MOVSQ
	{ CMPS_X86, DS_MEM_SI_B_X86_OC, ES_MEM_DI_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                     // 0xA6; alternate mnemonics: CMPSB
	{ CMPS_X86, DS_MEM_SI_WDQ_X86_OC, ES_MEM_DI_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                     // 0xA7; alternate mnemonics: CMPSW, CMPSD
	{ TEST_X86, AL_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                // 0xA8
	{ TEST_X86, AX_WDQ_X86_OC, IMM_WD_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                    // 0xA9
	{ STOS_X86, ES_MEM_DI_B_X86_OC, AL_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                // 0xAA; alternate mnemonics: STOSB
	{ STOS_X86, ES_MEM_DI_WDQ_X86_OC, AX_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                    // 0xAB; alternate mnemonics: STOSW, STOSD, STOSQ
	{ LODS_X86, AL_X86_OC, DS_MEM_SI_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                // 0xAC; alternate mnemonics: LODSB
	{ LODS_X86, AX_WDQ_X86_OC, DS_MEM_SI_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                    // 0xAD; alternate mnemonics: LODSW, LODSD, LODSQ
	{ SCAS_X86, AL_X86_OC, ES_MEM_DI_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                // 0xAE; alternate mnemonics: SCASB
	{ SCAS_X86, AX_WDQ_X86_OC, ES_MEM_DI_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                    // 0xAF; alternate mnemonics: SCASW, SCASD, SCASQ
	{ MOV_X86, AL_R8B_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                  // 0xB0
	{ MOV_X86, CL_R9B_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                  // 0xB1
	{ MOV_X86, DL_R10B_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                 // 0xB2
	{ MOV_X86, BL_R11B_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                 // 0xB3
	{ MOV_X86, AH_R12B_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                 // 0xB4
	{ MOV_X86, CH_R13B_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                 // 0xB5
	{ MOV_X86, DH_R14B_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                 // 0xB6
	{ MOV_X86, BH_R15B_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                 // 0xB7
	{ MOV_X86, AX_R8_WDQ_X86_OC, IMM_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                  // 0xB8
	{ MOV_X86, CX_R9_WDQ_X86_OC, IMM_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                  // 0xB9
	{ MOV_X86, DX_R10_WDQ_X86_OC, IMM_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                 // 0xBA
	{ MOV_X86, BX_R11_WDQ_X86_OC, IMM_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                 // 0xBB
	{ MOV_X86, SP_R12_WDQ_X86_OC, IMM_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                 // 0xBC
	{ MOV_X86, BP_R13_WDQ_X86_OC, IMM_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                 // 0xBD
	{ MOV_X86, SI_R14_WDQ_X86_OC, IMM_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                 // 0xBE
	{ MOV_X86, DI_R15_WDQ_X86_OC, IMM_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                 // 0xBF
	{ EXTENDED_GROUP2_X86, RM_B_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                           // 0xC0; an extended opcode
	{ EXTENDED_GROUP2_X86, RM_WDQ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                           // 0xC1; an extended opcode
	{ RET_NEAR_X86, IMM_W_X86_OC, NONE_OC, NONE_OC, NONE_OC, f64 },                               // 0xC2
	{ RET_NEAR_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, f64 },                  // 0xC3
	{ LES_X86, REG_WD_X86_OC, M_DF_X86_OC, NONE_OC, NONE_OC, i64 },                                                 // 0xC4; not an opcode if in 64-bit mode: VEX prefix starter +2 bytes
	{ LDS_X86, REG_WD_X86_OC, M_DF_X86_OC, NONE_OC, NONE_OC, i64 },                                                 // 0xC5; not an opcode if in 64-bit mode: VEX prefix starter +1 bytes
	{ EXTENDED_GROUP11_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },// 0xC6; an extended opcode
	{ EXTENDED_GROUP11_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },// 0xC7; an extended opcode
	{ ENTER_X86, IMM_W_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                    // 0xC8
	{ LEAVE_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, d64 },                     // 0xC9
	{ RET_FAR_X86, IMM_W_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                     // 0xCA
	{ RET_FAR_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },        // 0xCB
	{ INT3_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },           // 0xCC
	{ INT_X86, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                         // 0xCD
	{ INTO_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                      // 0xCE
	{ IRET_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },           // 0xCF; alternate mnemonics: IRETD, IRETQ
	{ EXTENDED_GROUP2_X86, RM_B_X86_OC, ONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xD0; an extended opcode
	{ EXTENDED_GROUP2_X86, RM_WDQ_X86_OC, ONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                          // 0xD1; an extended opcode
	{ EXTENDED_GROUP2_X86, RM_B_X86_OC, CL_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                      // 0xD2; an extended opcode
	{ EXTENDED_GROUP2_X86, RM_WDQ_X86_OC, CL_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                      // 0xD3; an extended opcode
	{ AAM_X86, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                                    // 0xD4
	{ AAD_X86, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                                    // 0xD5
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },          // 0xD6; not an opcode: reserved
	{ XLAT_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },           // 0xD7; alternate mnemonics: XLATB
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },          // 0xD8; not an opcode: escape to coprocessor instruction set
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },          // 0xD9; not an opcode: escape to coprocessor instruction set
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },          // 0xDA; not an opcode: escape to coprocessor instruction set
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },          // 0xDB; not an opcode: escape to coprocessor instruction set
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },          // 0xDC; not an opcode: escape to coprocessor instruction set
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },          // 0xDD; not an opcode: escape to coprocessor instruction set
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },          // 0xDE; not an opcode: escape to coprocessor instruction set
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },          // 0xDF; not an opcode: escape to coprocessor instruction set
	{ LOOPNZ_X86, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, f64 },                                 // 0xE0; alternate mnemonics: LOOPNE
	{ LOOPZ_X86, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, f64 },                                  // 0xE1; alternate mnemonics: LOOPE
	{ LOOP_X86, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, f64 },                                   // 0xE2
	{ JRCXZ_X86, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, f64 },                                  // 0xE3
	{ IN_X86, AL_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                 // 0xE4
	{ IN_X86, AX_WDQ_X86_OC, IMM_B_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                     // 0xE5
	{ OUT_X86, IMM_B_X86_OC, AL_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                // 0xE6
	{ OUT_X86, IMM_B_X86_OC, AX_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                    // 0xE7
	{ CALL_NEAR_X86, IMM_WD_X86_OC, NONE_OC, NONE_OC, NONE_OC, f64 },                              // 0xE8
	{ JMP_NEAR_X86, IMM_WD_X86_OC, NONE_OC, NONE_OC, NONE_OC, f64 },                               // 0xE9
	{ JMP_FAR_X86, CONST_SEG_MEM_X86_OC, NONE_OC, NONE_OC, NONE_OC, i64 },                                // 0xEA
	{ JMP_SHORT_X86, IMM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, f64 },                              // 0xEB
	{ IN_X86, AL_X86_OC, DX_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                            // 0xEC
	{ IN_X86, AX_WDQ_X86_OC, DX_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                                // 0xED
	{ OUT_X86, DX_X86_OC, AL_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                           // 0xEE
	{ OUT_X86, DX_X86_OC, AX_WDQ_X86_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },                               // 0xEF
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },          // 0xF0; not an opcode: LOCK prefix
	{ INT1_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },           // 0xF1
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },          // 0xF2; not an opcode: REPNZ prefix
	{ NO_OPCODE, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },          // 0xF3; not an opcode: REPZ prefix
	{ HLT_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },            // 0xF4
	{ CMC_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },            // 0xF5
	{ EXTENDED_GROUP3_X86, RM_B_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },              // 0xF6; an extended opcode
	{ EXTENDED_GROUP3_X86, RM_WDQ_X86_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },              // 0xF7; an extended opcode
	{ CLC_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },            // 0xF8
	{ STC_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },            // 0xF9
	{ CLI_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },            // 0xFA
	{ STI_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },            // 0xFB
	{ CLD_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },            // 0xFC
	{ STD_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT },            // 0xFD
	{ EXTENDED_GROUP4_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT }, // 0xFE; an extended opcode
	{ EXTENDED_GROUP5_X86, NONE_OC, NONE_OC, NONE_OC, NONE_OC, NO_SUPERSCRIPT }  // 0xFF; an extended opcode
};
const struct Opcode alternateX63 = { MOVSXD_X86, REG_WDQ_X86_OC, RM_WDQ_X86_OC, NONE_OC, NONE_OC, o64 };               // 0x63 when 64-bit mode
