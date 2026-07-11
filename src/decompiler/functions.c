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
			if (checkForAddressInArrInRange(calledAddresses, numOfCalledAddresses, currentInstruction->address, currentInstruction->address) || 
				(!doesInstructionGenerateInterruptOrException(currentInstruction) && !doesInstructionDoNothing(currentInstruction)))
			{
				result->firstInstructionIndex = i;
				foundFirstInstruction = 1;
			}
			else 
			{
				continue;
			}
		}

		if (isOpcodeJcc(currentInstruction->opcode) || isOpcodeJmp(currentInstruction->opcode))
		{
			unsigned long long jumpAddr = resolveJmpChain(params, i);
			int instructionIndex = findInstructionByAddress(params->instructions, params->numOfInstructions, jumpAddr);
			if (instructionIndex > indexToJumpTo && instructionIndex > i && jumpAddr <= currentSectionEndAddress)
			{
				if (!checkForAddressInArrInRange(calledAddresses, numOfCalledAddresses, currentInstruction->address, jumpAddr))
				{
					indexToJumpTo = instructionIndex;
				}
			}
		}
		
		if ((checkForReturnStatement(params, i) && i >= indexToJumpTo) || 
			findAddressInArr(calledAddresses, numOfCalledAddresses, params->instructions[i + 1].address) != -1)
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
	}

	return 0;
}

void getAllFunctionReturnTypes(struct DecompilationParameters* params) 
{
	unsigned char setAReturnType = 0;
	do
	{
		setAReturnType = 0;
		for (int i = 0; i < params->numOfFunctions; i++)
		{
			params->currentFunc = &params->functions[i];
			if (params->currentFunc->returnReg != NO_REG || 
				params->currentFunc->callingConvention == __UNKNOWNCALL) // __UNKNOWNCALL will only be set at this point if the function ends without a return instruction
			{
				continue;
			}

			unsigned char wasZero = setAReturnType == 0;
			for (int j = params->currentFunc->firstInstructionIndex; j <= params->currentFunc->lastInstructionIndex; j++)
			{
				if (checkForReturnStatement(params, j))
				{
					if (params->currentFunc->returnReg != NO_REG)
					{
						if (!isRegInitialized(params, j, params->currentFunc->firstInstructionIndex, params->currentFunc->returnReg, 0, 0))
						{
							params->currentFunc->returnType.primitiveType = VOID_TYPE;
							params->currentFunc->returnReg = NO_REG;
							if (wasZero) { setAReturnType = 0; }
						}
						else 
						{
							continue;
						}
					}
					
					enum Register specificReg = NO_REG;
					struct DataType dataType = { 0 };
					if (isRegInitialized(params, j, params->currentFunc->firstInstructionIndex, AX, &specificReg, &dataType))
					{
						params->currentFunc->returnType = dataType;
						params->currentFunc->returnReg = specificReg;
						setAReturnType = 1;
					}
					else if (isRegInitialized(params, j, params->currentFunc->firstInstructionIndex, XMM0, 0, &dataType))
					{
						params->currentFunc->returnType = dataType;
						params->currentFunc->returnReg = XMM0;
						setAReturnType = 1;
					}
					else if (isRegInitialized(params, j, params->currentFunc->firstInstructionIndex, ST0, 0, 0))
					{
						params->currentFunc->returnType.primitiveType = FLOAT_TYPE;
						params->currentFunc->returnReg = ST0;
						setAReturnType = 1;
					}
				}
			}
		}
	} while (setAReturnType); // a function's return type may depend on another function
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

		if (!getFunctionRegArgsAndStackVars(params))
		{
			return 0;
		}

		setStackVarTypes(params->currentFunc, params->is64Bit);
	}

	return fixAllFunctionArgs(params);
}

static unsigned char getFunctionRegArgsAndStackVars(struct DecompilationParameters* params)
{
	for (int i = params->currentFunc->firstInstructionIndex; i <= params->currentFunc->lastInstructionIndex; i++)
	{
		struct DisassembledInstruction* currentInstruction = &params->instructions[i];

		if (doesInstructionDoNothing(currentInstruction))
		{
			continue;
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
			if (doesInstructionAccessRegister(params, i, j, &specificReg) && !getRegArgByReg(params->currentFunc, j))
			{
				if (!isRegInitialized(params, i - 1, params->currentFunc->firstInstructionIndex, j, 0, 0))
				{
					if (!addRegVar(params, 0, 1, specificReg))
					{
						return 0;
					}
				}
			}
		}

		// checking for stack vars
		for (int j = currentInstruction->numOfOperands - 1; j >= 0; j--)
		{
			struct Operand* currentOperand = &currentInstruction->operands[j];
			long long stackOffset = 0;
			if (currentOperand->type == MEM_ADDRESS && isMemAddressStackVar(params, i, &currentOperand->memoryAddress, &stackOffset)) 
			{
				unsigned char isStackVarInitialized = 0;
				doesInstructionModifyOperand(currentInstruction, j, &isStackVarInitialized);

				unsigned char isArgument = stackOffset > 0 && !isStackVarInitialized;
				if (!addStackVar(params->currentFunc, getOperandDataType(currentInstruction->opcode, currentOperand), isArgument, stackOffset))
				{
					return 0;
				}
			}
		}
	}

	return 1;
}

static unsigned char isRegInitialized(struct DecompilationParameters* params, int startInstructionIndex, int minInstructionIndex, enum Register reg, enum Register* specificReg, struct DataType* dataType)
{
	struct RegisterVariable* regArg = getRegArgByReg(params->currentFunc, reg);
	if (regArg)
	{
		if (specificReg) { *specificReg = regArg->reg; }
		if (dataType) { *dataType = regArg->dataType; }
		return 1;
	}
	
	for (int i = startInstructionIndex; i >= minInstructionIndex; i--)
	{
		unsigned char overwrites = 0;
		if (doesInstructionModifyRegister(params, i, reg, specificReg, &overwrites) && overwrites)
		{
			if (dataType) { *dataType = getRegisterDataType(params->instructions[i].opcode, specificReg ? *specificReg : reg); }
			return 1;
		}

		int conditionIndex = getConditionEnd(params, i);
		if (conditionIndex != -1)
		{
			struct Condition* cond = &params->currentFunc->conditions[conditionIndex];
			if (cond->conditionType == ELSE_CT)
			{
				if (isRegInitialized(params, cond->endIndex - 1, cond->startIndex, reg, specificReg, dataType))
				{
					int ifIndex = getConditionEnd(params, cond->startIndex);
					if (ifIndex != -1 && params->currentFunc->conditions[ifIndex].conditionType == IF_CT) // I will handle else ifs later
					{
						struct Condition* ifCond = &params->currentFunc->conditions[ifIndex];
						if (isRegInitialized(params, ifCond->endIndex - 1, ifCond->startIndex, reg, specificReg, dataType))
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

static unsigned char fixAllFunctionArgs(struct DecompilationParameters* params) // checks for arguments that aren't used in the function but are just passed to another function call
{
	unsigned char fixedAFunc = 0;
	for (int i = 0; i < params->numOfFunctions; i++)
	{
		params->currentFunc = &params->functions[i];
		for (int j = params->currentFunc->firstInstructionIndex; j <= params->currentFunc->lastInstructionIndex; j++)
		{
			unsigned long long calleeAddress = resolveJmpChain(params, j);
			if (calleeAddress != 0)
			{
				int funcIndex = findFunctionByAddress(params, calleeAddress);
				if (funcIndex == -1 || funcIndex == i)
				{
					continue;
				}

				struct Function* func = &params->functions[funcIndex];
				for (int k = 0; k < func->numOfRegVars; k++)
				{
					if (func->regVars[k].isArgument && !isRegInitialized(params, j - 1, params->currentFunc->firstInstructionIndex, func->regVars[k].reg, 0, 0))
					{
						if (!addRegVar(params, &func->regVars[k].dataType, 1, func->regVars[k].reg))
						{
							return 0;
						}

						fixedAFunc = 1;
					}
				}
			}
		}
	}

	if (fixedAFunc)
	{
		return fixAllFunctionArgs(params);
	}

	return 1;
}

static void setStackVarTypes(struct Function* function, unsigned char is64Bit)
{
	for (int i = 0; i < function->numOfStackVars - 1; i++) 
	{
		struct StackVariable* var1 = &function->stackVars[i];
		struct StackVariable* var2 = &function->stackVars[i + 1];
		if(var1->isArgument || var2->isArgument)
		{
			continue;
		}

		long long size = var2->stackOffset - var1->stackOffset;

		if (size % 8 == 0)
		{
			var1->dataType.primitiveType = LONG_LONG_TYPE;
			var1->dataType.arrayLen = size / 8;
		}
		else if (size % 4 == 0)
		{
			var1->dataType.primitiveType = INT_TYPE;
			var1->dataType.arrayLen = size / 4;
		}
		else if (size % 2 == 0)
		{
			var1->dataType.primitiveType = SHORT_TYPE;
			var1->dataType.arrayLen = size / 2;
		}
		else 
		{
			var1->dataType.primitiveType = CHAR_TYPE;
			var1->dataType.arrayLen = size;
		}
	}
}

void freeFunction(struct Function* function)
{
	freeJdcStr(&function->name);

	for (int i = 0; i < function->numOfRegVars; i++)
	{
		freeJdcStr(&function->regVars[i].name);
	}

	for (int i = 0; i < function->numOfStackVars; i++)
	{
		freeJdcStr(&function->stackVars[i].name);
	}

	for (int i = 0; i < function->numOfReturnedVars; i++)
	{
		freeJdcStr(&function->returnedVars[i].name);
	}

	free(function->regVars);
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
			int dstIndex = findInstructionByAddress(params->instructions, params->numOfInstructions, resolveJmpChain(params, i));
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
int findFunctionByAddress(struct DecompilationParameters* params, unsigned long long address)
{
	int low = 0;
	int high = params->numOfFunctions - 1;
	while (low <= high)
	{
		int mid = low + (high - low) / 2;

		if (params->instructions[params->functions[mid].firstInstructionIndex].address == address) { return mid; }

		if (params->instructions[params->functions[mid].firstInstructionIndex].address < address) { low = mid + 1; }
		else { high = mid - 1; }
	}

	return -1;
}

int findFunctionByAddressInclusive(struct DecompilationParameters* params, unsigned long long address)
{
	int low = 0;
	int high = params->numOfFunctions - 1;
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

unsigned char isMemAddressStackVar(struct DecompilationParameters* params, int instructionIndex, struct MemoryAddress* memAddress, long long* stackOffset)
{
	if (!memAddress || memAddress->regDisplacement != NO_REG || memAddress->scale > 1)
	{
		return 0;
	}

	if (compareRegisters(memAddress->reg, BP))
	{
		if (stackOffset) { *stackOffset = memAddress->constDisplacement; }
		return 1;
	}
	else if (compareRegisters(memAddress->reg, SP))
	{
		if (stackOffset) { *stackOffset = memAddress->constDisplacement - getStackFrameSizeAtInstruction(params, instructionIndex); }
		return 1;
	}
	else 
	{
		// this is a simple check for other registers that are set to the BP or SP
		for (int i = instructionIndex - 1; i >= params->currentFunc->firstInstructionIndex; i--) 
		{
			struct DisassembledInstruction* instruction = &params->instructions[i];
			if (instruction->opcode == MOV && instruction->numOfOperands == 2 &&
				instruction->operands[0].type == REGISTER && instruction->operands[1].type == REGISTER) 
			{
				if (compareRegisters(instruction->operands[0].reg, memAddress->reg))
				{
					if (compareRegisters(instruction->operands[1].reg, BP))
					{
						if (stackOffset) { *stackOffset = memAddress->constDisplacement; }
						return 1;
					}
					else if (compareRegisters(instruction->operands[1].reg, SP))
					{
						if (stackOffset) { *stackOffset = memAddress->constDisplacement - getStackFrameSizeAtInstruction(params, i); }
						return 1;
					}
					else 
					{
						return 0;
					}
				}
			}
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

int getNumOfStackArgs(struct Function* function)
{
	int result = 0;
	for (int i = 0; i < function->numOfStackVars; i++) 
	{
		if (function->stackVars[i].isArgument) 
		{
			result++;
		}
	}

	return result;
}

int getNumOfRegArgs(struct Function* function)
{
	int result = 0;
	for (int i = 0; i < function->numOfRegVars; i++)
	{
		if (function->regVars[i].isArgument)
		{
			result++;
		}
	}

	return result;
}

struct RegisterVariable* getRegArgByReg(struct Function* function, enum Register reg)
{
	for (int i = 0; i < function->numOfRegVars; i++)
	{
		if (function->regVars[i].isArgument && compareRegisters(function->regVars[i].reg, reg))
		{
			return &function->regVars[i];
		}
	}

	return 0;
}

struct RegisterVariable* getLocalRegVarByReg(struct Function* function, enum Register reg)
{
	for (int i = 0; i < function->numOfRegVars; i++)
	{
		if (!function->regVars[i].isArgument && compareRegisters(function->regVars[i].reg, reg))
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

unsigned char addStackVar(struct Function* function, struct DataType dataType, unsigned char isArgument, long long stackOffset)
{
	if (getStackVarByOffset(function, stackOffset))
	{
		return 1;
	}
	
	struct StackVariable* newStackVars = (struct StackVariable*)realloc(function->stackVars, sizeof(struct StackVariable) * (function->numOfStackVars + 1));
	if (!newStackVars)
	{
		return 0;
	}

	function->stackVars = newStackVars;
	function->stackVars[function->numOfStackVars].stackOffset = stackOffset;
	function->stackVars[function->numOfStackVars].dataType = dataType;
	function->stackVars[function->numOfStackVars].isArgument = isArgument;
	function->stackVars[function->numOfStackVars].name = initializeJdcStr();
	sprintfJdc(&(function->stackVars[function->numOfStackVars].name), 0, "%s%X", isArgument ? "arg" : "var", stackOffset < 0 ? -stackOffset : stackOffset);
	function->numOfStackVars++;

	// sorting from least to greatest stack offset
	for (int i = 0; i < function->numOfStackVars - 1; i++)
	{
		char swapped = 0;
		for (int j = 0; j < function->numOfStackVars - i - 1; j++)
		{
			if (function->stackVars[j].stackOffset > function->stackVars[j + 1].stackOffset)
			{
				struct StackVariable temp = function->stackVars[j];
				function->stackVars[j] = function->stackVars[j + 1];
				function->stackVars[j + 1] = temp;

				swapped = 1;
			}
		}
		if (!swapped) { break; }
	}

	return 1;
}

unsigned char addRegVar(struct DecompilationParameters* params, struct DataType* dataTypeRef, unsigned char isArgument, enum Register reg)
{
	if ((isArgument && getRegArgByReg(params->currentFunc, reg)) || (!isArgument && getLocalRegVarByReg(params->currentFunc, reg)))
	{
		return 1;
	}
	
	struct RegisterVariable* newRegVars = (struct RegisterVariable*)realloc(params->currentFunc->regVars, sizeof(struct RegisterVariable) * (params->currentFunc->numOfRegVars + 1));
	if (!newRegVars)
	{
		return 0;
	}

	params->currentFunc->regVars = newRegVars;
	struct RegisterVariable* regVar = &params->currentFunc->regVars[params->currentFunc->numOfRegVars];
	params->currentFunc->numOfRegVars++;

	regVar->reg = reg;
	regVar->isArgument = isArgument;

	if (!dataTypeRef) 
	{
		setRegVarDataType(params, regVar);
	}
	else 
	{
		regVar->dataType = *dataTypeRef;
	}

	regVar->name = initializeJdcStr();
	sprintfJdc(&regVar->name, 0, "%s%s", isArgument ? "arg" : "var", registerStrs[regVar->reg]);

	if (isArgument) 
	{
		// sorting
		for (int i = 0; i < params->currentFunc->numOfRegVars; i++) // all reg vars in the buffer should be args
		{
			for (int j = 0; j < NUM_PLATFORM_REG_ARGS; j++)
			{
				if ((compareRegisters(params->currentFunc->regVars[i].reg, platformRegArgs[j]) || compareRegisters(params->currentFunc->regVars[i].reg, altPlatformRegArgs[j])) && params->currentFunc->numOfRegVars > j)
				{
					struct RegisterVariable temp = params->currentFunc->regVars[j];
					params->currentFunc->regVars[j] = params->currentFunc->regVars[i];
					params->currentFunc->regVars[i] = temp;
					break;
				}
			}
		}

		if (params->currentFunc->callingConvention != __UNKNOWNCALL)
		{
			if (!isRegisterPlatformArg(reg))
			{
				params->currentFunc->callingConvention = __UNKNOWNCALL;
			}
			else if (params->currentFunc->numOfRegVars == 1 && compareRegisters(reg, CX))
			{
				params->currentFunc->callingConvention = __THISCALL;
			}
			else
			{
				params->currentFunc->callingConvention = __FASTCALL;
				for (int i = 0; i < params->currentFunc->numOfRegVars && i < NUM_PLATFORM_REG_ARGS; i++) // this checks that all reg args are present that should be. if platformRegArgs[1] is there but platformRegArgs[0] isn't then its wrong
				{
					if (!compareRegisters(params->currentFunc->regVars[i].reg, platformRegArgs[i]) && !compareRegisters(params->currentFunc->regVars[i].reg, altPlatformRegArgs[i]))
					{
						params->currentFunc->callingConvention = __UNKNOWNCALL;
					}
				}
			}
		}
	}

	return 1;
}

static void setRegVarDataType(struct DecompilationParameters* params, struct RegisterVariable* regVar)
{
	regVar->dataType = getRegisterDataType(NO_MNEMONIC, regVar->reg);
	for (int i = params->currentFunc->firstInstructionIndex; i <= params->currentFunc->lastInstructionIndex; i++) 
	{
		struct DisassembledInstruction* instruction = &params->instructions[i];
		for (int j = 0; j < instruction->numOfOperands; j++) 
		{
			struct Operand* operand = &instruction->operands[j];
			enum Register reg = NO_REG;
			if (operand->type == REGISTER) 
			{
				reg = operand->reg;
				
			}
			else if (operand->type == MEM_ADDRESS)
			{
				reg = operand->memoryAddress.reg;
				if (reg == NO_REG) 
				{
					reg = operand->memoryAddress.regDisplacement;
				}
			}

			if (compareRegisters(reg, regVar->reg))
			{
				struct DataType dataType = getRegisterDataType(instruction->opcode, reg);
				if (getDataTypeSize(dataType, params->is64Bit) > getDataTypeSize(regVar->dataType, params->is64Bit)) // the type size is set to the largest version of the register used
				{
					regVar->dataType.primitiveType = dataType.primitiveType;
					regVar->reg = reg;
				}

				if (operand->type == MEM_ADDRESS && operand->memoryAddress.constDisplacement == 0 && operand->memoryAddress.regDisplacement == NO_REG)
				{
					regVar->dataType.pointerLevel = 1;
				}

				if (dataType.isUnsigned)
				{
					regVar->dataType.isUnsigned = 1;
				}
			}
		}
	}
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