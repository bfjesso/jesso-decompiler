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

enum JdcStatus handleModRM(struct DisassemblyParameters* params, enum ModRMSelection selection, uint8_t operandSize, struct Operand* result);

static enum JdcStatus handleSIB(struct DisassemblyParameters* params, bool* gotDisp, struct Operand* result);
