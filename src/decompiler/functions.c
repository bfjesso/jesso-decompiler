#include "functions.h"
#include "../disassembler/opcodes.h"
#include "../disassembler/registers.h"

unsigned char findNextFunction(struct DisassembledInstruction* instructions, unsigned long long* addresses, unsigned short numOfInstructions, struct Function* result, int* instructionIndex)
{
	struct Function function = { 0 };

	unsigned char initializedRegs[11]; // index is i - RAX
	for(int i = 0; i < 11; i++)
	{
		initializedRegs[i] = 0;
	}

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
			
			function.addresses = &addresses[i];
			function.instructions = &instructions[i];
			function.numOfInstructions = 0;

			function.returnType = VOID_TYPE;

			function.addressOfReturnFunction = 0;

			function.callingConvention = __CDECL;

			function.numOfRegArgs = 0;
			function.numOfStackArgs = 0;
			function.numOfLocalVars = 0;

			foundFirstInstruction = 1;
		}

		function.numOfInstructions++;

		if (currentInstruction->opcode == PUSH || currentInstruction->opcode == POP) { continue; }

		// check for arguments
		unsigned char overwrites = 0;
		for (int j = 0; j < 3; j++)
		{
			struct Operand* currentOperand = &currentInstruction->operands[j];

			if (currentOperand->type == REGISTER)
			{
				for(int k = RAX; k < R10; k++)
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
							function.regArgRegs[function.numOfRegArgs] = k;
							function.regArgTypes[function.numOfRegArgs] = getTypeOfOperand(currentInstruction->opcode, currentOperand);
							function.numOfRegArgs++;
							function.callingConvention = __FASTCALL;
						}
					}
				}
			}
			else if (isOperandStackArgument(currentOperand))
			{
				unsigned char alreadyFound = 0;
				for (int j = 0; j < function.numOfStackArgs; j++)
				{
					if (function.stackArgBpOffsets[j] == currentOperand->memoryAddress.constDisplacement)
					{
						alreadyFound = 1;
						break;
					}
				}

				if (!alreadyFound)
				{
					function.stackArgBpOffsets[function.numOfStackArgs] = currentOperand->memoryAddress.constDisplacement;
					function.stackArgTypes[function.numOfStackArgs] = getTypeOfOperand(currentInstruction->opcode, currentOperand);
					function.numOfStackArgs++;
				}

				// sort
				for (int j = 0; j < function.numOfStackArgs - 1; j++)
				{
					char swapped = 0;
					for (int k = 0; k < function.numOfStackArgs - j - 1; k++)
					{
						if (function.stackArgBpOffsets[k] > function.stackArgBpOffsets[k + 1])
						{
							unsigned char temp = function.stackArgBpOffsets[k];
							function.stackArgBpOffsets[k] = function.stackArgBpOffsets[k + 1];
							function.stackArgBpOffsets[k + 1] = temp;

							swapped = 1;
						}
					}
					if (!swapped) { break; }
				}
			}
		}

		// check for local vars
		if (isOperandLocalVariable(&currentInstruction->operands[0]))
		{
			int displacement = currentInstruction->operands[0].memoryAddress.constDisplacement;

			unsigned char overwritesVarValue = 0;
			if (doesInstructionModifyOperand(currentInstruction, 0, &overwritesVarValue) && overwritesVarValue)
			{
				unsigned char isAlreadyFound = 0;
				for (int j = 0; j < function.numOfLocalVars; j++)
				{
					if (function.localVars[j].stackOffset == displacement)
					{
						isAlreadyFound = 1;
						break;
					}
				}

				if (!isAlreadyFound)
				{
					function.localVars[function.numOfLocalVars].stackOffset = displacement;
					function.localVars[function.numOfLocalVars].type = getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[0]);
					function.numOfLocalVars++;
				}
			}
		}

		// check for return value
		overwrites = 0;
		if ((doesOpcodeModifyRegister(currentInstruction->opcode, AX, &overwrites) || (currentInstruction->operands[0].type == REGISTER && compareRegisters(currentInstruction->operands[0].reg, AX) && doesInstructionModifyOperand(currentInstruction, 0, &overwrites))) && overwrites)
		{
			struct Operand* operand = &currentInstruction->operands[getLastOperand(currentInstruction)];
			if (operand->type == IMMEDIATE) { operand = &currentInstruction->operands[0]; }

			function.returnType = getTypeOfOperand(currentInstruction->opcode, operand);
			function.addressOfReturnFunction = 0;
		}
		else if (currentInstruction->opcode == CALL_NEAR)
		{
			function.addressOfReturnFunction = addresses[i] + currentInstruction->operands[0].immediate;
		}
		else if (currentInstruction->opcode == FLD)
		{
			function.returnType = FLOAT_TYPE;
			function.addressOfReturnFunction = 0;
		}

		if (addressToJumpTo != 0 && addresses[i] < addressToJumpTo)
		{
			continue;
		}
		else
		{
			addressToJumpTo = 0;
		}

		if (currentInstruction->opcode >= JA_SHORT && currentInstruction->opcode <= JMP_SHORT)
		{
			addressToJumpTo = addresses[i] + currentInstruction->operands[0].immediate;
		}
		else if (currentInstruction->opcode == RET_NEAR || currentInstruction->opcode == RET_FAR)
		{
			if (function.callingConvention == __CDECL && currentInstruction->operands[0].type != NO_OPERAND)
			{
				function.callingConvention = __STDCALL;
			}
			else if (function.numOfStackArgs != 0 && function.numOfRegArgs == 1)
			{
				function.callingConvention = __THISCALL;
			}

			*result = function;
			return 1;
		}
		else if (currentInstruction->opcode == JMP_NEAR || currentInstruction->opcode == JMP_FAR || currentInstruction->opcode == INT3)
		{
			*result = function;
			return 1;
		}
	}

	return 0;
}

unsigned char fixAllFunctionReturnTypes(struct Function* functions, unsigned short numOfFunctions) // if a function's return type was that of another function, it must be resolved
{
	for (int i = 0; i < numOfFunctions; i++)
	{
		struct Function* currentFunc = &functions[i];
		if (functions[i].addressOfReturnFunction != 0)
		{
			int returnFunctionIndex = findFunctionByAddress(functions, 0, numOfFunctions - 1, functions[i].addressOfReturnFunction);

			while (returnFunctionIndex != -1 && functions[returnFunctionIndex].addressOfReturnFunction != 0)
			{
				returnFunctionIndex = findFunctionByAddress(functions, 0, numOfFunctions - 1, functions[returnFunctionIndex].addressOfReturnFunction);
			}

			if (returnFunctionIndex != -1 && functions[returnFunctionIndex].returnType != VOID_TYPE)
			{
				functions[i].returnType = functions[returnFunctionIndex].returnType;
			}
		}
	}

	return 1;
}

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

unsigned char getTypeOfOperand(unsigned char opcode, struct Operand* operand)
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

struct LocalVariable* getLocalVarByOffset(struct Function* function, int stackOffset) 
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
