#include "expressions.h"
#include "functionCalls.h"
#include "dataTypes.h"

unsigned char decompileOperand(struct DecompilationParameters params, struct Operand* operand, enum PrimitiveType type, char* resultBuffer, unsigned char resultBufferSize)
{
	if (operand->type == IMMEDIATE)
	{
		if (!getValueFromDataSection(params, type, operand->immediate, resultBuffer))
		{
			if (operand->immediate > -10 && operand->immediate < 10) // this is just arbitrary
			{
				sprintf(resultBuffer, "%llu", operand->immediate);
			}
			else 
			{
				sprintf(resultBuffer, "0x%llX", operand->immediate);
			}
		}
		
		return 1;
	}
	else if (operand->type == MEM_ADDRESS)
	{
		if (compareRegisters(operand->memoryAddress.reg, BP) || compareRegisters(operand->memoryAddress.reg, SP))
		{
			if (operand->memoryAddress.constDisplacement < 0)
			{
				struct StackVariable* localVar = getLocalVarByOffset(params.currentFunc, (int)(operand->memoryAddress.constDisplacement));
				if (localVar)
				{
					strcpy(resultBuffer, localVar->name);
				}
				else
				{
					return 0;
				}
			}
			else
			{
				struct StackVariable* stackArg = getStackArgByOffset(params.currentFunc, (int)(operand->memoryAddress.constDisplacement));
				if (stackArg)
				{
					strcpy(resultBuffer, stackArg->name);
				}
				else
				{
					return 0;
				}
			}
		}
		else if (compareRegisters(operand->memoryAddress.reg, IP))
		{
			sprintf(resultBuffer, "*(%s*)(0x%llX)", primitiveTypeStrs[type], params.currentFunc->addresses[params.startInstructionIndex + 1] + operand->memoryAddress.constDisplacement);
		}
		else if (operand->memoryAddress.reg == NO_REG)
		{
			if (!getValueFromDataSection(params, type, operand->memoryAddress.constDisplacement, resultBuffer))
			{
				sprintf(resultBuffer, "*(%s*)(0x%llX)", primitiveTypeStrs[type], operand->memoryAddress.constDisplacement);
			}
		}
		else
		{
			struct Operand baseReg = { 0 };
			baseReg.type = REGISTER;
			baseReg.reg = operand->memoryAddress.reg;

			params.startInstructionIndex--;
			char baseOperandStr[255] = { 0 };
			if (!decompileOperand(params, &baseReg, type, baseOperandStr, 255))
			{
				return 0;
			}

			if (operand->memoryAddress.constDisplacement != 0)
			{
				sprintf(resultBuffer, "*(%s*)(%s + 0x%llX)", primitiveTypeStrs[type], baseOperandStr, operand->memoryAddress.constDisplacement);
			}
			else
			{
				sprintf(resultBuffer, "*(%s*)(%s)", primitiveTypeStrs[type], baseOperandStr);
			}
		}


		return 1;
	}
	else if (operand->type == REGISTER)
	{
		if (compareRegisters(operand->reg, BP) || compareRegisters(operand->reg, SP))
		{
			strcpy(resultBuffer, registerStrs[operand->reg]);
			return 1;
		}
		else if (compareRegisters(operand->reg, IP))
		{
			sprintf(resultBuffer, "0x%llX", params.currentFunc->addresses[params.startInstructionIndex + 1]);
			return 1;
		}

		if (!decompileRegister(params, operand->reg, type, resultBuffer, resultBufferSize))
		{
			// register argument
			struct RegisterVariable* regArg = getRegArgByReg(params.currentFunc, operand->reg);
			if (regArg)
			{
				strcpy(resultBuffer, regArg->name);
				return 1;
			}

			return 0;
		}

		return 1;
	}

	return 0;
}

static unsigned char getValueFromDataSection(struct DecompilationParameters params, enum PrimitiveType type, unsigned long long address, char* resultBuffer)
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

	// checking for pointer to see if it is a string
	if (type == INT_TYPE || type == LONG_LONG_TYPE) 
	{
		char isString = 1;
		
		int len = 0;
		char byte = 0;
		do
		{
			byte = *(char*)(params.dataSectionByte + len + dataSectionIndex);
			
			if (byte == 0) 
			{
				break;
			}
			else if(len > 100 || byte > 126) // checking if byte is ascii and len isn't too long
			{
				isString = 0;
				break;
			}

			len++;
		} while (1);

		if (isString && len > 0)
		{
			sprintf(resultBuffer, "\"%s\"", params.dataSectionByte + dataSectionIndex);
			return 1;
		}
	}

	switch (type) 
	{
	case CHAR_TYPE:
		sprintf(resultBuffer, "%d", *(char*)(params.dataSectionByte + dataSectionIndex));
		break;
	case SHORT_TYPE:
		sprintf(resultBuffer, "%d", *(short*)(params.dataSectionByte + dataSectionIndex));
		break;
	case INT_TYPE:
		sprintf(resultBuffer, "%d", *(int*)(params.dataSectionByte + dataSectionIndex));
		break;
	case LONG_LONG_TYPE:
		sprintf(resultBuffer, "%lld", *(long long*)(params.dataSectionByte + dataSectionIndex));
		break;
	case FLOAT_TYPE:
		sprintf(resultBuffer, "%f", *(float*)(params.dataSectionByte + dataSectionIndex));
		break;
	case DOUBLE_TYPE:
		sprintf(resultBuffer, "%lf", *(double*)(params.dataSectionByte + dataSectionIndex));
		break;
	}

	return 1;
}

static unsigned char decompileRegister(struct DecompilationParameters params, enum Register targetReg, enum PrimitiveType type, char* resultBuffer, unsigned char resultBufferSize)
{
	char expressions[5][255] = { 0 };
	int expressionIndex = 0;

	unsigned char finished = 0;
	unsigned char isInUnreachableState = 0;

	for (int i = params.startInstructionIndex; i >= 0; i--)
	{
		if (finished)
		{
			break;
		}

		struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[i]);

		if (currentInstruction->opcode == RET_NEAR || currentInstruction->opcode == RET_FAR || currentInstruction->opcode == JMP_SHORT)
		{
			isInUnreachableState = 1;
		}
		else if (currentInstruction->opcode >= JA_SHORT && currentInstruction->opcode < JMP_SHORT) 
		{
			isInUnreachableState = 0;
		}

		if (isInUnreachableState) 
		{
			continue;
		}

		if ((currentInstruction->operands[0].type == REGISTER &&
			compareRegisters(currentInstruction->operands[0].reg, targetReg) &&
			doesInstructionModifyOperand(currentInstruction, 0, &finished))
			|| doesOpcodeModifyRegister(currentInstruction->opcode, targetReg, &finished))
		{
			unsigned char areOperandsEqual = currentInstruction->operands[1].type == REGISTER && compareRegisters(currentInstruction->operands[1].reg, targetReg);
			params.startInstructionIndex = areOperandsEqual ? i - 1 : i;

			if (currentInstruction->opcode == XOR && areOperandsEqual)
			{
				strcpy(expressions[expressionIndex], "0");
				expressionIndex++;

				break;
			}
			else if (currentInstruction->opcode == IMUL && currentInstruction->operands[2].type != NO_OPERAND)
			{
				char operandStr1[255] = { 0 };
				if (!decompileOperand(params, &currentInstruction->operands[1], getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[1], params.is64Bit), operandStr1, 255))
				{
					return 0;
				}
				char operandStr2[255] = { 0 };
				if (!decompileOperand(params, &currentInstruction->operands[2], getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[2], params.is64Bit), operandStr2, 255))
				{
					return 0;
				}

				sprintf(expressions[expressionIndex], "(%s * %s)", operandStr1, operandStr2);
				expressionIndex++;

				break;
			}
			else if (currentInstruction->opcode == INC) 
			{
				strcpy(expressions[expressionIndex], " + 1");
				expressionIndex++;
			}
			else if (currentInstruction->opcode == DEC)
			{
				strcpy(expressions[expressionIndex], " - 1");
				expressionIndex++;
			}
			else
			{
				struct Operand* targetOperand = &currentInstruction->operands[getLastOperand(currentInstruction)];
				char operandStr[255] = { 0 };
				if (!decompileOperand(params, targetOperand, getTypeOfOperand(currentInstruction->opcode, targetOperand, params.is64Bit), operandStr, 255))
				{
					return 0;
				}

				char operationStr[20] = { 0 };
				if (!getOperationStr(currentInstruction->opcode, 0, operationStr))
				{
					return 0;
				}

				sprintf(expressions[expressionIndex], "%s%s", operationStr, operandStr);
				expressionIndex++;
			}
		}
		else if (compareRegisters(targetReg, AX) && isOpcodeCall(currentInstruction->opcode))
		{
			int currentInstructionIndex = findInstructionByAddress(params.allAddresses, 0, params.totalNumOfInstructions - 1, params.currentFunc->addresses[i]);
			unsigned long long calleeAddress = resolveJmpChain(params.allInstructions, params.allAddresses, params.totalNumOfInstructions, currentInstructionIndex);
			int calleeIndex = findFunctionByAddress(params.functions, 0, params.numOfFunctions - 1, calleeAddress);
			
			params.startInstructionIndex = i;
			if (calleeIndex != -1 && params.functions[calleeIndex].returnType != VOID_TYPE)
			{
				int callNum = getFunctionCallNumber(params, calleeAddress);
				struct FuncReturnVariable* returnVar = findReturnVar(params.currentFunc, callNum, calleeAddress);
				if (returnVar != 0)
				{
					sprintf(expressions[expressionIndex], "%s", returnVar->name);
				}
				expressionIndex++;
				finished = 1;
				break;
			}
			else
			{
				// checking for imported function call
				int importIndex = checkForImportCall(params);

				if (importIndex != -1)
				{
					calleeAddress = params.imports[importIndex].address;
					int callNum = getFunctionCallNumber(params, calleeAddress);
					struct FuncReturnVariable* returnVar = findReturnVar(params.currentFunc, callNum, calleeAddress);
					if (returnVar != 0)
					{
						sprintf(expressions[expressionIndex], "%s", returnVar->name);
					}
					expressionIndex++;
					finished = 1;
					break;
				}
				else
				{
					return 0;
				}
			}
		}
	}

	if (!finished) { return 0; }

	for (int i = expressionIndex - 1; i >= 0; i--)
	{
		if (i < expressionIndex - 2)
		{
			wrapStrInParentheses(resultBuffer);
		}

		strcat(resultBuffer, expressions[i]);
	}

	if (expressionIndex > 1)
	{
		wrapStrInParentheses(resultBuffer);
	}

	return 1;
}

unsigned char decompileComparison(struct DecompilationParameters params, char* resultBuffer, unsigned char invertOperator)
{
	struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[params.startInstructionIndex]);

	char compOperator[3] = { 0 };

	if (invertOperator)
	{
		switch (currentInstruction->opcode)
		{
		case JZ_SHORT:
			strcpy(compOperator, "!=");
			break;
		case JNZ_SHORT:
			strcpy(compOperator, "==");
			break;
		case JG_SHORT:
			strcpy(compOperator, "<=");
			break;
		case JL_SHORT:
		case JB_SHORT:
			strcpy(compOperator, ">=");
			break;
		case JLE_SHORT:
		case JBE_SHORT:
			strcpy(compOperator, ">");
			break;
		case JGE_SHORT:
		case JNB_SHORT:
			strcpy(compOperator, "<");
			break;
		default:
			return 0;
		}
	}
	else
	{
		switch (currentInstruction->opcode)
		{
		case JZ_SHORT:
			strcpy(compOperator, "==");
			break;
		case JNZ_SHORT:
			strcpy(compOperator, "!=");
			break;
		case JG_SHORT:
			strcpy(compOperator, ">");
			break;
		case JL_SHORT:
		case JB_SHORT:
			strcpy(compOperator, "<");
			break;
		case JLE_SHORT:
		case JBE_SHORT:
			strcpy(compOperator, "<=");
			break;
		case JGE_SHORT:
		case JNB_SHORT:
			strcpy(compOperator, ">=");
			break;
		default:
			return 0;
		}
	}

	// looking for comparison opcode
	for (int i = params.startInstructionIndex - 1; i >= 0; i--)
	{
		currentInstruction = &(params.currentFunc->instructions[i]);

		if (currentInstruction->opcode == TEST)
		{
			if (areOperandsEqual(&currentInstruction->operands[0], &currentInstruction->operands[1])) 
			{
				if (params.currentFunc->instructions[i - 1].opcode == SETNZ) // redundant pattern ?
				{
					i--;
					continue;
				}

				params.startInstructionIndex = i;

				char operandStr[255] = { 0 };
				if (!decompileOperand(params, &currentInstruction->operands[0], INT_TYPE, operandStr, 255))
				{
					return 0;
				}

				sprintf(resultBuffer, "%s %s 0", operandStr, compOperator);

				return 1;
			}
			else 
			{
				params.startInstructionIndex = i;

				char operand1Str[255] = { 0 };
				if (!decompileOperand(params, &currentInstruction->operands[0], INT_TYPE, operand1Str, 255))
				{
					return 0;
				}

				char operand2Str[255] = { 0 };
				if (!decompileOperand(params, &currentInstruction->operands[1], INT_TYPE, operand2Str, 255))
				{
					return 0;
				}

				sprintf(resultBuffer, "(%s & %s) %s 0", operand1Str, operand2Str, compOperator);

				return 1;
			}
		}

		unsigned char type = 0;
		switch (currentInstruction->opcode)
		{
		case CMP:
			type = INT_TYPE;
			break;
		case COMISS:
			type = FLOAT_TYPE;
			break;
		case COMISD:
			type = DOUBLE_TYPE;
			break;
		}

		if (type != 0)
		{
			params.startInstructionIndex = i;

			char operand1Str[255] = { 0 };
			if (!decompileOperand(params, &currentInstruction->operands[0], type, operand1Str, 255))
			{
				return 0;
			}

			char operand2Str[255] = { 0 };
			if (!decompileOperand(params, &currentInstruction->operands[1], type, operand2Str, 255))
			{
				return 0;
			}

			sprintf(resultBuffer, "%s %s %s", operand1Str, compOperator, operand2Str);

			return 1;
		}
	}

	return 0;
}
