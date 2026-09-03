#include "intrinsics.h"
#include "decompilationUtils.h"
#include "expressions.h"
#include "functions.h"

// most intrinsics here come from intel's list of intrinsics, but some of them are OS specific, and some i just made up
// www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html

struct Intrinsic returningIntrinsics[NUM_OF_RETURNING_INTRINSICS] =
{
	{ AESDEC, SINGLE_IT, "_mm_aesdec" },
	{ AESDECLAST, SINGLE_IT, "_mm_aesdeclast" },
	{ AESENC, SINGLE_IT, "_mm_aesenc" },
	{ AESENCLAST, SINGLE_IT, "_mm_aesenclast" },
	{ AESIMC, SINGLE_IT, "_mm_aesimc" },
	{ AESKEYGENASSIST, SINGLE_IT, "_mm_aeskeygenassist" },

	{ STMXCSR, SINGLE_IT, "_mm_getcsr" },

	{ SHUFPD, SINGLE_IT, "_mm_shuffle_pd" },
	{ SHUFPS, SINGLE_IT, "_mm_shuffle_ps" },

	{ ROL, SINGLE_IT, "_rotl" },
	{ ROR, SINGLE_IT, "_rotr" },

	{ PUNPCKLBW, MMX_IT, "_m_punpcklbw" },
	{ PUNPCKLBW, SSE_IT, "_mm_unpacklo_epi8" },
	{ PUNPCKLWD, MMX_IT, "_m_punpcklwd" },
	{ PUNPCKLWD, SSE_IT, "_mm_unpacklo_epi16" },

	{ PSRAD, MMX_RM_IT, "_m_psrad" },
	{ PSRAD, MMX_IMM_IT, "_m_psradi" },
	{ PSRAD, SSE_RM_IT, "_mm_sra_epi32" },
	{ PSRAD, SSE_IMM_IT, "_mm_srai_epi32" },

	{ PADDD, MMX_IT, "_m_paddd" },
	{ PADDD, SSE_IT, "_mm_add_epi32" },

	{ PSRLDQ, SINGLE_IT, "_mm_srli_si128" },
};

struct Intrinsic voidIntrinsics[NUM_OF_VOID_INTRINSICS] =
{
	{ INT3, SINGLE_IT, "__debugbreak" },
	{ _INT, SINGLE_IT, "__fastfail" }, // this is only when the immediate is 0x29
	{ UD2, SINGLE_IT, "__ud2" },
	{ HLT, SINGLE_IT, "__halt" },
	{ DATA, SINGLE_IT, "DATA" },
	{ MOVS, REP_IT, "memcpy" },
	{ STOS, REP_IT, "memset" },
	{ XCHG, SINGLE_IT, "__xchg" }, // this intrinsic should only be used when both operands would be decompiled as an assignment
};

static unsigned char checkValidIntrinsicType(struct DisassembledInstruction* instruction, struct Intrinsic* intrinsic)
{
	if (intrinsic->type == SINGLE_IT)
	{
		return 1;
	}
	else if (intrinsic->type == REP_IT) 
	{
		return instruction->group1Prefix == REPZ;
	}

	if (instruction->numOfOperands == 0 || instruction->operands[0].type != REGISTER)
	{
		return 0;
	}

	switch (intrinsic->type)
	{
	case MMX_IT:
		if (!isRegMM(instruction->operands[0].reg)) { return 0; }
		break;
	case MMX_RM_IT:
		if (!isRegMM(instruction->operands[0].reg) || instruction->numOfOperands < 2 || instruction->operands[1].type == IMMEDIATE) { return 0; }
		break;
	case MMX_IMM_IT:
		if (!isRegMM(instruction->operands[0].reg) || instruction->numOfOperands < 2 || instruction->operands[1].type != IMMEDIATE) { return 0; }
		break;
	case SSE_IT:
		if (!isRegXMM(instruction->operands[0].reg)) { return 0; }
		break;
	case SSE_RM_IT:
		if (!isRegXMM(instruction->operands[0].reg) || instruction->numOfOperands < 2 || instruction->operands[1].type == IMMEDIATE) { return 0; }
		break;
	case SSE_IMM_IT:
		if (!isRegXMM(instruction->operands[0].reg) || instruction->numOfOperands < 2 || instruction->operands[1].type != IMMEDIATE) { return 0; }
		break;
	}

	return 1;
}

unsigned char isInstructionReturningIntrinsic(struct DisassembledInstruction* instruction, struct Intrinsic** intrinsicRef)
{
	for (int i = 0; i < NUM_OF_RETURNING_INTRINSICS; i++)
	{
		if (instruction->opcode == returningIntrinsics[i].opcode)
		{
			if (!checkValidIntrinsicType(instruction, &returningIntrinsics[i])) 
			{
				continue;
			}
			
			if (intrinsicRef) { *intrinsicRef = &returningIntrinsics[i]; }
			return 1;
		}
	}

	return 0;
}

unsigned char decompileReturningIntrinsic(struct DecompilationParameters* params, int instructionIndex, struct Intrinsic* intrinsic, unsigned char getAssignment, struct JdcStr* result)
{
	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];

	if (getAssignment)
	{
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (!decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			return 0;
		}

		sprintfJdc(result, 0, "%s = %s(", decompiledFirstOperand.buffer, intrinsic->name);
		freeJdcStr(&decompiledFirstOperand);
	}
	else
	{
		sprintfJdc(result, 0, "%s(", intrinsic->name);
	}

	for (int i = 0; i < instruction->numOfOperands; i++)
	{
		if (i == 0 && doesOpcodeOverwriteFirstOperand(intrinsic->opcode)) 
		{
			continue;
		}
		
		struct JdcStr decompiledOperand = initializeJdcStr();
		if (!decompileOperand(params, instructionIndex, i, 1, &decompiledOperand))
		{
			freeJdcStr(&decompiledOperand);
			return 0;
		}

		sprintfJdc(result, 1, "%s", decompiledOperand.buffer);
		freeJdcStr(&decompiledOperand);

		if (i < instruction->numOfOperands - 1)
		{
			strcatJdc(result, ", ");
		}
	}

	strcatJdc(result, ")");
	return 1;
}

unsigned char checkForVoidIntrinsic(struct DecompilationParameters* params, int instructionIndex, struct Intrinsic** intrinsicRef)
{
	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];
	
	for (int i = 0; i < NUM_OF_VOID_INTRINSICS; i++)
	{
		if (instruction->opcode == voidIntrinsics[i].opcode)
		{
			if (instruction->opcode == _INT && (instruction->operands[0].type != IMMEDIATE || instruction->operands[0].immediate.value != 0x29)) 
			{
				return 0;
			}
			else if (instruction->opcode == XCHG)
			{
				if (instruction->operands[0].type == MEM_ADDRESS && !getLocalRegVarByReg(params->currentFunc, instruction->operands[1].reg)) 
				{
					return 0;
				}
				else if (instruction->operands[1].type == MEM_ADDRESS && !getLocalRegVarByReg(params->currentFunc, instruction->operands[0].reg))
				{
					return 0;
				}
				else if (!getLocalRegVarByReg(params->currentFunc, instruction->operands[0].reg) && !getLocalRegVarByReg(params->currentFunc, instruction->operands[1].reg)) 
				{
					return 0;
				}
			}

			if (!checkValidIntrinsicType(instruction, &voidIntrinsics[i]))
			{
				continue;
			}

			*intrinsicRef = &voidIntrinsics[i];
			return 1;
		}
	}

	return 0;
}

unsigned char decompileVoidIntrinsic(struct DecompilationParameters* params, int instructionIndex, struct Intrinsic* intrinsic, struct JdcStr* result)
{
	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];

	addIndents(result, params->numOfIndents);
	
	sprintfJdc(result, 1, "%s(", intrinsic->name);

	for(int i = 0; i < instruction->numOfOperands; i++)
	{
		if (intrinsic->opcode == _INT) // the operand identifies the function
		{
			break;
		}

		struct JdcStr decompiledOperand = initializeJdcStr();

		if ((intrinsic->opcode == STOS && i == 0) || intrinsic->opcode == MOVS)
		{
			// the REP STOS/MOVS intrinsics take pointer(s) as arguments, so the reg in the mem address shouldnt be dereferenced
			if (!decompileRegister(params, instructionIndex, -1, instruction->operands[i].memoryAddress.reg, 1, &decompiledOperand, 0))
			{
				freeJdcStr(&decompiledOperand);
				return 0;
			}
		}
		else if (!decompileOperand(params, instructionIndex, i, 1, &decompiledOperand))
		{
			freeJdcStr(&decompiledOperand);
			return 0;
		}

		sprintfJdc(result, 1, "%s", decompiledOperand.buffer);
		freeJdcStr(&decompiledOperand);

		if (i < instruction->numOfOperands - 1)
		{
			strcatJdc(result, ", ");
		}
	}

	if (intrinsic->opcode == _INT)
	{
		struct JdcStr code = initializeJdcStr();
		if (!decompileRegister(params, instructionIndex, -1, CX, 1, &code, 0))
		{
			freeJdcStr(&code);
			return 0;
		}

		sprintfJdc(result, 1, "%s", code.buffer);
		freeJdcStr(&code);
	}
	else if (intrinsic->opcode == MOVS || intrinsic->opcode == STOS)
	{
		struct JdcStr count = initializeJdcStr();
		if (!decompileRegister(params, instructionIndex, -1, CX, 1, &count, 0))
		{
			freeJdcStr(&count);
			return 0;
		}

		switch (instruction->operands[0].memoryAddress.ptrSize)
		{
		case 1:
			strcatJdc(&count, " * sizeof(char)");
			break;
		case 2:
			strcatJdc(&count, " * sizeof(short)");
			break;
		case 4:
			strcatJdc(&count, " * sizeof(int)");
			break;
		case 8:
			strcatJdc(&count, " * sizeof(long long)");
			break;
		}

		sprintfJdc(result, 1, ", %s", count.buffer);
		freeJdcStr(&count);
	}

	strcatJdc(result, ");\n");
	addAssociatedInstruction(params->currentFunc, instructionIndex);
	params->currentFunc->numOfLines++;

	return 1;
}
