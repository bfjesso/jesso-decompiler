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
	EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI, EIP,
	RAX, RCX, RDX, RBX, RSP, RBP, RSI, RDI, RIP,
	R8, R9, R10, R11, R12, R13, R14, R15,
	ST0, ST1, ST2, ST3, ST4, ST5, ST6, ST7,

	NO_REG
};

extern const char* segmentStrs[];

extern const char* registerStrs[];

unsigned char compareRegisters(enum Register reg1, enum Register reg2);