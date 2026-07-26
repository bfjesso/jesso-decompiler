#pragma once
#include "decompilationStructs.h"

#define NUM_OF_RETURNING_INTRINSICS 13
#define NUM_OF_VOID_INTRINSICS 7

struct IntrinsicFunc 
{
	enum Mnemonic opcode;
	unsigned char operandsToDecompile[4]; // as arguemnts
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

unsigned char isOpcodeReturningIntrinsicFunc(enum Mnemonic opcode, struct IntrinsicFunc** intrinsicFuncRef);

unsigned char decompileReturningIntrinsicFunc(struct DecompilationParameters* params, int instructionIndex, struct IntrinsicFunc* intrinsicFunc, unsigned char getAssignment, struct JdcStr* result);

unsigned char checkForVoidIntrinsicFunc(struct DecompilationParameters* params, int instructionIndex, struct IntrinsicFunc** intrinsicFuncRef);

unsigned char decompileVoidIntrinsicFunc(struct DecompilationParameters* params, int instructionIndex, struct IntrinsicFunc* intrinsicFunc, struct JdcStr* result);