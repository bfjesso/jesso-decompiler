#pragma once
#include "opcodes.h" 

// Intel 64 and IA-32 Architectures Software Developer’s Manuals:
//	intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html

// Appendix A: A.3 table A-3
const struct Opcode twoByteOpcodeMap[0x100][4] = // [byte][prefix] 0 - no prefix; 1 - 0x66; 2 - 0xF3; 3 - 0xF2  (not all opcodes have different definitions for each prefix or any of them)
{
	{ EXTENDED_OPCODE, 6, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },  // 0x00
	{ EXTENDED_OPCODE, 7, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },  // 0x01
	{ LAR, -1, Gv, Ew, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                       // 0x02
	{ LSL, -1, Gv, Ew, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                       // 0x03
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },     // 0x04; not defined
	{ SYSCALL, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, o64 },                    // 0x05
	{ CLTS, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },            // 0x06
	{ SYSRET, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, o64 },                     // 0x07
	{ INVD, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },            // 0x08
	{ WBINVD, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },          // 0x09
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },     // 0x0A; not defined
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },     // 0x0B; 2-byte illegal opcodes UD2
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },     // 0x0C; not defined
	{ PREFETCHW, -1, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                    // 0x0D
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },     // 0x0E; not defined
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },     // 0x0F; not defined
	{                                                                                                            // 0x10
		{ VMOVUPS, -1, Vps, Wps, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VMOVUPD, -1, Vpd, Wpd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VMOVSS, -1, Vx, Hx, Wss, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VMOVSD, -1, Vx, Hx, Wsd, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x11
		{ VMOVUPS, -1, Wps, Vps, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VMOVUPD, -1, Wpd, Vpd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VMOVSS, -1, Wss, Hx, Vss, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VMOVSD, -1, Wsd, Hx, Vsd, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x12
		{ VMOVLPS, -1, Vq, Hq, Mq, NO_OPERAND_CODE, NO_SUPERSCRIPT }, // vmovhlps if ModRM.mod = 0b11
		{ VMOVLPD, -1, Vq, Hq, Mq, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VMOVSLDUP, -1, Vx, Wx, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VMOVDDUP, -1, Vx, Wx, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x13
		{ VMOVLPS, -1, Mq, Vq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VMOVLPD, -1, Mq, Vq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x14
		{ VUNPCKLPS, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VUNPCKLPD, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x15
		{ VUNPCKHPS, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VUNPCKHPD, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x16
		{ VMOVHPS, -1, Vdq, Hq, Mq, NO_OPERAND_CODE, NO_SUPERSCRIPT }, // vmovlhps if ModRM.mod = 0b11
		{ VMOVHPD, -1, Vdq, Hq, Mq, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VMOVSHDUP, -1, Vx, Wx, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x17
		{ VMOVHPS, -1, Mq, Vq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VMOVHPD, -1, Mq, Vq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{ EXTENDED_OPCODE, 16, Mb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },              // 0x18; prefetch
	{ NOP, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },             // 0x19; reserved nop
	{                                                                                                            // 0x1A
		{ BNDLDX, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ BNDMOV, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ BNDCL, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ BNDCU, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x1B
		{ BNDSTX, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ BNDMOV, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ BNDMK, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ BNDCN, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{ NOP, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },             // 0x1C; reserved nop
	{ NOP, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },             // 0x1D; reserved nop
	{ ENDBR, -1, A_BYTE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                    // 0x1E; reserved nop
	{ NOP, -1, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                          // 0x1F
	{ MOV, -1, Rd, Cd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                       // 0x20
	{ MOV, -1, Rd, Dd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                       // 0x21
	{ MOV, -1, Cd, Rd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                       // 0x22
	{ MOV, -1, Dd, Rd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                       // 0x23
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },     // 0x24; not defined
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },     // 0x25; not defined
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },     // 0x26; not defined
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },     // 0x27; not defined
	{                                                                                                            // 0x28
		{ VMOVAPS, -1, Vps, Wps, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VMOVAPD, -1, Vpd, Wpd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x29
		{ VMOVAPS, -1, Wps, Vps, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VMOVAPD, -1, Wpd, Vpd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x2A
		{ CVTPI2PS, -1, Vps, Qpi, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ CVTPI2PD, -1, Vpd, Qpi, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VCVTSI2SS, -1, Vss, Hss, Ey, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VCVTSI2SD, -1, Vsd, Hsd, Ey, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x2B
		{ VMOVNTPS, -1, Mps, Vps, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VMOVNTPD, -1, Mpd, Vpd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x2C
		{ CVTTPS2PI, -1, Ppi, Wps, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ CVTTPD2PI, -1, Ppi, Wpd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VCVTTSS2SI, -1, Gy, Wss, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VCVTTSD2SI, -1, Gy, Wsd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x2D
		{ CVTPS2PI, -1, Ppi, Wps, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ CVTPD2PI, -1, Qpi, Wpd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VCVTSS2SI, -1, Gy, Wss, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VCVTSD2SI, -1, Gy, Wsd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x2E
		{ VUCOMISS, -1, Vss, Wss, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VUCOMISD, -1, Vsd, Wsd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x2F
		{ VCOMISS, -1, Vss, Wss, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VCOMISD, -1, Vsd, Wsd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{ WRMSR, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },           // 0x30
	{ RDTSC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },           // 0x31
	{ RDMSR, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },           // 0x32
	{ RDPMC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },           // 0x33
	{ SYSENTER, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },        // 0x34
	{ SYSEXIT, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },         // 0x35
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },     // 0x36; not defined
	{ GETSEC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },          // 0x37
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },     // 0x38; 3-byte escape
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },     // 0x39; not defined
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },     // 0x3A; 3-byte escape
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },     // 0x3B; not defined
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },     // 0x3C; not defined
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },     // 0x3D; not defined
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },     // 0x3E; not defined
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },     // 0x3F; not defined
	{ CMOVO, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x40
	{ CMOVNO, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                    // 0x41
	{ CMOVB, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x42
	{ CMOVNB, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                    // 0x43
	{ CMOVZ, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x44
	{ CMOVNZ, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                    // 0x45
	{ CMOVBE, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                    // 0x46
	{ CMOVA, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x47
	{ CMOVS, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x48
	{ CMOVNS, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                    // 0x49
	{ CMOVP, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x4A
	{ CMOVNP, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                    // 0x4B
	{ CMOVL, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x4C
	{ CMOVGE, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                    // 0x4D
	{ CMOVLE, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                    // 0x4E
	{ CMOVG, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0x4F
	{                                                                                                            // 0x50
		{ VMOVMSKPS, -1, Gy, Ups, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT},
		{ VMOVMSKPD, -1, Gy, Upd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x51
		{ VSQRTPS, -1, Vps, Wps, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT},
		{ VSQRTPD, -1, Vpd, Wpd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VSQRTSS, -1, Vss, Hss, Wss, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VSQRTSD, -1, Vsd, Hsd, Wsd, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x52
		{ VRSQRTPS, -1, Vps, Wps, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT},
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VRSQRTSS, -1, Vss, Hss, Wss, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x53
		{ VRCPPS, -1, Vps, Wps, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT},
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VRCPSS, -1, Vss, Hss, Wss, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x54
		{ VANDPS, -1, Vps, Hps, Wps, NO_OPERAND_CODE, NO_SUPERSCRIPT},
		{ VANDPD, -1, Vpd, Hpd, Wpd, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x55
		{ VANDNPS, -1, Vps, Hps, Wps, NO_OPERAND_CODE, NO_SUPERSCRIPT},
		{ VANDNPD, -1, Vpd, Hpd, Wpd, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x56
		{ VORPS, -1, Vps, Hps, Wps, NO_OPERAND_CODE, NO_SUPERSCRIPT},
		{ VORPD, -1, Vpd, Hpd, Wpd, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x57
		{ VXORPS, -1, Vps, Hps, Wps, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VXORPD, -1, Vpd, Hpd, Wpd, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x58
		{ VADDPS, -1, Vps, Hps, Wps, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VADDPD, -1, Vpd, Hpd, Wpd, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VADDSS, -1, Vss, Hss, Wss, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VADDSD, -1, Vsd, Hsd, Wsd, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x59
		{ VMULPS, -1, Vps, Hps, Wps, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VMULPD, -1, Vpd, Hpd, Wpd, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VMULSS, -1, Vss, Hss, Wss, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VMULSD, -1, Vsd, Hsd, Wsd, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x5A
		{ VCVTPS2PD, -1, Vpd, Wps, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VCVTPD2PS, -1, Vps, Wpd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VCVTSS2SD, -1, Vsd, Hx, Wss, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VCVTSD2SS, -1, Vss, Hx, Wsd, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x5B
		{ VCVTDQ2PS, -1, Vps, Wdq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VCVTPS2DQ, -1, Vdq, Wps, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VCVTTPS2DQ, -1, Vdq, Wps, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x5C
		{ VSUBPS, -1, Vps, Hps, Wps, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VSUBPD, -1, Vpd, Hpd, Wpd, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VSUBSS, -1, Vss, Hss, Wss, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VSUBSD, -1, Vsd, Hsd, Wsd, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x5D
		{ VMINPS, -1, Vps, Hps, Wps, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VMINPD, -1, Vpd, Hpd, Wpd, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VMINSS, -1, Vss, Hss, Wss, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VMINSD, -1, Vsd, Hsd, Wsd, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x5E
		{ VDIVPS, -1, Vps, Hps, Wps, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VDIVPD, -1, Vpd, Hpd, Wpd, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VDIVSS, -1, Vss, Hss, Wss, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VDIVSD, -1, Vsd, Hsd, Wsd, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x5F
		{ VMAXPS, -1, Vps, Hps, Wps, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VMAXPD, -1, Vpd, Hpd, Wpd, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VMAXSS, -1, Vss, Hss, Wss, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VMAXSD, -1, Vsd, Hsd, Wsd, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x60
		{ PUNPCKLBW, -1, Pq, Qd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT},
		{ VPUNPCKLBW, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x61
		{ PUNPCKLWD, -1, Pq, Qd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT},
		{ VPUNPCKLWD, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x62
		{ PUNPCKLDQ, -1, Pq, Qd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT},
		{ VPUNPCKLDQ, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x63
		{ PACKSSWB, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT},
		{ VPACKSSWB, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x64
		{ PCMPGTB, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT},
		{ VPCMPGTB, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x65
		{ PCMPGTW, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT},
		{ VPCMPGTW, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x66
		{ PCMPGTD, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT},
		{ VPCMPGTD, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x67
		{ PACKUSWB, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT},
		{ VPACKUSWB, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x68
		{ PUNPCKHBW, -1, Pq, Qd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT},
		{ VPUNPCKHBW, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x69
		{ PUNPCKHWD, -1, Pq, Qd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT},
		{ VPUNPCKHWD, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x6A
		{ PUNPCKHDQ, -1, Pq, Qd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT},
		{ VPUNPCKHDQ, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x6B
		{ PACKSSDW, -1, Pq, Qd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT},
		{ VPACKSSDW, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x6C
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VPUNPCKLQDQ, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x6D
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VPUNPCKHQDQ, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x6E
		{ MOVD, -1, Pd, Ey, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VMOVD, -1, Vy, Ey, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x6F
		{ MOVQ, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VMOVDQA, -1, Vx, Wx, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VMOVDQU, -1, Vx, Wx, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x70
		{ PSHUFW, -1, Pq, Qq, Ib, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VPSHUFD, -1, Vx, Wx, Ib, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VPSHUFHW, -1, Vx, Wx, Ib, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ VPSHUFLW, -1, Vx, Wx, Ib, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{ EXTENDED_OPCODE, 12, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT }, // 0x71
	{ EXTENDED_OPCODE, 13, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT }, // 0x72
	{ EXTENDED_OPCODE, 14, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT }, // 0x73
	{                                                                                                            // 0x74
		{ PCMPEQB, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT},
		{ VPCMPEQB, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x75
		{ PCMPEQW, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT},
		{ VPCMPEQW, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x76
		{ PCMPEQD, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT},
		{ VPCMPEQD, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{ EMMS, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },            // 0x77; vzeroupper, vzeroall ?
	{ VMREAD, -1, Ey, Gy, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                    // 0x78
	{ VMWRITE, -1, Gy, Ey, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                   // 0x79
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },     // 0x7A; not defined
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },     // 0x7B; not defined
	{                                                                                                            // 0x7C
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ HADDPD, -1, Vpd, Hpd, Wpd, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ HADDPS, -1, Vps, Hps, Wps, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x7D
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ HSUBPD, -1, Vpd, Hpd, Wpd, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ HSUBPS, -1, Vps, Hps, Wps, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x7E
		{ MOVD, -1, Ey, Pd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ MOVD, -1, Ey, Vy, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ MOVQ, -1, Vq, Wq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0x7F
		{ MOVQ, -1, Qq, Pq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ MOVDQA, -1, Wx, Vx, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ MOVDQU, -1, Wx, Vx, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{ JO_SHORT, -1, Jz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                     // 0x80
	{ JNO_SHORT, -1, Jz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                    // 0x81
	{ JB_SHORT, -1, Jz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                     // 0x82
	{ JNB_SHORT, -1, Jz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                    // 0x83
	{ JZ_SHORT, -1, Jz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                     // 0x84
	{ JNZ_SHORT, -1, Jz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                    // 0x85
	{ JBE_SHORT, -1, Jz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                    // 0x86
	{ JA_SHORT, -1, Jz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                     // 0x87
	{ JS_SHORT, -1, Jz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                     // 0x88
	{ JNS_SHORT, -1, Jz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                    // 0x89
	{ JP_SHORT, -1, Jz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                     // 0x8A
	{ JNP_SHORT, -1, Jz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                    // 0x8B
	{ JL_SHORT, -1, Jz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                     // 0x8C
	{ JGE_SHORT, -1, Jz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                    // 0x8D
	{ JLE_SHORT, -1, Jz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                    // 0x8E
	{ JG_SHORT, -1, Jz, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                     // 0x8F
	{ SETO, -1, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                         // 0x90
	{ SETNO, -1, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                        // 0x91
	{ SETB, -1, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                         // 0x92
	{ SETNB, -1, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                        // 0x93
	{ SETZ, -1, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                         // 0x94
	{ SETNZ, -1, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                        // 0x95
	{ SETBE, -1, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                        // 0x96
	{ SETA, -1, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                         // 0x97
	{ SETS, -1, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                         // 0x98
	{ SETNS, -1, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                        // 0x99
	{ SETP, -1, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                         // 0x9A
	{ SETNP, -1, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                        // 0x9B
	{ SETL, -1, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                         // 0x9C
	{ SETNL, -1, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                        // 0x9D
	{ SETLE, -1, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                        // 0x9E
	{ SETG, -1, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                         // 0x9F
	{ PUSH_FS, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                    // 0xA0
	{ POP_FS, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                     // 0xA1
	{ CPUID, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },           // 0xA2
	{ BT, -1, Ev, Gv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                        // 0xA3
	{ SHLD, -1, Ev, Gv, Ib, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                                   // 0xA4
	{ SHLD, -1, Ev, Gv, CL_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                              // 0xA5
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },     // 0xA6; not defined
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },     // 0xA7; not defined
	{ PUSH_GS, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                    // 0xA8
	{ POP_GS, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },                     // 0xA9
	{ RSM, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },             // 0xAA
	{ BTS, -1, Ev, Gv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                       // 0xAB
	{ SHRD, -1, Ev, Gv, Ib, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                                   // 0xAC
	{ SHRD, -1, Ev, Gv, CL_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                              // 0xAD
	{ EXTENDED_OPCODE, 15, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT }, // 0xAE
	{ IMUL, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0xAF
	{ CMPXCHG, -1, Eb, Gb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                   // 0xB0
	{ CMPXCHG, -1, Ev, Gv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                   // 0xB1
	{ LSS, -1, Gv, Mp, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                       // 0xB2
	{ BTR, -1, Ev, Gv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                       // 0xB3
	{ LFS, -1, Gv, Mp, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                       // 0xB4
	{ LGS, -1, Gv, Mp, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                       // 0xB5
	{ MOVZX, -1, Gv, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0xB6
	{ MOVZX, -1, Gv, Ew, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0xB7
	{                                                                                                            // 0xB8
		{ JMPE, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ POPCNT, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{ EXTENDED_OPCODE, 10, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT }, // 0xB9
	{ EXTENDED_OPCODE, 8, Ev, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                            // 0xBA
	{ BTC, -1, Ev, Gv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                       // 0xBB
	{                                                                                                            // 0xBC
		{ BSF, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ TZCNT, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xBD
		{ BSR, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ LZCNT, -1, Gv, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{ MOVSX, -1, Gv, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0xBE
	{ MOVSX, -1, Gv, Ew, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                     // 0xBF
	{ XADD, -1, Eb, Gb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0xC0
	{ XADD, -1, Ev, Gv, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                      // 0xC1
	{                                                                                                            // 0xC2
		{ CMPPS, -1, Vps, Hps, Wps, Ib, NO_SUPERSCRIPT },
		{ CMPPD, -1, Vpd, Hpd, Wpd, Ib, NO_SUPERSCRIPT },
		{ CMPSS, -1, Vss, Hss, Wss, Ib, NO_SUPERSCRIPT },
		{ CMPSD, -1, Vsd, Hsd, Wsd, Ib, NO_SUPERSCRIPT },
	},
	{ MOVNTI, -1, My, Gy, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                                    // 0xC3
	{                                                                                                            // 0xC4
		{ PINSRW, -1, Pq, Ey, Ib, NO_OPERAND_CODE, NO_SUPERSCRIPT }, // second operand is Ry/Mw, leaving Ey
		{ PINSRW, -1, Vdq, Hdq, Ey, Ib, NO_SUPERSCRIPT }, // third operand is Ry/Mw, leaving Ey
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xC5
		{ PEXTRW, -1, Gd, Nq, Ib, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PEXTRW, -1, Gd, Udq, Ib, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xC6
		{ SHUFPS, -1, Vps, Hps, Wps, Ib, NO_SUPERSCRIPT },
		{ SHUFPD, -1, Vpd, Hpd, Wpd, Ib, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{ EXTENDED_OPCODE, 9, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },  // 0xC7
	{ BSWAP, -1, rAX_r8, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                    // 0xC8
	{ BSWAP, -1, rCX_r9, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                    // 0xC9
	{ BSWAP, -1, rDX_r10, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                   // 0xCA
	{ BSWAP, -1, rBX_r11, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                   // 0xCB
	{ BSWAP, -1, rSP_r12, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                   // 0xCC
	{ BSWAP, -1, rBP_r13, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                   // 0xCD
	{ BSWAP, -1, rSI_r14, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                   // 0xCE
	{ BSWAP, -1, rDI_r15, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },                   // 0xCF
	{                                                                                                            // 0xD0
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ ADDSUBPD, -1, Vpd, Hpd, Wpd, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ ADDSUBPS, -1, Vps, Hps, Wps, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xD1
		{ PSRLW, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PSRLW, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xD2
		{ PSRLD, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PSRLD, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xD3
		{ PSRLQ, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PSRLQ, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xD4
		{ PADDQ, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PADDQ, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xD5
		{ PMULLW, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PMULLW, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xD6
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ MOVQ, -1, Wq, Vq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ MOVQ2DQ, -1, Vdq, Nq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ MOVDQ2Q, -1, Pq, Uq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xD7
		{ PMOVMSKB, -1, Gd, Nq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PMOVMSKB, -1, Gd, Ux, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xD8
		{ PSUBUSB, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PSUBUSB, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xD9
		{ PSUBUSW, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PSUBUSW, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xDA
		{ PMINUB, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PMINUB, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xDB
		{ PAND, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PAND, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xDC
		{ PADDUSB, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PADDUSB, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xDD
		{ PADDUSW, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PADDUSW, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xDE
		{ PMAXUB, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PMAXUB, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xDF
		{ PANDN, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PANDN, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xE0
		{ PAVGB, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PAVGB, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xE1
		{ PSRAW, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PSRAW, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xE2
		{ PSRAD, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PSRAD, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xE3
		{ PAVGW, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PAVGW, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xE4
		{ PMULHUW, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PMULHUW, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xE5
		{ PMULHW, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PMULHW, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xE6
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ CVTTPD2DQ, -1, Vx, Wpd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ CVTDQ2PD, -1, Vx, Wpd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ CVTPD2DQ, -1, Vx, Wpd, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xE7
		{ MOVNTQ, -1, Mq, Pq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ MOVNTDQ, -1, Mx, Vx, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xE8
		{ PSUBSB, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PSUBSB, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xE9
		{ PSUBSW, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PSUBSW, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xEA
		{ PMINSW, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PMINSW, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xEB
		{ POR, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ POR, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xEC
		{ PADDSB, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT},
		{ PADDSB, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xED
		{ PADDSW, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT},
		{ PADDSW, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xEE
		{ PMAXSW, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT},
		{ PMAXSW, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xEF
		{ PXOR, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT},
		{ PXOR, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xF0
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ LDDQU, -1, Vx, Mx, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xF1
		{ PSLLW, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PSLLW, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xF2
		{ PSLLD, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PSLLD, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xF3
		{ PSLLQ, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PSLLQ, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xF4
		{ PMULUDQ, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PMULUDQ, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xF5
		{ PMADDWD, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PMADDWD, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xF6
		{ PSADBW, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PSADBW, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xF7
		{ MASKMOVQ, -1, Pq, Nq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ MASKMOVDQU, -1, Vdq, Udq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xF8
		{ PSUBB, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PSUBB, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xF9
		{ PSUBW, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PSUBW, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xFA
		{ PSUBD, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PSUBD, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xFB
		{ PSUBQ, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PSUBQ, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xFC
		{ PADDB, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PADDB, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xFD
		{ PADDW, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PADDW, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{                                                                                                            // 0xFE
		{ PADDD, -1, Pq, Qq, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ PADDD, -1, Vx, Hx, Wx, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
	},
	{ NO_MNEMONIC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT }      // 0xFF; UD0
};
const struct Opcode alternateX12 = { VMOVHLPS, -1, Vq, Hq, Uq, NO_OPERAND_CODE, NO_SUPERSCRIPT };                // no prefix 0x12 when ModRM.mod == 0b11
const struct Opcode alternateX16 = { VMOVLHPS, -1, Vdq, Hq, Uq, NO_OPERAND_CODE, NO_SUPERSCRIPT };               // no prefix 0x16 when ModRM.mod == 0b11