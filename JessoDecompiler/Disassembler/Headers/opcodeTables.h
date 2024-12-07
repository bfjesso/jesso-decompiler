#pragma once

// The information in this file comes from the
// Intel 64 and IA-32 architectures software developer's manual

// Appendix A: A.2
// "Operands are identified by a two-character code of the form Zz. The first character, an uppercase letter, specifies
//	the addressing method; the second character, a lowercase letter, specifies the type of operand."
enum Operand
{
	INVALID,
	NONE,
	Eb,
	Gb,
	Ev,
	Gv,
	AL,
	Ib,
	rAX,
	Iz,
	eAX,
	eCX,
	eDX,
	eBX,
	eSP,
	eBP,
	eSI,
	eDI,
	rAX_r8,
	rCX_r9,
	rDX_r10,
	rBX_r11,
	rSP_r12,
	rBP_r13,
	rSI_r14,
	rDI_r15,
	Ma,
	Ew,
	Gw,
	Yb,
	DX,
	Yz,
	Xb,
	Xz
};

struct Opcode
{
	const char* mnemonic;
	enum Operand operand1;
	enum Operand operand2;
	enum Operand operand3;
};

// Appendix A: A.3 table A-2
const struct Opcode oneByteMap[0xFE] =
{
	{ "ADD", Eb, Gb, NONE },						// 0x00
	{ "ADD", Ev, Gv, NONE },						// 0x01
	{ "ADD", Gb, Eb, NONE },						// 0x02
	{ "ADD", Gv, Ev, NONE },						// 0x03
	{ "ADD", AL, Ib, NONE },						// 0x04
	{ "ADD", rAX, Iz, NONE },						// 0x05
	{ "PUSH ES", NONE, NONE, NONE },				// 0x06
	{ "POP ES", NONE, NONE, NONE },					// 0x07
	{ "OR", Eb, Gb, NONE },							// 0x08
	{ "OR", Ev, Gv, NONE },							// 0x09
	{ "OR", Gb, Eb, NONE },							// 0x0A
	{ "OR", Gv, Ev, NONE },							// 0x0B
	{ "OR", AL, Ib, NONE },							// 0x0C
	{ "OR", rAX, Iz, NONE },						// 0x0D
	{ "PUSH CS", NONE, NONE, NONE },				// 0x0E
	{ "", INVALID, INVALID, INVALID },				// 0x0F; not an opcode: 2-byte escape
	{ "ADC", Eb, Gb, NONE },						// 0x10
	{ "ADC", Ev, Gv, NONE },						// 0x11
	{ "ADC", Gb, Eb, NONE },						// 0x12
	{ "ADC", Gv, Ev, NONE },						// 0x13
	{ "ADC", AL, Ib, NONE },						// 0x14
	{ "ADC", rAX, Iz, NONE },						// 0x15
	{ "PUSH SS", NONE, NONE, NONE },				// 0x16
	{ "POP SS", NONE, NONE, NONE },					// 0x17
	{ "SBB", Eb, Gb, NONE },						// 0x18
	{ "SBB", Ev, Gv, NONE },						// 0x19
	{ "SBB", Gb, Eb, NONE },						// 0x1A
	{ "SBB", Gv, Ev, NONE },						// 0x1B
	{ "SBB", AL, Ib, NONE },						// 0x1C
	{ "SBB", rAX, Iz, NONE },						// 0x1D
	{ "PUSH DS", NONE, NONE, NONE },				// 0x1E
	{ "POP DS", NONE, NONE, NONE },					// 0x1F
	{ "AND", Eb, Gb, NONE },						// 0x20
	{ "AND", Ev, Gv, NONE },						// 0x21
	{ "AND", Gb, Eb, NONE },						// 0x22
	{ "AND", Gv, Ev, NONE },						// 0x23
	{ "AND", AL, Ib, NONE },						// 0x24
	{ "AND", rAX, Iz, NONE },						// 0x25
	{ "", INVALID, INVALID, INVALID },				// 0x26; not an opcode: ES segment prefix
	{ "DAA", NONE, NONE, NONE },					// 0x27
	{ "SUB", Eb, Gb, NONE },						// 0x28
	{ "SUB", Ev, Gv, NONE },						// 0x29
	{ "SUB", Gb, Eb, NONE },						// 0x2A
	{ "SUB", Gv, Ev, NONE },						// 0x2B
	{ "SUB", AL, Ib, NONE },						// 0x2C
	{ "SUB", rAX, Iz, NONE },						// 0x2D
	{ "", INVALID, INVALID, INVALID },				// 0x2E; not an opcode: CS segment prefix
	{ "DAS", NONE, NONE, NONE },					// 0x2F
	{ "XOR", Eb, Gb, NONE },						// 0x30
	{ "XOR", Ev, Gv, NONE },						// 0x31
	{ "XOR", Gb, Eb, NONE },						// 0x32
	{ "XOR", Gv, Ev, NONE },						// 0x33
	{ "XOR", AL, Ib, NONE },						// 0x34
	{ "XOR", rAX, Iz, NONE },						// 0x35
	{ "", INVALID, INVALID, INVALID },				// 0x36; not an opcode: SS segment prefix
	{ "AAA", NONE, NONE, NONE },					// 0x37
	{ "CMP", Eb, Gb, NONE },						// 0x38
	{ "CMP", Ev, Gv, NONE },						// 0x39
	{ "CMP", Gb, Eb, NONE },						// 0x3A
	{ "CMP", Gv, Ev, NONE },						// 0x3B
	{ "CMP", AL, Ib, NONE },						// 0x3C
	{ "CMP", rAX, Iz, NONE },						// 0x3D
	{ "", INVALID, INVALID, INVALID },				// 0x3E; not an opcode: DS segment prefix
	{ "AAS", NONE, NONE, NONE },					// 0x3F
	{ "INC", eAX, NONE, NONE },						// 0x40; not an opcode if in 64-bit mode: REX
	{ "INC", eCX, NONE, NONE },						// 0x41; not an opcode if in 64-bit mode: REX.B
	{ "INC", eDX, NONE, NONE },						// 0x42; not an opcode if in 64-bit mode: REX.X
	{ "INC", eBX, NONE, NONE },						// 0x43; not an opcode if in 64-bit mode: REX.XB
	{ "INC", eSP, NONE, NONE },						// 0x44; not an opcode if in 64-bit mode: REX.R
	{ "INC", eBP, NONE, NONE },						// 0x45; not an opcode if in 64-bit mode: REX.RB
	{ "INC", eSI, NONE, NONE },						// 0x46; not an opcode if in 64-bit mode: REX.RX
	{ "INC", eDI, NONE, NONE },						// 0x47; not an opcode if in 64-bit mode: REX.RXB
	{ "DEC", eAX, NONE, NONE },						// 0x48; not an opcode if in 64-bit mode: REX.W
	{ "DEC", eCX, NONE, NONE },						// 0x49; not an opcode if in 64-bit mode: REX.WB
	{ "DEC", eDX, NONE, NONE },						// 0x4A; not an opcode if in 64-bit mode: REX.WX
	{ "DEC", eBX, NONE, NONE },						// 0x4B; not an opcode if in 64-bit mode: REX.WXB
	{ "DEC", eSP, NONE, NONE },						// 0x4C; not an opcode if in 64-bit mode: REX.WR
	{ "DEC", eBP, NONE, NONE },						// 0x4D; not an opcode if in 64-bit mode: REX.WRB
	{ "DEC", eSI, NONE, NONE },						// 0x4E; not an opcode if in 64-bit mode: REX.WRX
	{ "DEC", eDI, NONE, NONE },						// 0x4F; not an opcode if in 64-bit mode: REX.WRXB
	{ "PUSH", rAX_r8, NONE, NONE },					// 0x50
	{ "PUSH", rCX_r9, NONE, NONE },					// 0x51
	{ "PUSH", rDX_r10, NONE, NONE },				// 0x52
	{ "PUSH", rBX_r11, NONE, NONE },				// 0x53
	{ "PUSH", rSP_r12, NONE, NONE },				// 0x54
	{ "PUSH", rBP_r13, NONE, NONE },				// 0x55
	{ "PUSH", rSI_r14, NONE, NONE },				// 0x56
	{ "PUSH", rDI_r15, NONE, NONE },				// 0x57
	{ "POP", rAX_r8, NONE, NONE },					// 0x58
	{ "POP", rCX_r9, NONE, NONE },					// 0x59
	{ "POP", rDX_r10, NONE, NONE },					// 0x5A
	{ "POP", rBX_r11, NONE, NONE },					// 0x5B
	{ "POP", rSP_r12, NONE, NONE },					// 0x5C
	{ "POP", rBP_r13, NONE, NONE },					// 0x5D
	{ "POP", rSI_r14, NONE, NONE },					// 0x5E
	{ "POP", rDI_r15, NONE, NONE },					// 0x5F
	{ "PUSHAD", NONE, NONE, NONE },					// 0x60; PUSHA if operand size is 16-bit						!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	{ "POPAD", NONE, NONE, NONE },					// 0x61; POPA if operand size is 16-bit							!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	{ "BOUND", Gv, Ma, NONE },						// 0x62
	{ "ARPL", Ew, Gw, NONE },						// 0x63; { "MOVSXD", Gv, Ev, NONE } if 64-bit mode				!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	{ "", INVALID, INVALID, INVALID },				// 0x64; not an opcode: FS segment prefix
	{ "", INVALID, INVALID, INVALID },				// 0x65; not an opcode: GS segment prefix
	{ "", INVALID, INVALID, INVALID },				// 0x66; not an opcode: Operand size prefix
	{ "", INVALID, INVALID, INVALID },				// 0x67; not an opcode: Address size prefix
	{ "PUSH", Iz, NONE, NONE },						// 0x68
	{ "IMUL", Gv, Ev, Iz },							// 0x69
	{ "PUSH", Ib, NONE, NONE },						// 0x6A
	{ "IMUL", Gv, Ev, Ib },							// 0x6B
	{ "INS/INSB", Yb, DX, NONE },					// 0x6C; figure this out										!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	{ "INS/INSW/INSD", Yz, DX, NONE },				// 0x6D; figure this out										!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	{ "OUTS/OUTSB", DX, Xb, NONE },					// 0x6E; figure this out										!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	{ "OUTS/OUTSW/OUTSD", DX, Xz, NONE },			// 0x6F; figure this out										!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

};