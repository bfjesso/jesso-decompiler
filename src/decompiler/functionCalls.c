#include "functionCalls.h"
#include "expressions.h"
#include "dataTypes.h"

unsigned char checkForFunctionCall(struct DecompilationParameters params, struct Function** calleeRef)
{
	struct DisassembledInstruction* instruction = &(params.currentFunc->instructions[params.startInstructionIndex]);
	unsigned long long address = params.currentFunc->addresses[params.startInstructionIndex];

	if (instruction->opcode == CALL_NEAR || instruction->opcode == JMP_NEAR)
	{
		unsigned long long calleeAddress = resolveJmpChain(params, instruction, address);
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

unsigned char decompileFunctionCall(struct DecompilationParameters params, struct Function* callee, struct LineOfC* result)
{
	struct DisassembledInstruction* firstInstruction = &(params.currentFunc->instructions[params.startInstructionIndex]);

	if (firstInstruction->opcode == JMP_NEAR || firstInstruction->opcode == JMP_FAR)
	{
		sprintf(result->line, "%s();", callee->name);
		return 1;
	}

	if (callee->returnType != VOID_TYPE)
	{
		unsigned long long calleeAddress = params.currentFunc->addresses[params.startInstructionIndex] + firstInstruction->operands[0].immediate;
		int callNum = getFunctionCallNumber(params, calleeAddress);
		sprintf(result->line, "%s %sRetVal%d = ", primitiveTypeStrs[callee->returnType], callee->name, callNum);
	}

	sprintf(&(result->line[strlen(result->line)]), "%s(", callee->name);

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

					char argStr[100] = { 0 };
					if (!decompileOperand(params, &(currentInstruction->operands[0]), callee->regArgs[i].type, argStr, 100))
					{
						return 0;
					}

					sprintf(result->line + strlen(result->line), "%s, ", argStr);

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

			char argStr[100] = { 0 };
			if (!decompileOperand(params, &currentInstruction->operands[0], callee->stackArgs[stackArgsFound].type, argStr, 100))
			{
				return 0;
			}

			sprintf(result->line + strlen(result->line), "%s, ", argStr);

			stackArgsFound++;
		}
		else if (currentInstruction->operands[0].type == MEM_ADDRESS && (compareRegisters(currentInstruction->operands[0].memoryAddress.reg, BP) || compareRegisters(currentInstruction->operands[0].memoryAddress.reg, SP)))
		{
			unsigned char overwrites = 0;
			if (doesInstructionModifyOperand(currentInstruction, 0, &overwrites) && overwrites)
			{
				int operandIndex = getLastOperand(currentInstruction);

				params.startInstructionIndex = i;

				char argStr[100] = { 0 };
				if (!decompileOperand(params, &currentInstruction->operands[operandIndex], callee->stackArgs[stackArgsFound].type, argStr, 100))
				{
					return 0;
				}

				sprintf(result->line + strlen(result->line), "%s, ", argStr);

				stackArgsFound++;
			}
		}
	}

	if (callee->numOfRegArgs != 0 || callee->numOfStackArgs != 0)
	{
		result->line[strlen(result->line) - 2] = ')';
		result->line[strlen(result->line) - 1] = 0;
	}
	else
	{
		strcat(result->line, ")");
	}

	strcat(result->line, ";");

	return 1;
}

int checkForImportCall(struct DecompilationParameters params)
{
	struct DisassembledInstruction* instruction = &(params.currentFunc->instructions[params.startInstructionIndex]);
	unsigned long long address = params.currentFunc->addresses[params.startInstructionIndex];

	if (instruction->opcode == CALL_NEAR || instruction->opcode == JMP_NEAR)
	{
		unsigned long long calleeAddress = resolveJmpChain(params, instruction, address);

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

unsigned char decompileImportCall(struct DecompilationParameters params, const char* name, struct LineOfC* result)
{
	struct DisassembledInstruction* firstInstruction = &(params.currentFunc->instructions[params.startInstructionIndex]);

	// right now this works by checking if AX is ever accessed without being assigned after the call and until the next function call
	unsigned char returnType = VOID_TYPE;
	for (int i = params.startInstructionIndex + 1; i < params.currentFunc->numOfInstructions; i++)
	{
		enum Mnemonic opcode = params.currentFunc->instructions[i].opcode;
		if (opcode == CALL_NEAR || opcode == JMP_NEAR || opcode == JMP_SHORT)
		{
			break;
		}

		char isDone = 0;
		for (int j = 0; j < 3; j++)
		{
			struct Operand* op = &(params.currentFunc->instructions[i].operands[j]);
			if (op->type == REGISTER && compareRegisters(op->reg, AX))
			{
				if (!doesInstructionModifyOperand(&(params.currentFunc->instructions[i]), j, 0))
				{
					returnType = getTypeOfOperand(params.currentFunc->instructions[i].opcode, op);
				}
				isDone = 1;
			}
		}
		if (isDone)
		{
			break;
		}
	}

	if (returnType != VOID_TYPE)
	{
		unsigned long long calleeAddress = firstInstruction->operands[0].memoryAddress.constDisplacement;
		int callNum = getFunctionCallNumber(params, calleeAddress);
		sprintf(result->line, "%s %sRetVal%d = ", primitiveTypeStrs[returnType], name, callNum);
	}

	sprintf(&(result->line[strlen(result->line)]), "%s(", name);

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
		else if(currentInstruction->opcode == CALL_NEAR) // if call to function with known parameters check if it has any
		{
			unsigned long long calleeAddress = resolveJmpChain(params, currentInstruction, params.currentFunc->addresses[i]);
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

			unsigned char type = getTypeOfOperand(PUSH, &currentInstruction->operands[0]);

			params.startInstructionIndex = i;
			char argStr[100] = { 0 };
			if (!decompileOperand(params, &currentInstruction->operands[0], type, argStr, 100))
			{
				return 0;
			}

			sprintf(result->line + strlen(result->line), "%s, ", argStr);

			stackArgsFound++;
		}
		else if (currentInstruction->operands[0].type == MEM_ADDRESS && (compareRegisters(currentInstruction->operands[0].memoryAddress.reg, BP) || compareRegisters(currentInstruction->operands[0].memoryAddress.reg, SP)))
		{
			unsigned char overwrites = 0;
			if (doesInstructionModifyOperand(currentInstruction, 0, &overwrites) && overwrites)
			{
				int operandIndex = getLastOperand(currentInstruction);

				unsigned char type = getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[operandIndex]);

				params.startInstructionIndex = i;
				char argStr[100] = { 0 };
				if (!decompileOperand(params, &currentInstruction->operands[operandIndex], type, argStr, 100))
				{
					return 0;
				}

				sprintf(result->line + strlen(result->line), "%s, ", argStr);

				stackArgsFound++;
			}
		}
	}

	if (stackArgsFound != 0)
	{
		result->line[strlen(result->line) - 2] = ')';
		result->line[strlen(result->line) - 1] = 0;
	}
	else
	{
		strcat(result->line, ")");
	}

	strcat(result->line, ";");

	return 1;
}

int getFunctionCallNumber(struct DecompilationParameters params, unsigned long long callAddr)
{
	int result = 0;

	for (int i = 0; i < params.startInstructionIndex; i++)
	{
		if (params.currentFunc->instructions[i].opcode == CALL_NEAR || params.currentFunc->instructions[i].opcode == CALL_FAR)
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
