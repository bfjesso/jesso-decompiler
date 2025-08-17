#include "functions.h"
#include "dataTypes.h"
#include "../disassembler/opcodes.h"

#include <stdio.h>

unsigned char findNextFunction(struct DisassembledInstruction* instructions, unsigned long long* addresses, unsigned short numOfInstructions, struct Function* result, int* instructionIndex)
{
	unsigned char initializedRegs[NO_REG - RAX] = { 0 }; // index is (i - RAX)

	unsigned long long addressToJumpTo = 0;

	unsigned char foundFirstInstruction = 0;
	for (int i = 0; i < numOfInstructions; i++)
	{
		(*instructionIndex)++;

		struct DisassembledInstruction* currentInstruction = &instructions[i];

		if (!foundFirstInstruction)
		{
			if (currentInstruction->opcode == INT3)
			{
				continue;
			}

			result->addresses = &addresses[i];
			result->instructions = &instructions[i];

			foundFirstInstruction = 1;
		}

		result->numOfInstructions++;

		if (currentInstruction->opcode == CALL_NEAR)
		{
			initializedRegs[0] = 1; // AX
		}
		else if (currentInstruction->opcode == POP) 
		{
			continue;
		}

		// checking all operands for arguments or local variables
		unsigned char overwrites = 0;
		for (int j = 0; j < 3; j++)
		{
			struct Operand* currentOperand = &currentInstruction->operands[j];

			if (currentOperand->type == REGISTER)
			{
				for(int k = RAX; k < NO_REG; k++)
				{
					if(k == RBP || k == RSP) { continue; }

					if (compareRegisters(currentOperand->reg, k))
					{
						if (j == 0 && doesInstructionModifyOperand(currentInstruction, 0, &overwrites) && overwrites)
						{
							initializedRegs[k - RAX] = 1;
						}
						else if (!initializedRegs[k - RAX])
						{
							result->regArgs[result->numOfRegArgs].reg = k;
							result->regArgs[result->numOfRegArgs].type = getTypeOfOperand(currentInstruction->opcode, currentOperand);
							result->numOfRegArgs++;
							result->callingConvention = __FASTCALL;
						}
					}
				}
			}
			else if (isOperandStackArgument(currentOperand))
			{
				unsigned char alreadyFound = 0;
				for (int j = 0; j < result->numOfStackArgs; j++)
				{
					if (result->stackArgs[j].stackOffset == currentOperand->memoryAddress.constDisplacement)
					{
						alreadyFound = 1;
						break;
					}
				}

				if (!alreadyFound)
				{
					result->stackArgs[result->numOfStackArgs].stackOffset = (unsigned char)(currentOperand->memoryAddress.constDisplacement);
					result->stackArgs[result->numOfStackArgs].type = getTypeOfOperand(currentInstruction->opcode, currentOperand);
					result->numOfStackArgs++;
				}
			}
			else if (isOperandLocalVariable(currentOperand))
			{
				long long displacement = currentOperand->memoryAddress.constDisplacement;

				unsigned char overwritesVarValue = 0;
				if (doesInstructionModifyOperand(currentInstruction, 0, &overwritesVarValue) && overwritesVarValue)
				{
					unsigned char isAlreadyFound = 0;
					for (int j = 0; j < result->numOfLocalVars; j++)
					{
						if (result->localVars[j].stackOffset == displacement)
						{
							isAlreadyFound = 1;
							break;
						}
					}

					if (!isAlreadyFound)
					{
						result->localVars[result->numOfLocalVars].stackOffset = (int)displacement;
						result->localVars[result->numOfLocalVars].type = getTypeOfOperand(currentInstruction->opcode, currentOperand);
						result->numOfLocalVars++;
					}
				}
			}
		}

		// check for return value
		overwrites = 0;
		if ((doesOpcodeModifyRegister(currentInstruction->opcode, AX, &overwrites) || (currentInstruction->operands[0].type == REGISTER && compareRegisters(currentInstruction->operands[0].reg, AX) && doesInstructionModifyOperand(currentInstruction, 0, &overwrites))) && overwrites)
		{
			struct Operand* operand = &currentInstruction->operands[getLastOperand(currentInstruction)];
			if (operand->type == IMMEDIATE) { operand = &currentInstruction->operands[0]; }

			result->returnType = getTypeOfOperand(currentInstruction->opcode, operand);
			result->addressOfReturnFunction = 0;
		}
		else if (currentInstruction->opcode == CALL_NEAR)
		{
			unsigned long long calleeAddress = addresses[i] + currentInstruction->operands[0].immediate;
			if (calleeAddress != addresses[0]) // check for recursive function
			{
				result->addressOfReturnFunction = calleeAddress;
			}
		}
		else if (currentInstruction->opcode == FLD)
		{
			result->returnType = FLOAT_TYPE;
			result->addressOfReturnFunction = 0;
		}

		if (addressToJumpTo != 0 && addresses[i] < addressToJumpTo)
		{
			continue;
		}
		else
		{
			addressToJumpTo = 0;
		}

		if (currentInstruction->opcode >= JA_SHORT && currentInstruction->opcode <= JMP_SHORT && currentInstruction->operands[0].immediate > 0)
		{
			addressToJumpTo = addresses[i] + currentInstruction->operands[0].immediate;
		}
		else if (currentInstruction->opcode == RET_NEAR || currentInstruction->opcode == RET_FAR)
		{
			if (result->callingConvention == __CDECL && currentInstruction->operands[0].type != NO_OPERAND)
			{
				result->callingConvention = __STDCALL;
			}
			else if (result->numOfStackArgs != 0 && result->numOfRegArgs == 1)
			{
				result->callingConvention = __THISCALL;
			}

			initializeFunctionVarNames(result);
			sortFunctionArguments(result);
			return 1;
		}
		else if (currentInstruction->opcode == JMP_NEAR || currentInstruction->opcode == JMP_FAR || currentInstruction->opcode == INT3)
		{
			initializeFunctionVarNames(result);
			sortFunctionArguments(result);
			return 1;
		}
	}

	return 0;
}

unsigned char fixAllFunctionReturnTypes(struct Function* functions, unsigned short numOfFunctions) // if a function's return type was that of another function, it must be resolved
{
	for (int i = 0; i < numOfFunctions; i++)
	{
		if (functions[i].addressOfReturnFunction != 0)
		{
			int returnFunctionIndex = findFunctionByAddress(functions, 0, numOfFunctions - 1, functions[i].addressOfReturnFunction);

			struct Function* f = 0;
			while (returnFunctionIndex != -1 && functions[returnFunctionIndex].addressOfReturnFunction != 0)
			{
				returnFunctionIndex = findFunctionByAddress(functions, 0, numOfFunctions - 1, functions[returnFunctionIndex].addressOfReturnFunction);
				f = &functions[returnFunctionIndex];
			}

			if (returnFunctionIndex != -1 && functions[returnFunctionIndex].returnType != VOID_TYPE)
			{
				functions[i].returnType = functions[returnFunctionIndex].returnType;
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

		if (*functions[mid].addresses == address) { return mid; }

		if (*functions[mid].addresses < address) { low = mid + 1; }
		else { high = mid - 1; }
	}

	return -1;
}

// returns index of instruction, -1 if not found
int findInstructionByAddress(unsigned long long* addresses, int low, int high, unsigned long long address) 
{
	while (low <= high)
	{
		int mid = low + (high - low) / 2;

		if (addresses[mid] == address) { return mid; }

		if (addresses[mid] < address) { low = mid + 1; }
		else { high = mid - 1; }
	}

	return -1;
}

unsigned char getTypeOfOperand(enum Mnemonic opcode, struct Operand* operand)
{
	switch (opcode)
	{
	case MOVSS:
	case CVTPS2PD:
	case CVTSS2SD:
		return FLOAT_TYPE;
	case MOVSD:
	case CVTPD2PS:
	case CVTSD2SS:
		return DOUBLE_TYPE;
	}

	if (operand == 0) { return VOID_TYPE; }

	if (operand->type == IMMEDIATE) 
	{
		return INT_TYPE;
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

static void initializeFunctionVarNames(struct Function* function) 
{
	for (int i = 0; i < function->numOfRegArgs; i++) 
	{
		if (function->regArgs[i].reg >= RAX && function->regArgs[i].reg <= RIP) 
		{
			sprintf(function->regArgs[i].name, "arg%s", registerStrs[function->regArgs[i].reg - 18]); // make it DX rather than RDX for example
		}
		else 
		{
			sprintf(function->regArgs[i].name, "arg%s", registerStrs[function->regArgs[i].reg]);
		}
	}

	for (int i = 0; i < function->numOfStackArgs; i++)
	{
		sprintf(function->stackArgs[i].name, "arg%X", function->stackArgs[i].stackOffset);
	}

	for (int i = 0; i < function->numOfLocalVars; i++)
	{
		sprintf(function->localVars[i].name, "var%X", -function->localVars[i].stackOffset);
	}
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