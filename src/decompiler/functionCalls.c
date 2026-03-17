#include "functionCalls.h"
#include "decompilationUtils.h"
#include "functions.h"
#include "expressions.h"
#include "dataTypes.h"

unsigned char checkForFunctionCall(struct DecompilationParameters params, struct Function** calleeRef)
{
	struct DisassembledInstruction* instruction = &(params.currentFunc->instructions[params.startInstructionIndex]);
	unsigned long long address = params.currentFunc->instructions[params.startInstructionIndex].address;

	if (isOpcodeCall(instruction->opcode))
	{
		int currentInstructionIndex = findInstructionByAddress(params.allInstructions, 0, params.totalNumOfInstructions - 1, address);
		unsigned long long calleeAddress = resolveJmpChain(params, currentInstructionIndex);
		int calleeIndex = findFunctionByAddress(params.functions, 0, params.numOfFunctions - 1, calleeAddress);

		if (calleeIndex == -1)
		{
			return 0;
		}

		*calleeRef = &(params.functions[calleeIndex]);

		return 1;
	}

	return 0;
}

unsigned char decompileFunctionCall(struct DecompilationParameters params, struct Function* callee, struct JdcStr* result)
{
	struct DisassembledInstruction* firstInstruction = &(params.currentFunc->instructions[params.startInstructionIndex]);

	int callNum = getFunctionCallNumber(params, callee->instructions[0].address);
	struct ReturnedVariable* returnedVar = findReturnedVar(params.currentFunc, callNum, callee->instructions[0].address);
	if (returnedVar != 0)
	{
		unsigned char isReturnRegVar = 0;
		for (int i = 0; i < params.currentFunc->numOfRegVars; i++) 
		{
			if (compareRegisters(params.currentFunc->regVars[i].reg, callee->returnReg)) 
			{
				sprintfJdc(result, 1, "%s = ", params.currentFunc->regVars[i].name.buffer);
				isReturnRegVar = 1;
				break;
			}
		}
		
		if(!isReturnRegVar)
		{
			sprintfJdc(result, 1, "%s = ", returnedVar->name.buffer);
		}
	}

	sprintfJdc(result, 1, "%s(", callee->name.buffer);

	params.startInstructionIndex--;
	for (int i = 0; i < callee->numOfRegArgs; i++)
	{
		struct JdcStr argStr = initializeJdcStr();
		enum Register reg = callee->regArgs[i].reg;
		if (!decompileRegister(params, reg, callee->regArgs[i].type, &argStr, 0))
		{
			freeJdcStr(&argStr);
			return 0;
		}

		sprintfJdc(result, 1, "%s, ", argStr.buffer);
		freeJdcStr(&argStr);
	}

	unsigned short ogStartInstructionIndex = params.startInstructionIndex;
	int stackArgsFound = 0;
	for (int i = ogStartInstructionIndex; i >= 0; i--)
	{
		if (stackArgsFound == callee->numOfStackArgs) { break; }

		struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[i]);
		unsigned long long addr = params.currentFunc->instructions[i].address;

		if (currentInstruction->opcode == PUSH)
		{
			params.startInstructionIndex = i;

			struct JdcStr argStr = initializeJdcStr();
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
			struct JdcStr argStr = initializeJdcStr();
			if (!decompileOperand(params, &currentInstruction->operands[0], callee->stackArgs[stackArgsFound].type, &argStr)) // this should just get the stack var or arg
			{
				freeJdcStr(&argStr);
				return 0;
			}

			sprintfJdc(result, 1, "%s, ", argStr.buffer);
			freeJdcStr(&argStr);

			stackArgsFound++;
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
	unsigned long long address = params.currentFunc->instructions[params.startInstructionIndex].address;

	if (isOpcodeCall(instruction->opcode))
	{
		int currentInstructionIndex = findInstructionByAddress(params.allInstructions, 0, params.totalNumOfInstructions - 1, address);
		unsigned long long calleeAddress = resolveJmpChain(params, currentInstructionIndex);

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
	struct ReturnedVariable* returnedVar = findReturnedVar(params.currentFunc, callNum, calleeAddress);
	if (returnedVar != 0)
	{
		unsigned char isReturnRegVar = 0;
		for (int i = 0; i < params.currentFunc->numOfRegVars; i++)
		{
			if (compareRegisters(params.currentFunc->regVars[i].reg, AX))
			{
				sprintfJdc(result, 1, "%s = ", params.currentFunc->regVars[i].name.buffer);
				isReturnRegVar = 1;
				break;
			}
		}

		if (!isReturnRegVar)
		{
			sprintfJdc(result, 1, "%s = ", returnedVar->name.buffer);
		}
	}

	sprintfJdc(result, 1, "%s(", params.imports[importIndex].name.buffer);

	unsigned short ogStartInstructionIndex = params.startInstructionIndex;

	struct JdcStr decompiledStackArgs[10] = { 0 };
	int numOfStackArgs = 0;

	struct JdcStr decompiledRegArgs[ST0 - RAX] = { 0 };

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
			int currentInstructionIndex = findInstructionByAddress(params.allInstructions, 0, params.totalNumOfInstructions - 1, params.currentFunc->instructions[i].address);
			unsigned long long calleeAddress = resolveJmpChain(params, currentInstructionIndex);
			int calleeIndex = findFunctionByAddress(params.functions, 0, params.numOfFunctions - 1, calleeAddress);
			if (calleeIndex != -1 && (params.functions[calleeIndex].numOfRegArgs > 0 || params.functions[calleeIndex].numOfStackArgs > 0))
			{
				break;
			}
		}

		if (currentInstruction->opcode == PUSH)
		{
			if (currentInstruction->operands[0].type == REGISTER && isRegisterPointer(currentInstruction->operands[0].reg)) 
			{
				continue;
			}
			
			struct VarType type = getTypeOfOperand(PUSH, &currentInstruction->operands[0]);

			params.startInstructionIndex = i;
			decompiledStackArgs[numOfStackArgs] = initializeJdcStr();
			if (!decompileOperand(params, &currentInstruction->operands[0], type, &decompiledStackArgs[numOfStackArgs]))
			{
				for(int j = 0; j < numOfStackArgs + 1; j++) { freeJdcStr(&decompiledStackArgs[j]); }
				for(int j = 0; j < numOfPlatformRegArgs; j++) { freeJdcStr(&decompiledRegArgs[j]); }
				return 0;
			}

			numOfStackArgs++;
		}
		else if (currentInstruction->operands[0].type == MEM_ADDRESS && (compareRegisters(currentInstruction->operands[0].memoryAddress.reg, BP) || compareRegisters(currentInstruction->operands[0].memoryAddress.reg, SP)))
		{
			unsigned char overwrites = 0;
			if (doesInstructionModifyOperand(currentInstruction, 0, 0, &overwrites) && overwrites)
			{
				struct VarType type = getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[1]);

				params.startInstructionIndex = i;
				decompiledStackArgs[numOfStackArgs] = initializeJdcStr();
				if (!decompileOperand(params, &currentInstruction->operands[1], type, &decompiledStackArgs[numOfStackArgs]))
				{
					for(int j = 0; j < numOfStackArgs + 1; j++) { freeJdcStr(&decompiledStackArgs[j]); }
					for(int j = 0; j < numOfPlatformRegArgs; j++) { freeJdcStr(&decompiledRegArgs[j]); }
					return 0;
				}

				numOfStackArgs++;
			}
		}

		for(int j = 0; j < numOfPlatformRegArgs; j++)
		{
			if (!decompiledRegArgs[j].buffer)
			{
				unsigned char regOperandNum = 0;
				if(doesInstructionModifyRegister(currentInstruction, platformRegArgs[j], &regOperandNum, 0, 0))
				{
					struct VarType type = getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[regOperandNum]);

					params.startInstructionIndex = i;
					decompiledRegArgs[j] = initializeJdcStr();
					if (!decompileOperand(params, &currentInstruction->operands[regOperandNum], type, &decompiledRegArgs[j]))
					{
						for(int k = 0; k < numOfStackArgs; k++) { freeJdcStr(&decompiledStackArgs[k]); }
						for(int k = 0; k < numOfPlatformRegArgs; k++) { freeJdcStr(&decompiledRegArgs[j]); }
						return 0;
					}
				}
			}
		}
	}

	for(int i = 0; i < numOfPlatformRegArgs; i++)
	{
		if(decompiledRegArgs[i].buffer)
		{
			sprintfJdc(result, 1, "%s, ", decompiledRegArgs[i].buffer);
			freeJdcStr(&decompiledRegArgs[i]);
		}
	}

	for(int i = 0; i < numOfStackArgs; i++)
	{
		sprintfJdc(result, 1, "%s, ", decompiledStackArgs[i].buffer);
		freeJdcStr(&decompiledStackArgs[i]);
	}

	if (result->buffer[strlen(result->buffer) - 1] != '(')
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

int getFunctionCallNumber(struct DecompilationParameters params, unsigned long long callAddr)
{
	int result = 0;

	for (int i = 0; i < params.startInstructionIndex; i++)
	{
		if (isOpcodeCall(params.currentFunc->instructions[i].opcode))
		{
			if (params.currentFunc->instructions[i].address + params.currentFunc->instructions[i].operands[0].immediate.value == callAddr ||
				params.currentFunc->instructions[i].operands[0].memoryAddress.constDisplacement == callAddr) // check for imported func call
			{
				result++;
			}
		}
	}

	return result;
}
