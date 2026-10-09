#include "functions.h"
#include "decompilationUtils.h"
#include "conditions.h"
#include "returnStatements.h"

enum JdcStatus findNextFunction(struct DecompilationParameters* params, struct Function* result, int32_t* instructionIndexRef)
{
	int32_t indexToJumpTo = 0;

	params->currentFunc = result;

	int32_t startInstructionIndex = *instructionIndexRef;

	uint64_t currentSectionEndAddress = 0;
	bool foundFirstInstruction = false;
	for (int32_t i = startInstructionIndex; i < params->numOfInstructions; i++)
	{
		(*instructionIndexRef)++;

		struct DisassembledInstruction* currentInstruction = &params->instructions[i];

		if (!foundFirstInstruction)
		{
			for (int32_t j = 0; j < params->numOfSections; j++)
			{
				if (currentInstruction->address >= params->sections[j].rva + params->imageBase && 
					currentInstruction->address < params->sections[j].rva + params->sections[j].physicalSize + params->imageBase)
				{
					currentSectionEndAddress = params->sections[j].rva + params->sections[j].physicalSize + params->imageBase;
					break;
				}
			}

			if (currentInstruction->isCalled ||
				(!doesInstructionGenerateInterruptOrException(currentInstruction) && !doesInstructionDoNothing(currentInstruction)))
			{
				result->firstInstructionIndex = i;
				foundFirstInstruction = true;
			}
			else 
			{
				continue;
			}
		}

		if (isOpcodeJcc(currentInstruction->opcode) || isOpcodeJmp(currentInstruction->opcode))
		{
			uint64_t jumpDst = getJmpDst(params, i);
			int32_t dstIndex = findInstructionByAddress(params->instructions, params->numOfInstructions, jumpDst);
			if (dstIndex > indexToJumpTo && dstIndex > i && jumpDst < currentSectionEndAddress)
			{
				for (int32_t j = i + 1; j < dstIndex; j++) 
				{
					if (params->instructions[j].isCalled) 
					{
						result->callingConvention = __UNKNOWNCALL;
						result->lastInstructionIndex = i;
						return SUCCESS_JDC;
					}
				}

				indexToJumpTo = dstIndex;
			}
		}
		
		if ((checkForReturnStatement(params, i) && i >= indexToJumpTo) || params->instructions[i + 1].isCalled)
		{
			result->lastInstructionIndex = i;
			return SUCCESS_JDC;
		}
		else if((doesInstructionGenerateInterruptOrException(currentInstruction) && i >= indexToJumpTo) ||
			i == params->numOfInstructions - 1 || params->instructions[i + 1].address >= currentSectionEndAddress)
		{
			result->callingConvention = __UNKNOWNCALL;
			result->lastInstructionIndex = i;
			return SUCCESS_JDC;
		}
	}

	return ERROR_JDC;
}

enum JdcStatus analyzeAllFunctions(struct DecompilationParameters* params)
{
	if (ERROR_JDC == getAllFunctionReturnTypesAndConditions(params)) 
	{
		return ERROR_JDC;
	}

	if (ERROR_JDC == getAllFunctionRegArgsAndStackVars(params))
	{
		return ERROR_JDC;
	}

	if (ERROR_JDC == fixAllFunctionArgs(params))
	{
		return ERROR_JDC;
	}

	if (ERROR_JDC == setAllStackVarTypes(params))
	{
		return ERROR_JDC;
	}

	return SUCCESS_JDC;
}

static enum JdcStatus getAllFunctionReturnTypesAndConditions(struct DecompilationParameters* params)
{
	bool setAReturnType = false;
	bool getConditions = true;
	do
	{
		setAReturnType = false;
		for (int32_t i = 0; i < params->numOfFunctions; i++)
		{
			params->currentFunc = &params->functions[i];
			if (getConditions && ERROR_JDC == getAllConditions(params))
			{
				return ERROR_JDC;
			}

			if (params->currentFunc->returnReg != NO_REG || 
				params->currentFunc->callingConvention == __UNKNOWNCALL) // __UNKNOWNCALL will only be set at this point if the function ends without a return instruction
			{
				continue;
			}

			// if the return reg is not initialized at all return statements, then it cant be the return reg
			bool canBeAX = true;
			bool canBeXMM0 = true;
			bool canBeST0 = true;

			bool wasZero = !setAReturnType;
			for (int32_t j = params->currentFunc->firstInstructionIndex; j <= params->currentFunc->lastInstructionIndex; j++)
			{
				if (checkForReturnStatement(params, j) || checkForConditionalReturn(params, j))
				{
					if (params->currentFunc->returnReg != NO_REG)
					{
						if (!isRegInitialized(params, j, params->currentFunc->firstInstructionIndex, params->currentFunc->returnReg, 0, 0))
						{
							if (compareRegisters(params->currentFunc->returnReg, AX)) 
							{
								canBeAX = false;
							}
							else if (compareRegisters(params->currentFunc->returnReg, XMM0))
							{
								canBeXMM0 = false;
							}
							else if (compareRegisters(params->currentFunc->returnReg, ST0))
							{
								canBeST0 = false;
							}

							params->currentFunc->returnType.primitiveType = VOID_TYPE;
							params->currentFunc->returnReg = NO_REG;
							if (wasZero) { setAReturnType = false; }
						}
						else 
						{
							continue;
						}
					}
					
					enum Register specificReg = NO_REG;
					struct DataType dataType = { 0 };
					if (canBeAX)
					{
						if (isRegInitialized(params, j, params->currentFunc->firstInstructionIndex, AX, &specificReg, &dataType))
						{
							params->currentFunc->returnType = dataType;
							params->currentFunc->returnReg = specificReg;
							setAReturnType = true;
							continue;
						}
						else 
						{
							canBeAX = false;
						}
					}

					if (canBeXMM0)
					{
						if (isRegInitialized(params, j, params->currentFunc->firstInstructionIndex, XMM0, 0, &dataType))
						{
							params->currentFunc->returnType = dataType;
							params->currentFunc->returnReg = XMM0;
							setAReturnType = true;
							continue;
						}
						else 
						{
							canBeXMM0 = false;
						}
					}

					if (canBeST0)
					{
						if (isRegInitialized(params, j, params->currentFunc->firstInstructionIndex, ST0, 0, 0))
						{
							params->currentFunc->returnType.primitiveType = FLOAT_TYPE;
							params->currentFunc->returnReg = ST0;
							setAReturnType = true;
							continue;
						}
						else 
						{
							canBeST0 = false;
						}
					}

					if(!canBeAX && !canBeXMM0 && !canBeST0)
					{
						break;
					}
				}
			}
		}

		getConditions = false;
	} while (setAReturnType); // a function's return type may depend on another function

	return SUCCESS_JDC;
}

static enum JdcStatus getAllFunctionRegArgsAndStackVars(struct DecompilationParameters* params)
{
	for (int32_t i = 0; i < params->numOfFunctions; i++) 
	{
		params->currentFunc = &params->functions[i];

		// stack vars
		for (int32_t j = params->currentFunc->firstInstructionIndex; j <= params->currentFunc->lastInstructionIndex; j++)
		{
			struct DisassembledInstruction* instruction = &params->instructions[j];
			for (int32_t k = 0; k < instruction->numOfOperands; k++)
			{
				struct Operand* currentOperand = &instruction->operands[k];
				int64_t offsetFromInitSP = 0;
				if (currentOperand->type == MEM_ADDRESS && !doesInstructionModifyOperand(params, j, k, 0) &&
					isMemAddressStackVar(params, j, &currentOperand->memoryAddress, &offsetFromInitSP))
				{
					bool isArgument = offsetFromInitSP > 0;
					if (isArgument && isStackVarInitialized(params, j - 1, params->currentFunc->firstInstructionIndex, offsetFromInitSP))
					{
						isArgument = false;
					}

					struct DataType dataType = getMemoryAddressDataType(instruction->opcode, &currentOperand->memoryAddress);
					if (ERROR_JDC == addStackVar(params->currentFunc, offsetFromInitSP, isArgument, &dataType))
					{
						return ERROR_JDC;
					}
				}
			}
		}

		// reg args, this is a separate loop becaues of the PUSH stack var check
		for (int32_t j = params->currentFunc->firstInstructionIndex; j <= params->currentFunc->lastInstructionIndex; j++)
		{
			struct DisassembledInstruction* instruction = &params->instructions[j];

			int64_t offsetFromInitSP = 0;
			if (instruction->opcode == PUSH && instruction->operands[0].type == REGISTER)
			{
				continue;
			}
			else if (instruction->numOfOperands > 0 && // checking for stack var that is used as a "PUSH" only to save the reg
				instruction->operands[0].type == MEM_ADDRESS && doesInstructionModifyOperand(params, j, 0, 0) &&
				isMemAddressStackVar(params, j, &instruction->operands[0].memoryAddress, &offsetFromInitSP) &&
				!getStackVarByOffset(params->currentFunc, offsetFromInitSP))
			{
				continue;
			}

			for (int32_t k = RAX; k < ST0; k++)
			{
				if (k == RBP || k == RSP || k == RIP)
				{
					continue;
				}

				bool overwrites = false;
				enum Register specificReg = NO_REG;
				if (doesInstructionAccessRegister(params, j, k, 0, &specificReg) && !getRegArgByReg(params->currentFunc, k))
				{
					if (!isRegInitialized(params, j - 1, params->currentFunc->firstInstructionIndex, k, 0, 0))
					{
						if (ERROR_JDC == addRegVar(params, 0, 1, specificReg))
						{
							return ERROR_JDC;
						}
					}
				}
			}
		}
	}

	return SUCCESS_JDC;
}

static bool isRegInitialized(struct DecompilationParameters* params, int32_t startInstructionIndex, int32_t minInstructionIndex, enum Register reg, enum Register* specificReg, struct DataType* dataType)
{
	struct RegisterVariable* regArg = getRegArgByReg(params->currentFunc, reg);
	if (regArg)
	{
		if (specificReg) { *specificReg = regArg->reg; }
		if (dataType) { *dataType = regArg->dataType; }
		return true;
	}
	
	for (int32_t i = startInstructionIndex; i >= minInstructionIndex; i--)
	{
		struct Condition* cond = getConditionFromLastBodyInstruction(params, i);
		if (cond && i != startInstructionIndex)
		{
			if (cond->conditionType == ELSE_CT)
			{
				struct Condition* currentCond = cond;
				bool isRegInitializedInAllCases = true;
				while(currentCond->connectedUpperConditionIndex != -1)
				{
					if (!isRegInitialized(params, currentCond->lastBodyIndex, currentCond->firstBodyIndex, reg, specificReg, dataType))
					{
						isRegInitializedInAllCases = false;
						break;
					}

					currentCond = &params->currentFunc->conditions[currentCond->connectedUpperConditionIndex];
				}

				if (isRegInitializedInAllCases) 
				{
					return true;
				}
			}

			i = getConditionChainFirstBodyInstruction(params, cond);
			continue;
		}

		bool overwrites = false;
		if (doesInstructionModifyRegister(params, i, reg, specificReg, &overwrites) && overwrites)
		{
			if (dataType) { *dataType = getRegisterDataType(&params->instructions[i], -1, specificReg ? *specificReg : reg); }
			return true;
		}
	}

	return false;
}

static bool isStackVarInitialized(struct DecompilationParameters* params, int32_t startInstructionIndex, int32_t minInstructionIndex, int64_t offsetFromInitSP)
{
	struct StackVariable* stackArg = getStackVarByOffset(params->currentFunc, offsetFromInitSP);
	if (stackArg)
	{
		return true;
	}

	for (int32_t i = startInstructionIndex; i >= minInstructionIndex; i--)
	{
		struct Condition* cond = getConditionFromLastBodyInstruction(params, i);
		if (cond && i != startInstructionIndex)
		{
			if (cond->conditionType == ELSE_CT)
			{
				struct Condition* currentCond = cond;
				bool isVarInitializedInAllCases = true;
				while (currentCond->connectedUpperConditionIndex != -1)
				{
					if (!isStackVarInitialized(params, currentCond->lastBodyIndex, currentCond->firstBodyIndex, offsetFromInitSP))
					{
						isVarInitializedInAllCases = false;
						break;
					}

					currentCond = &params->currentFunc->conditions[currentCond->connectedUpperConditionIndex];
				}

				if (isVarInitializedInAllCases)
				{
					return true;
				}
			}

			i = getConditionChainFirstBodyInstruction(params, cond);
			continue;
		}

		struct DisassembledInstruction* instruction = &params->instructions[i];
		for (int32_t j = 0; j < instruction->numOfOperands; j++)
		{
			struct Operand* currentOperand = &instruction->operands[j];
			int64_t currentOffsetFromInitSP = 0;
			bool overwrites = false;
			if (currentOperand->type == MEM_ADDRESS &&
				doesInstructionModifyOperand(params, i, j, &overwrites) && overwrites &&
				isMemAddressStackVar(params, i, &currentOperand->memoryAddress, &currentOffsetFromInitSP) && 
				currentOffsetFromInitSP == offsetFromInitSP)
			{
				return true;
			}
		}
	}

	return false;
}

static enum JdcStatus fixAllFunctionArgs(struct DecompilationParameters* params) // checks for arguments that aren't used in the function but are just passed to another function call
{
	for (int32_t i = 0; i < params->numOfFunctions; i++)
	{
		params->currentFunc = &params->functions[i];
		for (int32_t j = params->currentFunc->firstInstructionIndex; j <= params->currentFunc->lastInstructionIndex; j++)
		{
			uint64_t calleeAddress = resolveJmpChain(params, j);
			if (calleeAddress != 0)
			{
				int32_t calleeIndex = findFunctionByAddress(params, calleeAddress);
				if (calleeIndex == -1 || calleeIndex == i)
				{
					continue;
				}

				struct Function* callee = &params->functions[calleeIndex];
				if (callee->callingConvention == __UNKNOWNCALL) 
				{
					continue;
				}

				for (int32_t k = 0; k < callee->numOfRegVars; k++)
				{
					struct RegisterVariable* regArg = &callee->regVars[k];
					if (regArg->isArgument && !isRegInitialized(params, j - 1, params->currentFunc->firstInstructionIndex, regArg->reg, 0, 0))
					{
						if (ERROR_JDC == addRegVar(params, &regArg->dataType, 1, regArg->reg))
						{
							return ERROR_JDC;
						}
					}
				}

				for (int32_t k = 0; k < callee->numOfStackVars; k++)
				{
					struct StackVariable* stackArg = &callee->stackVars[k];
					int64_t stackFrameSize = 0;
					if (stackArg->isArgument && !getStackArgInitializer(params, j, stackArg->offsetFromInitSP, 0, 0, &stackFrameSize))
					{
						bool isArgument = (stackArg->offsetFromInitSP - stackFrameSize) > 0;
						if (ERROR_JDC == addStackVar(params->currentFunc, stackArg->offsetFromInitSP - stackFrameSize, isArgument, &stackArg->dataType))
						{
							return ERROR_JDC;
						}
					}
				}
			}
		}
	}

	return SUCCESS_JDC;
}

bool getStackArgInitializer(struct DecompilationParameters* params, int32_t callInstructionIndex, int64_t stackArgOffset, struct StackVariable** stackVarRef, int32_t* pushInstructionRef, int64_t* stackFrameSizeRef)
{
	if (stackVarRef) { *stackVarRef = 0; }
	if (pushInstructionRef) { *pushInstructionRef = -1; }

	int64_t initialStackFrameSize = getStackFrameSizeAtInstruction(params, callInstructionIndex);
	if (params->instructions[callInstructionIndex].opcode == CALL_NEAR) 
	{
		initialStackFrameSize += params->is64Bit ? 8 : 4; // instruction ptr is pushed
	}

	if (stackFrameSizeRef) { *stackFrameSizeRef = initialStackFrameSize; }

	for (int32_t i = 0; i < params->currentFunc->numOfStackVars; i++)
	{
		struct StackVariable* stackVar = &params->currentFunc->stackVars[i];
		if (stackArgOffset - initialStackFrameSize == stackVar->offsetFromInitSP)
		{
			if (stackVarRef) { *stackVarRef = stackVar; }
			return true;
		}
	}

	int64_t currentStackFrameSize = initialStackFrameSize;
	for (int32_t i = callInstructionIndex - 1; i >= params->currentFunc->firstInstructionIndex; i--)
	{
		struct Condition* condition = getConditionFromLastBodyInstruction(params, i);
		if (condition)
		{
			i = getConditionChainFirstBodyInstruction(params, condition);
			continue;
		}
		
		struct DisassembledInstruction* instruction = &(params->instructions[i]);

		currentStackFrameSize -= getStackFrameChange(instruction);

		if (instruction->opcode == PUSH && initialStackFrameSize - currentStackFrameSize == stackArgOffset)
		{
			if (pushInstructionRef) { *pushInstructionRef = i; }
			return true;
		}
	}

	return false;
}

static enum JdcStatus setAllStackVarTypes(struct DecompilationParameters* params)
{
	for (int32_t i = 0; i < params->numOfFunctions; i++) 
	{
		params->currentFunc = &params->functions[i];

		// sorting from least to greatest stack offset
		bool keepSorting = true;
		while (keepSorting)
		{
			keepSorting = false;
			for (int32_t j = 0; j < params->currentFunc->numOfStackVars - 1; j++)
			{
				if (params->currentFunc->stackVars[j].offsetFromInitSP > params->currentFunc->stackVars[j + 1].offsetFromInitSP)
				{
					struct StackVariable temp = params->currentFunc->stackVars[j];
					params->currentFunc->stackVars[j] = params->currentFunc->stackVars[j + 1];
					params->currentFunc->stackVars[j + 1] = temp;
					keepSorting = true;
				}
			}
		}

		for (int32_t j = params->currentFunc->firstInstructionIndex; j <= params->currentFunc->lastInstructionIndex; j++)
		{
			struct DisassembledInstruction* instruction = &params->instructions[j];
			for (int32_t k = 0; k < instruction->numOfOperands; k++)
			{
				struct Operand* operand = &instruction->operands[k];
				int64_t offsetFromInitSP = 0;
				if (operand->type == MEM_ADDRESS && isMemAddressStackVar(params, j, &operand->memoryAddress, &offsetFromInitSP))
				{
					struct StackVariable* stackVar = getStackVarByOffset(params->currentFunc, offsetFromInitSP);
					if (!stackVar)
					{
						continue;
					}

					if (operand->memoryAddress.ptrSize < getPrimitiveTypeSize(stackVar->dataType.primitiveType)) // the smallest ptr size is used incase this is an array
					{
						stackVar->dataType.primitiveType = getOperandDataType(instruction->opcode, operand).primitiveType;
					}

					if (doesInstructionModifyOperand(params, j, k, 0) && stackVar->isArgument)
					{
						stackVar->dataType.pointerLevel = 1; // all stack vars could be treated as pointers since they are memory addresses, but it is only necesary for arguments that are modified because they can be accessed outside the function 
					}
				}
			}
		}

		for (int32_t j = 0; j < params->currentFunc->numOfStackVars - 1; j++)
		{
			struct StackVariable* var1 = &params->currentFunc->stackVars[j];
			struct StackVariable* var2 = &params->currentFunc->stackVars[j + 1];
			if (var2->offsetFromInitSP < var1->offsetFromInitSP) 
			{
				return ERROR_JDC; // they should be sorted at this point
			}

			uint16_t offsetDif = (uint16_t)(var2->offsetFromInitSP - var1->offsetFromInitSP);
			uint8_t primitiveTypeSize = getPrimitiveTypeSize(var1->dataType.primitiveType);
			if (!var1->isArgument && offsetDif > primitiveTypeSize && primitiveTypeSize != 0)
			{
				if (offsetDif % primitiveTypeSize == 0) 
				{
					var1->dataType.arrayLen = offsetDif / primitiveTypeSize;
				}
				else
				{
					var1->dataType.primitiveType = INT8_TYPE;
					var1->dataType.arrayLen = offsetDif;
				}
				
				var1->dataType.pointerLevel = 0;
			}
		}
	}

	return SUCCESS_JDC;
}

void freeFunction(struct Function* function)
{
	freeJdcStr(&function->name);

	for (int32_t i = 0; i < function->numOfRegVars; i++)
	{
		freeJdcStr(&function->regVars[i].name);
	}

	for (int32_t i = 0; i < function->numOfStackVars; i++)
	{
		freeJdcStr(&function->stackVars[i].name);
	}

	for (int32_t i = 0; i < function->numOfReturnedVars; i++)
	{
		freeJdcStr(&function->returnedVars[i].name);
	}

	free(function->regVars);
	free(function->stackVars);
	free(function->returnedVars);

	for (int32_t i = 0; i < function->numOfConditions; i++)
	{
		free(function->conditions[i].combinedJccIndexes);
	}
	free(function->conditions);
	free(function->directJmps);

	for (int32_t i = 0; i < function->numOfLines; i++) 
	{
		free(function->associatedInstructions[i].indexes);
	}
	free(function->associatedInstructions);
}

static int64_t getStackFrameChange(struct DisassembledInstruction* instruction) 
{	
	if (instruction->numOfOperands == 0)
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
		return instruction->operandSizeAttribute;
	}
	else if (instruction->opcode == POP)
	{
		return -instruction->operandSizeAttribute;
	}

	return 0;
}

int64_t getStackFrameSizeAtInstruction(struct DecompilationParameters* params, int32_t instructionIndex)
{
	int64_t result = 0;
	for (int32_t i = params->currentFunc->firstInstructionIndex; i < instructionIndex; i++)
	{
		struct DisassembledInstruction* instruction = &params->instructions[i];
		if (isOpcodeJcc(instruction->opcode) || isOpcodeJmp(instruction->opcode))
		{
			int32_t dstIndex = findInstructionByAddress(params->instructions, params->numOfInstructions, resolveJmpChain(params, i));
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
int32_t findFunctionByAddress(struct DecompilationParameters* params, uint64_t address)
{
	int32_t low = 0;
	int32_t high = params->numOfFunctions - 1;
	while (low <= high)
	{
		int32_t mid = low + (high - low) / 2;

		if (params->instructions[params->functions[mid].firstInstructionIndex].address == address) { return mid; }

		if (params->instructions[params->functions[mid].firstInstructionIndex].address < address) { low = mid + 1; }
		else { high = mid - 1; }
	}

	return -1;
}

int32_t findFunctionByAddressInclusive(struct DecompilationParameters* params, uint64_t address)
{
	int32_t low = 0;
	int32_t high = params->numOfFunctions - 1;
	while (low <= high)
	{
		int32_t mid = low + (high - low) / 2;

		uint64_t firstAddress = params->instructions[params->functions[mid].firstInstructionIndex].address;
		uint64_t lastAddress = params->instructions[params->functions[mid].lastInstructionIndex].address;

		if (address >= firstAddress && address <= lastAddress) { return mid; }

		if (address > lastAddress) { low = mid + 1; }
		else { high = mid - 1; }
	}

	return -1;
}

bool isMemAddressStackVar(struct DecompilationParameters* params, int32_t instructionIndex, struct MemoryAddress* memAddress, int64_t* offsetFromInitSP)
{
	if (!memAddress)
	{
		return false;
	}

	if (compareRegisters(memAddress->reg, SP))
	{
		if (offsetFromInitSP) { *offsetFromInitSP = memAddress->constDisplacement - getStackFrameSizeAtInstruction(params, instructionIndex); }
		return true;
	}

	// this is a simple check for other registers that are set to the SP, usually the BP
	for (int32_t i = instructionIndex - 1; i >= params->currentFunc->firstInstructionIndex; i--)
	{
		struct DisassembledInstruction* instruction = &params->instructions[i];
		if (instruction->opcode == MOV && instruction->numOfOperands == 2 &&
			instruction->operands[0].type == REGISTER && instruction->operands[1].type == REGISTER)
		{
			if (compareRegisters(instruction->operands[0].reg, memAddress->reg))
			{
				if (compareRegisters(instruction->operands[1].reg, SP))
				{
					if (offsetFromInitSP) { *offsetFromInitSP = memAddress->constDisplacement - getStackFrameSizeAtInstruction(params, i); }
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}

	if (compareRegisters(memAddress->reg, BP))
	{
		// if mov BP, SP is not found, the BP is assumed to be the initial SP value
		if (offsetFromInitSP) { *offsetFromInitSP = memAddress->constDisplacement; }
		return true;
	}

	return false;
}

struct StackVariable* getStackVarByOffset(struct Function* function, int64_t offsetFromInitSP)
{
	for (int32_t i = 0; i < function->numOfStackVars; i++)
	{
		if (function->stackVars[i].offsetFromInitSP == offsetFromInitSP)
		{
			return &function->stackVars[i];
		}
	}

	return 0;
}

int32_t getNumOfStackArgs(struct Function* function)
{
	int32_t result = 0;
	for (int32_t i = 0; i < function->numOfStackVars; i++) 
	{
		if (function->stackVars[i].isArgument) 
		{
			result++;
		}
	}

	return result;
}

int32_t getNumOfRegArgs(struct Function* function)
{
	int32_t result = 0;
	for (int32_t i = 0; i < function->numOfRegVars; i++)
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
	for (int32_t i = 0; i < function->numOfRegVars; i++)
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
	for (int32_t i = 0; i < function->numOfRegVars; i++)
	{
		if (!function->regVars[i].isArgument && compareRegisters(function->regVars[i].reg, reg))
		{
			return &function->regVars[i];
		}
	}

	return 0;
}

struct ReturnedVariable* findReturnedVar(struct Function* function, uint64_t callInstructionAddress)
{
	for (int32_t i = 0; i < function->numOfReturnedVars; i++)
	{
		if (function->returnedVars[i].callInstructionAddress == callInstructionAddress)
		{
			return &function->returnedVars[i];
		}
	}

	return 0;
}

static enum JdcStatus addStackVar(struct Function* function, int64_t offsetFromInitSP, bool isArgument, struct DataType* dataTypeRef)
{
	if (getStackVarByOffset(function, offsetFromInitSP))
	{
		return SUCCESS_JDC;
	}
	
	struct StackVariable* newStackVars = (struct StackVariable*)realloc(function->stackVars, sizeof(struct StackVariable) * (function->numOfStackVars + 1));
	if (!newStackVars)
	{
		return ERROR_JDC;
	}

	function->stackVars = newStackVars;
	struct StackVariable* stackVar = &function->stackVars[function->numOfStackVars];
	function->numOfStackVars++;

	stackVar->offsetFromInitSP = offsetFromInitSP;
	stackVar->isArgument = isArgument;
	stackVar->name = initializeJdcStr();
	sprintfJdc(&(stackVar->name), false, "%s%X", isArgument ? "arg" : "var", offsetFromInitSP < 0 ? -offsetFromInitSP : offsetFromInitSP);

	if (dataTypeRef)
	{
		stackVar->dataType = *dataTypeRef;
	}

	return SUCCESS_JDC;
}

enum JdcStatus addRegVar(struct DecompilationParameters* params, struct DataType* dataTypeRef, bool isArgument, enum Register reg)
{
	if ((isArgument && getRegArgByReg(params->currentFunc, reg)) || (!isArgument && getLocalRegVarByReg(params->currentFunc, reg)))
	{
		return SUCCESS_JDC;
	}
	
	struct RegisterVariable* newRegVars = (struct RegisterVariable*)realloc(params->currentFunc->regVars, sizeof(struct RegisterVariable) * (params->currentFunc->numOfRegVars + 1));
	if (!newRegVars)
	{
		return ERROR_JDC;
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

	// these are only used for local reg vars
	regVar->scopes = 0;
	regVar->numOfScopes = 0;

	regVar->name = initializeJdcStr();
	sprintfJdc(&regVar->name, false, "%s%s", isArgument ? "arg" : "var", registerStrs[regVar->reg]);

	if (isArgument) 
	{
		int32_t numOfPlatformRegArgs = getNumOfPlatformRegArgs(params->fileFormat);
		const enum Register* platformRegArgs = getPlatformRegArgs(params->fileFormat);
		const enum Register* altPlatformRegArgs = getAltPlatformRegArgs(params->fileFormat);

		// sorting
		for (int32_t i = 0; i < params->currentFunc->numOfRegVars; i++) // all reg vars in the buffer should be args
		{
			for (int32_t j = 0; j < numOfPlatformRegArgs; j++)
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
			if (!isRegisterPlatformArg(reg, params->fileFormat))
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
				for (int32_t i = 0; i < params->currentFunc->numOfRegVars && i < numOfPlatformRegArgs; i++) // this checks that all reg args are present that should be. if platformRegArgs[1] is there but platformRegArgs[0] isn't then its wrong
				{
					if (!compareRegisters(params->currentFunc->regVars[i].reg, platformRegArgs[i]) && !compareRegisters(params->currentFunc->regVars[i].reg, altPlatformRegArgs[i]))
					{
						params->currentFunc->callingConvention = __UNKNOWNCALL;
					}
				}
			}
		}
	}

	return SUCCESS_JDC;
}

enum JdcStatus addRegVarScope(struct RegisterVariable* regVar, int32_t startIndex, int32_t endIndex)
{
	struct RegVarScope* newScopes = (struct RegVarScope*)realloc(regVar->scopes, (regVar->numOfScopes + 1) * sizeof(struct RegVarScope));
	if(!newScopes)
	{
		return ERROR_JDC;
	}

	regVar->scopes = newScopes;
	regVar->scopes[regVar->numOfScopes].startIndex = startIndex;
	regVar->scopes[regVar->numOfScopes].endIndex = endIndex;
	regVar->numOfScopes++;

	return SUCCESS_JDC;
}

static void setRegVarDataType(struct DecompilationParameters* params, struct RegisterVariable* regVar)
{
	bool foundFirstInstance = false;
	regVar->dataType = getRegisterDataType(0, -1, regVar->reg);
	for (int32_t i = params->currentFunc->firstInstructionIndex; i <= params->currentFunc->lastInstructionIndex; i++) 
	{
		struct DisassembledInstruction* instruction = &params->instructions[i];
		for (int32_t j = 0; j < instruction->numOfOperands; j++) 
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
				if (!compareRegisters(reg, regVar->reg))
				{
					reg = operand->memoryAddress.regDisplacement;
				}
			}

			if (compareRegisters(reg, regVar->reg))
			{
				if (operand->type == MEM_ADDRESS && operand->memoryAddress.constDisplacement == 0 && operand->memoryAddress.regDisplacement == NO_REG)
				{
					regVar->dataType = getMemoryAddressDataType(instruction->opcode, &operand->memoryAddress);
					regVar->dataType.pointerLevel = 1;
					return;
				}
				
				struct DataType dataType = getRegisterDataType(instruction, j, reg);
				if (getDataTypeSize(dataType, params->is64Bit) > getDataTypeSize(regVar->dataType, params->is64Bit) || !foundFirstInstance) // the type size is set to the largest version of the register used
				{
					regVar->dataType.primitiveType = getIntType(getPrimitiveTypeSize(dataType.primitiveType), !isPrimitiveUnsigned(regVar->dataType.primitiveType));
					regVar->reg = reg;
					foundFirstInstance = true;
				}

				if (isPrimitiveUnsigned(dataType.primitiveType))
				{
					regVar->dataType.primitiveType = getIntType(getPrimitiveTypeSize(dataType.primitiveType), false);
				}
			}
		}
	}
}

enum JdcStatus addReturnedVar(struct Function* function, struct DataType dataType, uint64_t calleeAddress, uint64_t callInstructionAddress, enum Register returnReg, const char* calleeName)
{
	if (findReturnedVar(function, callInstructionAddress))
	{
		return SUCCESS_JDC;
	}
	
	struct ReturnedVariable* newReturnedVars = (struct ReturnedVariable*)realloc(function->returnedVars, sizeof(struct ReturnedVariable) * (function->numOfReturnedVars + 1));
	if (!newReturnedVars)
	{
		return ERROR_JDC;
	}

	function->returnedVars = newReturnedVars;

	int32_t callNum = 0;
	for (int32_t i = 0; i < function->numOfReturnedVars; i++)
	{
		if (function->returnedVars[i].calleeAddress == calleeAddress)
		{
			callNum++;
		}
	}

	function->returnedVars[function->numOfReturnedVars].dataType = dataType;

	function->returnedVars[function->numOfReturnedVars].name = initializeJdcStr();
	sprintfJdc(&(function->returnedVars[function->numOfReturnedVars].name), false, "%sRetVal%d", calleeName, callNum);
	replaceJdc(&(function->returnedVars[function->numOfReturnedVars].name), "::", "_");

	function->returnedVars[function->numOfReturnedVars].calleeAddress = calleeAddress;
	function->returnedVars[function->numOfReturnedVars].callInstructionAddress = callInstructionAddress;
	function->returnedVars[function->numOfReturnedVars].returnReg = returnReg;
	function->numOfReturnedVars++;

	return SUCCESS_JDC;
}

enum JdcStatus addAssociatedInstruction(struct Function* function, int32_t instructionIndex)
{
	if (instructionIndex == -1) 
	{
		return SUCCESS_JDC;
	}
	
	if (function->numOfLines >= function->associatedInstructionsBufferLen) 
	{
		int32_t ogBufferLen = function->associatedInstructionsBufferLen;
		function->associatedInstructionsBufferLen = function->numOfLines + 10;
		struct AssociatedInstructions* newAssociatedInstructions = (struct AssociatedInstructions*)realloc(function->associatedInstructions, function->associatedInstructionsBufferLen * sizeof(struct AssociatedInstructions));
		if (!newAssociatedInstructions) 
		{
			return ERROR_JDC;
		}

		function->associatedInstructions = newAssociatedInstructions;
		memset(function->associatedInstructions + ogBufferLen, 0, (function->associatedInstructionsBufferLen - ogBufferLen) * sizeof(struct AssociatedInstructions));
	}
	
	struct AssociatedInstructions* a = &function->associatedInstructions[function->numOfLines];
	if (!a) 
	{
		return ERROR_JDC;
	}

	int32_t* newIndexes = (int32_t*)realloc(a->indexes, (a->numOfIndexes + 1) * sizeof(int32_t));
	if (!newIndexes)
	{
		return ERROR_JDC;
	}

	a->indexes = newIndexes;
	a->indexes[a->numOfIndexes] = instructionIndex;
	a->numOfIndexes++;

	return SUCCESS_JDC;
}