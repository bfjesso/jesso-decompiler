#pragma once
#include "decompilationStructs.h"

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

	// these modify the dst operand
	extern struct IntrinsicFunc returningIntrinsicFuncs[];
	extern const int numOfReturningIntrinsicFuncs;

	// these do not modify any operand
	extern struct IntrinsicFunc voidIntrinsicFuncs[];
	extern const int numOfVoidIntrinsicFuncs;

#ifdef __cplusplus
}
#endif

unsigned char checkForReturningIntrinsicFunc(enum Mnemonic opcode, struct IntrinsicFunc** intrinsicFuncRef);

unsigned char decompileReturningIntrinsicFunc(struct DecompilationParameters* params, struct IntrinsicFunc* intrinsicFunc, unsigned char getAssignment, struct JdcStr* result);

unsigned char checkForVoidIntrinsicFunc(enum Mnemonic opcode, struct IntrinsicFunc** intrinsicFuncRef);

unsigned char decompileVoidIntrinsicFunc(struct DecompilationParameters* params, struct IntrinsicFunc* intrinsicFunc, struct JdcStr* result);