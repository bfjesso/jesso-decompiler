#include "../Headers/functions.h"
#include "../../Disassembler/Headers/opcodes.h"
#include "../../Disassembler/Headers/registers.h"

unsigned char findNextFunction(struct DisassembledInstruction* instructions, unsigned long long* addresses, unsigned short numOfInstructions, struct Function* result, int* instructionIndex)
{
	struct Function function = { 0 };

	unsigned char initializedCX = 0;
	unsigned char initializedDX = 0;
	unsigned char initializedR8 = 0;
	unsigned char initializedR9 = 0;

	unsigned long long addressToJumpTo = 0;

	unsigned char foundFirstInstruction = 0;
	for (int i = 0; i < numOfInstructions; i++)
	{
		(*instructionIndex)++;

		if (addressToJumpTo != 0 && addressToJumpTo != addresses[i])
		{
			function.numOfInstructions++;
			continue;
		}
		else
		{
			addressToJumpTo = 0;
		}

		struct DisassembledInstruction* currentInstruction = &instructions[i];

		if (currentInstruction->opcode == INT3)
		{
			if (foundFirstInstruction)
			{
				*result = function;
				return 1;
			}
			else
			{
				continue;
			}
		}

		if (!foundFirstInstruction)
		{
			function.addresses = &addresses[i];
			function.instructions = &instructions[i];
			function.numOfInstructions = 1;

			function.returnType.primitiveType = VOID_TYPE;
			function.returnType.isPointer = 0;
			function.returnType.isSigned = 0;

			function.addressOfReturnFunction = 0;

			function.callingConvention = __CDECL;

			function.numOfRegArgs = 0;
			function.numOfStackArgs = 0;

			foundFirstInstruction = 1;
		}
		else
		{
			function.numOfInstructions++;

			if (currentInstruction->opcode == PUSH || currentInstruction->opcode == POP) { continue; }

			// check for arguments
			for (int i = 0; i < 3; i++)
			{
				struct Operand* currentOperand = &currentInstruction->operands[i];

				if (currentOperand->type == REGISTER)
				{
					switch (currentOperand->reg)
					{
					case CX:
					case ECX:
					case RCX:
						if (currentInstruction->opcode == MOV && i == 0) { initializedCX = 1; }
						else if (!initializedCX)
						{
							function.regArgs[0].primitiveType = INT_TYPE;
							function.regArgs[0].isPointer = 0;
							function.regArgs[0].isSigned = 1;
							function.numOfRegArgs++;
							function.callingConvention = __FASTCALL;
						}
						break;
					case DX:
					case EDX:
					case RDX:
						if (currentInstruction->opcode == MOV && i == 0) { initializedDX = 1; }
						else if (!initializedDX)
						{
							function.regArgs[1].primitiveType = INT_TYPE;
							function.regArgs[1].isPointer = 0;
							function.regArgs[1].isSigned = 1;
							function.numOfRegArgs++;
							function.callingConvention = __FASTCALL;
						}
						break;
					case R8:
						if (currentInstruction->opcode == MOV && i == 0) { initializedR8 = 1; }
						else if (!initializedR8)
						{
							function.regArgs[2].primitiveType = INT_TYPE;
							function.regArgs[2].isPointer = 0;
							function.regArgs[2].isSigned = 1;
							function.numOfRegArgs++;
							function.callingConvention = __FASTCALL;
						}
						break;
					case R9:
						if (currentInstruction->opcode == MOV && i == 0) { initializedR9 = 1; }
						else if (!initializedR9)
						{
							function.regArgs[3].primitiveType = INT_TYPE;
							function.regArgs[3].isPointer = 0;
							function.regArgs[3].isSigned = 1;
							function.numOfRegArgs++;
							function.callingConvention = __FASTCALL;
						}
						break;
					}
				}
				else if (currentOperand->type == MEM_ADDRESS && (currentOperand->memoryAddress.reg == BP || currentOperand->memoryAddress.reg == EBP || currentOperand->memoryAddress.reg == RBP) && currentOperand->memoryAddress.constDisplacement > 0)
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
						function.stackArgs[function.numOfStackArgs].primitiveType = INT_TYPE;
						function.stackArgs[function.numOfStackArgs].isPointer = 0;
						function.stackArgs[function.numOfStackArgs].isSigned = 1;
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

			if (currentInstruction->opcode == MOV && currentInstruction->operands[0].type == REGISTER)
			{
				if (currentInstruction->operands[0].reg == AX || currentInstruction->operands[0].reg == EAX || currentInstruction->operands[0].reg == RAX)
				{
					function.returnType.primitiveType = INT_TYPE;
					function.returnType.isPointer = 0;
					function.returnType.isSigned = 1;

					function.addressOfReturnFunction = 0;
				}
			}
			else if (currentInstruction->opcode == CALL_NEAR)
			{
				function.addressOfReturnFunction = addresses[i] + currentInstruction->operands[0].immediate;
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
			else if (currentInstruction->opcode == JMP_NEAR || currentInstruction->opcode == JMP_FAR)
			{
				*result = function;
				return 1;
			}
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

			while (returnFunctionIndex != -1 && functions[returnFunctionIndex].addressOfReturnFunction != 0)
			{
				returnFunctionIndex = findFunctionByAddress(functions, 0, numOfFunctions - 1, functions[returnFunctionIndex].addressOfReturnFunction);
			}

			if (returnFunctionIndex != -1)
			{
				functions[i].returnType = functions[returnFunctionIndex].returnType;
			}
			else
			{
				return 0;
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