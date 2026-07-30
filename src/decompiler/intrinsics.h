#pragma once
#include "decompilationStructs.h"

#define NUM_OF_RETURNING_INTRINSICS 21
#define NUM_OF_VOID_INTRINSICS 7

enum IntrinsicFunctionType 
{
	SINGLE_IFT, // this is where one opcode corresponds to one intrinsic, so only the opcode needs to be checked
	MMX_IFT, // first operand is an MMX reg
	MMX_RM_IFT, // MMX_IFT, and second operand is a reg or memory address
	MMX_IMM_IFT, // MMX_IFT, and second operand is an immediate
	SSE_IFT, // first operand is an XMM reg
	SSE_RM_IFT, // SSE_IFT, and second operand is a reg or memory address
	SSE_IMM_IFT, // SSE_IFT, and second operand is an immediate
	
};

struct IntrinsicFunc 
{
	enum Mnemonic opcode;
	enum IntrinsicFuncType type;
	const char* name;
};

#ifdef __cplusplus
extern "C"
{
#endif

	// these modify the first operand
	extern struct IntrinsicFunc returningIntrinsicFuncs[]; 

	// these are not decompiled as asigning to any operand
	extern struct IntrinsicFunc voidIntrinsicFuncs[]; 

#ifdef __cplusplus
}
#endif

static unsigned char checkValidIntrinsicFunctionType(struct DisassembledInstruction* instruction, struct IntrinsicFunc* intrinsicFunc);

unsigned char isInstructionReturningIntrinsicFunc(struct DisassembledInstruction* instruction, struct IntrinsicFunc** intrinsicFuncRef);

unsigned char decompileReturningIntrinsicFunc(struct DecompilationParameters* params, int instructionIndex, struct IntrinsicFunc* intrinsicFunc, unsigned char getAssignment, struct JdcStr* result);

unsigned char checkForVoidIntrinsicFunc(struct DecompilationParameters* params, int instructionIndex, struct IntrinsicFunc** intrinsicFuncRef);

unsigned char decompileVoidIntrinsicFunc(struct DecompilationParameters* params, int instructionIndex, struct IntrinsicFunc* intrinsicFunc, struct JdcStr* result);