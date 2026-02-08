#pragma once
#include "disassemblyStructs.h"

// Intel 64 and IA-32 Architectures Software Developer’s Manuals:
//	intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html

// Appendix A: A.3 table A-2
const struct Opcode oneByteOpcodeMap[0x100] = // [byte]
{
	{ ADD, -1, Eb, Gb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x00
	{ ADD, -1, Ev, Gv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x01
	{ ADD, -1, Gb, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x02
	{ ADD, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x03
	{ ADD, -1, AL_CODE, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0x04
	{ ADD, -1, rAX, Iz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x05
	{ PUSH_ES, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                   // 0x06
	{ POP_ES, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                    // 0x07
	{ OR, -1, Eb, Gb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                       // 0x08
	{ OR, -1, Ev, Gv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                       // 0x09
	{ OR, -1, Gb, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                       // 0x0A
	{ OR, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                       // 0x0B
	{ OR, -1, AL_CODE, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                  // 0x0C
	{ OR, -1, rAX, Iz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x0D
	{ PUSH_CS, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                   // 0x0E
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },    // 0x0F; not an opcode: 2-byte escape
	{ ADC, -1, Eb, Gb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x10
	{ ADC, -1, Ev, Gv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x11
	{ ADC, -1, Gb, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x12
	{ ADC, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x13
	{ ADC, -1, AL_CODE, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0x14
	{ ADC, -1, rAX, Iz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x15
	{ PUSH_SS, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                   // 0x16
	{ POP_SS, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                    // 0x17
	{ SBB, -1, Eb, Gb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x18
	{ SBB, -1, Ev, Gv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x19
	{ SBB, -1, Gb, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x1A
	{ SBB, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x1B
	{ SBB, -1, AL_CODE, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0x1C
	{ SBB, -1, rAX, Iz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x1D
	{ PUSH_DS, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                   // 0x1E
	{ POP_DS, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                    // 0x1F
	{ AND, -1, Eb, Gb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x20
	{ AND, -1, Ev, Gv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x21
	{ AND, -1, Gb, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x22
	{ AND, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x23
	{ AND, -1, AL_CODE, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0x24
	{ AND, -1, rAX, Iz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x25
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },    // 0x26; not an opcode: ES segment prefix
	{ DAA, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                       // 0x27
	{ SUB, -1, Eb, Gb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x28
	{ SUB, -1, Ev, Gv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x29
	{ SUB, -1, Gb, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x2A
	{ SUB, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x2B
	{ SUB, -1, AL_CODE, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0x2C
	{ SUB, -1, rAX, Iz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x2D
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },    // 0x2E; not an opcode: CS segment prefix
	{ DAS, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                       // 0x2F
	{ XOR, -1, Eb, Gb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x30
	{ XOR, -1, Ev, Gv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x31
	{ XOR, -1, Gb, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x32
	{ XOR, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x33
	{ XOR, -1, AL_CODE, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0x34
	{ XOR, -1, rAX, Iz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x35
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },    // 0x36; not an opcode: SS segment prefix
	{ AAA, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                       // 0x37
	{ CMP, -1, Eb, Gb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x38
	{ CMP, -1, Ev, Gv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x39
	{ CMP, -1, Gb, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x3A
	{ CMP, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x3B
	{ CMP, -1, AL_CODE, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0x3C
	{ CMP, -1, rAX, Iz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x3D
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },    // 0x3E; not an opcode: DS segment prefix
	{ AAS, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                       // 0x3F
	{ INC, -1, rAX, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x40; not an opcode if in 64-bit mode: REX
	{ INC, -1, rCX, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x41; not an opcode if in 64-bit mode: REX.B
	{ INC, -1, rDX, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x42; not an opcode if in 64-bit mode: REX.X
	{ INC, -1, rBX, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x43; not an opcode if in 64-bit mode: REX.XB
	{ INC, -1, rSP, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x44; not an opcode if in 64-bit mode: REX.R
	{ INC, -1, rBP, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x45; not an opcode if in 64-bit mode: REX.RB
	{ INC, -1, rSI, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x46; not an opcode if in 64-bit mode: REX.RX
	{ INC, -1, rDI, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x47; not an opcode if in 64-bit mode: REX.RXB
	{ DEC, -1, rAX, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x48; not an opcode if in 64-bit mode: REX.W
	{ DEC, -1, rCX, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x49; not an opcode if in 64-bit mode: REX.WB
	{ DEC, -1, rDX, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x4A; not an opcode if in 64-bit mode: REX.WX
	{ DEC, -1, rBX, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x4B; not an opcode if in 64-bit mode: REX.WXB
	{ DEC, -1, rSP, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x4C; not an opcode if in 64-bit mode: REX.WR
	{ DEC, -1, rBP, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x4D; not an opcode if in 64-bit mode: REX.WRB
	{ DEC, -1, rSI, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x4E; not an opcode if in 64-bit mode: REX.WRX
	{ DEC, -1, rDI, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                   // 0x4F; not an opcode if in 64-bit mode: REX.WRXB
	{ PUSH, -1, rAX_r8, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                               // 0x50
	{ PUSH, -1, rCX_r9, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                               // 0x51
	{ PUSH, -1, rDX_r10, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                              // 0x52
	{ PUSH, -1, rBX_r11, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                              // 0x53
	{ PUSH, -1, rSP_r12, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                              // 0x54
	{ PUSH, -1, rBP_r13, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                              // 0x55
	{ PUSH, -1, rSI_r14, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                              // 0x56
	{ PUSH, -1, rDI_r15, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                              // 0x57
	{ POP, -1, rAX_r8, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                                // 0x58
	{ POP, -1, rCX_r9, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                                // 0x59
	{ POP, -1, rDX_r10, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                               // 0x5A
	{ POP, -1, rBX_r11, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                               // 0x5B
	{ POP, -1, rSP_r12, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                               // 0x5C
	{ POP, -1, rBP_r13, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                               // 0x5D
	{ POP, -1, rSI_r14, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                               // 0x5E
	{ POP, -1, rDI_r15, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                               // 0x5F
	{ PUSHAD, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                    // 0x60; PUSHA if operand size is 16-bit
	{ POPAD, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                     // 0x61; POPA if operand size is 16-bit
	{ BOUND, -1, Gv, Ma, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                               // 0x62
	{ ARPL, -1, Ew, Gw, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                                // 0x63; { MOVSXD, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, o64 } if 64-bit mode
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },    // 0x64; not an opcode: FS segment prefix
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },    // 0x65; not an opcode: GS segment prefix
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },    // 0x66; not an opcode: Operand size prefix
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },    // 0x67; not an opcode: Address size prefix
	{ PUSH, -1, Iz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                                   // 0x68
	{ IMUL, -1, Gv, Ev, Iz, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                                  // 0x69
	{ PUSH, -1, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                                   // 0x6A
	{ IMUL, -1, Gv, Ev, Ib, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                                  // 0x6B
	{ INS, -1, Yb, rDX, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x6C; alternate mnemonics: INSB
	{ INS, -1, Yz, rDX, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x6D; alternate mnemonics: INSW, INSD
	{ OUTS, -1, rDX, Xb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                    // 0x6E; alternate mnemonics: OUTSB
	{ OUTS, -1, rDX, Xz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                    // 0x6F; alternate mnemonics: OUTSW, OUTSD
	{ JO_SHORT, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                               // 0x70
	{ JNO_SHORT, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                              // 0x71
	{ JB_SHORT, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                               // 0x72; alternate mnemonics: JNAE, JC
	{ JNB_SHORT, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                              // 0x73; alternate mnemonics: JAE, JNC
	{ JZ_SHORT, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                               // 0x74; alternate mnemonics: JE
	{ JNZ_SHORT, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                              // 0x75; alternate mnemonics: JNE
	{ JBE_SHORT, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                              // 0x76; alternate mnemonics: JNA
	{ JA_SHORT, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                               // 0x77; alternate mnemonics: JNBE
	{ JS_SHORT, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                               // 0x78
	{ JNS_SHORT, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                              // 0x79
	{ JP_SHORT, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                               // 0x7A; alternate mnemonics: JPE
	{ JNP_SHORT, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                              // 0x7B; alternate mnemonics: JPO
	{ JL_SHORT, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                               // 0x7C; alternate mnemonics: JNGE
	{ JGE_SHORT, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                              // 0x7D; alternate mnemonics: JNL
	{ JLE_SHORT, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                              // 0x7E; alternate mnemonics: JNG
	{ JG_SHORT, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                               // 0x7F; alternate mnemonics: JNLE
	{ EXTENDED_OPCODE, 0, Eb, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                           // 0x80; an extended opcode
	{ EXTENDED_OPCODE, 0, Ev, Iz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                           // 0x81; an extended opcode
	{ EXTENDED_OPCODE, 0, Eb, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                      // 0x82; an extended opcode
	{ EXTENDED_OPCODE, 0, Ev, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                           // 0x83; an extended opcode
	{ TEST, -1, Eb, Gb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x84
	{ TEST, -1, Ev, Gv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x85
	{ XCHG, -1, Eb, Gb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x86
	{ XCHG, -1, Ev, Gv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x87
	{ MOV, -1, Eb, Gb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x88
	{ MOV, -1, Ev, Gv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x89
	{ MOV, -1, Gb, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x8A
	{ MOV, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x8B
	{ MOV, -1, Ev, Sw, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x8C
	{ LEA, -1, Gv, M, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                       // 0x8D
	{ MOV, -1, Sw, Ew, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0x8E
	{ EXTENDED_OPCODE, 1, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                         // 0x8F; an extended opcode
	{ NOP, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },            // 0x90; this instruction is technically XCHG rAX, rAX, but it functions as NOP due to not chaning state
	{ XCHG, -1, rCX_r9, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                    // 0x91
	{ XCHG, -1, rDX_r10, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                   // 0x92
	{ XCHG, -1, rBX_r11, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                   // 0x93
	{ XCHG, -1, rSP_r12, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                   // 0x94
	{ XCHG, -1, rBP_r13, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                   // 0x95
	{ XCHG, -1, rSI_r14, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                   // 0x96
	{ XCHG, -1, rDI_r15, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                   // 0x97
	{ CWDE, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },           // 0x98; CBW if operand size is 16-bit, CDQE if operand size is 64-bit
	{ CDQ, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },            // 0x99; CWD if operand size is 16-bit, CQO if operand size is 64-bit
	{ CALL_FAR, -1, Ap, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                               // 0x9A
	{ WAIT, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },           // 0x9B; alternate mnemonics: FWAIT
	{ PUSHF, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                     // 0x9C
	{ POPF, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                      // 0x9D
	{ SAHF, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },           // 0x9E
	{ LAHF, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },           // 0x9F
	{ MOV, -1, AL_CODE, Ob, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xA0
	{ MOV, -1, rAX, Ov, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0xA1
	{ MOV, -1, Ob, AL_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xA2
	{ MOV, -1, Ov, rAX, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0xA3
	{ MOVS, -1, Yb, Xb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0xA4; alternate mnemonics: MOVSB
	{ MOVS, -1, Yv, Xv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0xA5; alternate mnemonics: MOVSW, MOVSD, MOVSQ
	{ CMPS, -1, Xb, Yb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0xA6; alternate mnemonics: CMPSB
	{ CMPS, -1, Xv, Yv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0xA7; alternate mnemonics: CMPSW, CMPSD
	{ TEST, -1, AL_CODE, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                // 0xA8
	{ TEST, -1, rAX, Iz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                    // 0xA9
	{ STOS, -1, Yb, AL_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                // 0xAA; alternate mnemonics: STOSB
	{ STOS, -1, Yv, rAX, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                    // 0xAB; alternate mnemonics: STOSW, STOSD, STOSQ
	{ LODS, -1, AL_CODE, Xb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                // 0xAC; alternate mnemonics: LODSB
	{ LODS, -1, rAX, Xv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                    // 0xAD; alternate mnemonics: LODSW, LODSD, LODSQ
	{ SCAS, -1, AL_CODE, Yb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                // 0xAE; alternate mnemonics: SCASB
	{ SCAS, -1, rAX, Yv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                    // 0xAF; alternate mnemonics: SCASW, SCASD, SCASQ
	{ MOV, -1, AL_R8B, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                  // 0xB0
	{ MOV, -1, CL_R9B, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                  // 0xB1
	{ MOV, -1, DL_R10B, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xB2
	{ MOV, -1, BL_R11B, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xB3
	{ MOV, -1, AH_R12B, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xB4
	{ MOV, -1, CH_R13B, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xB5
	{ MOV, -1, DH_R14B, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xB6
	{ MOV, -1, BH_R15B, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xB7
	{ MOV, -1, rAX_r8, Iv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                  // 0xB8
	{ MOV, -1, rCX_r9, Iv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                  // 0xB9
	{ MOV, -1, rDX_r10, Iv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xBA
	{ MOV, -1, rBX_r11, Iv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xBB
	{ MOV, -1, rSP_r12, Iv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xBC
	{ MOV, -1, rBP_r13, Iv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xBD
	{ MOV, -1, rSI_r14, Iv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xBE
	{ MOV, -1, rDI_r15, Iv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xBF
	{ EXTENDED_OPCODE, 2, Eb, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                           // 0xC0; an extended opcode
	{ EXTENDED_OPCODE, 2, Ev, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                           // 0xC1; an extended opcode
	{ RET_NEAR, -1, Iw, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                               // 0xC2
	{ RET_NEAR, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                  // 0xC3
	{ LES, -1, Gz, Mp, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                                 // 0xC4; not an opcode if in 64-bit mode: VEX prefix starter +2 bytes
	{ LDS, -1, Gz, Mp, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                                 // 0xC5; not an opcode if in 64-bit mode: VEX prefix starter +1 bytes
	{ EXTENDED_OPCODE, 11, Eb, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                          // 0xC6; an extended opcode
	{ EXTENDED_OPCODE, 11, Ev, Iz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                          // 0xC7; an extended opcode
	{ ENTER, -1, Iw, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                    // 0xC8
	{ LEAVE, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                     // 0xC9
	{ RET_FAR, -1, Iw, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                     // 0xCA
	{ RET_FAR, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },        // 0xCB
	{ INT3, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },           // 0xCC
	{ _INT, -1, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                        // 0xCD
	{ INTO, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                      // 0xCE
	{ IRET, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },           // 0xCF; alternate mnemonics: IRETD, IRETQ
	{ EXTENDED_OPCODE, 2, Eb, ONE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                          // 0xD0; an extended opcode
	{ EXTENDED_OPCODE, 2, Ev, ONE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                          // 0xD1; an extended opcode
	{ EXTENDED_OPCODE, 2, Eb, CL_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                      // 0xD2; an extended opcode
	{ EXTENDED_OPCODE, 2, Ev, CL_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                      // 0xD3; an extended opcode
	{ AAM, -1, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                    // 0xD4
	{ AAD, -1, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                    // 0xD5
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },    // 0xD6; not an opcode: reserved
	{ XLAT, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },           // 0xD7; alternate mnemonics: XLATB
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },    // 0xD8; not an opcode: escape to coprocessor instruction set
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },    // 0xD9; not an opcode: escape to coprocessor instruction set
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },    // 0xDA; not an opcode: escape to coprocessor instruction set
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },    // 0xDB; not an opcode: escape to coprocessor instruction set
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },    // 0xDC; not an opcode: escape to coprocessor instruction set
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },    // 0xDD; not an opcode: escape to coprocessor instruction set
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },    // 0xDE; not an opcode: escape to coprocessor instruction set
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },    // 0xDF; not an opcode: escape to coprocessor instruction set
	{ LOOPNZ, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                                 // 0xE0; alternate mnemonics: LOOPNE
	{ LOOPZ, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                                  // 0xE1; alternate mnemonics: LOOPE
	{ LOOP, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                                   // 0xE2
	{ JRCXZ, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                                  // 0xE3
	{ _IN, -1, AL_CODE, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                 // 0xE4
	{ _IN, -1, rAX, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0xE5
	{ _OUT, -1, Ib, AL_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                // 0xE6
	{ _OUT, -1, Ib, rAX, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                    // 0xE7
	{ CALL_NEAR, -1, Jz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                              // 0xE8
	{ JMP_NEAR, -1, Jz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                               // 0xE9
	{ JMP_FAR, -1, Ap, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, i64 },                                // 0xEA
	{ JMP_SHORT, -1, Jb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },                              // 0xEB
	{ _IN, -1, AL_CODE, DX_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                            // 0xEC
	{ _IN, -1, rAX, DX_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                // 0xED
	{ _OUT, -1, DX_CODE, AL_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                           // 0xEE
	{ _OUT, -1, DX_CODE, rAX, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                               // 0xEF
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },    // 0xF0; not an opcode: LOCK prefix
	{ INT1, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },           // 0xF1
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },    // 0xF2; not an opcode: REPNZ prefix
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },    // 0xF3; not an opcode: REPZ prefix
	{ HLT, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },            // 0xF4
	{ CMC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },            // 0xF5
	{ EXTENDED_OPCODE, 3, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },              // 0xF6; an extended opcode
	{ EXTENDED_OPCODE, 3, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },              // 0xF7; an extended opcode
	{ CLC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },            // 0xF8
	{ STC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },            // 0xF9
	{ CLI, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },            // 0xFA
	{ STI, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },            // 0xFB
	{ CLD, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },            // 0xFC
	{ STD, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },            // 0xFD
	{ EXTENDED_OPCODE, 4, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT }, // 0xFE; an extended opcode
	{ EXTENDED_OPCODE, 5, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT }  // 0xFF; an extended opcode
};
const struct Opcode alternateX63 = { MOVSXD, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, o64 };               // 0x63 when 64-bit mode
