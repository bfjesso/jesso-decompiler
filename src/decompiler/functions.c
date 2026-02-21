#include "functions.h"

unsigned char findNextFunction(struct DecompilationParameters params, unsigned long long nextSectionStartAddress, struct Function* result, int* instructionIndex)
{
	unsigned char initializedRegs[ST0 - RAX] = { 0 }; // index is (i - RAX)

	int stackFrameSize = 0;

	unsigned long long addressToJumpTo = 0;
	unsigned char canReturnNothing = 0;

	unsigned char foundFirstInstruction = 0;
	for (int i = params.startInstructionIndex; i < params.totalNumOfInstructions; i++)
	{
		(*instructionIndex)++;

		struct DisassembledInstruction* currentInstruction = &params.allInstructions[i];

		if (currentInstruction->address == 0x4011D6) 
		{
			int ttt = 0;
		}

		if (!foundFirstInstruction)
		{
			if (currentInstruction->opcode == INT3 || currentInstruction->opcode == NOP)
			{
				continue;
			}

			result->instructions = &params.allInstructions[i];

			foundFirstInstruction = 1;
		}

		result->numOfInstructions++;

		if (isOpcodeCall(currentInstruction->opcode))
		{
			initializedRegs[0] = 1; // AX

			if (result->addressOfFirstFuncCall == 0)
			{
				unsigned long long calleeAddress = resolveJmpChain(params, i);
				if (calleeAddress != result->instructions[0].address && calleeAddress != 0) // check for recursive function
				{
					result->addressOfFirstFuncCall = calleeAddress;
					result->indexOfFirstFuncCall = result->numOfInstructions - 1;
				}
			}
		}
		else if ((currentInstruction->opcode == PUSH && currentInstruction->operands[0].type == REGISTER) || currentInstruction->opcode == POP)
		{
			continue;
		}
		else if (currentInstruction->opcode == SUB && currentInstruction->operands[0].type == REGISTER && compareRegisters(currentInstruction->operands[0].reg, SP)) 
		{
			stackFrameSize += currentInstruction->operands[1].immediate.value;
		}

		// checking all operands for arguments
		unsigned char overwrites = 0;
		for (int j = 3; j >= 0; j--)
		{
			struct Operand* currentOperand = &currentInstruction->operands[j];

			if (currentOperand->type == REGISTER)
			{
				if (isRegisterPointer(currentOperand->reg)) { continue; }
				
				for(int k = RAX; k < ST0; k++)
				{
					if(k == RBP || k == RSP || k == RIP) { continue; }

					if (compareRegisters(currentOperand->reg, k))
					{
						if (j == 0 && doesInstructionModifyOperand(currentInstruction, 0, &overwrites) && overwrites)
						{
							initializedRegs[k - RAX] = 1;
						}
						else if (!initializedRegs[k - RAX])
						{
							struct RegisterVariable* newRegArgs = (struct RegisterVariable*)realloc(result->regArgs, sizeof(struct RegisterVariable) * (result->numOfRegArgs + 1));
							if (newRegArgs) 
							{
								result->regArgs = newRegArgs;
							}
							else 
							{
								return 0;
							}

							result->regArgs[result->numOfRegArgs].reg = currentOperand->reg;
							result->regArgs[result->numOfRegArgs].type = getTypeOfOperand(currentInstruction->opcode, currentOperand, params.is64Bit);
							result->regArgs[result->numOfRegArgs].name = initializeJdcStr();
							sprintfJdc(&(result->regArgs[result->numOfRegArgs].name), 0, "arg%s", registerStrs[currentOperand->reg]);
							result->numOfRegArgs++;

							result->callingConvention = __FASTCALL;
							initializedRegs[k - RAX] = 1;
						}

						break;
					}
				}
			}
			else if (currentOperand->type == MEM_ADDRESS) 
			{
				// maybe memoryAddress.regDisplacement should be checked too ?
				if (currentOperand->memoryAddress.reg == NO_REG || compareRegisters(currentOperand->memoryAddress.reg, IP)) { continue; }

				for (int k = RAX; k < ST0; k++)
				{
					if (k == RIP) { continue; }
					
					if (compareRegisters(currentOperand->memoryAddress.reg, k))
					{
						if (isOperandStackArg(currentOperand, stackFrameSize))
						{
							int stackOffset = currentOperand->memoryAddress.constDisplacement;
							if (k == RSP) 
							{ 
								stackOffset -= stackFrameSize; 
							
							}
							if (!getStackArgByOffset(result, stackOffset) && !getLocalVarByOffset(result, stackOffset))
							{
								doesInstructionModifyOperand(currentInstruction, j, &overwrites);
								if (!overwrites) 
								{
									struct StackVariable* newStackArgs = (struct StackVariable*)realloc(result->stackArgs, sizeof(struct StackVariable) * (result->numOfStackArgs + 1));
									if (newStackArgs)
									{
										result->stackArgs = newStackArgs;
									}
									else
									{
										return 0;
									}

									result->stackArgs[result->numOfStackArgs].stackOffset = stackOffset;
									result->stackArgs[result->numOfStackArgs].type = getTypeOfOperand(currentInstruction->opcode, currentOperand, params.is64Bit);
									result->stackArgs[result->numOfStackArgs].name = initializeJdcStr();
									sprintfJdc(&(result->stackArgs[result->numOfStackArgs].name), 0, "arg%X", stackOffset);
									result->numOfStackArgs++;
								}
								else // treating stack args that are overwritten before being accessed as local vars
								{
									struct StackVariable* newLocalVars = (struct StackVariable*)realloc(result->localVars, sizeof(struct StackVariable) * (result->numOfLocalVars + 1));
									if (newLocalVars)
									{
										result->localVars = newLocalVars;
									}
									else
									{
										return 0;
									}

									result->localVars[result->numOfLocalVars].stackOffset = stackOffset;
									result->localVars[result->numOfLocalVars].type = getTypeOfOperand(currentInstruction->opcode, currentOperand, params.is64Bit);
									result->localVars[result->numOfLocalVars].name = initializeJdcStr();
									sprintfJdc(&(result->localVars[result->numOfLocalVars].name), 0, "var%X", stackOffset);
									result->numOfLocalVars++;
								}
							}
						}
						else if (isOperandStackVar(currentOperand, stackFrameSize))
						{
							int stackOffset = currentOperand->memoryAddress.constDisplacement;
							if (k == RSP)
							{
								stackOffset -= stackFrameSize;

							}
							if (!getStackArgByOffset(result, stackOffset) && !getLocalVarByOffset(result, stackOffset))
							{
								struct StackVariable* newLocalVars = (struct StackVariable*)realloc(result->localVars, sizeof(struct StackVariable) * (result->numOfLocalVars + 1));
								if (newLocalVars)
								{
									result->localVars = newLocalVars;
								}
								else
								{
									return 0;
								}

								result->localVars[result->numOfLocalVars].stackOffset = stackOffset;
								result->localVars[result->numOfLocalVars].type = getTypeOfOperand(currentInstruction->opcode, currentOperand, params.is64Bit);
								result->localVars[result->numOfLocalVars].name = initializeJdcStr();

								if (stackOffset > 0)
								{
									sprintfJdc(&(result->localVars[result->numOfLocalVars].name), 0, "var%X", stackOffset);
								}
								else
								{
									sprintfJdc(&(result->localVars[result->numOfLocalVars].name), 0, "var%X", -stackOffset);
								}

								result->numOfLocalVars++;
							}
						}
						else if (!initializedRegs[k - RAX] && k != RBP && k != RSP)
						{
							struct RegisterVariable* newRegArgs = (struct RegisterVariable*)realloc(result->regArgs, sizeof(struct RegisterVariable) * (result->numOfRegArgs + 1));
							if (newRegArgs)
							{
								result->regArgs = newRegArgs;
							}
							else
							{
								return 0;
							}

							result->regArgs[result->numOfRegArgs].reg = currentOperand->memoryAddress.reg;
							result->regArgs[result->numOfRegArgs].type = getTypeOfOperand(currentInstruction->opcode, currentOperand, params.is64Bit);
							result->regArgs[result->numOfRegArgs].name = initializeJdcStr();
							sprintfJdc(&(result->regArgs[result->numOfRegArgs].name), 0, "arg%s", registerStrs[currentOperand->memoryAddress.reg]);
							result->numOfRegArgs++;

							result->callingConvention = __FASTCALL;
							initializedRegs[k - RAX] = 1;
						}

						break;
					}
				}
			}
		}

		if ((isOpcodeJcc(currentInstruction->opcode) || currentInstruction->opcode == JMP_SHORT) && currentInstruction->operands[0].immediate.value > 0)
		{
			unsigned long long jumpAddr = params.allInstructions[i].address + currentInstruction->operands[0].immediate.value;
			int instructionIndex = findInstructionByAddress(params.allInstructions, 0, params.totalNumOfInstructions - 1, jumpAddr);
			if (jumpAddr > addressToJumpTo && params.allInstructions[instructionIndex - 1].opcode != JMP_SHORT)
			{
				addressToJumpTo = jumpAddr;
			}
		}

		if (isOpcodeReturn(currentInstruction->opcode) && result->returnType == VOID_TYPE)
		{
			canReturnNothing = 1;
		}

		if (addressToJumpTo != 0 && params.allInstructions[i].address < addressToJumpTo)
		{
			continue;
		}
		else
		{
			addressToJumpTo = 0;
		}

		// check for return value
		if (!canReturnNothing) // if the function can return nothing, its return type must be void
		{
			unsigned char operandNum = 0;
			if (doesInstructionModifyRegister(currentInstruction, AX, &operandNum, 0))
			{
				result->returnType = getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[operandNum], params.is64Bit);
				result->addressOfReturnFunction = 0;
			}
			else if (isOpcodeCall(currentInstruction->opcode))
			{
				unsigned long long calleeAddress = resolveJmpChain(params, i);
				if (calleeAddress != result->instructions[0].address) // check for recursive function
				{
					result->addressOfReturnFunction = calleeAddress;
				}
			}
			else if (currentInstruction->opcode == FLD)
			{
				result->returnType = FLOAT_TYPE;
				result->addressOfReturnFunction = 0;
			}
		}
		
		if (isOpcodeReturn(currentInstruction->opcode))
		{
			if (result->callingConvention == __CDECL && currentInstruction->operands[0].type != NO_OPERAND)
			{
				result->callingConvention = __STDCALL;
			}
			else if (result->numOfStackArgs != 0 && result->numOfRegArgs == 1)
			{
				result->callingConvention = __THISCALL;
			}

			sortFunctionArguments(result);
			return 1;
		}
		else if (currentInstruction->opcode == JMP_NEAR || currentInstruction->opcode == JMP_FAR || currentInstruction->opcode == HLT || currentInstruction->opcode == INT3 || params.allInstructions[i + 1].address == nextSectionStartAddress)
		{
			sortFunctionArguments(result);
			return 1;
		}
	}

	return 0;
}

unsigned char fixAllFunctionReturnTypes(struct Function* functions, unsigned short numOfFunctions, unsigned char is64Bit) // resolves if a function's return type depends on another function
{
	for (int i = 0; i < numOfFunctions; i++)
	{
		if (functions[i].addressOfReturnFunction != 0)
		{
			int returnFunctionIndex = findFunctionByAddress(functions, 0, numOfFunctions - 1, functions[i].addressOfReturnFunction);

			while (returnFunctionIndex != -1 && functions[returnFunctionIndex].addressOfReturnFunction != 0)
			{
				returnFunctionIndex = findFunctionByAddress(functions, 0, numOfFunctions - 1, functions[returnFunctionIndex].addressOfReturnFunction);
			}

			if (returnFunctionIndex != -1)
			{
				if (functions[returnFunctionIndex].returnType != VOID_TYPE) 
				{
					functions[i].returnType = functions[returnFunctionIndex].returnType;
				}
			}
			else // probably an imported function
			{
				functions[i].returnType = is64Bit ? LONG_LONG_TYPE : INT_TYPE; // assume something is returned
			}
		}
	}

	return 1;
}

unsigned char fixAllFunctionArgs(struct Function* functions, unsigned short numOfFunctions) // checks for arguments that aren't used in the function but are just passed to another function call
{
	int numFixed = 0;

	for (int i = 0; i < numOfFunctions; i++) 
	{
		struct Function* currentFunc = &functions[i];
		if (currentFunc->addressOfFirstFuncCall != 0)
		{
			int functionIndex = findFunctionByAddress(functions, 0, numOfFunctions - 1, currentFunc->addressOfFirstFuncCall);
			if (functionIndex != -1 && functions[functionIndex].numOfRegArgs > 0)
			{
				struct Function* callee = &functions[functionIndex];

				int numOfRegArgsInit = 0;
				enum Register* initializedRegs = (enum Register*)malloc(sizeof(enum Register) * callee->numOfRegArgs);
				if (!initializedRegs) 
				{
					return 0;
				}

				for (int j = currentFunc->indexOfFirstFuncCall - 1; j >= 0; j--)
				{
					struct DisassembledInstruction* instruction = &currentFunc->instructions[j];

					if (isOpcodeJcc(instruction->opcode) || instruction->opcode == JMP_SHORT)
					{
						break;
					}

					for (int k = 0; k < callee->numOfRegArgs; k++)
					{
						int alreadyFound = 0;
						for (int l = 0; l < numOfRegArgsInit; l++) 
						{
							if (initializedRegs[l] == callee->regArgs[k].reg) 
							{
								alreadyFound = 1;
								break;
							}
						}
						if (alreadyFound) { continue; }
						
						int overwrites = 0;
						if (doesInstructionModifyRegister(instruction, callee->regArgs[k].reg, 0, &overwrites) && overwrites)
						{
							initializedRegs[numOfRegArgsInit] = callee->regArgs[k].reg;
							numOfRegArgsInit++;
							break;
						}
					}
				}

				if (numOfRegArgsInit != callee->numOfRegArgs) 
				{
					for (int k = 0; k < callee->numOfRegArgs; k++)
					{
						int isInitialized = 0;
						for (int l = 0; l < numOfRegArgsInit; l++)
						{
							if (initializedRegs[l] == callee->regArgs[k].reg)
							{
								isInitialized = 1;
								break;
							}
						}
						if (isInitialized) { continue; }

						struct RegisterVariable* newRegArgs = (struct RegisterVariable*)realloc(currentFunc->regArgs, sizeof(struct RegisterVariable) * (currentFunc->numOfRegArgs + 1));
						if (newRegArgs)
						{
							currentFunc->regArgs = newRegArgs;
						}
						else
						{
							return 0;
						}

						currentFunc->regArgs[currentFunc->numOfRegArgs].reg = callee->regArgs[k].reg;
						currentFunc->regArgs[currentFunc->numOfRegArgs].type = callee->regArgs[k].type;
						currentFunc->regArgs[currentFunc->numOfRegArgs].name = copyJdcStr(&callee->regArgs[k].name);
						currentFunc->numOfRegArgs++;
					}

					numFixed++;
				}

				free(initializedRegs);
				currentFunc->addressOfFirstFuncCall = 0;
			}
		}
	}

	if (numFixed != 0) 
	{
		return fixAllFunctionArgs(functions, numOfFunctions);
	}

	return 1;
}

void freeFunction(struct Function* function) 
{
	freeJdcStr(&function->name);

	for (int i = 0; i < function->numOfRegArgs; i++)
	{
		freeJdcStr(&function->regArgs[i].name);
	}
	for (int i = 0; i < function->numOfStackArgs; i++)
	{
		freeJdcStr(&function->stackArgs[i].name);
	}
	for (int i = 0; i < function->numOfLocalVars; i++)
	{
		freeJdcStr(&function->localVars[i].name);
	}
	for (int i = 0; i < function->numOfReturnedVars; i++)
	{
		freeJdcStr(&function->returnedVars[i].name);
	}

	free(function->regArgs);
	free(function->stackArgs);
	free(function->localVars);
	free(function->returnedVars);
}

// returns index of function, -1 if not found
int findFunctionByAddress(struct Function* functions, int low, int high, unsigned long long address)
{
	while (low <= high)
	{
		int mid = low + (high - low) / 2;

		if (functions[mid].instructions[0].address == address) { return mid; }

		if (functions[mid].instructions[0].address < address) { low = mid + 1; }
		else { high = mid - 1; }
	}

	return -1;
}

// returns address of final instruction jumped to that isnt a jmp
unsigned long long resolveJmpChain(struct DecompilationParameters params, int startInstructionIndex)
{
	struct DisassembledInstruction* instruction = &params.allInstructions[startInstructionIndex];

	unsigned long long jmpAddress = params.allInstructions[startInstructionIndex].address + instruction->operands[0].immediate.value;
	if (instruction->operands[0].type == MEM_ADDRESS)
	{
		jmpAddress = instruction->operands[0].memoryAddress.constDisplacement;
		if (compareRegisters(instruction->operands[0].memoryAddress.reg, IP))
		{
			jmpAddress += params.allInstructions[startInstructionIndex + 1].address;
		}
	}
	else if (instruction->operands[0].type == REGISTER)
	{
		if (!operandToValue(params, &instruction->operands[0], &jmpAddress))
		{
			return 0;
		}
	}

	int instructionIndex = findInstructionByAddress(params.allInstructions, 0, params.totalNumOfInstructions - 1, jmpAddress);
	if (instructionIndex != -1)
	{
		struct DisassembledInstruction* jmpInstruction = &(params.allInstructions[instructionIndex]);
		if (instructionIndex != startInstructionIndex && (jmpInstruction->opcode == JMP_FAR || jmpInstruction->opcode == JMP_NEAR))
		{
			return resolveJmpChain(params, instructionIndex);
		}
	}

	return jmpAddress;
}

// returns index of instruction, -1 if not found
int findInstructionByAddress(struct DisassembledInstruction* instructions, int low, int high, unsigned long long address)
{
	while (low <= high)
	{
		int mid = low + (high - low) / 2;

		if (instructions[mid].address == address) { return mid; }

		if (instructions[mid].address < address) { low = mid + 1; }
		else { high = mid - 1; }
	}

	return -1;
}

enum PrimitiveType getTypeOfOperand(enum Mnemonic opcode, struct Operand* operand, unsigned char is64Bit)
{
	switch (opcode)
	{
	case MOVSS:
	case ADDSS:
	case CVTPS2PD:
	case CVTSS2SD:
	case COMISS:
		return FLOAT_TYPE;
	case MOVSD:
	case ADDSD:
	case CVTPD2PS:
	case CVTSD2SS:
	case COMISD:
		return DOUBLE_TYPE;
	}

	if (!operand) 
	{ 
		return VOID_TYPE; 
	}
	else if (operand->type == IMMEDIATE) 
	{
		return is64Bit ? LONG_LONG_TYPE : INT_TYPE;
	}

	unsigned char size = 0;
	if (operand->type == MEM_ADDRESS) 
	{
		size = operand->memoryAddress.ptrSize;
	}
	else if (operand->type == REGISTER) 
	{
		size = getSizeOfRegister(operand->reg);
	}

	switch (size)
	{
	case 1:
		return CHAR_TYPE;
	case 2:
		return SHORT_TYPE;
	case 4:
		return INT_TYPE;
	case 8:
		return LONG_LONG_TYPE;
	case 16:
		return INT_128_TPYE;
	case 32:
		return INT_256_TPYE;
	case 64:
		return INT_512_TPYE;
	}
	
	return VOID_TYPE;
}

static unsigned char operandToValue(struct DecompilationParameters params, struct Operand* operand, unsigned long long* result)
{
	if (operand->type == IMMEDIATE)
	{
		*result = operand->immediate.value;
		return 1;
	}
	else if (operand->type == MEM_ADDRESS)
	{
		if (compareRegisters(operand->memoryAddress.reg, IP))
		{
			unsigned long long address = params.allInstructions[params.startInstructionIndex + 1].address + operand->memoryAddress.constDisplacement;
			if (!getNumFromData(params, address, result)) 
			{
				*result = address;
			}
			return 1;
		}
		else if (operand->memoryAddress.reg == NO_REG)
		{
			unsigned long long address = operand->memoryAddress.constDisplacement;
			if (!getNumFromData(params, address, result))
			{
				*result = address;
			}
			return 1;
		}
		else
		{
			struct Operand baseReg = { 0 };
			baseReg.type = REGISTER;
			baseReg.reg = operand->memoryAddress.reg;

			unsigned long long regValue = 0;
			if (!operandToValue(params, &baseReg, &regValue))
			{
				return 0;
			}

			unsigned long long address = regValue + operand->memoryAddress.constDisplacement;
			if (!getNumFromData(params, address, result))
			{
				*result = address;
			}
		}

		return 1;
	}
	else if (operand->type == REGISTER)
	{
		for (int i = params.startInstructionIndex - 1; i >= 0; i--)
		{
			if (params.allInstructions[i].opcode == MOV && compareRegisters(params.allInstructions[i].operands[0].reg, operand->reg))
			{
				params.startInstructionIndex = i;
				return operandToValue(params, &(params.allInstructions[i].operands[1]), result);
			}
		}

		return 0;
	}

	return 0;
}

static unsigned char getNumFromData(struct DecompilationParameters params, unsigned long long address, unsigned long long* result)
{
	if (address < params.imageBase + params.dataSections[0].virtualAddress)
	{
		return 0;
	}

	int dataSectionIndex = -1;
	int totalSize = 0;
	for (int i = 0; i < params.numOfDataSections; i++)
	{
		if (address > params.imageBase + params.dataSections[i].virtualAddress && address < params.imageBase + params.dataSections[i].virtualAddress + params.dataSections[i].size)
		{
			dataSectionIndex = (int)((totalSize + address) - (params.dataSections[i].virtualAddress + params.imageBase));
		}

		totalSize += params.dataSections[i].size;
	}

	if (dataSectionIndex == -1 || dataSectionIndex >= totalSize)
	{
		return 0;
	}

	if (params.is64Bit)
	{
		*result = *(unsigned long long*)(params.dataSectionByte + dataSectionIndex);
	}
	else
	{
		*result = *(unsigned int*)(params.dataSectionByte + dataSectionIndex);
	}

	return 1;
}

unsigned char isOperandStackVar(struct Operand* operand, int stackFrameSize)
{
	return operand->type == MEM_ADDRESS && ((operand->memoryAddress.constDisplacement < 0 && compareRegisters(operand->memoryAddress.reg, BP)) || ((operand->memoryAddress.constDisplacement < stackFrameSize || stackFrameSize == 0) && compareRegisters(operand->memoryAddress.reg, SP)));
}

unsigned char isOperandStackArg(struct Operand* operand, int stackFrameSize)
{
	return operand->type == MEM_ADDRESS && ((compareRegisters(BP, operand->memoryAddress.reg) && operand->memoryAddress.constDisplacement > 0) || (compareRegisters(SP, operand->memoryAddress.reg) && stackFrameSize != 0 && operand->memoryAddress.constDisplacement > stackFrameSize));
}

struct StackVariable* getLocalVarByOffset(struct Function* function, int stackOffset)
{
	for (int i = 0; i < function->numOfLocalVars; i++)
	{
		if (function->localVars[i].stackOffset == stackOffset)
		{
			return &function->localVars[i];
		}
	}

	return 0;
}

struct StackVariable* getStackArgByOffset(struct Function* function, int stackOffset)
{
	for (int i = 0; i < function->numOfStackArgs; i++) 
	{
		if (function->stackArgs[i].stackOffset == stackOffset) 
		{
			return &function->stackArgs[i];
		}
	}

	return 0;
}

struct RegisterVariable* getRegArgByReg(struct Function* function, enum Register reg)
{
	for (int i = 0; i < function->numOfRegArgs; i++)
	{
		if (compareRegisters(function->regArgs[i].reg, reg))
		{
			return &function->regArgs[i];
		}
	}

	return 0;
}

struct FuncReturnVariable* findReturnedVar(struct Function* function, char callNum, unsigned long long callAddr)
{
	for (int i = 0; i < function->numOfReturnedVars; i++)
	{
		if (function->returnedVars[i].callAddr == callAddr && function->returnedVars[i].callNum == callNum)
		{
			return &function->returnedVars[i];
		}
	}

	return 0;
}

static void sortFunctionArguments(struct Function* function) 
{
	// order for fastcall should be RCX, RDX, R8, R9
	for (int i = 0; i < function->numOfRegArgs; i++)
	{
		if (compareRegisters(function->regArgs[i].reg, RCX)) 
		{
			struct RegisterVariable temp = function->regArgs[0];
			function->regArgs[0] = function->regArgs[i];
			function->regArgs[i] = temp;
		}
		else if (compareRegisters(function->regArgs[i].reg, RDX) && function->numOfRegArgs > 1)
		{
			struct RegisterVariable temp = function->regArgs[1];
			function->regArgs[1] = function->regArgs[i];
			function->regArgs[i] = temp;
		}
		else if (compareRegisters(function->regArgs[i].reg, R8) && function->numOfRegArgs > 2)
		{
			struct RegisterVariable temp = function->regArgs[2];
			function->regArgs[2] = function->regArgs[i];
			function->regArgs[i] = temp;
		}
		else if (compareRegisters(function->regArgs[i].reg, R9) && function->numOfRegArgs > 3)
		{
			struct RegisterVariable temp = function->regArgs[3];
			function->regArgs[3] = function->regArgs[i];
			function->regArgs[i] = temp;
		}
	}

	// order should be from least to greatest stack offset
	for (int i = 0; i < function->numOfStackArgs - 1; i++)
	{
		char swapped = 0;
		for (int j = 0; j < function->numOfStackArgs - i - 1; j++)
		{
			if (function->stackArgs[j].stackOffset > function->stackArgs[j + 1].stackOffset)
			{
				struct StackVariable temp = function->stackArgs[j];
				function->stackArgs[j] = function->stackArgs[j + 1];
				function->stackArgs[j + 1] = temp;

				swapped = 1;
			}
		}
		if (!swapped) { break; }
	}
}
