#include "functionCalls.h"
#include "decompilationUtils.h"
#include "functions.h"
#include "expressions.h"
#include "dataTypes.h"

unsigned char checkForKnownFunctionCall(struct DecompilationParameters* params, int instructionIndex, struct Function** calleeRef)
{
	struct DisassembledInstruction* instruction = &(params->instructions[instructionIndex]);

	unsigned long long calleeAddress = 0;
	if (isOpcodeCall(instruction->opcode) || isOpcodeJmp(instruction->opcode))
	{
		calleeAddress = resolveJmpChain(params, instructionIndex);
	}
	else if (instructionIndex == params->currentFunc->lastInstructionIndex && !isOpcodeReturn(instruction->opcode) && !doesOpcodeGenerateInterruptOrException(instruction->opcode))
	{
		calleeAddress = params->instructions[instructionIndex + 1].address; // this is the case where the current function ends without a ret instruction and the following instruction is the begining of a new function
	}
	else 
	{
		return 0;
	}

	int calleeIndex = findFunctionByAddress(params, 0, params->numOfFunctions - 1, calleeAddress);
	if (calleeIndex == -1)
	{
		return 0;
	}

	if (calleeRef) 
	{
		*calleeRef = &(params->functions[calleeIndex]);
	}

	return 1;
}

unsigned char decompileKnownFunctionCall(struct DecompilationParameters* params, int callInstructionIndex, struct Function* callee, struct JdcStr* result)
{
	struct DisassembledInstruction* callInstruction = &(params->instructions[callInstructionIndex]);

	addIndents(result, params->numOfIndents);

	struct ReturnedVariable* returnedVar = findReturnedVar(params->currentFunc, callInstruction->address);
	if (returnedVar != 0)
	{
		unsigned char isReturnRegVar = 0;
		for (int i = 0; i < params->currentFunc->numOfRegVars; i++) 
		{
			if (compareRegisters(params->currentFunc->regVars[i].reg, callee->returnReg)) 
			{
				sprintfJdc(result, 1, "%s = ", params->currentFunc->regVars[i].name.buffer);
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

	for (int i = 0; i < callee->numOfRegArgs; i++)
	{
		struct JdcStr argStr = initializeJdcStr();
		enum Register reg = callee->regArgs[i].reg;
		if (!decompileRegister(params, callInstructionIndex, reg, 1, &argStr, 0))
		{
			freeJdcStr(&argStr);
			return 0;
		}

		sprintfJdc(result, 1, "%s, ", argStr.buffer);
		freeJdcStr(&argStr);
	}

	int stackArgsFound = 0;
	for (int i = callInstructionIndex - 1; i >= params->currentFunc->firstInstructionIndex; i--)
	{
		if (stackArgsFound == callee->numOfStackArgs) { break; }

		struct DisassembledInstruction* currentInstruction = &(params->instructions[i]);

		if (currentInstruction->opcode == PUSH)
		{
			struct JdcStr argStr = initializeJdcStr();
			if (!decompileOperand(params, i, &currentInstruction->operands[0], 1, &argStr))
			{
				freeJdcStr(&argStr);
				return 0;
			}

			sprintfJdc(result, 1, "%s, ", argStr.buffer);
			freeJdcStr(&argStr);

			stackArgsFound++;
			addAssociatedInstruction(params->currentFunc, i);
		}
		else if (currentInstruction->operands[0].type == MEM_ADDRESS && (compareRegisters(currentInstruction->operands[0].memoryAddress.reg, BP) || compareRegisters(currentInstruction->operands[0].memoryAddress.reg, SP)))
		{
			struct JdcStr argStr = initializeJdcStr();
			if (!decompileOperand(params, i, &currentInstruction->operands[0], 1, &argStr)) // this should just get the stack var or arg
			{
				freeJdcStr(&argStr);
				return 0;
			}

			sprintfJdc(result, 1, "%s, ", argStr.buffer);
			freeJdcStr(&argStr);

			stackArgsFound++;
			addAssociatedInstruction(params->currentFunc, i);
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

	strcatJdc(result, ";\n");
	addAssociatedInstruction(params->currentFunc, callInstructionIndex);
	params->currentFunc->numOfLines++;

	return 1;
}

unsigned char checkForUnknownFunctionCall(struct DecompilationParameters* params, int instructionIndex)
{
	if (checkForKnownFunctionCall(params, instructionIndex, 0))
	{
		return 0;
	}
	
	struct DisassembledInstruction* instruction = &(params->instructions[instructionIndex]);

	if (isOpcodeCall(instruction->opcode)) 
	{
		return 1;
	}
	else if (instruction->opcode == JMP_NEAR)
	{
		unsigned long long calleeAddress = resolveJmpChain(params, instructionIndex);
		return (calleeAddress == 0) || (getImportIndexByAddress(params, calleeAddress) != -1);
	}

	return 0;
}

unsigned char decompileUnknownFunctionCall(struct DecompilationParameters* params, int callInstructionIndex, struct JdcStr* result)
{
	struct DisassembledInstruction* callInstruction = &(params->instructions[callInstructionIndex]);
	unsigned long long unknownFuncAddress = resolveJmpChain(params, callInstructionIndex);

	addIndents(result, params->numOfIndents);

	struct ReturnedVariable* returnedVar = findReturnedVar(params->currentFunc, callInstruction->address);
	if (returnedVar != 0)
	{
		unsigned char isReturnRegVar = 0;
		for (int i = 0; i < params->currentFunc->numOfRegVars; i++)
		{
			if (compareRegisters(params->currentFunc->regVars[i].reg, AX))
			{
				sprintfJdc(result, 1, "%s = ", params->currentFunc->regVars[i].name.buffer);
				isReturnRegVar = 1;
				break;
			}
		}

		if (!isReturnRegVar)
		{
			sprintfJdc(result, 1, "%s = ", returnedVar->name.buffer);
		}
	}

	const int maxStackArgs = 10;
	struct JdcStr stackArgTypeStrs[10] = { 0 };
	struct JdcStr decompiledStackArgs[10] = { 0 };
	int numOfStackArgs = 0;

	struct JdcStr regArgTypeStrs[NUM_PLATFORM_REG_ARGS] = { 0 };
	struct JdcStr decompiledRegArgs[NUM_PLATFORM_REG_ARGS] = { 0 };

	for (int i = callInstructionIndex - 1; i >= params->currentFunc->firstInstructionIndex; i--)
	{
		struct DisassembledInstruction* currentInstruction = &(params->instructions[i]);

		if (doesInstructionDoNothing(currentInstruction)) 
		{
			continue;
		}

		if (isOpcodeJmp(currentInstruction->opcode) || isOpcodeJcc(currentInstruction->opcode) || checkForUnknownFunctionCall(params, i)) // stop looking for parameters if instruction is jmp or another call with unknown parameters
		{
			break;
		}
		else if(isOpcodeCall(currentInstruction->opcode)) // if call to function with known parameters check if it has any
		{
			unsigned long long calleeAddress = resolveJmpChain(params, i);
			int calleeIndex = findFunctionByAddress(params, 0, params->numOfFunctions - 1, calleeAddress);
			if (calleeIndex != -1 && (params->functions[calleeIndex].numOfRegArgs > 0 || params->functions[calleeIndex].numOfStackArgs > 0))
			{
				break;
			}
		}

		if (numOfStackArgs < maxStackArgs && currentInstruction->opcode == PUSH)
		{
			if (currentInstruction->operands[0].type == REGISTER && isRegisterPointer(currentInstruction->operands[0].reg)) 
			{
				continue;
			}

			decompiledStackArgs[numOfStackArgs] = initializeJdcStr();
			if (decompileOperand(params, i, &currentInstruction->operands[0], 0, &decompiledStackArgs[numOfStackArgs]))
			{
				stackArgTypeStrs[numOfStackArgs] = initializeJdcStr();
				dataTypeToStr(getOperandDataType(currentInstruction->opcode, &currentInstruction->operands[0]), &stackArgTypeStrs[numOfStackArgs]);
				numOfStackArgs++;
				addAssociatedInstruction(params->currentFunc, i);
			}
		}
		else if (numOfStackArgs < maxStackArgs && currentInstruction->operands[0].type == MEM_ADDRESS && (compareRegisters(currentInstruction->operands[0].memoryAddress.reg, BP) || compareRegisters(currentInstruction->operands[0].memoryAddress.reg, SP)))
		{
			unsigned char overwrites = 0;
			if (doesInstructionModifyOperand(currentInstruction, 0, &overwrites) && overwrites)
			{
				decompiledStackArgs[numOfStackArgs] = initializeJdcStr();
				if (decompileOperand(params, i, &currentInstruction->operands[1], 0, &decompiledStackArgs[numOfStackArgs]))
				{
					stackArgTypeStrs[numOfStackArgs] = initializeJdcStr();
					dataTypeToStr(getOperandDataType(currentInstruction->opcode, &currentInstruction->operands[1]), &stackArgTypeStrs[numOfStackArgs]);
					numOfStackArgs++;
					addAssociatedInstruction(params->currentFunc, i);
				}
			}
		}

		for(int j = 0; j < NUM_PLATFORM_REG_ARGS; j++)
		{
			if (!decompiledRegArgs[j].buffer)
			{
				enum Register specificReg = NO_REG;
				if(doesInstructionModifyRegister(params, i, platformRegArgs[j], &specificReg, 0))
				{
					regArgTypeStrs[j] = initializeJdcStr();
					dataTypeToStr(getRegisterDataType(currentInstruction->opcode, specificReg), &regArgTypeStrs[j]);
					
					decompiledRegArgs[j] = initializeJdcStr();
					if (!decompileRegister(params, i + 1, specificReg, 1, &decompiledRegArgs[j], 0))
					{
						for (int j = 0; j < maxStackArgs; j++) { freeJdcStr(&stackArgTypeStrs[j]); }
						for (int j = 0; j < maxStackArgs; j++) { freeJdcStr(&decompiledStackArgs[j]); }
						for (int j = 0; j < NUM_PLATFORM_REG_ARGS; j++) { freeJdcStr(&regArgTypeStrs[j]); }
						for (int j = 0; j < NUM_PLATFORM_REG_ARGS; j++) { freeJdcStr(&decompiledRegArgs[j]); }
						return 0;
					}

					addAssociatedInstruction(params->currentFunc, i);
				}
			}
		}
	}

	int importIndex = getImportIndexByAddress(params, unknownFuncAddress);
	if (importIndex != -1) 
	{
		sprintfJdc(result, 1, "%s(", params->imports[importIndex].name.buffer);
	}
	else 
	{
		strcatJdc(result, "((");

		if (returnedVar != 0) 
		{
			struct JdcStr returnTypeStr = initializeJdcStr();
			dataTypeToStr(returnedVar->dataType, &returnTypeStr);
			strcatJdc(result, returnTypeStr.buffer);
			freeJdcStr(&returnTypeStr);
		}
		else 
		{
			strcatJdc(result, "void");
		}

		strcatJdc(result, " (*)("); // calling convention should go here

		for (int i = 0; i < NUM_PLATFORM_REG_ARGS; i++)
		{
			if (regArgTypeStrs[i].buffer)
			{
				sprintfJdc(result, 1, "%s, ", regArgTypeStrs[i].buffer);
			}
		}

		for (int i = 0; i < numOfStackArgs; i++)
		{
			sprintfJdc(result, 1, "%s, ", stackArgTypeStrs[i].buffer);
		}

		if (result->buffer[strlen(result->buffer) - 1] != '(')
		{
			result->buffer[strlen(result->buffer) - 2] = ')'; // removing the last comma
			result->buffer[strlen(result->buffer) - 1] = 0;
		}
		else
		{
			strcatJdc(result, ")");
		}

		strcatJdc(result, ")");

		struct JdcStr functionPointer = initializeJdcStr();
		if (!decompileOperand(params, callInstructionIndex, &callInstruction->operands[0], 1, &functionPointer))
		{
			for (int j = 0; j < numOfStackArgs + 1; j++) { freeJdcStr(&stackArgTypeStrs[j]); }
			for (int j = 0; j < numOfStackArgs + 1; j++) { freeJdcStr(&decompiledStackArgs[j]); }
			for (int j = 0; j < NUM_PLATFORM_REG_ARGS; j++) { freeJdcStr(&regArgTypeStrs[j]); }
			for (int j = 0; j < NUM_PLATFORM_REG_ARGS; j++) { freeJdcStr(&decompiledRegArgs[j]); }
			return 0;
		}

		if (callInstruction->opcode == CALL_NEAR && callInstruction->operands[0].type == IMMEDIATE) 
		{
			sprintfJdc(result, 1, "(0x%llX + %s)", callInstruction->address, functionPointer.buffer);
		}
		else 
		{
			sprintfJdc(result, 1, "(%s)", functionPointer.buffer);
		}
		
		freeJdcStr(&functionPointer);

		strcatJdc(result, ")(");
	}

	for(int i = 0; i < NUM_PLATFORM_REG_ARGS; i++)
	{
		if(decompiledRegArgs[i].buffer)
		{
			sprintfJdc(result, 1, "%s, ", decompiledRegArgs[i].buffer);
			freeJdcStr(&regArgTypeStrs[i]);
			freeJdcStr(&decompiledRegArgs[i]);
		}
	}

	for(int i = 0; i < numOfStackArgs; i++)
	{
		sprintfJdc(result, 1, "%s, ", decompiledStackArgs[i].buffer);
		freeJdcStr(&stackArgTypeStrs[i]);
		freeJdcStr(&decompiledStackArgs[i]);
	}

	if (result->buffer[strlen(result->buffer) - 1] != '(')
	{
		result->buffer[strlen(result->buffer) - 2] = ')'; // removing the last comma
		result->buffer[strlen(result->buffer) - 1] = 0;
		strcatJdc(result, "; // arguments are guessed\n");
	}
	else
	{
		strcatJdc(result, ");\n");
	}

	addAssociatedInstruction(params->currentFunc, callInstructionIndex);
	params->currentFunc->numOfLines++;

	return 1;
}

int getImportIndexByAddress(struct DecompilationParameters* params, unsigned long long calleeAddress)
{
	if (calleeAddress == 0) 
	{
		return -1;
	}
	
	for (int i = 0; i < params->numOfImports; i++)
	{
		if (params->imports[i].address == calleeAddress)
		{
			return i;
		}
	}

	return -1;
}
