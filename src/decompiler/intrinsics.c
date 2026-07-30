#include "intrinsics.h"
#include "decompilationUtils.h"
#include "expressions.h"
#include "functions.h"

struct IntrinsicFunc returningIntrinsicFuncs[NUM_OF_RETURNING_INTRINSICS] =
{
	{ AESDEC, SINGLE_IFT, "_mm_aesdec" },
	{ AESDECLAST, SINGLE_IFT, "_mm_aesdeclast" },
	{ AESENC, SINGLE_IFT, "_mm_aesenc" },
	{ AESENCLAST, SINGLE_IFT, "_mm_aesenclast" },
	{ AESIMC, SINGLE_IFT, "_mm_aesimc" },
	{ AESKEYGENASSIST, SINGLE_IFT, "_mm_aeskeygenassist" },

	{ STMXCSR, SINGLE_IFT, "_mm_getcsr" },

	{ SHUFPD, SINGLE_IFT, "_mm_shuffle_pd" },
	{ SHUFPS, SINGLE_IFT, "_mm_shuffle_ps" },

	{ ROL, SINGLE_IFT, "_rotl" },
	{ ROR, SINGLE_IFT, "_rotr" },

	{ PUNPCKLBW, MMX_IFT, "_m_punpcklbw" },
	{ PUNPCKLBW, SSE_IFT, "_mm_unpacklo_epi8" },
	{ PUNPCKLWD, MMX_IFT, "_m_punpcklwd" },
	{ PUNPCKLWD, SSE_IFT, "_mm_unpacklo_epi16" },

	{ PSRAD, MMX_RM_IFT, "_m_psrad" },
	{ PSRAD, MMX_IMM_IFT, "_m_psradi" },
	{ PSRAD, SSE_RM_IFT, "_mm_sra_epi32" },
	{ PSRAD, SSE_IMM_IFT, "_mm_srai_epi32" },
};

struct IntrinsicFunc voidIntrinsicFuncs[NUM_OF_VOID_INTRINSICS] =
{
	{ INT3, SINGLE_IFT, "__debugbreak" },
	{ _INT, SINGLE_IFT, "__fastfail" }, // this is only when the immediate is 0x29
	{ UD2, SINGLE_IFT, "__ud2" },
	{ HLT, SINGLE_IFT, "__halt" },
	{ DATA, SINGLE_IFT, "DATA" },
	{ MOVS, SINGLE_IFT, "__movs" }, // REPZ prefix must be used
	{ XCHG, SINGLE_IFT, "__xchg" }, // this intrinsic should only be used when both operands would be decompiled as an assignment
};

static unsigned char checkValidIntrinsicFunctionType(struct DisassembledInstruction* instruction, struct IntrinsicFunc* intrinsicFunc)
{
	if (intrinsicFunc->type == SINGLE_IFT)
	{
		return 1;
	}

	if (instruction->numOfOperands == 0 || instruction->operands[0].type != REGISTER)
	{
		return 0;
	}

	switch (intrinsicFunc->type)
	{
	case MMX_IFT:
		if (!isRegMM(instruction->operands[0].reg)) { return 0; }
		break;
	case MMX_RM_IFT:
		if (!isRegMM(instruction->operands[0].reg) || instruction->numOfOperands < 2 || instruction->operands[1].type == IMMEDIATE) { return 0; }
		break;
	case MMX_IMM_IFT:
		if (!isRegMM(instruction->operands[0].reg) || instruction->numOfOperands < 2 || instruction->operands[1].type != IMMEDIATE) { return 0; }
		break;
	case SSE_IFT:
		if (!isRegXMM(instruction->operands[0].reg)) { return 0; }
		break;
	case SSE_RM_IFT:
		if (!isRegXMM(instruction->operands[0].reg) || instruction->numOfOperands < 2 || instruction->operands[1].type == IMMEDIATE) { return 0; }
		break;
	case SSE_IMM_IFT:
		if (!isRegXMM(instruction->operands[0].reg) || instruction->numOfOperands < 2 || instruction->operands[1].type != IMMEDIATE) { return 0; }
		break;
	}

	return 1;
}

unsigned char isInstructionReturningIntrinsicFunc(struct DisassembledInstruction* instruction, struct IntrinsicFunc** intrinsicFuncRef)
{
	for (int i = 0; i < NUM_OF_RETURNING_INTRINSICS; i++)
	{
		if (instruction->opcode == returningIntrinsicFuncs[i].opcode)
		{
			if (!checkValidIntrinsicFunctionType(instruction, &returningIntrinsicFuncs[i])) 
			{
				continue;
			}
			
			if (intrinsicFuncRef) { *intrinsicFuncRef = &returningIntrinsicFuncs[i]; }
			return 1;
		}
	}

	return 0;
}

unsigned char decompileReturningIntrinsicFunc(struct DecompilationParameters* params, int instructionIndex, struct IntrinsicFunc* intrinsicFunc, unsigned char getAssignment, struct JdcStr* result)
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

		sprintfJdc(result, 0, "%s = %s(", decompiledFirstOperand.buffer, intrinsicFunc->name);
		freeJdcStr(&decompiledFirstOperand);
	}
	else
	{
		sprintfJdc(result, 0, "%s(", intrinsicFunc->name);
	}

	for (int i = 0; i < instruction->numOfOperands; i++)
	{
		if (i == 0 && doesOpcodeOverwriteFirstOperand(intrinsicFunc->opcode)) 
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

unsigned char checkForVoidIntrinsicFunc(struct DecompilationParameters* params, int instructionIndex, struct IntrinsicFunc** intrinsicFuncRef)
{
	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];
	
	for (int i = 0; i < NUM_OF_VOID_INTRINSICS; i++)
	{
		if (instruction->opcode == voidIntrinsicFuncs[i].opcode)
		{
			if (instruction->opcode == _INT && (instruction->operands[0].type != IMMEDIATE || instruction->operands[0].immediate.value != 0x29)) 
			{
				return 0;
			}
			else if (instruction->opcode == MOVS && instruction->group1Prefix != REPZ)
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

			if (!checkValidIntrinsicFunctionType(instruction, &voidIntrinsicFuncs[i]))
			{
				continue;
			}

			*intrinsicFuncRef = &voidIntrinsicFuncs[i];
			return 1;
		}
	}

	return 0;
}

unsigned char decompileVoidIntrinsicFunc(struct DecompilationParameters* params, int instructionIndex, struct IntrinsicFunc* intrinsicFunc, struct JdcStr* result)
{
	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];

	addIndents(result, params->numOfIndents);
	
	sprintfJdc(result, 1, "%s(", intrinsicFunc->name);

	for(int i = 0; i < instruction->numOfOperands; i++)
	{
		if (intrinsicFunc->opcode == _INT) // the operand identifies the function
		{
			break;
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

	if (intrinsicFunc->opcode == _INT)
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
	else if (intrinsicFunc->opcode == MOVS)
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
