#include "intrinsics.h"
#include "decompilationUtils.h"
#include "expressions.h"
#include "functions.h"

// most intrinsics here come from intel's list of intrinsics, but some of them are OS specific, and some i just made up
// www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html

struct Intrinsic returningIntrinsics[NUM_OF_RETURNING_INTRINSICS] =
{
	{ AESDEC_X86, SINGLE_IT, "_mm_aesdec" },
	{ AESDECLAST_X86, SINGLE_IT, "_mm_aesdeclast" },
	{ AESENC_X86, SINGLE_IT, "_mm_aesenc" },
	{ AESENCLAST_X86, SINGLE_IT, "_mm_aesenclast" },
	{ AESIMC_X86, SINGLE_IT, "_mm_aesimc" },
	{ AESKEYGENASSIST_X86, SINGLE_IT, "_mm_aeskeygenassist" },

	{ STMXCSR_X86, SINGLE_IT, "_mm_getcsr" },

	{ SHUFPD_X86, SINGLE_IT, "_mm_shuffle_pd" },
	{ SHUFPS_X86, SINGLE_IT, "_mm_shuffle_ps" },

	{ ROL_X86, SINGLE_IT, "_rotl" },
	{ ROR_X86, SINGLE_IT, "_rotr" },

	{ PUNPCKLBW_X86, MMX_IT, "_m_punpcklbw" },
	{ PUNPCKLBW_X86, SSE_IT, "_mm_unpacklo_epi8" },
	{ PUNPCKLWD_X86, MMX_IT, "_m_punpcklwd" },
	{ PUNPCKLWD_X86, SSE_IT, "_mm_unpacklo_epi16" },

	{ PSRAD_X86, MMX_RM_IT, "_m_psrad" },
	{ PSRAD_X86, MMX_IMM_IT, "_m_psradi" },
	{ PSRAD_X86, SSE_RM_IT, "_mm_sra_epi32" },
	{ PSRAD_X86, SSE_IMM_IT, "_mm_srai_epi32" },

	{ PADDD_X86, MMX_IT, "_m_paddd" },
	{ PADDD_X86, SSE_IT, "_mm_add_epi32" },

	{ PSRLDQ_X86, SINGLE_IT, "_mm_srli_si128" },
};

struct Intrinsic voidIntrinsics[NUM_OF_VOID_INTRINSICS] =
{
	{ INT3_X86, SINGLE_IT, "__debugbreak" },
	{ INT_X86, SINGLE_IT, "__fastfail" }, // this is only when the immediate is 0x29
	{ UD2_X86, SINGLE_IT, "__ud2" },
	{ HLT_X86, SINGLE_IT, "__halt" },
	{ DATA_OPCODE, SINGLE_IT, "DATA" },
	{ MOVS_X86, REP_IT, "memcpy" },
	{ STOS_X86, REP_IT, "memset" },
	{ XCHG_X86, SINGLE_IT, "__xchg" }, // this intrinsic should only be used when both operands would be decompiled as an assignment
};

static bool checkValidIntrinsicType(struct DisassembledInstruction* instruction, struct Intrinsic* intrinsic)
{
	if (intrinsic->type == SINGLE_IT)
	{
		return true;
	}
	else if (intrinsic->type == REP_IT)
	{
		return instruction->group1Prefix == REPZ_LP;
	}

	if (instruction->numOfOperands == 0 || instruction->operands[0].type != REGISTER_OPERAND)
	{
		return false;
	}

	switch (intrinsic->type)
	{
	case MMX_IT:
		if (!isRegMM(instruction->operands[0].reg)) { return false; }
		break;
	case MMX_RM_IT:
		if (!isRegMM(instruction->operands[0].reg) || instruction->numOfOperands < 2 || instruction->operands[1].type == IMMEDIATE_OPERAND) { return false; }
		break;
	case MMX_IMM_IT:
		if (!isRegMM(instruction->operands[0].reg) || instruction->numOfOperands < 2 || instruction->operands[1].type != IMMEDIATE_OPERAND) { return false; }
		break;
	case SSE_IT:
		if (!isRegXMM(instruction->operands[0].reg)) { return false; }
		break;
	case SSE_RM_IT:
		if (!isRegXMM(instruction->operands[0].reg) || instruction->numOfOperands < 2 || instruction->operands[1].type == IMMEDIATE_OPERAND) { return false; }
		break;
	case SSE_IMM_IT:
		if (!isRegXMM(instruction->operands[0].reg) || instruction->numOfOperands < 2 || instruction->operands[1].type != IMMEDIATE_OPERAND) { return false; }
		break;
	}

	return true;
}

bool isInstructionReturningIntrinsic(struct DisassembledInstruction* instruction, struct Intrinsic** intrinsicRef)
{
	for (int32_t i = 0; i < NUM_OF_RETURNING_INTRINSICS; i++)
	{
		if (instruction->opcode == returningIntrinsics[i].opcode)
		{
			if (!checkValidIntrinsicType(instruction, &returningIntrinsics[i]))
			{
				continue;
			}

			if (intrinsicRef) { *intrinsicRef = &returningIntrinsics[i]; }
			return true;
		}
	}

	return false;
}

enum JdcStatus decompileReturningIntrinsic(struct DecompilationParameters* params, int32_t instructionIndex, struct Intrinsic* intrinsic, bool getAssignment, struct JdcStr* result)
{
	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];

	if (getAssignment)
	{
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			return ERROR_JDC;
		}

		sprintfJdc(result, false, "%s = %s(", decompiledFirstOperand.buffer, intrinsic->name);
		freeJdcStr(&decompiledFirstOperand);
	}
	else
	{
		sprintfJdc(result, false, "%s(", intrinsic->name);
	}

	for (int32_t i = 0; i < instruction->numOfOperands; i++)
	{
		if (i == 0 && doesOpcodeOverwriteFirstOperand(intrinsic->opcode))
		{
			continue;
		}

		struct JdcStr decompiledOperand = initializeJdcStr();
		if (ERROR_JDC == decompileOperand(params, instructionIndex, i, 1, &decompiledOperand))
		{
			freeJdcStr(&decompiledOperand);
			return ERROR_JDC;
		}

		sprintfJdc(result, true, "%s", decompiledOperand.buffer);
		freeJdcStr(&decompiledOperand);

		if (i < instruction->numOfOperands - 1)
		{
			strcatJdc(result, ", ");
		}
	}

	strcatJdc(result, ")");
	return SUCCESS_JDC;
}

bool checkForVoidIntrinsic(struct DecompilationParameters* params, int32_t instructionIndex, struct Intrinsic** intrinsicRef)
{
	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];

	for (int32_t i = 0; i < NUM_OF_VOID_INTRINSICS; i++)
	{
		if (instruction->opcode == voidIntrinsics[i].opcode)
		{
			if (instruction->opcode == INT_X86 && (instruction->operands[0].type != IMMEDIATE_OPERAND || instruction->operands[0].immediate.value != 0x29))
			{
				return false;
			}
			else if (instruction->opcode == XCHG_X86)
			{
				if (instruction->operands[0].type == MEMORY_OPERAND && !getLocalRegVarByReg(params->currentFunc, instruction->operands[1].reg))
				{
					return false;
				}
				else if (instruction->operands[1].type == MEMORY_OPERAND && !getLocalRegVarByReg(params->currentFunc, instruction->operands[0].reg))
				{
					return false;
				}
				else if (!getLocalRegVarByReg(params->currentFunc, instruction->operands[0].reg) && !getLocalRegVarByReg(params->currentFunc, instruction->operands[1].reg))
				{
					return false;
				}
			}

			if (!checkValidIntrinsicType(instruction, &voidIntrinsics[i]))
			{
				continue;
			}

			*intrinsicRef = &voidIntrinsics[i];
			return true;
		}
	}

	return false;
}

enum JdcStatus decompileVoidIntrinsic(struct DecompilationParameters* params, int32_t instructionIndex, struct Intrinsic* intrinsic, struct JdcStr* result)
{
	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];

	struct JdcStr decompiledCall = initializeJdcStrWithVal(intrinsic->name);
	strcatJdc(&decompiledCall, "(");

	for (int32_t i = 0; i < instruction->numOfOperands; i++)
	{
		if (intrinsic->opcode == INT_X86) // the operand identifies the function
		{
			break;
		}

		struct JdcStr decompiledOperand = initializeJdcStr();

		if ((intrinsic->opcode == STOS_X86 && i == 0) || intrinsic->opcode == MOVS_X86)
		{
			// the REP STOS/MOVS intrinsics take pointer(s) as arguments, so the reg in the mem address shouldnt be dereferenced
			if (ERROR_JDC == decompileRegister(params, instructionIndex, -1, instruction->operands[i].memoryAddress.reg, 1, 0, &decompiledOperand, 0))
			{
				freeJdcStr(&decompiledOperand);
				return ERROR_JDC;
			}
		}
		else if (ERROR_JDC == decompileOperand(params, instructionIndex, i, 1, &decompiledOperand))
		{
			freeJdcStr(&decompiledOperand);
			return ERROR_JDC;
		}

		sprintfJdc(&decompiledCall, true, "%s", decompiledOperand.buffer);
		freeJdcStr(&decompiledOperand);

		if (i < instruction->numOfOperands - 1)
		{
			strcatJdc(&decompiledCall, ", ");
		}
	}

	if (intrinsic->opcode == INT_X86)
	{
		struct JdcStr code = initializeJdcStr();
		if (ERROR_JDC == decompileRegister(params, instructionIndex, -1, CX_X86_REG, 1, 0, &code, 0))
		{
			freeJdcStr(&code);
			return ERROR_JDC;
		}

		sprintfJdc(&decompiledCall, true, "%s", code.buffer);
		freeJdcStr(&code);
	}
	else if (intrinsic->opcode == MOVS_X86 || intrinsic->opcode == STOS_X86)
	{
		struct JdcStr count = initializeJdcStr();
		if (ERROR_JDC == decompileRegister(params, instructionIndex, -1, CX_X86_REG, 1, 0, &count, 0))
		{
			freeJdcStr(&count);
			return ERROR_JDC;
		}

		switch (instruction->operands[0].memoryAddress.ptrSize)
		{
		case 1:
			sprintfJdc(&count, true, " * sizeof(%s)", primitiveTypeToStr(INT8_TYPE, params->useStdInt));
			break;
		case 2:
			sprintfJdc(&count, true, " * sizeof(%s)", primitiveTypeToStr(INT16_TYPE, params->useStdInt));
			break;
		case 4:
			sprintfJdc(&count, true, " * sizeof(%)", primitiveTypeToStr(INT32_TYPE, params->useStdInt));
			break;
		case 8:
			sprintfJdc(&count, true, " * sizeof(%s)", primitiveTypeToStr(INT64_TYPE, params->useStdInt));
			break;
		}

		sprintfJdc(&decompiledCall, true, ", %s", count.buffer);
		freeJdcStr(&count);
	}

	addDecompiledLine(params, result, instructionIndex, "%s);", decompiledCall.buffer);
	freeJdcStr(&decompiledCall);
	return SUCCESS_JDC;
}
