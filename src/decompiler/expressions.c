#include "expressions.h"
#include "functionCalls.h"
#include "dataTypes.h"

unsigned char decompileOperand(struct DecompilationParameters params, struct Operand* operand, enum PrimitiveType type, struct JdcStr* result)
{
	if (operand->type == IMMEDIATE)
	{
		if (!getValueFromDataSection(params, type, operand->immediate, result))
		{
			if (operand->immediate > -10 && operand->immediate < 10) // this is just arbitrary
			{
				return sprintfJdc(result, 0, "%lli", operand->immediate);
			}

			return sprintfJdc(result, 0, "0x%llX", operand->immediate);
		}
		
		return 1;
	}
	else if (operand->type == MEM_ADDRESS)
	{
		struct DisassembledInstruction* instruction = &(params.currentFunc->instructions[params.startInstructionIndex]);
		
		if (compareRegisters(operand->memoryAddress.reg, BP) || compareRegisters(operand->memoryAddress.reg, SP))
		{
			if (operand->memoryAddress.constDisplacement < 0)
			{
				struct StackVariable* localVar = getLocalVarByOffset(params.currentFunc, (int)(operand->memoryAddress.constDisplacement));
				if (localVar)
				{
					strcpyJdc(result, localVar->name.buffer);
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
					strcpyJdc(result, stackArg->name.buffer);
				}
				else
				{
					return 0;
				}
			}
		}
		else if (compareRegisters(operand->memoryAddress.reg, IP))
		{
			if (instruction->opcode == LEA) 
			{
				sprintfJdc(result, 0, "0x%llX", params.currentFunc->instructions[params.startInstructionIndex + 1].address + operand->memoryAddress.constDisplacement);
			}
			else 
			{
				sprintfJdc(result, 0, "*(%s*)(0x%llX)", primitiveTypeStrs[type], params.currentFunc->instructions[params.startInstructionIndex + 1].address + operand->memoryAddress.constDisplacement);
			}
		}
		else if (operand->memoryAddress.reg == NO_REG)
		{
			if (instruction->opcode == LEA)
			{
				sprintfJdc(result, 0, "0x%llX", primitiveTypeStrs[type], operand->memoryAddress.constDisplacement);
			}
			else
			{
				sprintfJdc(result, 0, "*(%s*)(0x%llX)", primitiveTypeStrs[type], operand->memoryAddress.constDisplacement);
			}
		}
		else
		{
			struct Operand baseReg = { 0 };
			baseReg.type = REGISTER;
			baseReg.reg = operand->memoryAddress.reg;

			params.startInstructionIndex--;
			struct JdcStr baseOperandStr = initializeJdcStr();
			if (!decompileOperand(params, &baseReg, getTypeOfOperand(NO_MNEMONIC, &baseReg, params.is64Bit), &baseOperandStr))
			{
				freeJdcStr(&baseOperandStr);
				return 0;
			}

			if (operand->memoryAddress.constDisplacement != 0)
			{
				if (instruction->opcode == LEA)
				{
					sprintfJdc(result, 0, "%s + 0x%llX", baseOperandStr.buffer, operand->memoryAddress.constDisplacement);
				}
				else
				{
					sprintfJdc(result, 0, "*(%s*)(%s + 0x%llX)", primitiveTypeStrs[type], baseOperandStr.buffer, operand->memoryAddress.constDisplacement);
				}
			}
			else
			{
				if (instruction->opcode == LEA)
				{
					sprintfJdc(result, 0, "%s", baseOperandStr.buffer);
				}
				else
				{
					sprintfJdc(result, 0, "*(%s*)(%s)", primitiveTypeStrs[type], baseOperandStr.buffer);
				}
			}

			freeJdcStr(&baseOperandStr);
		}

		return 1;
	}
	else if (operand->type == REGISTER)
	{
		if (compareRegisters(operand->reg, BP) || compareRegisters(operand->reg, SP))
		{
			return strcpyJdc(result, registerStrs[operand->reg]);
		}
		else if (compareRegisters(operand->reg, IP))
		{
			return sprintfJdc(result, 0, "0x%llX", params.currentFunc->instructions[params.startInstructionIndex + 1].address);
		}

		if (!decompileRegister(params, operand->reg, type, result))
		{
			// register argument
			struct RegisterVariable* regArg = getRegArgByReg(params.currentFunc, operand->reg);
			if (regArg)
			{
				return strcpyJdc(result, regArg->name.buffer);
			}

			return 0;
		}

		return 1;
	}

	return 0;
}

static unsigned char getValueFromDataSection(struct DecompilationParameters params, enum PrimitiveType type, unsigned long long address, struct JdcStr* result)
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

		struct JdcStr tmp = initializeJdcStr();
		int len = 0;
		char byte = 0;
		while (1)
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

			char c[2] = { byte, 0 }; // so the byte is null terminated
			switch (byte) 
			{
			case '\a':
				strcatJdc(&tmp, "\\n");
				break;
			case '\b':
				strcatJdc(&tmp, "\\b");
				break;
			case '\f':
				strcatJdc(&tmp, "\\f");
				break;
			case '\n':
				strcatJdc(&tmp, "\\n");
				break;
			case '\r':
				strcatJdc(&tmp, "\\r");
				break;
			case '\t':
				strcatJdc(&tmp, "\\t");
				break;
			case '\v':
				strcatJdc(&tmp, "\\");
				break;
			default:
				strcatJdc(&tmp, c);
				break;
			}

			len++;
		}

		if (isString && len > 0)
		{
			
			sprintfJdc(result, 0, "\"%s\"", tmp.buffer);
			return freeJdcStr(&tmp);
		}

		freeJdcStr(&tmp);
	}

	switch (type) 
	{
	case CHAR_TYPE:
		sprintfJdc(result, 0, "%d", *(char*)(params.dataSectionByte + dataSectionIndex));
		break;
	case SHORT_TYPE:
		sprintfJdc(result, 0, "%d", *(short*)(params.dataSectionByte + dataSectionIndex));
		break;
	case INT_TYPE:
		sprintfJdc(result, 0, "%d", *(int*)(params.dataSectionByte + dataSectionIndex));
		break;
	case LONG_LONG_TYPE:
		sprintfJdc(result, 0, "%lld", *(long long*)(params.dataSectionByte + dataSectionIndex));
		break;
	case FLOAT_TYPE:
		sprintfJdc(result, 0, "%f", *(float*)(params.dataSectionByte + dataSectionIndex));
		break;
	case DOUBLE_TYPE:
		sprintfJdc(result, 0, "%lf", *(double*)(params.dataSectionByte + dataSectionIndex));
		break;
	}

	return 1;
}

static unsigned char decompileRegister(struct DecompilationParameters params, enum Register targetReg, enum PrimitiveType type, struct JdcStr* result)
{
	struct JdcStr expressions[5] = { 0 };
	int expressionIndex = 0;

	unsigned char finished = 0;
	unsigned char isInUnreachableState = 0;

	for (int i = params.startInstructionIndex; i >= 0; i--)
	{
		if (finished)
		{
			break;
		}

		if (i == params.skipLowerBound) 
		{
			i = params.skipUpperBound;
			continue;
		}

		struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[i]);

		if (isOpcodeReturn(currentInstruction->opcode) || currentInstruction->opcode == JMP_SHORT)
		{
			isInUnreachableState = 1;
		}
		else if (isOpcodeJcc(currentInstruction->opcode))
		{
			isInUnreachableState = 0;
		}

		if (isInUnreachableState) 
		{
			continue;
		}

		params.startInstructionIndex = i;

		if (doesInstructionModifyRegister(currentInstruction, targetReg, 0, &finished))
		{
			expressions[expressionIndex] = initializeJdcStr();
			if (decompileOperation(params, type, 0, &expressions[expressionIndex]))
			{
				if (finished && 
					getTypeOfOperand(currentInstruction->opcode, &(currentInstruction->operands[0]), params.is64Bit) != type &&
					getTypeOfOperand(currentInstruction->opcode, &(currentInstruction->operands[1]), params.is64Bit) != type)
				{
					struct JdcStr tmp = copyJdcStr(&expressions[expressionIndex]);
					sprintfJdc(&expressions[expressionIndex], 0, "(%s)(%s)", primitiveTypeStrs[type], tmp.buffer);
					freeJdcStr(&tmp);
				}
				
				expressionIndex++;
			}
			else 
			{
				for (int i = 0; i < expressionIndex; i++)
				{
					freeJdcStr(&expressions[i]);
				}
				return 0;
			}
		}
		else if (compareRegisters(targetReg, AX) && isOpcodeCall(currentInstruction->opcode))
		{
			int currentInstructionIndex = findInstructionByAddress(params.allInstructions, 0, params.totalNumOfInstructions - 1, params.currentFunc->instructions[i].address);
			unsigned long long calleeAddress = resolveJmpChain(params.allInstructions, params.totalNumOfInstructions, currentInstructionIndex);
			int calleeIndex = findFunctionByAddress(params.functions, 0, params.numOfFunctions - 1, calleeAddress);
			
			if (calleeIndex != -1 && params.functions[calleeIndex].returnType != VOID_TYPE)
			{
				int callNum = getFunctionCallNumber(params, calleeAddress);
				struct ReturnedVariable* returnedVar = findReturnedVar(params.currentFunc, callNum, calleeAddress);
				if (returnedVar != 0)
				{
					expressions[expressionIndex] = initializeJdcStr();
					sprintfJdc(&expressions[expressionIndex], 0, "%s", returnedVar->name.buffer);
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
					struct ReturnedVariable* returnedVar = findReturnedVar(params.currentFunc, callNum, calleeAddress);
					if (returnedVar != 0)
					{
						expressions[expressionIndex] = initializeJdcStr();
						sprintfJdc(&expressions[expressionIndex], 0, "%s", returnedVar->name.buffer);
					}
					expressionIndex++;
					finished = 1;
					break;
				}
				else
				{
					for (int i = 0; i < expressionIndex; i++)
					{
						freeJdcStr(&expressions[i]);
					}
					return 0;
				}
			}
		}
	}

	if (!finished) 
	{
		for (int i = 0; i < expressionIndex; i++)
		{
			freeJdcStr(&expressions[i]);
		}
		return 0; 
	}

	for (int i = expressionIndex - 1; i >= 0; i--)
	{
		if (i < expressionIndex - 2)
		{
			wrapJdcStrInParentheses(result);
		}

		strcatJdc(result, expressions[i].buffer);
		freeJdcStr(&expressions[i]);
	}

	if (expressionIndex > 1)
	{
		wrapJdcStrInParentheses(result);
	}

	return 1;
}

unsigned char decompileComparison(struct DecompilationParameters params, unsigned char invertOperator, struct JdcStr* result)
{
	struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[params.startInstructionIndex]);

	char compOperator[3] = { 0 };
	switch (currentInstruction->opcode)
	{
	case JZ_SHORT:
	case CMOVZ:
		if (invertOperator) { strcpy(compOperator, "!="); }
		else { strcpy(compOperator, "=="); }
		break;
	case JNZ_SHORT:
	case CMOVNZ:
		if (invertOperator) { strcpy(compOperator, "=="); }
		else { strcpy(compOperator, "!="); }
		break;
	case JG_SHORT:
	case CMOVG:
		if (invertOperator) { strcpy(compOperator, "<="); }
		else { strcpy(compOperator, ">"); }
		break;
	case JL_SHORT:
	case JB_SHORT:
	case CMOVL:
	case CMOVB:
		if (invertOperator) { strcpy(compOperator, ">="); }
		else { strcpy(compOperator, "<"); }
		break;
	case JLE_SHORT:
	case JBE_SHORT:
	case CMOVLE:
	case CMOVBE:
		if (invertOperator) { strcpy(compOperator, ">"); }
		else { strcpy(compOperator, "<="); }
		break;
	case JGE_SHORT:
	case JNB_SHORT:
	case CMOVGE:
	case CMOVNB:
		if (invertOperator) { strcpy(compOperator, "<"); }
		else { strcpy(compOperator, ">="); }
		break;
	default:
		return 0;
	}

	// looking for comparison opcode
	for (int i = params.startInstructionIndex - 1; i >= 0; i--)
	{
		currentInstruction = &(params.currentFunc->instructions[i]);

		params.startInstructionIndex = i;
		if (currentInstruction->opcode == TEST)
		{
			if (areOperandsEqual(&currentInstruction->operands[0], &currentInstruction->operands[1])) 
			{
				if (params.currentFunc->instructions[i - 1].opcode == SETNZ) // redundant pattern ?
				{
					i--;
					continue;
				}

				struct JdcStr operandStr = initializeJdcStr();
				if (!decompileOperand(params, &currentInstruction->operands[0], getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[0], params.is64Bit), &operandStr))
				{
					freeJdcStr(&operandStr);
					return 0;
				}

				sprintfJdc(result, 0, "%s %s 0", operandStr.buffer, compOperator);
				freeJdcStr(&operandStr);

				return 1;
			}
			else 
			{
				struct JdcStr operand1Str = initializeJdcStr();
				if (!decompileOperand(params, &currentInstruction->operands[0], getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[0], params.is64Bit), &operand1Str))
				{
					freeJdcStr(&operand1Str);
					return 0;
				}

				struct JdcStr operand2Str = initializeJdcStr();
				if (!decompileOperand(params, &currentInstruction->operands[1], getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[1], params.is64Bit), &operand2Str))
				{
					freeJdcStr(&operand1Str);
					freeJdcStr(&operand2Str);
					return 0;
				}

				sprintfJdc(result, 0, "(%s & %s) %s 0", operand1Str.buffer, operand2Str.buffer, compOperator);
				freeJdcStr(&operand1Str);
				freeJdcStr(&operand2Str);

				return 1;
			}
		}
		else if (currentInstruction->opcode >= CMP && currentInstruction->opcode <= COMISD) 
		{
			struct JdcStr operand1Str = initializeJdcStr();
			if (!decompileOperand(params, &currentInstruction->operands[0], getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[0], params.is64Bit), &operand1Str))
			{
				freeJdcStr(&operand1Str);
				return 0;
			}

			struct JdcStr operand2Str = initializeJdcStr();
			if (!decompileOperand(params, &currentInstruction->operands[1], getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[1], params.is64Bit), &operand2Str))
			{
				freeJdcStr(&operand1Str);
				freeJdcStr(&operand2Str);
				return 0;
			}

			sprintfJdc(result, 0, "%s %s %s", operand1Str.buffer, compOperator, operand2Str.buffer);
			freeJdcStr(&operand1Str);
			freeJdcStr(&operand2Str);

			return 1;
		}
	}

	return 0;
}

unsigned char decompileOperation(struct DecompilationParameters params, enum PrimitiveType type, unsigned char getAssignment, struct JdcStr* result)
{
	struct DisassembledInstruction* instruction = &(params.currentFunc->instructions[params.startInstructionIndex]);

	if (isOpcodeXor(instruction->opcode) && areOperandsEqual(&instruction->operands[0], &instruction->operands[1]))
	{
		if (getAssignment) { strcpyJdc(result, " = 0"); }
		else { strcpyJdc(result, "0"); }
		return 1;
	}
	else if (instruction->opcode == INC) 
	{
		if (getAssignment) { strcpyJdc(result, "++"); }
		else { strcpyJdc(result, " + 1"); }
		return 1;
	}
	else if (instruction->opcode == DEC) 
	{
		if (getAssignment) { strcpyJdc(result, "--"); }
		else { strcpyJdc(result, " - 1"); }
		return 1;
	}
	else if (instruction->opcode == STMXCSR) 
	{
		if (getAssignment) { strcpyJdc(result, " = stmxcsr()"); }
		else { strcpyJdc(result, "stmxcsr()"); }
		return 1;
	}


	if (instruction->opcode == IMUL && instruction->operands[2].type != NO_OPERAND)
	{
		params.startInstructionIndex--;

		struct JdcStr operandStr1 = initializeJdcStr();
		if (!decompileOperand(params, &instruction->operands[1], type, &operandStr1))
		{
			freeJdcStr(&operandStr1);
			return 0;
		}

		struct JdcStr operandStr2 = initializeJdcStr();
		if (!decompileOperand(params, &instruction->operands[2], type, &operandStr2))
		{
			freeJdcStr(&operandStr1);
			freeJdcStr(&operandStr2);
			return 0;
		}

		if (getAssignment) { sprintfJdc(result, 0, " = (%s * %s)", operandStr1.buffer, operandStr2.buffer); }
		else { sprintfJdc(result, 0, "(%s * %s)", operandStr1.buffer, operandStr2.buffer); }
		freeJdcStr(&operandStr1);
		freeJdcStr(&operandStr2);

		return 1;
	}
	else if (isOpcodeCMOVcc(instruction->opcode))
	{
		struct JdcStr comparisonStr = initializeJdcStr();
		if (!decompileComparison(params, 0, &comparisonStr))
		{
			freeJdcStr(&comparisonStr);
			return 0;
		}

		struct JdcStr trueOperand = initializeJdcStr();
		if (!decompileOperand(params, &instruction->operands[1], type, &trueOperand))
		{
			freeJdcStr(&comparisonStr);
			freeJdcStr(&trueOperand);
			return 0;
		}

		params.startInstructionIndex--;
		struct JdcStr falseOperand = initializeJdcStr();
		if (!decompileOperand(params, &instruction->operands[0], type, &falseOperand))
		{
			freeJdcStr(&comparisonStr);
			freeJdcStr(&trueOperand);
			freeJdcStr(&falseOperand);
			return 0;
		}

		// conditional moves cant assign to a memory address, getAssignment must be 0
		sprintfJdc(result, 0, "(%s ? %s : %s)", comparisonStr.buffer, trueOperand.buffer, falseOperand.buffer);
		freeJdcStr(&comparisonStr);
		freeJdcStr(&trueOperand);
		freeJdcStr(&falseOperand);
		return 1;
	}

	struct Operand* secondOperand = &instruction->operands[1];
	if (secondOperand->type == REGISTER)
	{
		params.startInstructionIndex--; // avoiding infinite recursive loop with decompileRegister. startInstructionIndex could just be decremented for most instructions, but if secondOperand is dependent on the instruction pointer it needs startInstructionIndex to be the current instruction index
	}
	struct JdcStr secondOperandStr = initializeJdcStr();
	if (!decompileOperand(params, secondOperand, type, &secondOperandStr))
	{
		freeJdcStr(&secondOperandStr);
		return 0;
	}

	if (isOpcodeMov(instruction->opcode) || 
		(instruction->opcode == LEA && secondOperand->type == MEM_ADDRESS && !compareRegisters(secondOperand->memoryAddress.reg, BP) && !compareRegisters(secondOperand->memoryAddress.reg, SP)))
	{
		if (getAssignment) { sprintfJdc(result, 0, " = %s", secondOperandStr.buffer); }
		else { sprintfJdc(result, 0, "%s", secondOperandStr.buffer); }
	}
	else if (instruction->opcode == LEA) 
	{
		if (getAssignment) { sprintfJdc(result, 0, " = &%s", secondOperandStr.buffer); }
		else { sprintfJdc(result, 0, "&%s", secondOperandStr.buffer); }
	}
	else if (isOpcodeAdd(instruction->opcode)) 
	{
		if (getAssignment) { sprintfJdc(result, 0, " += %s", secondOperandStr.buffer); }
		else { sprintfJdc(result, 0, " + %s", secondOperandStr.buffer); }
	}
	else if (isOpcodeSub(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, " -= %s", secondOperandStr.buffer); }
		else { sprintfJdc(result, 0, " - %s", secondOperandStr.buffer); }
	}
	else if (isOpcodeAnd(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, " &= %s", secondOperandStr.buffer); }
		else { sprintfJdc(result, 0, " & %s", secondOperandStr.buffer); }
	}
	else if (isOpcodeOr(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, " |= %s", secondOperandStr.buffer); }
		else { sprintfJdc(result, 0, " | %s", secondOperandStr.buffer); }
	}
	else if (isOpcodeXor(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, " ^= %s", secondOperandStr.buffer); }
		else { sprintfJdc(result, 0, " ^ %s", secondOperandStr.buffer); }
	}
	else if (isOpcodeShl(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, " <<= %s", secondOperandStr.buffer); }
		else { sprintfJdc(result, 0, " << %s", secondOperandStr.buffer); }
	}
	else if (isOpcodeShr(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, " >>= %s", secondOperandStr.buffer); }
		else { sprintfJdc(result, 0, " >> %s", secondOperandStr.buffer); }
	}
	else if (isOpcodeMul(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, " *= %s", secondOperandStr.buffer); }
		else { sprintfJdc(result, 0, " * %s", secondOperandStr.buffer); }
	}
	else if (isOpcodeDiv(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, " /= %s", secondOperandStr.buffer); }
		else { sprintfJdc(result, 0, " / %s", secondOperandStr.buffer); }
	}
	else if (isOpcodeCvtToDbl(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, " = (double)%s", secondOperandStr.buffer); }
		else { sprintfJdc(result, 0, "(double)%s", secondOperandStr.buffer); }
	}
	else if (isOpcodeCvtToFlt(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, " = (float)%s", secondOperandStr.buffer); }
		else { sprintfJdc(result, 0, "(float)%s", secondOperandStr.buffer); }
	}
	else 
	{
		freeJdcStr(&secondOperandStr);
		return 0;
	}

	freeJdcStr(&secondOperandStr);
	return 1;
}
