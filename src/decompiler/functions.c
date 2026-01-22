#include "functions.h"
#include "../disassembler/opcodes.h"

#include <stdio.h>

unsigned char findNextFunction(struct DisassembledInstruction* instructions, int startInstructionIndex, int numOfInstructions, unsigned long long nextSectionStartAddress, struct Function* result, int* instructionIndex, unsigned char is64Bit)
{
	unsigned char initializedRegs[ST0 - RAX] = { 0 }; // index is (i - RAX)

	unsigned long long addressToJumpTo = 0;
	unsigned char canReturnNothing = 0;

	unsigned char foundFirstInstruction = 0;
	for (int i = startInstructionIndex; i < numOfInstructions; i++)
	{
		(*instructionIndex)++;

		struct DisassembledInstruction* currentInstruction = &instructions[i];

		if (!foundFirstInstruction)
		{
			if (currentInstruction->opcode == INT3 || currentInstruction->opcode == NOP)
			{
				continue;
			}

			result->instructions = &instructions[i];

			foundFirstInstruction = 1;
		}

		result->numOfInstructions++;

		if (isOpcodeCall(currentInstruction->opcode))
		{
			initializedRegs[0] = 1; // AX
		}
		else if ((currentInstruction->opcode == PUSH && currentInstruction->operands[0].type == REGISTER) || currentInstruction->opcode == POP)
		{
			continue;
		}

		// checking all operands for arguments
		unsigned char overwrites = 0;
		for (int j = 0; j < 4; j++)
		{
			struct Operand* currentOperand = &currentInstruction->operands[j];

			if (currentOperand->type == REGISTER)
			{
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
							result->regArgs[result->numOfRegArgs].reg = currentOperand->reg;
							result->regArgs[result->numOfRegArgs].type = getTypeOfOperand(currentInstruction->opcode, currentOperand, is64Bit);
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
			else if (isOperandStackArgument(currentOperand))
			{
				unsigned char alreadyFound = 0;
				for (int k = 0; k < result->numOfStackArgs; k++)
				{
					if (result->stackArgs[k].stackOffset == currentOperand->memoryAddress.constDisplacement)
					{
						alreadyFound = 1;
						break;
					}
				}

				if (!alreadyFound)
				{
					result->stackArgs[result->numOfStackArgs].stackOffset = (int)(currentOperand->memoryAddress.constDisplacement);
					result->stackArgs[result->numOfStackArgs].type = getTypeOfOperand(currentInstruction->opcode, currentOperand, is64Bit);
					result->stackArgs[result->numOfStackArgs].name = initializeJdcStr();
					sprintfJdc(&(result->stackArgs[result->numOfStackArgs].name), 0, "arg%X", result->stackArgs[result->numOfStackArgs].stackOffset);

					result->numOfStackArgs++;
				}
			}
		}

		// check for return value
		overwrites = 0;
		if (!canReturnNothing) // if the function can return nothing, its return type must be void
		{
			unsigned char operandNum = 0;
			if (doesInstructionModifyRegister(currentInstruction, AX, &operandNum, &overwrites) && overwrites)
			{
				struct Operand* operand = &currentInstruction->operands[getLastOperand(currentInstruction)];
				if (operand->type == IMMEDIATE) { operand = &currentInstruction->operands[operandNum]; }

				result->returnType = getTypeOfOperand(currentInstruction->opcode, operand, is64Bit);
				result->addressOfReturnFunction = 0;
			}
			else if (isOpcodeCall(currentInstruction->opcode))
			{
				unsigned long long calleeAddress = resolveJmpChain(instructions, numOfInstructions, i);
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
			else if (isOpcodeReturn(currentInstruction->opcode) && result->returnType == VOID_TYPE)
			{
				canReturnNothing = 1;
			}
		}

		if ((isOpcodeJcc(currentInstruction->opcode) || currentInstruction->opcode == JMP_SHORT) && currentInstruction->operands[0].immediate > 0)
		{
			unsigned long long jumpAddr = instructions[i].address + currentInstruction->operands[0].immediate;
			if (jumpAddr > addressToJumpTo)
			{
				addressToJumpTo = jumpAddr;
			}
		}

		if (addressToJumpTo != 0 && instructions[i].address < addressToJumpTo)
		{
			continue;
		}
		else
		{
			addressToJumpTo = 0;
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
		else if (currentInstruction->opcode == JMP_NEAR || currentInstruction->opcode == JMP_FAR || currentInstruction->opcode == HLT || currentInstruction->opcode == INT3 || instructions[i + 1].address == nextSectionStartAddress)
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
unsigned long long resolveJmpChain(struct DisassembledInstruction* instructions, int numOfInstructions, int startInstructionIndex)
{
	struct DisassembledInstruction* instruction = &instructions[startInstructionIndex];

	unsigned long long jmpAddress = instructions[startInstructionIndex].address + instruction->operands[0].immediate;
	if (instruction->operands[0].type == MEM_ADDRESS)
	{
		jmpAddress = instruction->operands[0].memoryAddress.constDisplacement;
		if (compareRegisters(instruction->operands[0].memoryAddress.reg, IP))
		{
			jmpAddress += instructions[startInstructionIndex + 1].address;
		}
	}
	else if (instruction->operands[0].type == REGISTER)
	{
		if (!operandToValue(instructions, startInstructionIndex, &instruction->operands[0], &jmpAddress))
		{
			return 0;
		}
	}

	int instructionIndex = findInstructionByAddress(instructions, 0, numOfInstructions - 1, jmpAddress);
	if (instructionIndex != -1)
	{
		struct DisassembledInstruction* jmpInstruction = &(instructions[instructionIndex]);
		if (instructionIndex != startInstructionIndex && (jmpInstruction->opcode == JMP_FAR || jmpInstruction->opcode == JMP_NEAR))
		{
			return resolveJmpChain(instructions, numOfInstructions, instructionIndex);
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
		return FLOAT_TYPE;
	case 32:
		return DOUBLE_TYPE;
	}
	
	return VOID_TYPE;
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
