#pragma once
#include "disassemblyStructs.h"

enum ModRMSelection
{
	REG_MODRM,
	REG_SEG_MODRM,
	REG_MMX_MODRM,
	REG_CONTROL_MODRM,
	REG_DEBUG_MODRM,

	RM_MODRM,
	RM_MMX_MODRM,

	M_MODRM,
};

enum JdcStatus getModRM(struct DisassemblyParameters* params);

enum JdcStatus handleModRM(struct DisassemblyParameters* params, enum ModRMSelection selection, uint8_t operandSize, struct Operand* result);

static enum JdcStatus handleSIB(struct DisassemblyParameters* params, bool* gotDisp, struct Operand* result);
