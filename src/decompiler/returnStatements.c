#include "returnStatements.h"
#include "decompilationUtils.h"
#include "functions.h"
#include "expressions.h"
#include "functionCalls.h"

unsigned char doesInstructionModifyReturnRegister(struct DecompilationParameters* params)
{
	struct DisassembledInstruction* instruction = &(params->instructions[params->startInstructionIndex]);
	unsigned long long address = params->instructions[params->startInstructionIndex].address;

	if (doesInstructionModifyRegister(instruction, params->currentFunc->returnReg, 0, 0, 0))
	{
		return 1;
	}
	else if (isOpcodeCall(instruction->opcode))
	{
		unsigned long long calleeAddress = address + instruction->operands[0].immediate.value;
		int calleeIndex = findFunctionByAddress(params, 0, params->numOfFunctions - 1, calleeAddress);

		if (calleeIndex == -1)
		{
			return checkForUnknownFunctionCall(params);
		}

		return params->functions[calleeIndex].returnReg == params->currentFunc->returnReg;
	}

	return 0;
}

unsigned char checkForReturnStatement(struct DecompilationParameters* params)
{
	struct DisassembledInstruction* instruction = &params->instructions[params->startInstructionIndex];

	if (isOpcodeReturn(instruction->opcode) || (params->currentFunc && params->startInstructionIndex == params->currentFunc->lastInstructionIndex))
	{
		return 1;
	}

	// check if jump to a return. this will only count if the jump leads directly to a ret, meaning the jmp is effectivly a ret instruction
	if (isOpcodeJmp(instruction->opcode))
	{
		return checkForJumpToReturnStatement(params);
	}

	return 0;
}

unsigned char checkForJumpToReturnStatement(struct DecompilationParameters* params)
{
	struct DisassembledInstruction* instruction = &params->instructions[params->startInstructionIndex]; // this function assumes the current instruction is a jmp or jcc

	if (instruction->operands[0].type != IMMEDIATE) 
	{
		return 0;
	}

	unsigned long long jmpDstAddr = resolveJmpChain(params);
	int jmpDstIndex = findInstructionByAddress(params->instructions, 0, params->numOfInstructions, jmpDstAddr);

	if (jmpDstIndex == -1 || (params->currentFunc && (jmpDstIndex < params->currentFunc->firstInstructionIndex || jmpDstIndex > params->currentFunc->lastInstructionIndex)))
	{
		return 1;
	}

	int lastInstruction = params->currentFunc ? params->currentFunc->lastInstructionIndex : params->numOfInstructions - 1;
	for (int i = jmpDstIndex; i <= lastInstruction; i++) // checking if the function leads to a return without doing anything in between
	{
		if (isOpcodeReturn(params->instructions[i].opcode) || i == lastInstruction)
		{
			return 1;
		}

		if ((params->instructions[i].operands[0].type == MEM_ADDRESS && doesInstructionModifyOperand(&params->instructions[i], 0, 0, 0)) || 
			isOpcodeCall(params->instructions[i].opcode) || isOpcodeJcc(params->instructions[i].opcode) || 
			(params->currentFunc && doesInstructionModifyRegister(&params->instructions[i], params->currentFunc->returnReg, 0, 0, 0)))
		{
			return 0;
		}
	}

	return 0;
}

unsigned char decompileReturnStatement(struct DecompilationParameters* params, unsigned char* isInUnreachableStateRef, struct JdcStr* result)
{
	for (int i = 0; i < params->currentFunc->numOfConditions; i++) 
	{
		struct Condition* condition = &params->currentFunc->conditions[i];
		if (condition->conditionType == SWITCH_CASE_CT && params->startInstructionIndex == condition->dstIndex - 1) 
		{
			return 1;
		}
	}

	if (isInUnreachableStateRef) { *isInUnreachableStateRef = 1; }

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
