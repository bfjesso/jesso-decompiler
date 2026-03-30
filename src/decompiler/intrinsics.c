#include "intrinsics.h"
#include "expressions.h"

struct IntrinsicFunc returningIntrinsicFuncs[] =
{
	{ AESDEC, { 1, 1, 0, 0 }, "_mm_aesdec" },
	{ AESDECLAST, { 1, 1, 0, 0 }, "_mm_aesdeclast" },
	{ AESENC, { 1, 1, 0, 0 }, "_mm_aesenc" },
	{ AESENCLAST, { 1, 1, 0, 0 }, "_mm_aesenclast" },
	{ AESIMC, { 0, 1, 0, 0 }, "_mm_aesimc" },
	{ AESKEYGENASSIST, { 0, 1, 1, 0 }, "_mm_aesimc" },
	{ STMXCSR, { 0, 0, 0, 0 }, "_mm_getcsr" },
	{ SHUFPD, { 1, 1, 1, 0 }, "_mm_shuffle_pd" },
	{ SHUFPS, { 1, 1, 1, 0 }, "_mm_shuffle_ps" },
};
const int numOfReturningIntrinsicFuncs = 9;

struct IntrinsicFunc voidIntrinsicFuncs[] =
{
	{ INT3, { 0, 0, 0, 0 }, "__debugbreak" },
	{ _INT, { 0, 0, 0, 0 }, "__fastfail" }, // this is only when the immediate is 0x29
	{ UD2, { 0, 0, 0, 0 }, "__ud2" },
};
const int numOfVoidIntrinsicFuncs = 3;

unsigned char checkForReturningIntrinsicFunc(enum Mnemonic opcode, struct IntrinsicFunc** intrinsicFuncRef)
{
	for (int i = 0; i < numOfReturningIntrinsicFuncs; i++)
	{
		if (opcode == returningIntrinsicFuncs[i].opcode)
		{
			*intrinsicFuncRef = &returningIntrinsicFuncs[i];
			return 1;
		}
	}

	return 0;
}

unsigned char decompileReturningIntrinsicFunc(struct DecompilationParameters params, struct IntrinsicFunc* intrinsicFunc, unsigned char getAssignment, struct VarType type, struct JdcStr* result)
{
	struct DisassembledInstruction* instruction = &params.currentFunc->instructions[params.startInstructionIndex];

	if (getAssignment)
	{
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (!decompileOperand(params, &instruction->operands[0], type, &decompiledFirstOperand))
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
			struct Operand* currentOperand = &instruction->operands[i];

			struct VarType operandType = getTypeOfOperand(instruction->opcode, currentOperand);

			struct JdcStr decompiledOperand = initializeJdcStr();
			if (!decompileOperand(params, currentOperand, operandType, &decompiledOperand))
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

unsigned char checkForVoidIntrinsicFunc(enum Mnemonic opcode, struct IntrinsicFunc** intrinsicFuncRef)
{
	for (int i = 0; i < numOfVoidIntrinsicFuncs; i++)
	{
		if (opcode == voidIntrinsicFuncs[i].opcode)
		{
			*intrinsicFuncRef = &voidIntrinsicFuncs[i];
			return 1;
		}
	}

	return 0;
}

unsigned char decompileVoidIntrinsicFunc(struct DecompilationParameters params, struct IntrinsicFunc* intrinsicFunc, struct JdcStr* result)
{
	struct DisassembledInstruction* instruction = &params.currentFunc->instructions[params.startInstructionIndex];

	if (intrinsicFunc->opcode == _INT) 
	{
		if (instruction->operands[0].type == IMMEDIATE && instruction->operands[0].immediate.value == 0x29) 
		{
			struct JdcStr code = initializeJdcStr();
			struct VarType type = { 0 };
			type.isUnsigned = 1;
			type.primitiveType = INT_TYPE;

			if (decompileRegister(params, CX, type, &code, 0))
			{
				sprintfJdc(result, 1, "%s(%s);\n", intrinsicFunc->name, code.buffer);
			}

			freeJdcStr(&code);
			return 1;
		}

		return 0;
	}
	
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
			struct Operand* currentOperand = &instruction->operands[i];
			
			struct VarType operandType = getTypeOfOperand(instruction->opcode, currentOperand);

			struct JdcStr decompiledOperand = initializeJdcStr();
			if (!decompileOperand(params, currentOperand, operandType, &decompiledOperand))
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

	strcatJdc(result, ");");
	return 1;
}
