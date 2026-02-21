#pragma once

enum Segment
{
	CS,
	SS,
	DS,
	ES,
	FS,
	GS,

	NO_SEGMENT
};

enum Register
{
	AL, CL, DL, BL, AH, CH, DH, BH,
	R8B, R9B, R10B, R11B, R12B, R13B, R14B, R15B,
	AX, CX, DX, BX, SP, BP, SI, DI, IP,
	R8W, R9W, R10W, R11W, R12W, R13W, R14W, R15W,
	EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI, EIP,
	R8D, R9D, R10D, R11D, R12D, R13D, R14D, R15D,
	RAX, RCX, RDX, RBX, RSP, RBP, RSI, RDI, RIP,
	R8, R9, R10, R11, R12, R13, R14, R15,
	ST0, ST1, ST2, ST3, ST4, ST5, ST6, ST7,
	MM0, MM1, MM2, MM3, MM4, MM5, MM6, MM7,
	XMM0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6, XMM7, XMM8, XMM9, XMM10, XMM11, XMM12, XMM13, XMM14, XMM15,
	YMM0, YMM1, YMM2, YMM3, YMM4, YMM5, YMM6, YMM7, YMM8, YMM9, YMM10, YMM11, YMM12, YMM13, YMM14, YMM15,
	ZMM0, ZMM1, ZMM2, ZMM3, ZMM4, ZMM5, ZMM6, ZMM7, ZMM8, ZMM9, ZMM10, ZMM11, ZMM12, ZMM13, ZMM14, ZMM15, ZMM16, ZMM17, ZMM18, ZMM19, ZMM20, ZMM21, ZMM22, ZMM23, ZMM24, ZMM25, ZMM26, ZMM27, ZMM28, ZMM29, ZMM30, ZMM31,

	CR0, CR1, CR2, CR3, CR4, CR5, CR6, CR7, CR8, CR9, CR10, CR11, CR12, CR13, CR14, CR15,
	DR0, DR1, DR2, DR3, DR4, DR5, DR6, DR7, DR8, DR9, DR10, DR11, DR12, DR13, DR14, DR15,

	NO_REG
};

#ifdef __cplusplus
extern "C"
{
#endif

	extern const char* segmentStrs[];

	extern const char* registerStrs[];

	extern const int numOfRegisters;

#ifdef __cplusplus
}
#endif

unsigned char compareRegisters(enum Register reg1, enum Register reg2);

unsigned char isRegisterPointer(enum Register reg);

unsigned char getSizeOfRegister(enum Register reg);

enum Register extendRegister(enum Register reg);

enum Register increaseRegisterSize(enum Register reg);