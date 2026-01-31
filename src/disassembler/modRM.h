#pragma once
#include "disassemblyStructs.h"

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

unsigned char handleModRM(struct DisassemblyParameters* params, enum ModRMSelection selection, unsigned char operandSize, struct Operand* result);

static unsigned char handleSIB(struct DisassemblyParameters* params, unsigned char* gotDisp, struct Operand* result);
