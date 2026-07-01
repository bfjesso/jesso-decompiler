#include "functions.h"
#include "../disassembler/operands.h"
#include "decompilationUtils.h"
#include "conditions.h"
#include "returnStatements.h"

unsigned char findNextFunction(struct DecompilationParameters* params, unsigned long long currentSectionEndAddress, unsigned long long* calledAddresses, int numOfCalledAddresses, struct Function* result, int* instructionIndex)
{
	int indexToJumpTo = 0;

	params->currentFunc = result;

	int startInstructionIndex = *instructionIndex;

	unsigned char foundFirstInstruction = 0;
	for (int i = startInstructionIndex; i < params->numOfInstructions; i++)
	{
		(*instructionIndex)++;

		struct DisassembledInstruction* currentInstruction = &params->instructions[i];

		if (!foundFirstInstruction)
		{
			if (doesInstructionGenerateInterruptOrException(currentInstruction) || doesInstructionDoNothing(currentInstruction))
			{
				continue;
			}

			result->firstInstructionIndex = i;
			foundFirstInstruction = 1;
		}

		if (isOpcodeJcc(currentInstruction->opcode) || isOpcodeJmp(currentInstruction->opcode))
		{
			unsigned long long jumpAddr = resolveJmpChain(params, i);
			int instructionIndex = findInstructionByAddress(params->instructions, 0, params->numOfInstructions - 1, jumpAddr);
			if (instructionIndex > indexToJumpTo && instructionIndex > i && jumpAddr <= currentSectionEndAddress)
			{
				if (!checkForAddressInArrInRange(calledAddresses, 0, numOfCalledAddresses - 1, currentInstruction->address, jumpAddr))
				{
					indexToJumpTo = instructionIndex;
				}
			}
		}
		
		if (checkForReturnStatement(params, i) && i >= indexToJumpTo)
		{
			result->lastInstructionIndex = i;
			return 1;
		}
		else if((doesInstructionGenerateInterruptOrException(currentInstruction) && i >= indexToJumpTo) ||
			i == params->numOfInstructions - 1 || params->instructions[i + 1].address >= currentSectionEndAddress)
		{
			result->callingConvention = __UNKNOWNCALL;
			result->lastInstructionIndex = i;
			return 1;
		}
		else if (findAddressInArr(calledAddresses, 0, numOfCalledAddresses - 1, params->instructions[i + 1].address) != -1)
		{
			result->returningFunctionAddress = params->instructions[i + 1].address;
			result->lastInstructionIndex = i;
			return 1;
		}
	}

	return 0;
}

void getAllFunctionReturnTypes(struct DecompilationParameters* params) 
{
	for (int i = 0; i < params->numOfFunctions; i++)
	{
		params->currentFunc = &params->functions[i];
		if (params->currentFunc->callingConvention == __UNKNOWNCALL) // __UNKNOWNCALL will only be set at this point if the function ends without a return instruction
		{
			continue;
		}

		for (int j = params->currentFunc->firstInstructionIndex; j <= params->currentFunc->lastInstructionIndex; j++)
		{
			struct DisassembledInstruction* currentInstruction = &params->instructions[j];

			if (doesInstructionDoNothing(currentInstruction))
			{
				continue;
			}

			// this will take every jump
			if (isOpcodeJcc(currentInstruction->opcode) || isOpcodeJmp(currentInstruction->opcode))
			{
				int instructionIndex = findInstructionByAddress(params->instructions, 0, params->numOfInstructions - 1, resolveJmpChain(params, j));
				if (instructionIndex > j && instructionIndex <= params->currentFunc->lastInstructionIndex)
				{
					j = instructionIndex - 1;
					continue;
				}
			}

			enum Register specificReg = NO_REG;
			if (doesInstructionModifyRegister(params, j, AX, &specificReg, 0))
			{
				params->currentFunc->returnType = getRegisterDataType(currentInstruction->opcode, specificReg);
				params->currentFunc->returnReg = specificReg;
				params->currentFunc->returningFunctionAddress = 0;
			}
			else if (doesInstructionModifyRegister(params, j, XMM0, 0, 0) && !compareRegisters(params->currentFunc->returnReg, AX)) // assuming AX is more likely to be the return register
			{
				params->currentFunc->returnType = getRegisterDataType(currentInstruction->opcode, XMM0);
				params->currentFunc->returnReg = XMM0;
				params->currentFunc->returningFunctionAddress = 0;
			}
			else if (doesInstructionModifyRegister(params, j, ST0, 0, 0))
			{
				params->currentFunc->returnType.primitiveType = FLOAT_TYPE;
				params->currentFunc->returnReg = ST0;
				params->currentFunc->returningFunctionAddress = 0;
			}
			else if (isOpcodeCall(currentInstruction->opcode))
			{
				params->currentFunc->returnType.primitiveType = VOID_TYPE;
				params->currentFunc->returnReg = NO_REG;
				params->currentFunc->returningFunctionAddress = resolveJmpChain(params, j);
			}
			else if ((checkForReturnStatement(params, j)))
			{
				break;
			}
		}
	}
}

unsigned char fixAllFunctionReturnTypes(struct DecompilationParameters* params) // resolves if a function's return type depends on another function
{
	for (int i = 0; i < params->numOfFunctions; i++)
	{
		params->currentFunc = &params->functions[i];
		if (params->functions[i].returningFunctionAddress != 0)
		{
			int returningFunctionIndex = findFunctionByAddress(params, 0, params->numOfFunctions - 1, params->functions[i].returningFunctionAddress);
			if (returningFunctionIndex == -1) 
			{
				params->functions[i].returnType.primitiveType = params->is64Bit ? LONG_LONG_TYPE : INT_TYPE;
				params->functions[i].returnReg = params->is64Bit ? RAX : EAX;
				continue;
			}

			int count = 0; // to avoid an infinite loop
			while (params->functions[returningFunctionIndex].returningFunctionAddress != 0 && count < 10)
			{
				returningFunctionIndex = findFunctionByAddress(params, 0, params->numOfFunctions - 1, params->functions[returningFunctionIndex].returningFunctionAddress);
				count++;
			}

			if (params->functions[returningFunctionIndex].returnType.primitiveType != VOID_TYPE)
			{
				params->functions[i].returnType = params->functions[returningFunctionIndex].returnType;
				params->functions[i].returnReg = params->functions[returningFunctionIndex].returnReg;
			}
		}
	}

	return 1;
}

unsigned char getAllFunctionConditionsAndArguments(struct DecompilationParameters* params)
{
	for (int i = 0; i < params->numOfFunctions; i++) 
	{
		params->currentFunc = &params->functions[i];

		if (!getAllConditions(params))
		{
			return 0;
		}

		if (!getFunctionArguments(params))
		{
			return 0;
		}
	}

	return 1;
}

static unsigned char getFunctionArguments(struct DecompilationParameters* params)
{
	for (int i = params->currentFunc->firstInstructionIndex; i <= params->currentFunc->lastInstructionIndex; i++)
	{
		struct DisassembledInstruction* currentInstruction = &params->instructions[i];

		if (doesInstructionDoNothing(currentInstruction))
		{
			continue;
		}

		if (isOpcodeCall(currentInstruction->opcode) && params->currentFunc->firstCalledFunc == 0)
		{
			int calleeIndex = findFunctionByAddress(params, 0, params->numOfFunctions - 1, resolveJmpChain(params, i));
			if (calleeIndex != -1 && &params->functions[calleeIndex] != params->currentFunc)
			{
				params->currentFunc->firstCalledFunc = &params->functions[calleeIndex];
				params->currentFunc->firstFuncCallInstructionIndex = i;
			}
		}

		// checking for reg args
		for (int j = RAX; j < ST0; j++)
		{
			if (currentInstruction->opcode == PUSH && currentInstruction->operands[0].type == REGISTER)
			{
				break;
			}

			if (j == RBP || j == RSP || j == RIP)
			{
				continue;
			}

			unsigned char overwrites = 0;
			enum Register specificReg = NO_REG;
			struct DataType regDataType = { 0 };
			if (doesInstructionAccessRegister(params, i, j, &specificReg, &regDataType) && !getRegArgByReg(params->currentFunc, j))
			{
				if (!isRegInitialized(params, i, params->currentFunc->firstInstructionIndex, j))
				{
					if (!addRegArg(params->currentFunc, regDataType, specificReg))
					{
						return 0;
					}
				}
			}
		}

		// checking for stack arguments and stack vars
		long long stackFrameSize = getStackFrameSizeAtInstruction(params, i);
		unsigned char overwrites = 0;
		for (int j = currentInstruction->numOfOperands - 1; j >= 0; j--)
		{
			struct Operand* currentOperand = &currentInstruction->operands[j];
			if (isOperandStackArg(currentOperand, stackFrameSize))
			{
				long long stackOffset = currentOperand->memoryAddress.constDisplacement;
				if (compareRegisters(currentOperand->memoryAddress.reg, SP))
				{
					stackOffset -= stackFrameSize;
				}

				struct StackVariable* stackArg = getStackArgByOffset(params->currentFunc, stackOffset);
				struct StackVariable* stackVar = getStackVarByOffset(params->currentFunc, stackOffset);
				if (!stackArg && !stackVar)
				{
					doesInstructionModifyOperand(currentInstruction, j, &overwrites);
					if (!overwrites)
					{
						if (!addStackArg(params->currentFunc, getOperandDataType(currentInstruction->opcode, currentOperand), stackOffset))
						{
							return 0;
						}
					}
					else if (!addStackVar(params->currentFunc, getOperandDataType(currentInstruction->opcode, currentOperand), stackOffset)) // treating stack args that are overwritten before being accessed as stack vars
					{
						return 0;
					}
				}
			}
			else if (isOperandStackVar(currentOperand, stackFrameSize))
			{
				long long stackOffset = currentOperand->memoryAddress.constDisplacement;
				if (compareRegisters(currentOperand->memoryAddress.reg, SP))
				{
					stackOffset -= stackFrameSize;
				}

				struct StackVariable* stackArg = getStackArgByOffset(params->currentFunc, stackOffset);
				struct StackVariable* stackVar = getStackVarByOffset(params->currentFunc, stackOffset);
				if (!stackArg && !stackVar)
				{
					if (!addStackVar(params->currentFunc, getOperandDataType(currentInstruction->opcode, currentOperand), stackOffset))
					{
						return 0;
					}
				}
			}
		}
	}

	return 1;
}

static unsigned char isRegInitialized(struct DecompilationParameters* params, int startInstructionIndex, int minInstructionIndex, enum Register reg)
{
	for (int i = startInstructionIndex - 1; i >= minInstructionIndex; i--)
	{
		unsigned char overwrites = 0;
		if (doesInstructionModifyRegister(params, i, reg, 0, &overwrites) && overwrites)
		{
			return 1;
		}

		int conditionIndex = getConditionEnd(params, i);
		if (conditionIndex != -1)
		{
			struct Condition* cond = &params->currentFunc->conditions[conditionIndex];
			if (cond->conditionType == ELSE_CT)
			{
				if (isRegInitialized(params, cond->endIndex, cond->startIndex, reg))
				{
					int ifIndex = getConditionEnd(params, cond->startIndex);
					if (ifIndex != -1 && params->currentFunc->conditions[ifIndex].conditionType == IF_CT) // I will handle else ifs later
					{
						struct Condition* ifCond = &params->currentFunc->conditions[ifIndex];
						if (isRegInitialized(params, ifCond->endIndex, ifCond->startIndex, reg)) 
						{
							return 1;
						}
					}
				}
			}

			if (cond->startIndex < i) 
			{
				i = cond->startIndex + 1;
			}
		}
	}

	return 0;
}

unsigned char fixAllFunctionArgs(struct DecompilationParameters* params) // checks for arguments that aren't used in the function but are just passed to another function call
{
	int numFixed = 0;
	for (int i = 0; i < params->numOfFunctions; i++) 
	{
		struct Function* currentFunc = &params->functions[i];
		if (currentFunc->firstCalledFunc != 0)
		{
			int numOfRegArgsInit = 0;
			enum Register* initializedRegs = (enum Register*)calloc(currentFunc->firstCalledFunc->numOfRegArgs, sizeof(enum Register));
			if (!initializedRegs)
			{
				return 0;
			}

			for (int j = currentFunc->firstFuncCallInstructionIndex - 1; j >= currentFunc->firstInstructionIndex; j--)
			{
				struct DisassembledInstruction* instruction = &params->instructions[j];

				for (int k = 0; k < currentFunc->firstCalledFunc->numOfRegArgs; k++)
				{
					int alreadyFound = 0;
					for (int l = 0; l < numOfRegArgsInit; l++)
					{
						if (initializedRegs[l] == currentFunc->firstCalledFunc->regArgs[k].reg)
						{
							alreadyFound = 1;
							break;
						}
					}
					if (alreadyFound) { continue; }

					unsigned char overwrites = 0;
					if (doesInstructionModifyRegister(params, j, currentFunc->firstCalledFunc->regArgs[k].reg, 0, &overwrites) && overwrites)
					{
						initializedRegs[numOfRegArgsInit] = currentFunc->firstCalledFunc->regArgs[k].reg;
						numOfRegArgsInit++;
						break;
					}
				}
			}

			if (numOfRegArgsInit != currentFunc->firstCalledFunc->numOfRegArgs)
			{
				for (int k = 0; k < currentFunc->firstCalledFunc->numOfRegArgs; k++)
				{
					int isInitialized = 0;
					for (int l = 0; l < numOfRegArgsInit; l++)
					{
						if (initializedRegs[l] == currentFunc->firstCalledFunc->regArgs[k].reg)
						{
							isInitialized = 1;
							break;
						}
					}

					if (!isInitialized)
					{
						if (!addRegArg(currentFunc, currentFunc->firstCalledFunc->regArgs[k].dataType, currentFunc->firstCalledFunc->regArgs[k].reg))
						{
							free(initializedRegs);
							return 0;
						}
					}
				}

				numFixed++;
			}

			free(initializedRegs);

			currentFunc->firstCalledFunc = 0;
		}
	}

	if (numFixed != 0) 
	{
		return fixAllFunctionArgs(params);
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
	for (int i = 0; i < function->numOfRegVars; i++)
	{
		freeJdcStr(&function->regVars[i].name);
	}

	for (int i = 0; i < function->numOfStackArgs; i++)
	{
		freeJdcStr(&function->stackArgs[i].name);
	}
	for (int i = 0; i < function->numOfStackVars; i++)
	{
		freeJdcStr(&function->stackVars[i].name);
	}

	for (int i = 0; i < function->numOfReturnedVars; i++)
	{
		freeJdcStr(&function->returnedVars[i].name);
	}

	free(function->regArgs);
	free(function->regVars);
	free(function->stackArgs);
	free(function->stackVars);
	free(function->returnedVars);

	for (int i = 0; i < function->numOfConditions; i++)
	{
		free(function->conditions[i].combinedJccIndexes);
	}
	free(function->conditions);
	free(function->directJmps);

	for (int i = 0; i < function->numOfLines; i++) 
	{
		free(function->associatedInstructions[i].indexes);
	}
	free(function->associatedInstructions);
}

static long long getStackFrameChange(struct DisassembledInstruction* instruction) 
{	
	if (instruction->numOfOperands == 0 || (instruction->operands[0].type == REGISTER && compareRegisters(instruction->operands[0].reg, BP)))
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

long long getStackFrameSizeAtInstruction(struct DecompilationParameters* params, int instructionIndex)
{
	long long result = 0;
	for (int i = params->currentFunc->firstInstructionIndex; i < instructionIndex; i++)
	{
		struct DisassembledInstruction* instruction = &params->instructions[i];
		if (isOpcodeJcc(instruction->opcode) || isOpcodeJmp(instruction->opcode))
		{
			int dstIndex = findInstructionByAddress(params->instructions, 0, params->numOfInstructions - 1, resolveJmpChain(params, i));
			if (dstIndex > i && dstIndex <= instructionIndex && !doesInstructionLeadStraightToReturn(params, dstIndex))
			{
				i = dstIndex - 1;
			}
		}
		else 
		{
			result += getStackFrameChange(&params->instructions[i]);
		}
	}

	return result;
}

// returns index of function, -1 if not found
int findFunctionByAddress(struct DecompilationParameters* params, int low, int high, unsigned long long address)
{
	while (low <= high)
	{
		int mid = low + (high - low) / 2;

		if (params->instructions[params->functions[mid].firstInstructionIndex].address == address) { return mid; }

		if (params->instructions[params->functions[mid].firstInstructionIndex].address < address) { low = mid + 1; }
		else { high = mid - 1; }
	}

	return -1;
}

int findFunctionByAddressInclusive(struct DecompilationParameters* params, int low, int high, unsigned long long address)
{
	while (low <= high)
	{
		int mid = low + (high - low) / 2;

		unsigned long long firstAddress = params->instructions[params->functions[mid].firstInstructionIndex].address;
		unsigned long long lastAddress = params->instructions[params->functions[mid].lastInstructionIndex].address;

		if (address >= firstAddress && address <= lastAddress) { return mid; }

		if (address > lastAddress) { low = mid + 1; }
		else { high = mid - 1; }
	}

	return -1;
}

struct StackVariable* getStackArgByOffset(struct Function* function, long long stackOffset)
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

struct StackVariable* getStackVarByOffset(struct Function* function, long long stackOffset)
{
	for (int i = 0; i < function->numOfStackVars; i++)
	{
		if (function->stackVars[i].stackOffset == stackOffset)
		{
			return &function->stackVars[i];
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

struct ReturnedVariable* findReturnedVar(struct Function* function, unsigned long long callInstructionAddress)
{
	for (int i = 0; i < function->numOfReturnedVars; i++)
	{
		if (function->returnedVars[i].callInstructionAddress == callInstructionAddress)
		{
			return &function->returnedVars[i];
		}
	}

	return 0;
}

unsigned char addStackArg(struct Function* function, struct DataType dataType, long long stackOffset)
{
	if (getStackArgByOffset(function, stackOffset)) 
	{
		return 1;
	}
	
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
	function->stackArgs[function->numOfStackArgs].dataType = dataType;
	function->stackArgs[function->numOfStackArgs].name = initializeJdcStr();
	sprintfJdc(&(function->stackArgs[function->numOfStackArgs].name), 0, "arg%X", stackOffset);
	function->numOfStackArgs++;

	// sorting from least to greatest stack offset
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

	return 1;
}

unsigned char addStackVar(struct Function* function, struct DataType dataType, long long stackOffset)
{
	if (getStackVarByOffset(function, stackOffset))
	{
		return 1;
	}
	
	struct StackVariable* newLocalVars = (struct StackVariable*)realloc(function->stackVars, sizeof(struct StackVariable) * (function->numOfStackVars + 1));
	if (newLocalVars)
	{
		function->stackVars = newLocalVars;
	}
	else
	{
		return 0;
	}

	function->stackVars[function->numOfStackVars].stackOffset = stackOffset;
	function->stackVars[function->numOfStackVars].dataType = dataType;
	function->stackVars[function->numOfStackVars].name = initializeJdcStr();

	if (stackOffset > 0)
	{
		sprintfJdc(&(function->stackVars[function->numOfStackVars].name), 0, "var%X", stackOffset);
	}
	else
	{
		sprintfJdc(&(function->stackVars[function->numOfStackVars].name), 0, "var%X", -stackOffset);
	}

	function->numOfStackVars++;

	return 1;
}

unsigned char addRegArg(struct Function* function, struct DataType dataType, enum Register reg)
{
	if (getRegArgByReg(function, reg))
	{
		return 1;
	}
	
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
	function->regArgs[function->numOfRegArgs].dataType = dataType;
	function->regArgs[function->numOfRegArgs].name = initializeJdcStr();
	sprintfJdc(&(function->regArgs[function->numOfRegArgs].name), 0, "arg%s", registerStrs[reg]);
	function->numOfRegArgs++;

	// sorting
	for (int i = 0; i < function->numOfRegArgs; i++)
	{
		for (int j = 0; j < NUM_PLATFORM_REG_ARGS; j++)
		{
			if ((compareRegisters(function->regArgs[i].reg, platformRegArgs[j]) || compareRegisters(function->regArgs[i].reg, altPlatformRegArgs[j])) && function->numOfRegArgs > j)
			{
				struct RegisterVariable temp = function->regArgs[j];
				function->regArgs[j] = function->regArgs[i];
				function->regArgs[i] = temp;
				break;
			}
		}
	}

	if (function->callingConvention != __UNKNOWNCALL) 
	{
		if (!isRegisterPlatformArg(reg))
		{
			function->callingConvention = __UNKNOWNCALL;
		}
		else if (function->numOfRegArgs == 1 && compareRegisters(reg, CX))
		{
			function->callingConvention = __THISCALL;
		}
		else
		{
			function->callingConvention = __FASTCALL;
			for (int i = 0; i < function->numOfRegArgs && i < NUM_PLATFORM_REG_ARGS; i++) // this checks that all reg args are present that should be. if platformRegArgs[1] is there but platformRegArgs[0] isn't then its wrong
			{
				if (!compareRegisters(function->regArgs[i].reg, platformRegArgs[i]) && !compareRegisters(function->regArgs[i].reg, altPlatformRegArgs[i]))
				{
					function->callingConvention = __UNKNOWNCALL;
				}
			}
		}
	}
	
	return 1;
}

unsigned char addRegVar(struct Function* function, struct DataType dataType, enum Register reg)
{
	if (getRegVarByReg(function, reg))
	{
		return 1;
	}
	
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
	function->regVars[function->numOfRegVars].dataType = dataType;
	function->regVars[function->numOfRegVars].name = initializeJdcStr();
	sprintfJdc(&(function->regVars[function->numOfRegVars].name), 0, "var%s", registerStrs[reg]);
	function->numOfRegVars++;

	return 1;
}

unsigned char addReturnedVar(struct Function* function, struct DataType dataType, unsigned long long calleeAddress, unsigned long long callInstructionAddress, enum Register returnReg, const char* calleeName)
{
	if (findReturnedVar(function, callInstructionAddress))
	{
		return 1;
	}
	
	struct ReturnedVariable* newReturnedVars = (struct ReturnedVariable*)realloc(function->returnedVars, sizeof(struct ReturnedVariable) * (function->numOfReturnedVars + 1));
	if (!newReturnedVars)
	{
		return 0;
	}

	function->returnedVars = newReturnedVars;

	int callNum = 0;
	for (int i = 0; i < function->numOfReturnedVars; i++)
	{
		if (function->returnedVars[i].calleeAddress == calleeAddress)
		{
			callNum++;
		}
	}

	function->returnedVars[function->numOfReturnedVars].dataType = dataType;

	function->returnedVars[function->numOfReturnedVars].name = initializeJdcStr();
	sprintfJdc(&(function->returnedVars[function->numOfReturnedVars].name), 0, "%sRetVal%d", calleeName, callNum);
	replaceJdc(&(function->returnedVars[function->numOfReturnedVars].name), "::", "_");

	function->returnedVars[function->numOfReturnedVars].calleeAddress = calleeAddress;
	function->returnedVars[function->numOfReturnedVars].callInstructionAddress = callInstructionAddress;
	function->returnedVars[function->numOfReturnedVars].returnReg = returnReg;
	function->numOfReturnedVars++;

	return 1;
}

unsigned char addAssociatedInstruction(struct Function* function, int instructionIndex)
{
	if (function->numOfLines >= function->associatedInstructionsBufferLen) 
	{
		int ogBufferLen = function->associatedInstructionsBufferLen;
		function->associatedInstructionsBufferLen = function->numOfLines + 10;
		struct AssociatedInstructions* newAssociatedInstructions = (struct AssociatedInstructions*)realloc(function->associatedInstructions, function->associatedInstructionsBufferLen * sizeof(struct AssociatedInstructions));
		if (!newAssociatedInstructions) 
		{
			return 0;
		}

		function->associatedInstructions = newAssociatedInstructions;
		memset(function->associatedInstructions + ogBufferLen, 0, (function->associatedInstructionsBufferLen - ogBufferLen) * sizeof(struct AssociatedInstructions));
	}
	
	struct AssociatedInstructions* a = &function->associatedInstructions[function->numOfLines];
	if (!a) 
	{
		return 0;
	}

	int* newIndexes = (int*)realloc(a->indexes, (a->numOfIndexes + 1) * sizeof(int));
	if (!newIndexes)
	{
		return 0;
	}

	a->indexes = newIndexes;
	a->indexes[a->numOfIndexes] = instructionIndex;
	a->numOfIndexes++;

	return 1;
}