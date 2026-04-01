#include "returnStatements.h"
#include "decompilationUtils.h"
#include "functions.h"
#include "expressions.h"
#include "functionCalls.h"

unsigned char doesInstructionModifyReturnRegister(struct DecompilationParameters* params)
{
	struct DisassembledInstruction* instruction = &(params->currentFunc->instructions[params->startInstructionIndex]);
	unsigned long long address = params->currentFunc->instructions[params->startInstructionIndex].address;

	if (doesInstructionModifyRegister(instruction, params->currentFunc->returnReg, 0, 0, 0))
	{
		return 1;
	}
	else if (isOpcodeCall(instruction->opcode))
	{
		unsigned long long calleeAddress = address + instruction->operands[0].immediate.value;
		int calleeIndex = findFunctionByAddress(params->functions, 0, params->numOfFunctions - 1, calleeAddress);

		if (calleeIndex == -1)
		{
			return checkForImportCall(params) != -1;
		}

		return params->functions[calleeIndex].returnReg == params->currentFunc->returnReg;
	}

	return 0;
}

unsigned char checkForReturnStatement(struct Function* function, int startInstructionIndex, struct DisassembledInstruction* instructions, int numOfInstructions)
{
	struct DisassembledInstruction* instruction = &instructions[startInstructionIndex];

	if (isOpcodeReturn(instruction->opcode) || startInstructionIndex == numOfInstructions - 1)
	{
		return 1;
	}

	// check if jump to a return. this will only count if the jump leads directly to a ret, meaning the jmp is effectivly a ret instruction
	if (isOpcodeJmp(instruction->opcode))
	{
		return checkForJumpToReturnStatement(function, startInstructionIndex, instructions, numOfInstructions);
	}

	return 0;
}

unsigned char checkForJumpToReturnStatement(struct Function* function, int startInstructionIndex, struct DisassembledInstruction* instructions, int numOfInstructions)
{
	struct DisassembledInstruction* instruction = &instructions[startInstructionIndex]; // this function assumes the current instruction is a jmp or jcc

	unsigned long long jmpDst = instruction->address + instruction->operands[0].immediate.value;
	int jmpDstIndex = findInstructionByAddress(instructions, 0, numOfInstructions - 1, jmpDst);

	if (jmpDstIndex == -1 || jmpDstIndex >= numOfInstructions)
	{
		return 1;
	}

	for (int i = jmpDstIndex; i < numOfInstructions; i++) // checking if the function leads to a return without doing anything in between
	{
		if (isOpcodeReturn(instructions[i].opcode) || i == numOfInstructions - 1)
		{
			return 1;
		}

		if ((instructions[i].operands[0].type == MEM_ADDRESS && doesInstructionModifyOperand(&instructions[i], 0, 0, 0)) || isOpcodeCall(instructions[i].opcode) || isOpcodeJcc(instructions[i].opcode) || doesInstructionModifyRegister(&instructions[i], function->returnReg, 0, 0, 0))
		{
			return 0;
		}
	}

	return 0;
}

unsigned char decompileReturnStatement(struct DecompilationParameters* params, struct JdcStr* result)
{
	for (int i = 0; i < params->currentFunc->numOfConditions; i++) 
	{
		struct Condition* condition = &params->currentFunc->conditions[i];
		if (condition->conditionType == SWITCH_CASE_CT && params->startInstructionIndex == condition->dstIndex - 1) 
		{
			return 1;
		}
	}

	addIndents(result, params->numOfIndents);
	
	if (params->currentFunc->returnType.primitiveType == VOID_TYPE)
	{
		return strcatJdc(result, "return;\n");
	}

	struct JdcStr returnExpression = initializeJdcStr();
	if (!decompileRegister(params, params->currentFunc->returnReg, &returnExpression, 0))
	{
		freeJdcStr(&returnExpression);
		return 0;
	}

	sprintfJdc(result, 1, "return %s;\n", returnExpression.buffer);
	freeJdcStr(&returnExpression);

	return 1;
}
