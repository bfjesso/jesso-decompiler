#include "returnStatements.h"
#include "decompilationUtils.h"
#include "functions.h"
#include "expressions.h"
#include "assignment.h"
#include "functionCalls.h"

unsigned char doesInstructionModifyReturnRegister(struct DecompilationParameters params)
{
	struct DisassembledInstruction* instruction = &(params.currentFunc->instructions[params.startInstructionIndex]);
	unsigned long long address = params.currentFunc->instructions[params.startInstructionIndex].address;

	if (doesInstructionModifyRegister(instruction, params.currentFunc->returnReg, 0, 0, 0))
	{
		return 1;
	}
	else if (isOpcodeCall(instruction->opcode))
	{
		unsigned long long calleeAddress = address + instruction->operands[0].immediate.value;
		int calleeIndex = findFunctionByAddress(params.functions, 0, params.numOfFunctions - 1, calleeAddress);

		if (calleeIndex == -1)
		{
			return checkForImportCall(params) != -1;
		}

		return params.functions[calleeIndex].returnReg == params.currentFunc->returnReg;
	}

	return 0;
}

unsigned char checkForReturnStatement(struct DecompilationParameters params)
{
	struct DisassembledInstruction* instruction = &(params.currentFunc->instructions[params.startInstructionIndex]);

	if (isOpcodeReturn(instruction->opcode) || params.startInstructionIndex == params.currentFunc->numOfInstructions - 1)
	{
		return 1;
	}

	// check if jump to a return. this will only count if the jump leads directly to a ret, meaning the jmp is effectivly a ret instruction
	if (isOpcodeJmp(instruction->opcode))
	{
		unsigned long long jmpDst = params.currentFunc->instructions[params.startInstructionIndex].address + instruction->operands[0].immediate.value;
		int jmpDstIndex = findInstructionByAddress(params.currentFunc->instructions, 0, params.currentFunc->numOfInstructions - 1, jmpDst);

		if (jmpDstIndex == -1 || jmpDstIndex >= params.currentFunc->numOfInstructions)
		{
			return 1;
		}

		for (int i = jmpDstIndex; i < params.currentFunc->numOfInstructions; i++) // checking if the function leads to a return without doing anything in between
		{
			params.startInstructionIndex = i;

			if (checkForReturnStatement(params))
			{
				return 1;
			}

			enum Mnemonic opcode = params.currentFunc->instructions[i].opcode;
			if (checkForAssignment(params) || isOpcodeCall(opcode) || isOpcodeJcc(opcode) || doesInstructionModifyReturnRegister(params))
			{
				return 0;
			}
		}
	}

	return 0;
}

unsigned char decompileReturnStatement(struct DecompilationParameters params, struct JdcStr* result)
{
	if (params.currentFunc->returnType.primitiveType == VOID_TYPE)
	{
		return strcatJdc(result, "return;");
	}

	struct JdcStr returnExpression = initializeJdcStr();
	if (!decompileRegister(params, params.currentFunc->returnReg, params.currentFunc->returnType, &returnExpression, 0))
	{
		freeJdcStr(&returnExpression);
		return 0;
	}

	sprintfJdc(result, 1, "return %s;", returnExpression.buffer);
	freeJdcStr(&returnExpression);

	return 1;
}
