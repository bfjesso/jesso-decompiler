#include "functions.h"
#include "../disassembler/operands.h"
#include "decompilationUtils.h"

unsigned char findNextFunction(struct DecompilationParameters params, unsigned long long nextSectionStartAddress, struct Function* result, int* instructionIndex)
{
	unsigned char initializedRegs[6] = { 0 }; // this corresponds with platformRegArgs

	int stackFrameSize = 0;

	unsigned long long addressToJumpTo = 0;
	unsigned char isAfterJmp = 0;
	unsigned char canReturnNothing = 0;

	unsigned char foundFirstInstruction = 0;
	for (int i = params.startInstructionIndex; i < params.totalNumOfInstructions; i++)
	{
		(*instructionIndex)++;

		struct DisassembledInstruction* currentInstruction = &params.allInstructions[i];

		if (currentInstruction->address == 0x1400012A0) 
		{
			int TT = 0;
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

		// checking all operands for arguments and stack vars
		unsigned char overwrites = 0;
		for (int j = 3; j >= 0; j--)
		{
			struct Operand* currentOperand = &currentInstruction->operands[j];

			if (isOperandStackArg(currentOperand, stackFrameSize))
			{
				int stackOffset = currentOperand->memoryAddress.constDisplacement;
				if (compareRegisters(currentOperand->memoryAddress.reg, SP))
				{
					stackOffset -= stackFrameSize;
				}

				struct StackVariable* stackArg = getStackArgByOffset(result, stackOffset);
				struct StackVariable* localVar = getLocalVarByOffset(result, stackOffset);
				if (stackArg)
				{
					stackArg->type.isUnsigned = doesOpcodeUseUnsignedInt(currentInstruction->opcode);
				}
				else if (localVar)
				{
					localVar->type.isUnsigned = doesOpcodeUseUnsignedInt(currentInstruction->opcode);
				}
				else
				{
					doesInstructionModifyOperand(currentInstruction, j, &overwrites);
					if (!overwrites)
					{
						if (!addStackArg(result, getTypeOfOperand(currentInstruction->opcode, currentOperand), stackOffset)) 
						{
							return 0;
						}
					}
					else if(!addStackVar(result, getTypeOfOperand(currentInstruction->opcode, currentOperand), stackOffset)) // treating stack args that are overwritten before being accessed as local vars
					{
						return 0;
					}
				}
			}
			else if (isOperandStackVar(currentOperand, stackFrameSize))
			{
				int stackOffset = currentOperand->memoryAddress.constDisplacement;
				if (compareRegisters(currentOperand->memoryAddress.reg, SP))
				{
					stackOffset -= stackFrameSize;
				}

				struct StackVariable* stackArg = getStackArgByOffset(result, stackOffset);
				struct StackVariable* localVar = getLocalVarByOffset(result, stackOffset);
				if (stackArg)
				{
					stackArg->type.isUnsigned = doesOpcodeUseUnsignedInt(currentInstruction->opcode);
				}
				else if (localVar)
				{
					localVar->type.isUnsigned = doesOpcodeUseUnsignedInt(currentInstruction->opcode);
				}
				else if(!addStackVar(result, getTypeOfOperand(currentInstruction->opcode, currentOperand), stackOffset))
				{
					return 0;
				}
			}
			else if (currentOperand->type == REGISTER && currentInstruction->opcode != PUSH)
			{
				if (isRegisterPointer(currentOperand->reg)) { continue; }
				
				for(int k = 0; k < numOfPlatformRegArgs; k++)
				{
					if (compareRegisters(currentOperand->reg, platformRegArgs[k]))
					{
						struct RegisterVariable* regArg = getRegArgByReg(result, platformRegArgs[k]);
						if (regArg) 
						{
							regArg->type.isUnsigned = doesOpcodeUseUnsignedInt(currentInstruction->opcode);
						}
						else if (doesInstructionModifyRegister(currentInstruction, platformRegArgs[k], 0, &overwrites) && overwrites)
						{
							if (!isAfterJmp) 
							{
								initializedRegs[k] = 1;
							}
						}
						else if (!initializedRegs[k])
						{
							if (!addRegArg(result, getTypeOfOperand(currentInstruction->opcode, currentOperand), currentOperand->reg)) 
							{
								return 0;
							}

							result->callingConvention = __FASTCALL;
							initializedRegs[k] = 1;
						}

						break;
					}
				}
			}
			else if (currentOperand->type == MEM_ADDRESS) 
			{
				// maybe memoryAddress.regDisplacement should be checked too ?
				if (currentOperand->memoryAddress.reg == NO_REG || compareRegisters(currentOperand->memoryAddress.reg, IP)) { continue; }

				for (int k = 0; k < numOfPlatformRegArgs; k++)
				{
					if (compareRegisters(currentOperand->memoryAddress.reg, platformRegArgs[k]))
					{
						if (!initializedRegs[k])
						{
							if (!addRegArg(result, getTypeOfOperand(currentInstruction->opcode, currentOperand), currentOperand->memoryAddress.reg))
							{
								return 0;
							}

							result->callingConvention = __FASTCALL;
							initializedRegs[k] = 1;
						}

						break;
					}
				}
			}
		}

		int stackFrameSizeChange = getStackFrameChange(currentInstruction);
		if (stackFrameSizeChange != 0)
		{
			stackFrameSize += stackFrameSizeChange;
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

		// check for return value
		if (!canReturnNothing) // if the function can return nothing, its return type must be void
		{
			unsigned char overwrites = 0;
			if (doesInstructionModifyRegister(currentInstruction, AX, 0, &overwrites) && overwrites)
			{
				result->returnType = getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[1]);
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
				result->returnType.primitiveType = FLOAT_TYPE;
				result->addressOfReturnFunction = 0;
			}
			else if (isOpcodeReturn(currentInstruction->opcode) && result->returnType.primitiveType == VOID_TYPE && result->addressOfReturnFunction != 0)
			{
				canReturnNothing = 1;
			}

		}

		isAfterJmp = addressToJumpTo != 0 && params.allInstructions[i].address < addressToJumpTo;
		if (isAfterJmp)
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
		else if(currentInstruction->opcode == HLT || currentInstruction->opcode == INT3 || params.allInstructions[i + 1].address == nextSectionStartAddress)
		{
			result->returnType.primitiveType = VOID_TYPE;
			result->addressOfReturnFunction = 0;

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
				if (functions[returnFunctionIndex].returnType.primitiveType != VOID_TYPE) 
				{
					functions[i].returnType = functions[returnFunctionIndex].returnType;
				}
			}
			else // probably an imported function
			{
				functions[i].returnType.primitiveType = is64Bit ? LONG_LONG_TYPE : INT_TYPE; // assume something is returned
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

						int alreadyFound = 0;
						for (int l = 0; l < currentFunc->numOfRegArgs; l++)
						{
							if (compareRegisters(currentFunc->regArgs[l].reg, callee->regArgs[k].reg))
							{
								alreadyFound = 1;
								break;
							}
						}
						if (alreadyFound) { continue; }

						if (!addRegArg(currentFunc, callee->regArgs[k].type, callee->regArgs[k].reg)) 
						{
							return 0;
						}
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

static int getStackFrameChange(struct DisassembledInstruction* instruction) 
{
	if (instruction->operands[0].type == REGISTER && compareRegisters(instruction->operands[0].reg, BP)) 
	{
		return 0;
	}

	if(instruction->operands[0].type == REGISTER && compareRegisters(instruction->operands[0].reg, SP))
	{
		if (instruction->opcode == SUB)
		{
			return instruction->operands[1].immediate.value;
		}
		else if (instruction->opcode == ADD)
		{
			return -instruction->operands[1].immediate.value;
		}
	}
	else if (instruction->opcode == PUSH) 
	{
		return getSizeOfOperand(&instruction->operands[0]);
	}
	else if (instruction->opcode == POP)
	{
		return -getSizeOfOperand(&instruction->operands[0]);
	}

	return 0;
}

int getStackFrameSizeAtInstruction(struct Function* function, int instructionIndex)
{
	int result = 0;
	for (int i = 0; i < instructionIndex; i++) 
	{
		result += getStackFrameChange(&function->instructions[i]);
	}

	return result;
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

struct RegisterVariable* getRegVarByReg(struct Function* function, enum Register reg)
{
	for (int i = 0; i < function->numOfRegVars; i++)
	{
		if (compareRegisters(function->regVars[i].reg, reg))
		{
			return &function->regVars[i];
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

unsigned char addStackArg(struct Function* function, struct VarType type, int stackOffset) 
{
	struct StackVariable* newStackArgs = (struct StackVariable*)realloc(function->stackArgs, sizeof(struct StackVariable) * (function->numOfStackArgs + 1));
	if (newStackArgs)
	{
		function->stackArgs = newStackArgs;
	}
	else
	{
		return 0;
	}

	function->stackArgs[function->numOfStackArgs].stackOffset = stackOffset;
	function->stackArgs[function->numOfStackArgs].type = type;
	function->stackArgs[function->numOfStackArgs].name = initializeJdcStr();
	sprintfJdc(&(function->stackArgs[function->numOfStackArgs].name), 0, "arg%X", stackOffset);
	function->numOfStackArgs++;

	return 1;
}

unsigned char addStackVar(struct Function* function, struct VarType type, int stackOffset)
{
	struct StackVariable* newLocalVars = (struct StackVariable*)realloc(function->localVars, sizeof(struct StackVariable) * (function->numOfLocalVars + 1));
	if (newLocalVars)
	{
		function->localVars = newLocalVars;
	}
	else
	{
		return 0;
	}

	function->localVars[function->numOfLocalVars].stackOffset = stackOffset;
	function->localVars[function->numOfLocalVars].type = type;
	function->localVars[function->numOfLocalVars].name = initializeJdcStr();

	if (stackOffset > 0)
	{
		sprintfJdc(&(function->localVars[function->numOfLocalVars].name), 0, "var%X", stackOffset);
	}
	else
	{
		sprintfJdc(&(function->localVars[function->numOfLocalVars].name), 0, "var%X", -stackOffset);
	}

	function->numOfLocalVars++;

	return 1;
}

unsigned char addRegArg(struct Function* function, struct VarType type, enum Register reg) 
{
	struct RegisterVariable* newRegArgs = (struct RegisterVariable*)realloc(function->regArgs, sizeof(struct RegisterVariable) * (function->numOfRegArgs + 1));
	if (newRegArgs)
	{
		function->regArgs = newRegArgs;
	}
	else
	{
		return 0;
	}

	function->regArgs[function->numOfRegArgs].reg = reg;
	function->regArgs[function->numOfRegArgs].type = type;
	function->regArgs[function->numOfRegArgs].name = initializeJdcStr();
	sprintfJdc(&(function->regArgs[function->numOfRegArgs].name), 0, "arg%s", registerStrs[reg]);
	function->numOfRegArgs++;

	return 1;
}

unsigned char addRegVar(struct Function* function, struct VarType type, enum Register reg) 
{
	struct RegisterVariable* newRegVars = (struct RegisterVariable*)realloc(function->regVars, sizeof(struct RegisterVariable) * (function->numOfRegVars + 1));
	if (newRegVars)
	{
		function->regVars = newRegVars;
	}
	else
	{
		return 0;
	}

	function->regVars[function->numOfRegVars].reg = reg;
	function->regVars[function->numOfRegVars].type = type;
	function->regVars[function->numOfRegVars].name = initializeJdcStr();
	sprintfJdc(&(function->regVars[function->numOfRegVars].name), 0, "var%s", registerStrs[reg]);
	function->numOfRegVars++;

	return 1;
}

unsigned char addReturnedVar(struct Function* function, struct VarType type, char callNum, unsigned long long callAddr, const char* calleeName) 
{
	struct ReturnedVariable* newReturnedVars = (struct ReturnedVariable*)realloc(function->returnedVars, sizeof(struct ReturnedVariable) * (function->numOfReturnedVars + 1));
	if (newReturnedVars)
	{
		function->returnedVars = newReturnedVars;
	}
	else
	{
		return 0;
	}

	function->returnedVars[function->numOfReturnedVars].type = type;
	function->returnedVars[function->numOfReturnedVars].name = initializeJdcStr();
	sprintfJdc(&(function->returnedVars[function->numOfReturnedVars].name), 0, "%sRetVal%d", calleeName, callNum);
	function->returnedVars[function->numOfReturnedVars].callAddr = callAddr;
	function->returnedVars[function->numOfReturnedVars].callNum = callNum;
	function->numOfReturnedVars++;

	return 1;
}

static void sortFunctionArguments(struct Function* function) 
{
	#ifdef _WIN32
	// order for Microsoft x64 calling convention should be RCX, RDX, R8, R9
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
	#endif

	#ifdef linux
	// order for Linux x64 calling convention should be RDI, RSI, RDX, RCX, R8, R9
	for (int i = 0; i < function->numOfRegArgs; i++)
	{
		if (compareRegisters(function->regArgs[i].reg, RDI))
		{
			struct RegisterVariable temp = function->regArgs[0];
			function->regArgs[0] = function->regArgs[i];
			function->regArgs[i] = temp;
		}
		else if (compareRegisters(function->regArgs[i].reg, RSI) && function->numOfRegArgs > 1)
		{
			struct RegisterVariable temp = function->regArgs[1];
			function->regArgs[1] = function->regArgs[i];
			function->regArgs[i] = temp;
		}
		else if (compareRegisters(function->regArgs[i].reg, RDX) && function->numOfRegArgs > 2)
		{
			struct RegisterVariable temp = function->regArgs[2];
			function->regArgs[2] = function->regArgs[i];
			function->regArgs[i] = temp;
		}
		else if (compareRegisters(function->regArgs[i].reg, RCX) && function->numOfRegArgs > 3)
		{
			struct RegisterVariable temp = function->regArgs[3];
			function->regArgs[3] = function->regArgs[i];
			function->regArgs[i] = temp;
		}
		else if (compareRegisters(function->regArgs[i].reg, R8) && function->numOfRegArgs > 4)
		{
			struct RegisterVariable temp = function->regArgs[4];
			function->regArgs[4] = function->regArgs[i];
			function->regArgs[i] = temp;
		}
		else if (compareRegisters(function->regArgs[i].reg, R9) && function->numOfRegArgs > 5)
		{
			struct RegisterVariable temp = function->regArgs[5];
			function->regArgs[5] = function->regArgs[i];
			function->regArgs[i] = temp;
		}
	}
	#endif

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
