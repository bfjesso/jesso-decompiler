#pragma once
#include "disassemblyStructs.h"

enum ModRMSelection
{
	REG_MODRM,
	SEG_REG_MODRM,
	MMX_REG_MODRM,
	CONTROL_REG_MODRM,
	DEBUG_REG_MODRM,

	RM_MODRM,
	RM_MMX_MODRM,
};

enum JdcStatus getModRM(struct DisassemblyParameters* params);

enum JdcStatus handleModRM(struct DisassemblyParameters* params, enum ModRMSelection selection, uint8_t operandSize, struct Operand* result);

static enum JdcStatus handleSIB(struct DisassemblyParameters* params, bool* gotDisp, struct Operand* result);
