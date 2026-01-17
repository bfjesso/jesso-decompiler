#include "functionCalls.h"
#include "expressions.h"
#include "dataTypes.h"

unsigned char checkForFunctionCall(struct DecompilationParameters params, struct Function** calleeRef)
{
	struct DisassembledInstruction* instruction = &(params.currentFunc->instructions[params.startInstructionIndex]);
	unsigned long long address = params.currentFunc->addresses[params.startInstructionIndex];

	if (isOpcodeCall(instruction->opcode))
	{
		int currentInstructionIndex = findInstructionByAddress(params.allAddresses, 0, params.totalNumOfInstructions - 1, address);
		unsigned long long calleeAddress = resolveJmpChain(params.allInstructions, params.allAddresses, params.totalNumOfInstructions, currentInstructionIndex);
		int calleIndex = findFunctionByAddress(params.functions, 0, params.numOfFunctions - 1, calleeAddress);

		if (calleIndex == -1)
		{
			return 0;
		}

		*calleeRef = &(params.functions[calleIndex]);

		return 1;
	}

	return 0;
}

unsigned char decompileFunctionCall(struct DecompilationParameters params, struct Function* callee, struct JdcStr* result)
{
	struct DisassembledInstruction* firstInstruction = &(params.currentFunc->instructions[params.startInstructionIndex]);

	if (firstInstruction->opcode == JMP_NEAR || firstInstruction->opcode == JMP_FAR)
	{
		sprintfJdc(result, 1, "%s();", callee->name);
		return 1;
	}

	int callNum = getFunctionCallNumber(params, callee->addresses[0]);
	struct FuncReturnVariable* returnVar = findReturnVar(params.currentFunc, callNum, callee->addresses[0]);
	if (returnVar != 0)
	{
		sprintfJdc(result, 1, "%s = ", returnVar->name);
	}

	sprintfJdc(result, 1, "%s(", callee->name);

	unsigned short ogStartInstructionIndex = params.startInstructionIndex;

	for (int i = 0; i < callee->numOfRegArgs; i++)
	{
		for (int j = ogStartInstructionIndex; j >= 0; j--)
		{
			struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[j]);

			if (currentInstruction->operands[0].type == REGISTER && doesInstructionModifyOperand(currentInstruction, 0, 0))
			{
				unsigned char reg = currentInstruction->operands[0].reg;

				if (compareRegisters(reg, callee->regArgs[i].reg))
				{
					params.startInstructionIndex = j;

					struct JdcStr argStr = { 0 };
					initializeJdcStr(&argStr, 255);
					if (!decompileOperand(params, &(currentInstruction->operands[0]), callee->regArgs[i].type, &argStr))
					{
						freeJdcStr(&argStr);
						return 0;
					}

					sprintfJdc(result, 1, "%s, ", argStr.buffer);
					freeJdcStr(&argStr);
					break;
				}
			}
		}
	}

	int stackArgsFound = 0;
	for (int i = ogStartInstructionIndex; i >= 0; i--)
	{
		if (stackArgsFound == callee->numOfStackArgs) { break; }

		struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[i]);
		unsigned long long addr = params.currentFunc->addresses[i];

		if (currentInstruction->opcode == PUSH)
		{
			params.startInstructionIndex = i;

			struct JdcStr argStr = { 0 };
			initializeJdcStr(&argStr, 255);
			if (!decompileOperand(params, &currentInstruction->operands[0], callee->stackArgs[stackArgsFound].type, &argStr))
			{
				freeJdcStr(&argStr);
				return 0;
			}

			sprintfJdc(result, 1, "%s, ", argStr.buffer);
			freeJdcStr(&argStr);

			stackArgsFound++;
		}
		else if (currentInstruction->operands[0].type == MEM_ADDRESS && (compareRegisters(currentInstruction->operands[0].memoryAddress.reg, BP) || compareRegisters(currentInstruction->operands[0].memoryAddress.reg, SP)))
		{
			unsigned char overwrites = 0;
			if (doesInstructionModifyOperand(currentInstruction, 0, &overwrites) && overwrites)
			{
				int operandIndex = getLastOperand(currentInstruction);

				params.startInstructionIndex = i;

				struct JdcStr argStr = { 0 };
				initializeJdcStr(&argStr, 255);
				if (!decompileOperand(params, &currentInstruction->operands[operandIndex], callee->stackArgs[stackArgsFound].type, &argStr))
				{
					freeJdcStr(&argStr);
					return 0;
				}

				sprintfJdc(result, 1, "%s, ", argStr.buffer);
				freeJdcStr(&argStr);

				stackArgsFound++;
			}
		}
	}

	if (result->buffer[strlen(result->buffer)-1] != '(')
	{
		result->buffer[strlen(result->buffer) - 2] = ')';
		result->buffer[strlen(result->buffer) - 1] = 0;
	}
	else
	{
		strcatJdc(result, ")");
	}

	strcatJdc(result, ";");

	return 1;
}

int checkForImportCall(struct DecompilationParameters params)
{
	struct DisassembledInstruction* instruction = &(params.currentFunc->instructions[params.startInstructionIndex]);
	unsigned long long address = params.currentFunc->addresses[params.startInstructionIndex];

	if (isOpcodeCall(instruction->opcode))
	{
		int currentInstructionIndex = findInstructionByAddress(params.allAddresses, 0, params.totalNumOfInstructions - 1, address);
		unsigned long long calleeAddress = resolveJmpChain(params.allInstructions, params.allAddresses, params.totalNumOfInstructions, currentInstructionIndex);

		for (int i = 0; i < params.numOfImports; i++)
		{
			if (params.imports[i].address == calleeAddress)
			{
				return i;
			}
		}
	}

	return -1;
}

unsigned char decompileImportCall(struct DecompilationParameters params, int importIndex, struct JdcStr* result)
{
	struct DisassembledInstruction* firstInstruction = &(params.currentFunc->instructions[params.startInstructionIndex]);

	unsigned long long calleeAddress = params.imports[importIndex].address;
	int callNum = getFunctionCallNumber(params, calleeAddress);
	struct FuncReturnVariable* returnVar = findReturnVar(params.currentFunc, callNum, calleeAddress);
	if (returnVar != 0)
	{
		sprintfJdc(result, 1, "%s = ", returnVar->name);
	}

	sprintfJdc(result, 1, "%s(", params.imports[importIndex].name);

	unsigned short ogStartInstructionIndex = params.startInstructionIndex;

	int stackArgsFound = 0;
	for (int i = ogStartInstructionIndex - 1; i >= 0; i--)
	{
		struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[i]);

		params.startInstructionIndex = i;
		if (currentInstruction->opcode == JMP_SHORT || checkForImportCall(params) != -1) // stop looking for parameters if instruction is jmp or another import call with unknown parameters
		{
			break;
		}
		else if(isOpcodeCall(currentInstruction->opcode)) // if call to function with known parameters check if it has any
		{
			int currentInstructionIndex = findInstructionByAddress(params.allAddresses, 0, params.totalNumOfInstructions - 1, params.currentFunc->addresses[i]);
			unsigned long long calleeAddress = resolveJmpChain(params.allInstructions, params.allAddresses, params.totalNumOfInstructions, currentInstructionIndex);
			int calleIndex = findFunctionByAddress(params.functions, 0, params.numOfFunctions - 1, calleeAddress);
			if (calleIndex != -1 && (params.functions[calleIndex].numOfRegArgs > 0 || params.functions[calleIndex].numOfStackArgs > 0))
			{
				break;
			}
		}

		if (currentInstruction->opcode == PUSH)
		{
			if (currentInstruction->operands[0].type == REGISTER && (compareRegisters(currentInstruction->operands[0].reg, BP) || compareRegisters(currentInstruction->operands[0].reg, SP)))
			{
				break;
			}

			unsigned char type = getTypeOfOperand(PUSH, &currentInstruction->operands[0], params.is64Bit);

			params.startInstructionIndex = i;
			struct JdcStr argStr = { 0 };
			initializeJdcStr(&argStr, 255);
			if (!decompileOperand(params, &currentInstruction->operands[0], type, &argStr))
			{
				freeJdcStr(&argStr);
				return 0;
			}

			sprintfJdc(result, 1, "%s, ", argStr.buffer);
			freeJdcStr(&argStr);

			stackArgsFound++;
		}
		else if (currentInstruction->operands[0].type == MEM_ADDRESS && (compareRegisters(currentInstruction->operands[0].memoryAddress.reg, BP) || compareRegisters(currentInstruction->operands[0].memoryAddress.reg, SP)))
		{
			unsigned char overwrites = 0;
			if (doesInstructionModifyOperand(currentInstruction, 0, &overwrites) && overwrites)
			{
				int operandIndex = getLastOperand(currentInstruction);

				unsigned char type = getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[operandIndex], params.is64Bit);

				params.startInstructionIndex = i;
				struct JdcStr argStr = { 0 };
				initializeJdcStr(&argStr, 255);
				if (!decompileOperand(params, &currentInstruction->operands[operandIndex], type, &argStr))
				{
					freeJdcStr(&argStr);
					return 0;
				}

				sprintfJdc(result, 1, "%s, ", argStr.buffer);
				freeJdcStr(&argStr);

				stackArgsFound++;
			}
		}
	}

	if (stackArgsFound != 0)
	{
		result->buffer[strlen(result->buffer) - 2] = ')';
		result->buffer[strlen(result->buffer) - 1] = 0;
	}
	else
	{
		strcat(result->buffer, ")");
	}

	strcat(result->buffer, ";");

	return 1;
}

int getFunctionCallNumber(struct DecompilationParameters params, unsigned long long callAddr)
{
	int result = 0;

	for (int i = 0; i < params.startInstructionIndex; i++)
	{
		if (isOpcodeCall(params.currentFunc->instructions[i].opcode))
		{
			if (params.currentFunc->addresses[i] + params.currentFunc->instructions[i].operands[0].immediate == callAddr ||
				params.currentFunc->instructions[i].operands[0].memoryAddress.constDisplacement == callAddr) // check for imported func call
			{
				result++;
			}
		}
	}

	return result;
}
