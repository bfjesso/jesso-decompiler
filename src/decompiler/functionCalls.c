#include "functionCalls.h"
#include "decompilationUtils.h"
#include "functions.h"
#include "expressions.h"
#include "dataTypes.h"

unsigned char checkForKnownFunctionCall(struct DecompilationParameters* params, struct Function** calleeRef)
{
	struct DisassembledInstruction* instruction = &(params->instructions[params->startInstructionIndex]);
	unsigned long long address = params->instructions[params->startInstructionIndex].address;

	unsigned long long calleeAddress = 0;
	if (isOpcodeCall(instruction->opcode))
	{
		calleeAddress = resolveJmpChain(params);
	}
	else if (params->startInstructionIndex == params->currentFunc->lastInstructionIndex && !isOpcodeReturn(instruction->opcode))
	{
		calleeAddress = params->instructions[params->startInstructionIndex + 1].address; // this is the case where the current function ends without a ret instruction and the following instruction is the begining of a new function
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

unsigned char decompileKnownFunctionCall(struct DecompilationParameters* params, struct Function* callee, struct JdcStr* result)
{
	struct DisassembledInstruction* firstInstruction = &(params->instructions[params->startInstructionIndex]);

	addIndents(result, params->numOfIndents);

	unsigned long long calleeAddress = params->instructions[callee->firstInstructionIndex].address;
	int callNum = getFunctionCallNumber(params, calleeAddress);
	struct ReturnedVariable* returnedVar = findReturnedVar(params->currentFunc, callNum, calleeAddress);
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

	unsigned short ogStartInstructionIndex = params->startInstructionIndex;

	params->startInstructionIndex--;
	for (int i = 0; i < callee->numOfRegArgs; i++)
	{
		struct JdcStr argStr = initializeJdcStr();
		enum Register reg = callee->regArgs[i].reg;
		if (!decompileRegister(params, reg, &argStr, 0))
		{
			freeJdcStr(&argStr);
			return 0;
		}

		sprintfJdc(result, 1, "%s, ", argStr.buffer);
		freeJdcStr(&argStr);
	}

	int stackArgsFound = 0;
	for (int i = ogStartInstructionIndex - 1; i >= params->currentFunc->firstInstructionIndex; i--)
	{
		if (stackArgsFound == callee->numOfStackArgs) { break; }

		struct DisassembledInstruction* currentInstruction = &(params->instructions[i]);

		if (currentInstruction->opcode == PUSH)
		{
			params->startInstructionIndex = i;

			struct JdcStr argStr = initializeJdcStr();
			if (!decompileOperand(params, &currentInstruction->operands[0], &argStr))
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
			if (!decompileOperand(params, &currentInstruction->operands[0], &argStr)) // this should just get the stack var or arg
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

	strcatJdc(result, ";\n");

	params->startInstructionIndex = ogStartInstructionIndex;
	return 1;
}

int checkForUnknownFunctionCall(struct DecompilationParameters* params)
{
	if (checkForKnownFunctionCall(params, 0)) 
	{
		return 0;
	}
	
	struct DisassembledInstruction* instruction = &(params->instructions[params->startInstructionIndex]);

	if (isOpcodeCall(instruction->opcode)) 
	{
		return 1;
	}
	else if (instruction->opcode == JMP_NEAR)
	{
		unsigned long long calleeAddress = resolveJmpChain(params);
		return getImportIndexByAddress(params, calleeAddress) != -1;
	}

	return 0;
}

unsigned char decompileUnknownFunctionCall(struct DecompilationParameters* params, struct JdcStr* result)
{
	struct DisassembledInstruction* callInstruction = &(params->instructions[params->startInstructionIndex]);
	unsigned long long calleeAddress = resolveJmpChain(params);

	addIndents(result, params->numOfIndents);

	int callNum = getFunctionCallNumber(params, calleeAddress);
	struct ReturnedVariable* returnedVar = findReturnedVar(params->currentFunc, callNum, calleeAddress);
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

	unsigned short ogStartInstructionIndex = params->startInstructionIndex;

	struct JdcStr stackArgTypeStrs[10] = { 0 };
	struct JdcStr decompiledStackArgs[10] = { 0 };
	int numOfStackArgs = 0;

	struct JdcStr regArgTypeStrs[NUM_PLATFORM_REG_ARGS] = { 0 };
	struct JdcStr decompiledRegArgs[NUM_PLATFORM_REG_ARGS] = { 0 };

	for (int i = ogStartInstructionIndex - 1; i >= params->currentFunc->firstInstructionIndex; i--)
	{
		struct DisassembledInstruction* currentInstruction = &(params->instructions[i]);

		params->startInstructionIndex = i;
		if (currentInstruction->opcode == JMP_SHORT || checkForUnknownFunctionCall(params)) // stop looking for parameters if instruction is jmp or another call with unknown parameters
		{
			break;
		}
		else if(isOpcodeCall(currentInstruction->opcode)) // if call to function with known parameters check if it has any
		{
			unsigned long long calleeAddress = resolveJmpChain(params);
			int calleeIndex = findFunctionByAddress(params, 0, params->numOfFunctions - 1, calleeAddress);
			if (calleeIndex != -1 && (params->functions[calleeIndex].numOfRegArgs > 0 || params->functions[calleeIndex].numOfStackArgs > 0))
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

			stackArgTypeStrs[numOfStackArgs] = initializeJdcStr();
			varTypeToStr(getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[0]), &stackArgTypeStrs[numOfStackArgs]);

			params->startInstructionIndex = i;
			decompiledStackArgs[numOfStackArgs] = initializeJdcStr();
			if (!decompileOperand(params, &currentInstruction->operands[0], &decompiledStackArgs[numOfStackArgs]))
			{
				for (int j = 0; j < numOfStackArgs + 1; j++) { freeJdcStr(&stackArgTypeStrs[j]); }
				for(int j = 0; j < numOfStackArgs + 1; j++) { freeJdcStr(&decompiledStackArgs[j]); }
				for (int j = 0; j < NUM_PLATFORM_REG_ARGS; j++) { freeJdcStr(&regArgTypeStrs[j]); }
				for(int j = 0; j < NUM_PLATFORM_REG_ARGS; j++) { freeJdcStr(&decompiledRegArgs[j]); }
				return 0;
			}

			numOfStackArgs++;
		}
		else if (currentInstruction->operands[0].type == MEM_ADDRESS && (compareRegisters(currentInstruction->operands[0].memoryAddress.reg, BP) || compareRegisters(currentInstruction->operands[0].memoryAddress.reg, SP)))
		{
			unsigned char overwrites = 0;
			if (doesInstructionModifyOperand(currentInstruction, 0, 0, &overwrites) && overwrites)
			{
				stackArgTypeStrs[numOfStackArgs] = initializeJdcStr();
				varTypeToStr(getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[1]), &stackArgTypeStrs[numOfStackArgs]);
				
				params->startInstructionIndex = i;
				decompiledStackArgs[numOfStackArgs] = initializeJdcStr();
				if (!decompileOperand(params, &currentInstruction->operands[1], &decompiledStackArgs[numOfStackArgs]))
				{
					for (int j = 0; j < numOfStackArgs + 1; j++) { freeJdcStr(&stackArgTypeStrs[j]); }
					for (int j = 0; j < numOfStackArgs + 1; j++) { freeJdcStr(&decompiledStackArgs[j]); }
					for (int j = 0; j < NUM_PLATFORM_REG_ARGS; j++) { freeJdcStr(&regArgTypeStrs[j]); }
					for (int j = 0; j < NUM_PLATFORM_REG_ARGS; j++) { freeJdcStr(&decompiledRegArgs[j]); }
					return 0;
				}

				numOfStackArgs++;
			}
		}

		for(int j = 0; j < NUM_PLATFORM_REG_ARGS; j++)
		{
			if (!decompiledRegArgs[j].buffer)
			{
				unsigned char regOperandNum = 0;
				if(doesInstructionModifyRegister(currentInstruction, platformRegArgs[j], &regOperandNum, 0, 0))
				{
					regArgTypeStrs[j] = initializeJdcStr();
					varTypeToStr(getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[regOperandNum]), &regArgTypeStrs[j]);
					
					params->startInstructionIndex = i;
					decompiledRegArgs[j] = initializeJdcStr();
					if (!decompileOperand(params, &currentInstruction->operands[regOperandNum], &decompiledRegArgs[j]))
					{
						for (int j = 0; j < numOfStackArgs + 1; j++) { freeJdcStr(&stackArgTypeStrs[j]); }
						for (int j = 0; j < numOfStackArgs + 1; j++) { freeJdcStr(&decompiledStackArgs[j]); }
						for (int j = 0; j < NUM_PLATFORM_REG_ARGS; j++) { freeJdcStr(&regArgTypeStrs[j]); }
						for (int j = 0; j < NUM_PLATFORM_REG_ARGS; j++) { freeJdcStr(&decompiledRegArgs[j]); }
						return 0;
					}
				}
			}
		}
	}

	int importIndex = getImportIndexByAddress(params, calleeAddress);
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
			varTypeToStr(returnedVar->type, &returnTypeStr);
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

		params->startInstructionIndex = ogStartInstructionIndex;
		if(callInstruction->operands[0].type == REGISTER)
		{
			params->startInstructionIndex--;
		}

		struct JdcStr functionPointer = initializeJdcStr();
		if (!decompileOperand(params, &callInstruction->operands[0], &functionPointer))
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

	params->startInstructionIndex = ogStartInstructionIndex;
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

int getFunctionCallNumber(struct DecompilationParameters* params, unsigned long long calleeAddress)
{
	int ogStartInstructionIndex = params->startInstructionIndex;
	
	int result = 0;
	for (int i = params->currentFunc->firstInstructionIndex; i < ogStartInstructionIndex; i++)
	{
		struct DisassembledInstruction* instruction = &params->instructions[i];
		if (isOpcodeCall(instruction->opcode) || instruction->opcode == JMP_NEAR)
		{
			params->startInstructionIndex = i;
			if (resolveJmpChain(params) == calleeAddress)
			{
				result++;
			}
		}
	}

	params->startInstructionIndex = ogStartInstructionIndex;
	return result;
}
