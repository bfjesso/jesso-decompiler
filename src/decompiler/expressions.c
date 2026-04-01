#include "expressions.h"
#include "../disassembler/operands.h"
#include "decompilationUtils.h"
#include "functions.h"
#include "functionCalls.h"
#include "operations.h"
#include "dataTypes.h"

unsigned char decompileOperand(struct DecompilationParameters* params, struct Operand* operand, struct JdcStr* result)
{
	if (operand->type == IMMEDIATE)
	{
		if (!getStringFromDataSection(params, operand->immediate.value, result))
		{
			if (operand->immediate.value > -10 && operand->immediate.value < 10) // this is just arbitrary
			{
				return sprintfJdc(result, 0, "%lli", operand->immediate.value);
			}

			return sprintfJdc(result, 0, "0x%llX", operand->immediate.value);
		}
		
		return 1;
	}
	else if (operand->type == MEM_ADDRESS)
	{
		struct DisassembledInstruction* instruction = &(params->currentFunc->instructions[params->startInstructionIndex]);

		struct VarType memAddrType = getTypeOfOperand(instruction->opcode, operand);
		struct JdcStr typeStr = initializeJdcStr();
		varTypeToStr(memAddrType, &typeStr);
		
		if (compareRegisters(operand->memoryAddress.reg, BP) || compareRegisters(operand->memoryAddress.reg, SP))
		{
			int stackOffset = (int)(operand->memoryAddress.constDisplacement);
			if (compareRegisters(operand->memoryAddress.reg, SP)) 
			{
				stackOffset -= getStackFrameSizeAtInstruction(params->currentFunc, params->startInstructionIndex);
			}
			
			struct StackVariable* localVar = getStackVarByOffset(params->currentFunc, stackOffset);
			if (localVar)
			{
				strcpyJdc(result, localVar->name.buffer);
			}
			else
			{
				struct StackVariable* stackArg = getStackArgByOffset(params->currentFunc, stackOffset);
				if (stackArg)
				{
					strcpyJdc(result, stackArg->name.buffer);
				}
				else
				{
					freeJdcStr(&typeStr);
					return 0;
				}
			}
		}
		else if (compareRegisters(operand->memoryAddress.reg, IP))
		{
			unsigned long long address = params->currentFunc->instructions[params->startInstructionIndex + 1].address + operand->memoryAddress.constDisplacement;
			if (instruction->opcode == LEA) 
			{
				if (!getStringFromDataSection(params, address, result))
				{
					sprintfJdc(result, 0, "0x%llX", address);
				}
			}
			else 
			{
				sprintfJdc(result, 0, "*(%s*)(0x%llX)", typeStr.buffer, address);
			}
		}
		else if (operand->memoryAddress.reg == NO_REG)
		{
			if (instruction->opcode == LEA)
			{
				if (!getStringFromDataSection(params, operand->memoryAddress.constDisplacement, result)) 
				{
					sprintfJdc(result, 0, "0x%llX", operand->memoryAddress.constDisplacement);
				}
			}
			else
			{
				if (!getValueFromDataSection(params, memAddrType, operand->memoryAddress.constDisplacement, result))
				{
					sprintfJdc(result, 0, "*(%s*)(0x%llX)", typeStr.buffer, operand->memoryAddress.constDisplacement);
				}
			}
		}
		else
		{
			struct RegisterVariable* regArgVar = 0; // will be set if the register is decompiled to only a regVar or regArg. this is so it can be just dereferenced if it is a pointer type
			
			params->startInstructionIndex--;
			struct JdcStr baseOperandStr = initializeJdcStr();
			if (!decompileRegister(params, operand->memoryAddress.reg, &baseOperandStr, &regArgVar))
			{
				freeJdcStr(&typeStr);
				freeJdcStr(&baseOperandStr);
				return 0;
			}
			params->startInstructionIndex++;

			if (regArgVar && regArgVar->type.pointerLevel == 1 && regArgVar->type.primitiveType == memAddrType.primitiveType && regArgVar->type.isUnsigned == memAddrType.isUnsigned && operand->memoryAddress.regDisplacement == NO_REG && operand->memoryAddress.constDisplacement == 0)
			{
				if (instruction->opcode == LEA)
				{
					sprintfJdc(result, 0, "%s", regArgVar->name.buffer);
				}
				else
				{
					sprintfJdc(result, 0, "*%s", regArgVar->name.buffer);
				}

				return 1;
			}

			struct JdcStr displacementOperandStr = initializeJdcStr();
			if (operand->memoryAddress.regDisplacement != NO_REG) 
			{
				if (!decompileRegister(params, operand->memoryAddress.regDisplacement, &displacementOperandStr, 0))
				{
					freeJdcStr(&typeStr);
					freeJdcStr(&baseOperandStr);
					freeJdcStr(&displacementOperandStr);
					return 0;
				}
			}

			long long constDisplacement = operand->memoryAddress.constDisplacement;
			char constDisplacementOperator = '+';
			if (constDisplacement < 0) 
			{
				constDisplacement = -constDisplacement;
				constDisplacementOperator = '-';
			}
			
			if (constDisplacement != 0)
			{
				if (operand->memoryAddress.regDisplacement != NO_REG) 
				{
					if (instruction->opcode == LEA)
					{
						sprintfJdc(result, 0, "%s + %s %c 0x%llX", baseOperandStr.buffer, displacementOperandStr.buffer, constDisplacementOperator, constDisplacement);
					}
					else
					{
						sprintfJdc(result, 0, "*(%s*)(%s + %s %c 0x%llX)", typeStr.buffer, baseOperandStr.buffer, displacementOperandStr.buffer, constDisplacementOperator, constDisplacement);
					}
				}
				else 
				{
					if (instruction->opcode == LEA)
					{
						sprintfJdc(result, 0, "%s %c 0x%llX", baseOperandStr.buffer, constDisplacementOperator, constDisplacement);
					}
					else
					{
						sprintfJdc(result, 0, "*(%s*)(%s %c 0x%llX)", typeStr.buffer, baseOperandStr.buffer, constDisplacementOperator, constDisplacement);
					}
				}
			}
			else
			{
				if (operand->memoryAddress.regDisplacement != NO_REG) 
				{
					if (instruction->opcode == LEA)
					{
						sprintfJdc(result, 0, "%s + %s", baseOperandStr.buffer, displacementOperandStr.buffer);
					}
					else
					{
						sprintfJdc(result, 0, "*(%s*)(%s + %s)", typeStr.buffer, baseOperandStr.buffer, displacementOperandStr.buffer);
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
						sprintfJdc(result, 0, "*(%s*)(%s)", typeStr.buffer, baseOperandStr.buffer);
					}
				}
			}

			freeJdcStr(&baseOperandStr);
			freeJdcStr(&displacementOperandStr);
		}

		freeJdcStr(&typeStr);
		return 1;
	}
	else if (operand->type == REGISTER)
	{
		return decompileRegister(params, operand->reg, result, 0);
	}
	else if (operand->type == SEGMENT) 
	{
		return strcpyJdc(result, segmentStrs[operand->segment]);
	}

	return 0;
}

static unsigned char getValueFromDataSection(struct DecompilationParameters* params, struct VarType type, unsigned long long address, struct JdcStr* result)
{
	if (address < params->imageBase + params->dataSections[0].virtualAddress)
	{
		return 0;
	}
	
	int dataSectionIndex = -1;
	int totalSize = 0;
	for (int i = 0; i < params->numOfDataSections; i++)
	{
		if (address > params->imageBase + params->dataSections[i].virtualAddress && address < params->imageBase + params->dataSections[i].virtualAddress + params->dataSections[i].size)
		{
			dataSectionIndex = (int)((totalSize + address) - (params->dataSections[i].virtualAddress + params->imageBase));
		}

		totalSize += params->dataSections[i].size;
	}

	if (dataSectionIndex == -1 || dataSectionIndex >= totalSize)
	{
		return 0;
	}

	if (type.primitiveType == FLOAT_TYPE) 
	{
		sprintfJdc(result, 0, "%f", *(float*)(params->dataSectionByte + dataSectionIndex));
	}
	else if (type.primitiveType == DOUBLE_TYPE) 
	{
		sprintfJdc(result, 0, "%lf", *(double*)(params->dataSectionByte + dataSectionIndex));
	}
	else 
	{
		if (getStringFromDataSection(params, address, result))
		{
			return 1;
		}

		if (type.isUnsigned)
		{
			switch (type.primitiveType)
			{
			case CHAR_TYPE:
				sprintfJdc(result, 0, "%u", *(unsigned char*)(params->dataSectionByte + dataSectionIndex));
				break;
			case SHORT_TYPE:
				sprintfJdc(result, 0, "%u", *(unsigned short*)(params->dataSectionByte + dataSectionIndex));
				break;
			case INT_TYPE:
				sprintfJdc(result, 0, "%u", *(unsigned int*)(params->dataSectionByte + dataSectionIndex));
				break;
			case LONG_LONG_TYPE:
				sprintfJdc(result, 0, "%llu", *(unsigned long long*)(params->dataSectionByte + dataSectionIndex));
				break;
			}
		}
		else
		{
			switch (type.primitiveType)
			{
			case CHAR_TYPE:
				sprintfJdc(result, 0, "%d", *(char*)(params->dataSectionByte + dataSectionIndex));
				break;
			case SHORT_TYPE:
				sprintfJdc(result, 0, "%d", *(short*)(params->dataSectionByte + dataSectionIndex));
				break;
			case INT_TYPE:
				sprintfJdc(result, 0, "%d", *(int*)(params->dataSectionByte + dataSectionIndex));
				break;
			case LONG_LONG_TYPE:
				sprintfJdc(result, 0, "%lld", *(long long*)(params->dataSectionByte + dataSectionIndex));
				break;
			}
		}
	}

	return 1;
}

static unsigned char getStringFromDataSection(struct DecompilationParameters* params, unsigned long long address, struct JdcStr* result)
{
	if (address < params->imageBase + params->dataSections[0].virtualAddress)
	{
		return 0;
	}

	int dataSectionIndex = -1;
	int totalSize = 0;
	for (int i = 0; i < params->numOfDataSections; i++)
	{
		if (address > params->imageBase + params->dataSections[i].virtualAddress && address < params->imageBase + params->dataSections[i].virtualAddress + params->dataSections[i].size)
		{
			dataSectionIndex = (int)((totalSize + address) - (params->dataSections[i].virtualAddress + params->imageBase));
		}

		totalSize += params->dataSections[i].size;
	}

	if (dataSectionIndex == -1 || dataSectionIndex >= totalSize)
	{
		return 0;
	}

	char isString = 1;

	struct JdcStr tmp = initializeJdcStr();
	int len = 0;
	char byte = 0;
	while (1)
	{
		byte = *(char*)(params->dataSectionByte + len + dataSectionIndex);

		if (byte == 0)
		{
			break;
		}
		else if (len > 100 || (byte < 32 && (byte < 7 || byte > 13)) || byte > 126) // checking if len isn't too long, and the byte is either an escape char or a character
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
	return 0;
}

unsigned char decompileRegister(struct DecompilationParameters* params, enum Register targetReg, struct JdcStr* result, struct RegisterVariable** regArgVarRef)
{
	if (compareRegisters(targetReg, BP) || compareRegisters(targetReg, SP))
	{
		return strcpyJdc(result, registerStrs[targetReg]);
	}
	else if (compareRegisters(targetReg, IP))
	{
		return sprintfJdc(result, 0, "0x%llX", params->currentFunc->instructions[params->startInstructionIndex + 1].address);
	}

	struct RegisterVariable* regVar = getRegVarByReg(params->currentFunc, targetReg);
	if (regVar)
	{
		if (regArgVarRef)
		{
			*regArgVarRef = regVar;
		}
		
		return strcpyJdc(result, regVar->name.buffer);
	}
	
	struct JdcStr* expressions = (struct JdcStr*)calloc(5, sizeof(struct JdcStr));
	int expressionsBufferSize = 5;

	int expressionIndex = 0;

	unsigned char finished = 0;
	unsigned char isInUnreachableState = 0;

	int ogStartInstructionIndex = params->startInstructionIndex;

	for (int i = params->startInstructionIndex; i >= 0; i--)
	{
		if (finished)
		{
			break;
		}

		struct DisassembledInstruction* currentInstruction = &(params->currentFunc->instructions[i]);

		if (isInUnreachableState || doesInstructionDoNothing(currentInstruction))
		{
			continue;
		}

		params->startInstructionIndex = i;

		unsigned char regOperandNum = 0;
		unsigned char srcOperandNum = 0;
		if (doesInstructionModifyRegister(currentInstruction, targetReg, &regOperandNum, &srcOperandNum, &finished))
		{
			expressions[expressionIndex] = initializeJdcStr();
			if (!decompileOperation(params, targetReg, 0, &expressions[expressionIndex]))
			{
				for (int j = 0; j < expressionIndex; j++)
				{
					freeJdcStr(&expressions[j]);
				}
				free(expressions);
				return 0;
			}

			expressionIndex++;
		}
		else 
		{
			struct Function* callee;
			if (checkForFunctionCall(params, &callee) && compareRegisters(callee->returnReg, targetReg))
			{
				int callNum = getFunctionCallNumber(params, callee->instructions[0].address);
				struct ReturnedVariable* returnedVar = findReturnedVar(params->currentFunc, callNum, callee->instructions[0].address);
				if (returnedVar != 0)
				{
					expressions[expressionIndex] = initializeJdcStr();
					sprintfJdc(&expressions[expressionIndex], 0, "%s", returnedVar->name.buffer);
				}
				expressionIndex++;
				finished = 1;
			}
			else 
			{
				int importIndex = checkForImportCall(params);
				if (importIndex != -1 && compareRegisters(targetReg, AX))
				{
					unsigned long long calleeAddress = params->imports[importIndex].address;
					int callNum = getFunctionCallNumber(params, calleeAddress);
					struct ReturnedVariable* returnedVar = findReturnedVar(params->currentFunc, callNum, calleeAddress);
					if (returnedVar != 0)
					{
						expressions[expressionIndex] = initializeJdcStr();
						sprintfJdc(&expressions[expressionIndex], 0, "%s", returnedVar->name.buffer);
					}
					expressionIndex++;
					finished = 1;
				}
			}
		}

		if (expressionIndex >= expressionsBufferSize)
		{
			expressionsBufferSize += 5;

			struct JdcStr* newExpressions = (struct JdcStr*)realloc(expressions, expressionsBufferSize * sizeof(struct JdcStr));
			if (newExpressions)
			{
				expressions = newExpressions;
			}
			else
			{
				for (int j = 0; j < expressionIndex; j++)
				{
					freeJdcStr(&expressions[j]);
				}
				free(expressions);
				return 0;
			}
		}

		if (i != ogStartInstructionIndex) 
		{
			if (isOpcodeReturn(currentInstruction->opcode) || currentInstruction->opcode == JMP_SHORT)
			{
				isInUnreachableState = 1;
			}
			else if (isOpcodeJcc(currentInstruction->opcode))
			{
				isInUnreachableState = 0;
			}
		}
	}

	params->startInstructionIndex = ogStartInstructionIndex;

	if (!finished) 
	{
		// check if register argument
		struct RegisterVariable* regArg = getRegArgByReg(params->currentFunc, targetReg);
		if (regArg)
		{
			expressions[expressionIndex] = initializeJdcStr();
			strcpyJdc(&expressions[expressionIndex], regArg->name.buffer);
			expressionIndex++;

			if (regArgVarRef && expressionIndex == 1)
			{
				*regArgVarRef = regArg;
			}
		}
		else 
		{
			for (int i = 0; i < expressionIndex; i++)
			{
				freeJdcStr(&expressions[i]);
			}
			free(expressions);

			return strcpyJdc(result, registerStrs[targetReg]);
		}
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

	free(expressions);

	if (expressionIndex > 1)
	{
		wrapJdcStrInParentheses(result);
	}

	return 1;
}

unsigned char decompileComparison(struct DecompilationParameters* params, unsigned char invertOperator, struct JdcStr* result)
{
	struct DisassembledInstruction* currentInstruction = &(params->currentFunc->instructions[params->startInstructionIndex]);

	enum Mnemonic jcc = currentInstruction->opcode;

	char compOperator[3] = { 0 };
	switch (jcc)
	{
	case JZ_SHORT:
	case CMOVZ:
	case SETZ:
		if (invertOperator) { strcpy(compOperator, "!="); }
		else { strcpy(compOperator, "=="); }
		break;
	case JNZ_SHORT:
	case CMOVNZ:
	case SETNZ:
		if (invertOperator) { strcpy(compOperator, "=="); }
		else { strcpy(compOperator, "!="); }
		break;
	case JG_SHORT:
	case JA_SHORT:
	case CMOVG:
	case CMOVA:
	case SETG:
	case SETA:
		if (invertOperator) { strcpy(compOperator, "<="); }
		else { strcpy(compOperator, ">"); }
		break;
	case JL_SHORT:
	case JB_SHORT:
	case JS_SHORT:
	case CMOVL:
	case CMOVB:
	case CMOVS:
	case SETL:
	case SETB:
	case SETS:
		if (invertOperator) { strcpy(compOperator, ">="); }
		else { strcpy(compOperator, "<"); }
		break;
	case JLE_SHORT:
	case JBE_SHORT:
	case CMOVLE:
	case CMOVBE:
	case SETLE:
	case SETBE:
		if (invertOperator) { strcpy(compOperator, ">"); }
		else { strcpy(compOperator, "<="); }
		break;
	case JGE_SHORT:
	case JNB_SHORT:
	case JNS_SHORT:
	case CMOVGE:
	case CMOVNB:
	case CMOVNS:
	case SETNB:
	case SETNS:
		if (invertOperator) { strcpy(compOperator, "<"); }
		else { strcpy(compOperator, ">="); }
		break;
	default:
		return 0;
	}

	// looking for instruction that modifies the appropriate flags
	for (int i = params->startInstructionIndex - 1; i >= 0; i--)
	{
		currentInstruction = &(params->currentFunc->instructions[i]);

		params->startInstructionIndex = i;
		if (currentInstruction->opcode == TEST || currentInstruction->opcode == AND)
		{
			if (compareOperands(&currentInstruction->operands[0], &currentInstruction->operands[1]))
			{
				if (params->currentFunc->instructions[i - 1].opcode == SETNZ) // redundant pattern ?
				{
					i--;
					continue;
				}

				struct JdcStr operandStr = initializeJdcStr();
				if (!decompileOperand(params, &currentInstruction->operands[0], &operandStr))
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
				if (!decompileOperand(params, &currentInstruction->operands[0], &operand1Str))
				{
					freeJdcStr(&operand1Str);
					return 0;
				}

				struct JdcStr operand2Str = initializeJdcStr();
				if (!decompileOperand(params, &currentInstruction->operands[1], &operand2Str))
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
		else if (isOpcodeCmp(currentInstruction->opcode) || currentInstruction->opcode == SUB)
		{
			struct JdcStr operand1Str = initializeJdcStr();
			if (!decompileOperand(params, &currentInstruction->operands[0], &operand1Str))
			{
				freeJdcStr(&operand1Str);
				return 0;
			}

			struct JdcStr operand2Str = initializeJdcStr();
			if (!decompileOperand(params, &currentInstruction->operands[1], &operand2Str))
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
		else if ((jcc == JZ_SHORT || jcc == JNZ_SHORT) && doesInstructionModifyZF(currentInstruction)) 
		{
			struct JdcStr operand1Str = initializeJdcStr();
			if (!decompileOperand(params, &currentInstruction->operands[0], &operand1Str))
			{
				freeJdcStr(&operand1Str);
				return 0;
			}

			sprintfJdc(result, 0, "%s %s 0", operand1Str.buffer, compOperator);
			freeJdcStr(&operand1Str);

			return 1;
		}
	}

	return 0;
}