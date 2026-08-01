#pragma once
#include "decompilationStructs.h"

#define NUM_OF_RETURNING_INTRINSICS 22
#define NUM_OF_VOID_INTRINSICS 7

enum IntrinsicType
{
	SINGLE_IT, // this is where one opcode corresponds to one intrinsic, so only the opcode needs to be checked
	MMX_IT, // first operand is an MMX reg
	MMX_RM_IT, // MMX_IT, and second operand is a reg or memory address
	MMX_IMM_IT, // MMX_IT, and second operand is an immediate
	SSE_IT, // first operand is an XMM reg
	SSE_RM_IT, // SSE_IT, and second operand is a reg or memory address
	SSE_IMM_IT, // SSE_IT, and second operand is an immediate
	
};

struct Intrinsic
{
	enum Mnemonic opcode;
	enum IntrinsicType type;
	const char* name;
};

#ifdef __cplusplus
extern "C"
{
#endif

	// these modify the first operand
	extern struct Intrinsic returningIntrinsics[];

	// these are not decompiled as asigning to any operand
	extern struct Intrinsic voidIntrinsics[];

#ifdef __cplusplus
}
#endif

static unsigned char checkValidIntrinsicType(struct DisassembledInstruction* instruction, struct Intrinsic* intrinsic);

unsigned char isInstructionReturningIntrinsic(struct DisassembledInstruction* instruction, struct Intrinsic** intrinsicRef);

unsigned char decompileReturningIntrinsic(struct DecompilationParameters* params, int instructionIndex, struct Intrinsic* intrinsic, unsigned char getAssignment, struct JdcStr* result);

unsigned char checkForVoidIntrinsic(struct DecompilationParameters* params, int instructionIndex, struct Intrinsic** intrinsicRef);

unsigned char decompileVoidIntrinsic(struct DecompilationParameters* params, int instructionIndex, struct Intrinsic* intrinsic, struct JdcStr* result);