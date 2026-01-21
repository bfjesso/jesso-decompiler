#pragma once
#include "prefixes.h"
#include "operands.h"

enum ModRMSelection
{
	GET_MEM_ADDRESS,
	GET_REGISTER,
	GET_SEGMENT,
	GET_MMX_REG,
	GET_CONTROL_REG,
	GET_DEBUG_REG,
	GET_MEM_ADDRESS_MMX,
};

unsigned char handleModRM(unsigned char** bytesPtr, unsigned char* maxBytesAddr, char hasGotModRM, unsigned char* modRMByteRef, enum ModRMSelection selection, unsigned char operandSize, char addressSizeOverride, unsigned char is64bitMode, struct REXPrefix* rexPrefix, struct Operand* result);

static unsigned char handleSIB(unsigned char** bytesPtr, unsigned char mod, unsigned char is64Bit, struct REXPrefix* rexPrefix, unsigned char* gotDisp, struct Operand* result);