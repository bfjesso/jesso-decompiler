#include "returnStatements.h"
#include "functions.h"
#include "decompilationUtils.h"
#include "functionCalls.h"
#include "expressions.h"

bool checkForReturnStatement(struct DecompilationParameters* params, int instructionIndex)
{
	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];

	if (isOpcodeReturn(instruction->opcode))
	{
		if (params->currentFunc->lastInstructionIndex == 0) // this is for when this function is called in findNextFunction
		{
			if (instruction->opcode == RET_NEAR && instruction->numOfOperands == 1) // this isn't checked in findNextFunction because this function can return 1 if there is a jmp to a return instruction
			{
				params->currentFunc->callingConvention = __STDCALL;
			}
			else
			{
				params->currentFunc->callingConvention = __CDECL;
			}
		}
		
		return true;
	}

	// check if jump to a return. this only counts if the jump goes to a location that leads directly to a return with nothing but stack clean up before
	if (isOpcodeJmp(instruction->opcode))
	{
		unsigned long long jmpDstAddr = resolveJmpChain(params, instructionIndex);
		int jmpDstIndex = findInstructionByAddress(params->instructions, params->numOfInstructions, jmpDstAddr);

		if (jmpDstIndex == -1)
		{
			params->currentFunc->callingConvention = __UNKNOWNCALL;
			return true;
		}
		else if (params->currentFunc)
		{
			if (jmpDstIndex < params->currentFunc->firstInstructionIndex)
			{
				params->currentFunc->callingConvention = __UNKNOWNCALL;
				return true;
			}
			else if (jmpDstIndex > params->currentFunc->lastInstructionIndex && params->currentFunc->lastInstructionIndex != 0)
			{
				params->currentFunc->callingConvention = __UNKNOWNCALL;
				return true;
			}
		}

		return doesInstructionLeadStraightToReturn(params, jmpDstIndex);
	}

	return false;
}

bool doesInstructionLeadStraightToReturn(struct DecompilationParameters* params, int startInstructionIndex) // checks if the function leads to a return without doing anything in between
{
	int lastInstruction = params->currentFunc && params->currentFunc->lastInstructionIndex != 0 ? params->currentFunc->lastInstructionIndex : params->numOfInstructions - 1;
	for (int i = startInstructionIndex; i <= lastInstruction; i++)
	{
		struct DisassembledInstruction* instruction = &params->instructions[i];
		if (isOpcodeReturn(instruction->opcode))
		{
			return true;
		}
		
		if (doesInstructionDoNothing(instruction) || instruction->opcode == POP || instruction->opcode == LEAVE || 
			(instruction->opcode == ADD && instruction->operands[0].type == REGISTER && compareRegisters(instruction->operands[0].reg, SP)))
		{
			continue;
		}

		return false;
	}

	return false;
}

enum JdcStatus decompileReturnStatement(struct DecompilationParameters* params, int instructionIndex, bool* isInUnreachableStateRef, struct JdcStr* result)
{
	if (isInUnreachableStateRef) { *isInUnreachableStateRef = true; }

	if (params->currentFunc->returnType.primitiveType == VOID_TYPE)
	{
		return addDecompiledLine(params, result, instructionIndex, "return;");
	}

	if (checkForKnownFunctionCall(params, instructionIndex, 0) || checkForUnknownFunctionCall(params, instructionIndex))
	{
		instructionIndex++; // this is because decompileRegister decrements the instruction index
	}

	struct JdcStr returnExpression = initializeJdcStr();
	if (ERROR_JDC == decompileRegister(params, instructionIndex, -1, params->currentFunc->returnReg, 1, 0, &returnExpression, 0))
	{
		freeJdcStr(&returnExpression);
		return ERROR_JDC;
	}

	addDecompiledLine(params, result, instructionIndex, "return %s;", returnExpression.buffer);

	freeJdcStr(&returnExpression);
	return SUCCESS_JDC;
}
