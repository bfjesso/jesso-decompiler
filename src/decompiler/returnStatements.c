#include "returnStatements.h"
#include "expressions.h"
#include "assignment.h"
#include "functionCalls.h"

unsigned char doesInstructionModifyReturnRegister(struct DecompilationParameters params)
{
	struct DisassembledInstruction* instruction = &(params.currentFunc->instructions[params.startInstructionIndex]);
	unsigned long long address = params.currentFunc->instructions[params.startInstructionIndex].address;

	if ((params.currentFunc->returnType == FLOAT_TYPE || params.currentFunc->returnType == DOUBLE_TYPE) && instruction->opcode == FLD)
	{
		return 1;
	}
	else
	{
		if (doesInstructionModifyRegister(instruction, AX, 0, 0))
		{
			return 1;
		}
		else if (isOpcodeCall(instruction->opcode))
		{
			unsigned long long calleeAddress = address + instruction->operands[0].immediate;
			int calleIndex = findFunctionByAddress(params.functions, 0, params.numOfFunctions - 1, calleeAddress);

			if (calleIndex == -1)
			{
				return checkForImportCall(params) != -1;
			}
			else if (params.functions[calleIndex].returnType == VOID_TYPE)
			{
				return 0;
			}

			return 1;
		}
	}

	return 0;
}

unsigned char checkForReturnStatement(struct DecompilationParameters params)
{
	struct DisassembledInstruction* instruction = &(params.currentFunc->instructions[params.startInstructionIndex]);

	if (isOpcodeReturn(instruction->opcode))
	{
		return 1;
	}

	// check if jump to a return. this will only count if the jump leads directly to a ret, meaning the jmp is effectivly a ret instruction
	if (instruction->opcode == JMP_SHORT)
	{
		unsigned long long jmpDst = params.currentFunc->instructions[params.startInstructionIndex].address + instruction->operands[0].immediate;

		int jmpDstIndex = findInstructionByAddress(params.currentFunc->instructions, 0, params.currentFunc->numOfInstructions - 1, jmpDst);
		for (int i = jmpDstIndex; i < params.currentFunc->numOfInstructions; i++) // checking if the function leads to a return without doing anything in between
		{
			params.startInstructionIndex = i;

			if (checkForReturnStatement(params))
			{
				return 1;
			}

			if (checkForAssignment(params) || doesInstructionModifyReturnRegister(params))
			{
				return 0;
			}
		}
	}

	return 0;
}

unsigned char decompileReturnStatement(struct DecompilationParameters params, struct JdcStr* result)
{
	if (params.currentFunc->returnType == VOID_TYPE)
	{
		return strcatJdc(result, "return;");
	}

	struct JdcStr returnExpression = initializeJdcStr();
	int newStartInstruction = -1;

	// find where a return register is first being modified
	for (int i = params.startInstructionIndex; i >= 0; i--)
	{
		if (i == params.skipLowerBound)
		{
			i = params.skipUpperBound;
			continue;
		}

		params.startInstructionIndex = i;
		if (doesInstructionModifyReturnRegister(params))
		{
			newStartInstruction = i;
			break;
		}
	}

	if (newStartInstruction == -1)
	{
		return 0;
	}

	if (params.currentFunc->instructions[newStartInstruction].opcode == FLD)
	{
		params.startInstructionIndex = newStartInstruction - 1;
		if (!decompileOperand(params, &(params.currentFunc->instructions[newStartInstruction].operands[0]), params.currentFunc->returnType, &returnExpression))
		{
			freeJdcStr(&returnExpression);
			return 0;
		}
	}
	else
	{
		struct Operand eax = { 0 };
		eax.type = REGISTER;
		eax.reg = AX;

		params.startInstructionIndex = newStartInstruction;
		if (!decompileOperand(params, &eax, params.currentFunc->returnType, &returnExpression))
		{
			freeJdcStr(&returnExpression);
			return 0;
		}
	}


	sprintfJdc(result, 1, "return %s;", returnExpression.buffer);
	freeJdcStr(&returnExpression);

	return 1;
}