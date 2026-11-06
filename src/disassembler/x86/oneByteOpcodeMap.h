#pragma once
#include "disassemblyStructs.h"

// Intel 64 and IA-32 Architectures Software Developer’s Manuals:
//	intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html

// Appendix A: A.3 table A-2
const struct Opcode oneByteOpcodeMap[0x100] = // [byte]
{
	{ ADD_X86, -1, Eb, Gb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x00
	{ ADD_X86, -1, Ev, Gv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x01
	{ ADD_X86, -1, Gb, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x02
	{ ADD_X86, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x03
	{ ADD_X86, -1, AL_CODE, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0x04
	{ ADD_X86, -1, rAX, Iz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x05
	{ PUSH_X86, -1, ES_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                              // 0x06
	{ POP_X86, -1, ES_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                               // 0x07
	{ OR_X86, -1, Eb, Gb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                       // 0x08
	{ OR_X86, -1, Ev, Gv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                       // 0x09
	{ OR_X86, -1, Gb, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                       // 0x0A
	{ OR_X86, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                       // 0x0B
	{ OR_X86, -1, AL_CODE, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                  // 0x0C
	{ OR_X86, -1, rAX, Iz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x0D
	{ PUSH_X86, -1, CS_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                              // 0x0E
	{ NO_OPCODE, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },          // 0x0F; not an opcode: 2-byte escape
	{ ADC_X86, -1, Eb, Gb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x10
	{ ADC_X86, -1, Ev, Gv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x11
	{ ADC_X86, -1, Gb, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x12
	{ ADC_X86, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x13
	{ ADC_X86, -1, AL_CODE, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0x14
	{ ADC_X86, -1, rAX, Iz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x15
	{ PUSH_X86, -1, SS_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                              // 0x16
	{ POP_X86, -1, SS_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                               // 0x17
	{ SBB_X86, -1, Eb, Gb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x18
	{ SBB_X86, -1, Ev, Gv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x19
	{ SBB_X86, -1, Gb, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x1A
	{ SBB_X86, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x1B
	{ SBB_X86, -1, AL_CODE, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0x1C
	{ SBB_X86, -1, rAX, Iz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x1D
	{ PUSH_X86, -1, DS_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                              // 0x1E
	{ POP_X86, -1, DS_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                               // 0x1F
	{ AND_X86, -1, Eb, Gb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x20
	{ AND_X86, -1, Ev, Gv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x21
	{ AND_X86, -1, Gb, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x22
	{ AND_X86, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x23
	{ AND_X86, -1, AL_CODE, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0x24
	{ AND_X86, -1, rAX, Iz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x25
	{ NO_OPCODE, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },          // 0x26; not an opcode: ES segment prefix
	{ DAA_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                       // 0x27
	{ SUB_X86, -1, Eb, Gb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x28
	{ SUB_X86, -1, Ev, Gv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x29
	{ SUB_X86, -1, Gb, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x2A
	{ SUB_X86, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x2B
	{ SUB_X86, -1, AL_CODE, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0x2C
	{ SUB_X86, -1, rAX, Iz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x2D
	{ NO_OPCODE, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },          // 0x2E; not an opcode: CS segment prefix
	{ DAS_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                       // 0x2F
	{ XOR_X86, -1, Eb, Gb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x30
	{ XOR_X86, -1, Ev, Gv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x31
	{ XOR_X86, -1, Gb, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x32
	{ XOR_X86, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x33
	{ XOR_X86, -1, AL_CODE, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0x34
	{ XOR_X86, -1, rAX, Iz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x35
	{ NO_OPCODE, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },          // 0x36; not an opcode: SS segment prefix
	{ AAA_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                       // 0x37
	{ CMP_X86, -1, Eb, Gb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x38
	{ CMP_X86, -1, Ev, Gv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x39
	{ CMP_X86, -1, Gb, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x3A
	{ CMP_X86, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x3B
	{ CMP_X86, -1, AL_CODE, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0x3C
	{ CMP_X86, -1, rAX, Iz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x3D
	{ NO_OPCODE, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },          // 0x3E; not an opcode: DS segment prefix
	{ AAS_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                       // 0x3F
	{ INC_X86, -1, rAX, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x40; not an opcode if in 64-bit mode: REX
	{ INC_X86, -1, rCX, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x41; not an opcode if in 64-bit mode: REX.B
	{ INC_X86, -1, rDX, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x42; not an opcode if in 64-bit mode: REX.X
	{ INC_X86, -1, rBX, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x43; not an opcode if in 64-bit mode: REX.XB
	{ INC_X86, -1, rSP, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x44; not an opcode if in 64-bit mode: REX.R
	{ INC_X86, -1, rBP, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x45; not an opcode if in 64-bit mode: REX.RB
	{ INC_X86, -1, rSI, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x46; not an opcode if in 64-bit mode: REX.RX
	{ INC_X86, -1, rDI, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x47; not an opcode if in 64-bit mode: REX.RXB
	{ DEC_X86, -1, rAX, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x48; not an opcode if in 64-bit mode: REX.W
	{ DEC_X86, -1, rCX, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x49; not an opcode if in 64-bit mode: REX.WB
	{ DEC_X86, -1, rDX, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x4A; not an opcode if in 64-bit mode: REX.WX
	{ DEC_X86, -1, rBX, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x4B; not an opcode if in 64-bit mode: REX.WXB
	{ DEC_X86, -1, rSP, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x4C; not an opcode if in 64-bit mode: REX.WR
	{ DEC_X86, -1, rBP, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x4D; not an opcode if in 64-bit mode: REX.WRB
	{ DEC_X86, -1, rSI, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x4E; not an opcode if in 64-bit mode: REX.WRX
	{ DEC_X86, -1, rDI, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x4F; not an opcode if in 64-bit mode: REX.WRXB
	{ PUSH_X86, -1, rAX_r8, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                               // 0x50
	{ PUSH_X86, -1, rCX_r9, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                               // 0x51
	{ PUSH_X86, -1, rDX_r10, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                              // 0x52
	{ PUSH_X86, -1, rBX_r11, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                              // 0x53
	{ PUSH_X86, -1, rSP_r12, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                              // 0x54
	{ PUSH_X86, -1, rBP_r13, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                              // 0x55
	{ PUSH_X86, -1, rSI_r14, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                              // 0x56
	{ PUSH_X86, -1, rDI_r15, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                              // 0x57
	{ POP_X86, -1, rAX_r8, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                                // 0x58
	{ POP_X86, -1, rCX_r9, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                                // 0x59
	{ POP_X86, -1, rDX_r10, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                               // 0x5A
	{ POP_X86, -1, rBX_r11, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                               // 0x5B
	{ POP_X86, -1, rSP_r12, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                               // 0x5C
	{ POP_X86, -1, rBP_r13, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                               // 0x5D
	{ POP_X86, -1, rSI_r14, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                               // 0x5E
	{ POP_X86, -1, rDI_r15, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                               // 0x5F
	{ PUSHAD_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                    // 0x60; PUSHA if operand size is 16-bit
	{ POPAD_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                     // 0x61; POPA if operand size is 16-bit
	{ BOUND_X86, -1, Gv, Ma, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                               // 0x62
	{ ARPL_X86, -1, Ew, Gw, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                                // 0x63; { MOVSXD_X86, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, o64 } if 64-bit mode
	{ NO_OPCODE, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },          // 0x64; not an opcode: FS segment prefix
	{ NO_OPCODE, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },          // 0x65; not an opcode: GS segment prefix
	{ NO_OPCODE, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },          // 0x66; not an opcode: Operand size prefix
	{ NO_OPCODE, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },          // 0x67; not an opcode: Address size prefix
	{ PUSH_X86, -1, Iz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                                   // 0x68
	{ IMUL_X86, -1, Gv, Ev, Iz, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                                  // 0x69
	{ PUSH_X86, -1, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                                   // 0x6A
	{ IMUL_X86, -1, Gv, Ev, Ib, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                                  // 0x6B
	{ INS_X86, -1, Yb, DX_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0x6C; alternate mnemonics: INSB
	{ INS_X86, -1, Yz, DX_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0x6D; alternate mnemonics: INSW, INSD
	{ OUTS_X86, -1, DX_CODE, Xb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                // 0x6E; alternate mnemonics: OUTSB
	{ OUTS_X86, -1, DX_CODE, Xz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                // 0x6F; alternate mnemonics: OUTSW, OUTSD
	{ JO_SHORT_X86, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                               // 0x70
	{ JNO_SHORT_X86, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                              // 0x71
	{ JB_SHORT_X86, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                               // 0x72; alternate mnemonics: JNAE, JC
	{ JNB_SHORT_X86, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                              // 0x73; alternate mnemonics: JAE, JNC
	{ JZ_SHORT_X86, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                               // 0x74; alternate mnemonics: JE
	{ JNZ_SHORT_X86, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                              // 0x75; alternate mnemonics: JNE
	{ JBE_SHORT_X86, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                              // 0x76; alternate mnemonics: JNA
	{ JA_SHORT_X86, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                               // 0x77; alternate mnemonics: JNBE
	{ JS_SHORT_X86, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                               // 0x78
	{ JNS_SHORT_X86, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                              // 0x79
	{ JP_SHORT_X86, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                               // 0x7A; alternate mnemonics: JPE
	{ JNP_SHORT_X86, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                              // 0x7B; alternate mnemonics: JPO
	{ JL_SHORT_X86, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                               // 0x7C; alternate mnemonics: JNGE
	{ JGE_SHORT_X86, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                              // 0x7D; alternate mnemonics: JNL
	{ JLE_SHORT_X86, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                              // 0x7E; alternate mnemonics: JNG
	{ JG_SHORT_X86, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                               // 0x7F; alternate mnemonics: JNLE
	{ EXTENDED_OPCODE_X86, 0, Eb, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                           // 0x80; an extended opcode
	{ EXTENDED_OPCODE_X86, 0, Ev, Iz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                           // 0x81; an extended opcode
	{ EXTENDED_OPCODE_X86, 0, Eb, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                      // 0x82; an extended opcode
	{ EXTENDED_OPCODE_X86, 0, Ev, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                           // 0x83; an extended opcode
	{ TEST_X86, -1, Eb, Gb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x84
	{ TEST_X86, -1, Ev, Gv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x85
	{ XCHG_X86, -1, Eb, Gb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x86
	{ XCHG_X86, -1, Ev, Gv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x87
	{ MOV_X86, -1, Eb, Gb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x88
	{ MOV_X86, -1, Ev, Gv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x89
	{ MOV_X86, -1, Gb, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x8A
	{ MOV_X86, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x8B
	{ MOV_X86, -1, MW_RWDQ_OC, Sw, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                              // 0x8C
	{ LEA_X86, -1, Gv, M, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                       // 0x8D
	{ MOV_X86, -1, Sw, MW_RWDQ_OC, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                              // 0x8E
	{ EXTENDED_OPCODE_X86, 1, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                         // 0x8F; an extended opcode
	{ NOP_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },            // 0x90; this instruction is technically XCHG rAX, rAX, but it functions as NOP due to not chaning state
	{ XCHG_X86, -1, rCX_r9, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                    // 0x91
	{ XCHG_X86, -1, rDX_r10, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                   // 0x92
	{ XCHG_X86, -1, rBX_r11, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                   // 0x93
	{ XCHG_X86, -1, rSP_r12, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                   // 0x94
	{ XCHG_X86, -1, rBP_r13, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                   // 0x95
	{ XCHG_X86, -1, rSI_r14, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                   // 0x96
	{ XCHG_X86, -1, rDI_r15, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                   // 0x97
	{ CWDE_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },           // 0x98; CBW if operand size is 16-bit, CDQE if operand size is 64-bit
	{ CDQ_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },            // 0x99; CWD if operand size is 16-bit, CQO if operand size is 64-bit
	{ CALL_FAR_X86, -1, Ap, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                               // 0x9A
	{ WAIT_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },           // 0x9B; alternate mnemonics: FWAIT
	{ PUSHF_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                     // 0x9C
	{ POPF_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                      // 0x9D
	{ SAHF_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },           // 0x9E
	{ LAHF_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },           // 0x9F
	{ MOV_X86, -1, AL_CODE, Ob, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xA0
	{ MOV_X86, -1, rAX, Ov, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0xA1
	{ MOV_X86, -1, Ob, AL_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xA2
	{ MOV_X86, -1, Ov, rAX, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0xA3
	{ MOVS_X86, -1, Yb, Xb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0xA4; alternate mnemonics: MOVSB
	{ MOVS_X86, -1, Yv, Xv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0xA5; alternate mnemonics: MOVSW, MOVSD, MOVSQ
	{ CMPS_X86, -1, Xb, Yb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0xA6; alternate mnemonics: CMPSB
	{ CMPS_X86, -1, Xv, Yv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0xA7; alternate mnemonics: CMPSW, CMPSD
	{ TEST_X86, -1, AL_CODE, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                // 0xA8
	{ TEST_X86, -1, rAX, Iz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                    // 0xA9
	{ STOS_X86, -1, Yb, AL_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                // 0xAA; alternate mnemonics: STOSB
	{ STOS_X86, -1, Yv, rAX, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                    // 0xAB; alternate mnemonics: STOSW, STOSD, STOSQ
	{ LODS_X86, -1, AL_CODE, Xb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                // 0xAC; alternate mnemonics: LODSB
	{ LODS_X86, -1, rAX, Xv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                    // 0xAD; alternate mnemonics: LODSW, LODSD, LODSQ
	{ SCAS_X86, -1, AL_CODE, Yb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                // 0xAE; alternate mnemonics: SCASB
	{ SCAS_X86, -1, rAX, Yv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                    // 0xAF; alternate mnemonics: SCASW, SCASD, SCASQ
	{ MOV_X86, -1, AL_R8B, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                  // 0xB0
	{ MOV_X86, -1, CL_R9B, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                  // 0xB1
	{ MOV_X86, -1, DL_R10B, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xB2
	{ MOV_X86, -1, BL_R11B, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xB3
	{ MOV_X86, -1, AH_R12B, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xB4
	{ MOV_X86, -1, CH_R13B, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xB5
	{ MOV_X86, -1, DH_R14B, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xB6
	{ MOV_X86, -1, BH_R15B, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xB7
	{ MOV_X86, -1, rAX_r8, Iv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                  // 0xB8
	{ MOV_X86, -1, rCX_r9, Iv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                  // 0xB9
	{ MOV_X86, -1, rDX_r10, Iv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xBA
	{ MOV_X86, -1, rBX_r11, Iv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xBB
	{ MOV_X86, -1, rSP_r12, Iv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xBC
	{ MOV_X86, -1, rBP_r13, Iv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xBD
	{ MOV_X86, -1, rSI_r14, Iv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xBE
	{ MOV_X86, -1, rDI_r15, Iv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xBF
	{ EXTENDED_OPCODE_X86, 2, Eb, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                           // 0xC0; an extended opcode
	{ EXTENDED_OPCODE_X86, 2, Ev, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                           // 0xC1; an extended opcode
	{ RET_NEAR_X86, -1, Iw, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                               // 0xC2
	{ RET_NEAR_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                  // 0xC3
	{ LES_X86, -1, Gz, Mp, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                                 // 0xC4; not an opcode if in 64-bit mode: VEX prefix starter +2 bytes
	{ LDS_X86, -1, Gz, Mp, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                                 // 0xC5; not an opcode if in 64-bit mode: VEX prefix starter +1 bytes
	{ EXTENDED_OPCODE_X86, 11, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },// 0xC6; an extended opcode
	{ EXTENDED_OPCODE_X86, 11, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },// 0xC7; an extended opcode
	{ ENTER_X86, -1, Iw, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                    // 0xC8
	{ LEAVE_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                     // 0xC9
	{ RET_FAR_X86, -1, Iw, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                     // 0xCA
	{ RET_FAR_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },        // 0xCB
	{ INT3_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },           // 0xCC
	{ INT_X86, -1, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                         // 0xCD
	{ INTO_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                      // 0xCE
	{ IRET_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },           // 0xCF; alternate mnemonics: IRETD, IRETQ
	{ EXTENDED_OPCODE_X86, 2, Eb, ONE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                          // 0xD0; an extended opcode
	{ EXTENDED_OPCODE_X86, 2, Ev, ONE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                          // 0xD1; an extended opcode
	{ EXTENDED_OPCODE_X86, 2, Eb, CL_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                      // 0xD2; an extended opcode
	{ EXTENDED_OPCODE_X86, 2, Ev, CL_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                      // 0xD3; an extended opcode
	{ AAM_X86, -1, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                    // 0xD4
	{ AAD_X86, -1, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                    // 0xD5
	{ NO_OPCODE, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },          // 0xD6; not an opcode: reserved
	{ XLAT_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },           // 0xD7; alternate mnemonics: XLATB
	{ NO_OPCODE, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },          // 0xD8; not an opcode: escape to coprocessor instruction set
	{ NO_OPCODE, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },          // 0xD9; not an opcode: escape to coprocessor instruction set
	{ NO_OPCODE, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },          // 0xDA; not an opcode: escape to coprocessor instruction set
	{ NO_OPCODE, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },          // 0xDB; not an opcode: escape to coprocessor instruction set
	{ NO_OPCODE, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },          // 0xDC; not an opcode: escape to coprocessor instruction set
	{ NO_OPCODE, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },          // 0xDD; not an opcode: escape to coprocessor instruction set
	{ NO_OPCODE, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },          // 0xDE; not an opcode: escape to coprocessor instruction set
	{ NO_OPCODE, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },          // 0xDF; not an opcode: escape to coprocessor instruction set
	{ LOOPNZ_X86, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                                 // 0xE0; alternate mnemonics: LOOPNE
	{ LOOPZ_X86, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                                  // 0xE1; alternate mnemonics: LOOPE
	{ LOOP_X86, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                                   // 0xE2
	{ JRCXZ_X86, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                                  // 0xE3
	{ _IN_X86, -1, AL_CODE, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xE4
	{ _IN_X86, -1, rAX, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0xE5
	{ _OUT_X86, -1, Ib, AL_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                // 0xE6
	{ _OUT_X86, -1, Ib, rAX, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                    // 0xE7
	{ CALL_NEAR_X86, -1, Jz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                              // 0xE8
	{ JMP_NEAR_X86, -1, Jz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                               // 0xE9
	{ JMP_FAR_X86, -1, Ap, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                // 0xEA
	{ JMP_SHORT_X86, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                              // 0xEB
	{ _IN_X86, -1, AL_CODE, DX_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                            // 0xEC
	{ _IN_X86, -1, rAX, DX_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                // 0xED
	{ _OUT_X86, -1, DX_CODE, AL_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                           // 0xEE
	{ _OUT_X86, -1, DX_CODE, rAX, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                               // 0xEF
	{ NO_OPCODE, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },          // 0xF0; not an opcode: LOCK prefix
	{ INT1_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },           // 0xF1
	{ NO_OPCODE, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },          // 0xF2; not an opcode: REPNZ prefix
	{ NO_OPCODE, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },          // 0xF3; not an opcode: REPZ prefix
	{ HLT_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },            // 0xF4
	{ CMC_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },            // 0xF5
	{ EXTENDED_OPCODE_X86, 3, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },              // 0xF6; an extended opcode
	{ EXTENDED_OPCODE_X86, 3, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },              // 0xF7; an extended opcode
	{ CLC_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },            // 0xF8
	{ STC_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },            // 0xF9
	{ CLI_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },            // 0xFA
	{ STI_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },            // 0xFB
	{ CLD_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },            // 0xFC
	{ STD_X86, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },            // 0xFD
	{ EXTENDED_OPCODE_X86, 4, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT }, // 0xFE; an extended opcode
	{ EXTENDED_OPCODE_X86, 5, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT }  // 0xFF; an extended opcode
};
const struct Opcode alternateX63 = { MOVSXD_X86, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, o64 };               // 0x63 when 64-bit mode
