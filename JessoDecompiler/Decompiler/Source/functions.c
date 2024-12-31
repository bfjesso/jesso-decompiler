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

		struct DisassembledInstruction* currentInstruction = &instructions[i];

		if (!foundFirstInstruction)
		{
			if (currentInstruction->opcode == INT3)
			{
				continue;
			}
			
			function.addresses = &addresses[i];
			function.instructions = &instructions[i];
			function.numOfInstructions = 1;

			function.returnType.primitiveType = VOID_TYPE;
			function.returnType.numOfPtrs = 0;
			function.returnType.isSigned = 1;

			function.addressOfReturnFunction = 0;

			function.callingConvention = __CDECL;

			function.numOfRegArgs = 0;
			function.numOfStackArgs = 0;
			function.numOflocalVars = 0;

			foundFirstInstruction = 1;
		}
		else
		{
			function.numOfInstructions++;

			if (currentInstruction->opcode == PUSH || currentInstruction->opcode == POP) { continue; }

			// check for arguments
			for (int j = 0; j < 3; j++)
			{
				struct Operand* currentOperand = &currentInstruction->operands[j];

				if (currentOperand->type == REGISTER)
				{
					if (compareRegisters(currentOperand->reg, CX)) 
					{
						if (j == 0) 
						{ 
							initializedCX = 1; 
						}
						else if (!initializedCX)
						{
							function.regArgs[0].numOfPtrs = 0;
							function.regArgs[0].isSigned = 1;
							
							if (!getVariableType(&function, &currentInstruction->operands[0], &function.regArgs[0]))
							{
								return 0;
							}
							
							function.numOfRegArgs++;
							function.callingConvention = __FASTCALL;
						}
					}
					else if (compareRegisters(currentOperand->reg, DX))
					{
						if (j == 0)
						{
							initializedDX = 1;
						}
						else if (!initializedDX)
						{
							function.regArgs[1].numOfPtrs = 0;
							function.regArgs[1].isSigned = 1;
							
							if (!getVariableType(&function, &currentInstruction->operands[0], &function.regArgs[1]))
							{
								return 0;
							}
							
							function.numOfRegArgs++;
							function.callingConvention = __FASTCALL;
						}
					}
					else if (compareRegisters(currentOperand->reg, R8))
					{
						if (j == 0)
						{
							initializedR8 = 1;
						}
						else if (!initializedR8)
						{
							function.regArgs[2].numOfPtrs = 0;
							function.regArgs[2].isSigned = 1;
							
							if (!getVariableType(&function, &currentInstruction->operands[0], &function.regArgs[2]))
							{
								return 0;
							}
							
							function.numOfRegArgs++;
							function.callingConvention = __FASTCALL;
						}
					}
					else if (compareRegisters(currentOperand->reg, R9))
					{
						if (j == 0)
						{
							initializedR9 = 1;
						}
						else if (!initializedR9)
						{
							function.regArgs[3].numOfPtrs = 0;
							function.regArgs[3].isSigned = 1;
							
							if (!getVariableType(&function, &currentInstruction->operands[0], &function.regArgs[3]))
							{
								return 0;
							}
							
							function.numOfRegArgs++;
							function.callingConvention = __FASTCALL;
						}
					}
				}
				else if (currentOperand->type == MEM_ADDRESS && compareRegisters(currentOperand->memoryAddress.reg, BP) && currentOperand->memoryAddress.constDisplacement > 0)
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

						function.stackArgs[function.numOfStackArgs].numOfPtrs = 0;
						function.stackArgs[function.numOfStackArgs].isSigned = 1;

						if (!getVariableType(&function, currentOperand, &function.stackArgs[function.numOfStackArgs]))
						{
							return 0;
						}
						
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
			if (currentInstruction->operands[0].type == MEM_ADDRESS && compareRegisters(currentInstruction->operands[0].memoryAddress.reg, BP)) 
			{
				int displacement = currentInstruction->operands[0].memoryAddress.constDisplacement;

				if (currentInstruction->opcode <= MOV && displacement < 0)
				{
					unsigned char isAlreadyFound = 0;
					for (int j = 0; j < function.numOflocalVars; j++)
					{
						if (function.localVars[j].stackOffset == displacement)
						{
							isAlreadyFound = 1;
							break;
						}
					}

					if (!isAlreadyFound)
					{
						function.localVars[function.numOflocalVars].stackOffset = displacement;

						function.localVars[function.numOflocalVars].type.numOfPtrs = 0;
						function.localVars[function.numOflocalVars].type.isSigned = 1;
						
						if (!getVariableType(&function, &currentInstruction->operands[0], &function.localVars[function.numOflocalVars].type))
						{
							return 0;
						}

						function.numOflocalVars++;
					}
				}
			}
			
			// check for return value
			if (currentInstruction->opcode <= MOV && currentInstruction->operands[0].type == REGISTER)
			{
				if (compareRegisters(currentInstruction->operands[0].reg, AX))
				{
					function.returnType.numOfPtrs = 0;
					function.returnType.isSigned = 1;
					
					if(!getVariableType(&function, &currentInstruction->operands[1], &function.returnType)) 
					{
						return 0;
					}

					function.addressOfReturnFunction = 0;
				}
			}
			else if (currentInstruction->opcode == CALL_NEAR)
			{
				function.addressOfReturnFunction = addresses[i] + currentInstruction->operands[0].immediate;
			}
			else if (currentInstruction->opcode == FLD) 
			{
				function.returnType.primitiveType = FLOAT_TYPE;
				function.returnType.numOfPtrs = 0;

				function.addressOfReturnFunction = 0;
			}

			if (addressToJumpTo != 0 && addressToJumpTo != addresses[i])
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

			if (returnFunctionIndex != -1 && functions[returnFunctionIndex].returnType.primitiveType != VOID_TYPE)
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

static unsigned char getVariableType(struct Function* function, struct Operand* var, struct VariableType* result)
{
	struct DisassembledInstruction* currentInstruction = &function->instructions[function->numOfInstructions - 1];

	switch (currentInstruction->opcode) 
	{
	case MOVSS:
	case CVTPS2PD:
	case CVTSS2SD:
		result->primitiveType = FLOAT_TYPE;
		return 1;
	case MOVSD:
	case CVTPD2PS:
	case CVTSD2SS:
		result->primitiveType = DOUBLE_TYPE;
		return 1;
	}

	
	if (currentInstruction->operands[1].type == REGISTER)
	{
		if (handleIfVarIsPtr(function, currentInstruction->operands[1].reg, result))
		{
			return 1;
		}
	}

	switch (var->memoryAddress.ptrSize)
	{
	case 1:
		result->primitiveType = CHAR_TYPE;
		break;
	case 2:
		result->primitiveType = SHORT_TYPE;
		break;
	case 4:
		result->primitiveType = INT_TYPE;
		break;
	case 8:
		result->primitiveType = LONG_LONG_TYPE;
		break;
	}

	return 1;
}

static unsigned char handleIfVarIsPtr(struct Function* function, unsigned char reg, struct VariableType* result) 
{
	for (int i = function->numOfInstructions - 1; i >= 0; i--) 
	{
		struct DisassembledInstruction* currentInstruction = &function->instructions[i];
		
		if (currentInstruction->opcode == LEA && currentInstruction->operands[0].type == REGISTER && compareRegisters(currentInstruction->operands[0].reg, reg)) 
		{
			for (int i = 0; i < function->numOflocalVars; i++)
			{
				if (function->localVars[i].stackOffset == currentInstruction->operands[1].memoryAddress.constDisplacement) 
				{
					result->primitiveType = function->localVars[i].type.primitiveType;
					result->numOfPtrs = function->localVars[i].type.numOfPtrs + 1;
					return 1;
				}
			}

			return 0;
		}
	}

	return 0;
}

struct LocalVariable* getLocalVarByOffset(struct Function* function, int stackOffset) 
{
	for (int i = 0; i < function->numOflocalVars; i++) 
	{
		if (function->localVars[i].stackOffset == stackOffset) 
		{
			return &function->localVars[i];
		}
	}

	return 0;
}