#include "intrinsics.h"
#include "decompilationUtils.h"
#include "expressions.h"
#include "functions.h"

struct IntrinsicFunc returningIntrinsicFuncs[NUM_OF_RETURNING_INTRINSICS] =
{
	{ AESDEC, { 1, 1, 0, 0 }, "_mm_aesdec" },
	{ AESDECLAST, { 1, 1, 0, 0 }, "_mm_aesdeclast" },
	{ AESENC, { 1, 1, 0, 0 }, "_mm_aesenc" },
	{ AESENCLAST, { 1, 1, 0, 0 }, "_mm_aesenclast" },
	{ AESIMC, { 0, 1, 0, 0 }, "_mm_aesimc" },
	{ AESKEYGENASSIST, { 0, 1, 1, 0 }, "_mm_aeskeygenassist" },
	{ STMXCSR, { 0, 0, 0, 0 }, "_mm_getcsr" },
	{ SHUFPD, { 1, 1, 1, 0 }, "_mm_shuffle_pd" },
	{ SHUFPS, { 1, 1, 1, 0 }, "_mm_shuffle_ps" },
	{ ROL, { 1, 1, 0, 0 }, "_rotl" },
	{ ROR, { 1, 1, 0, 0 }, "_rotr" },
};

struct IntrinsicFunc voidIntrinsicFuncs[NUM_OF_VOID_INTRINSICS] =
{
	{ INT3, { 0, 0, 0, 0 }, "__debugbreak" },
	{ _INT, { 0, 0, 0, 0 }, "__fastfail" }, // this is only when the immediate is 0x29
	{ UD2, { 0, 0, 0, 0 }, "__ud2" },
	{ HLT, { 0, 0, 0, 0 }, "__halt" },
	{ MOVS, { 1, 1, 0, 0 }, "__movs" }, // REPZ prefix must be used
	{ XCHG, { 1, 1, 0, 0 }, "__xchg" }, // this intrinsic should only be used when both operands would be decompiled as an assignment
};

unsigned char checkForReturningIntrinsicFunc(enum Mnemonic opcode, struct IntrinsicFunc** intrinsicFuncRef)
{
	for (int i = 0; i < NUM_OF_RETURNING_INTRINSICS; i++)
	{
		if (opcode == returningIntrinsicFuncs[i].opcode)
		{
			*intrinsicFuncRef = &returningIntrinsicFuncs[i];
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
		if (!decompileOperand(params, instructionIndex, &instruction->operands[0], 1, &decompiledFirstOperand))
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

	int lastDecompiledOperand = 0;
	for (int i = 3; i >= 0; i--)
	{
		if (intrinsicFunc->operandsToDecompile[i]) 
		{
			lastDecompiledOperand = i;
			break;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		if (intrinsicFunc->operandsToDecompile[i]) 
		{
			struct JdcStr decompiledOperand = initializeJdcStr();
			if (!decompileOperand(params, instructionIndex, &instruction->operands[i], 1, &decompiledOperand))
			{
				freeJdcStr(&decompiledOperand);
				return 0;
			}

			sprintfJdc(result, 1, "%s", decompiledOperand.buffer);
			freeJdcStr(&decompiledOperand);

			if (i < lastDecompiledOperand)
			{
				strcatJdc(result, ", ");
			}
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
				continue;
			}
			else if (instruction->opcode == MOVS && instruction->group1Prefix != REPZ)
			{
				continue;
			}
			else if (instruction->opcode == XCHG)
			{
				if (instruction->operands[0].type == MEM_ADDRESS && !getRegVarByReg(params->currentFunc, instruction->operands[1].reg)) 
				{
					continue;
				}
				else if (instruction->operands[1].type == MEM_ADDRESS && !getRegVarByReg(params->currentFunc, instruction->operands[0].reg))
				{
					continue;
				}
				else if (!getRegVarByReg(params->currentFunc, instruction->operands[0].reg) && !getRegVarByReg(params->currentFunc, instruction->operands[1].reg)) 
				{
					continue;
				}
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

	int lastDecompiledOperand = 0;
	for (int i = 3; i >= 0; i--)
	{
		if (intrinsicFunc->operandsToDecompile[i])
		{
			lastDecompiledOperand = i;
			break;
		}
	}

	for(int i = 0; i < 4; i++)
	{
		if (intrinsicFunc->operandsToDecompile[i])
		{
			struct JdcStr decompiledOperand = initializeJdcStr();
			if (!decompileOperand(params, instructionIndex , &instruction->operands[i], 1, &decompiledOperand))
			{
				freeJdcStr(&decompiledOperand);
				return 0;
			}
			
			sprintfJdc(result, 1, "%s", decompiledOperand.buffer);
			freeJdcStr(&decompiledOperand);

			if (i < lastDecompiledOperand)
			{
				strcatJdc(result, ", ");
			}
		}
	}

	if (intrinsicFunc->opcode == _INT)
	{
		struct JdcStr code = initializeJdcStr();
		if (!decompileRegister(params, instructionIndex, CX, 1, &code, 0))
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
		if (!decompileRegister(params, instructionIndex, CX, 1, &count, 0))
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
