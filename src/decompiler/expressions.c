#include "expressions.h"
#include "../disassembler/operands.h"
#include "decompilationUtils.h"
#include "functions.h"
#include "functionCalls.h"
#include "dataTypes.h"

#include <ctype.h>

unsigned char decompileOperand(struct DecompilationParameters params, struct Operand* operand, struct VarType type, struct JdcStr* result)
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
		struct DisassembledInstruction* instruction = &(params.currentFunc->instructions[params.startInstructionIndex]);

		struct JdcStr typeStr = initializeJdcStr();
		varTypeToStr(type, &typeStr);
		
		if (compareRegisters(operand->memoryAddress.reg, BP) || compareRegisters(operand->memoryAddress.reg, SP))
		{
			int stackOffset = (int)(operand->memoryAddress.constDisplacement);
			if (compareRegisters(operand->memoryAddress.reg, SP)) 
			{
				stackOffset -= getStackFrameSizeAtInstruction(params.currentFunc, params.startInstructionIndex);
			}
			
			struct StackVariable* localVar = getStackVarByOffset(params.currentFunc, stackOffset);
			if (localVar)
			{
				strcpyJdc(result, localVar->name.buffer);
			}
			else
			{
				struct StackVariable* stackArg = getStackArgByOffset(params.currentFunc, stackOffset);
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
			unsigned long long address = params.currentFunc->instructions[params.startInstructionIndex + 1].address + operand->memoryAddress.constDisplacement;
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
				if (!getValueFromDataSection(params, type, operand->memoryAddress.constDisplacement, result))
				{
					sprintfJdc(result, 0, "*(%s*)(0x%llX)", typeStr.buffer, operand->memoryAddress.constDisplacement);
				}
			}
		}
		else
		{
			struct RegisterVariable* regArgVar = 0; // will be set if the register is decompiled to only a regVar or regArg. this is so it can be just dereferenced if it is a pointer type
			
			params.startInstructionIndex--;
			struct JdcStr baseOperandStr = initializeJdcStr();
			if (!decompileRegister(params, operand->memoryAddress.reg, getTypeOfRegister(NO_MNEMONIC, operand->memoryAddress.reg), &baseOperandStr, &regArgVar))
			{
				freeJdcStr(&typeStr);
				freeJdcStr(&baseOperandStr);
				return 0;
			}

			if (regArgVar && regArgVar->type.pointerLevel == 1 && regArgVar->type.primitiveType == type.primitiveType && regArgVar->type.isUnsigned == type.isUnsigned && operand->memoryAddress.regDisplacement == NO_REG && operand->memoryAddress.constDisplacement == 0)
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
				if (!decompileRegister(params, operand->memoryAddress.regDisplacement, getTypeOfRegister(NO_MNEMONIC, operand->memoryAddress.regDisplacement), &displacementOperandStr, 0))
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
		return decompileRegister(params, operand->reg, type, result, 0);
	}
	else if (operand->type == SEGMENT) 
	{
		return strcpyJdc(result, segmentStrs[operand->segment]);
	}

	return 0;
}

static unsigned char getValueFromDataSection(struct DecompilationParameters params, struct VarType type, unsigned long long address, struct JdcStr* result)
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

	if (type.primitiveType == FLOAT_TYPE) 
	{
		sprintfJdc(result, 0, "%f", *(float*)(params.dataSectionByte + dataSectionIndex));
	}
	else if (type.primitiveType == DOUBLE_TYPE) 
	{
		sprintfJdc(result, 0, "%lf", *(double*)(params.dataSectionByte + dataSectionIndex));
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
				sprintfJdc(result, 0, "%u", *(unsigned char*)(params.dataSectionByte + dataSectionIndex));
				break;
			case SHORT_TYPE:
				sprintfJdc(result, 0, "%u", *(unsigned short*)(params.dataSectionByte + dataSectionIndex));
				break;
			case INT_TYPE:
				sprintfJdc(result, 0, "%u", *(unsigned int*)(params.dataSectionByte + dataSectionIndex));
				break;
			case LONG_LONG_TYPE:
				sprintfJdc(result, 0, "%llu", *(unsigned long long*)(params.dataSectionByte + dataSectionIndex));
				break;
			}
		}
		else
		{
			switch (type.primitiveType)
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
			}
		}
	}

	return 1;
}

static unsigned char getStringFromDataSection(struct DecompilationParameters params, unsigned long long address, struct JdcStr* result)
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

unsigned char decompileRegister(struct DecompilationParameters params, enum Register targetReg, struct VarType type, struct JdcStr* result, struct RegisterVariable** regArgVarRef)
{
	if (compareRegisters(targetReg, BP) || compareRegisters(targetReg, SP))
	{
		return strcpyJdc(result, registerStrs[targetReg]);
	}
	else if (compareRegisters(targetReg, IP))
	{
		return sprintfJdc(result, 0, "0x%llX", params.currentFunc->instructions[params.startInstructionIndex + 1].address);
	}

	struct RegisterVariable* regVar = getRegVarByReg(params.currentFunc, targetReg);
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

		if (isInUnreachableState || doesInstructionDoNothing(currentInstruction))
		{
			continue;
		}

		params.startInstructionIndex = i;

		unsigned char regOperandNum = 0;
		unsigned char srcOperandNum = 0;
		if (doesInstructionModifyRegister(currentInstruction, targetReg, &regOperandNum, &srcOperandNum, &finished))
		{
			expressions[expressionIndex] = initializeJdcStr();
			if (decompileOperation(params, type, 0, &expressions[expressionIndex]))
			{
				if (finished) 
				{
					struct VarType startRegType = getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[regOperandNum]);
					struct VarType startSrcType = getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[srcOperandNum]);

					if (!compareTypes(type, startRegType) && !compareTypes(type, startSrcType))
					{
						struct JdcStr typeStr = initializeJdcStr();
						varTypeToStr(type, &typeStr);

						struct JdcStr tmp = copyJdcStr(&expressions[expressionIndex]);
						sprintfJdc(&expressions[expressionIndex], 0, "(%s)(%s)", typeStr.buffer, tmp.buffer);

						freeJdcStr(&tmp);
						freeJdcStr(&typeStr);
					}
				}
				
				expressionIndex++;
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
		else if (compareRegisters(targetReg, AX) && isOpcodeCall(currentInstruction->opcode))
		{
			int currentInstructionIndex = findInstructionByAddress(params.allInstructions, 0, params.totalNumOfInstructions - 1, params.currentFunc->instructions[i].address);
			unsigned long long calleeAddress = resolveJmpChain(params, currentInstructionIndex);
			int calleeIndex = findFunctionByAddress(params.functions, 0, params.numOfFunctions - 1, calleeAddress);
			
			if (calleeIndex != -1)
			{
				if (params.functions[calleeIndex].returnType.primitiveType != VOID_TYPE) 
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
				}
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

		if (isOpcodeReturn(currentInstruction->opcode) || currentInstruction->opcode == JMP_SHORT)
		{
			isInUnreachableState = 1;
		}
		else if (isOpcodeJcc(currentInstruction->opcode))
		{
			isInUnreachableState = 0;
		}
	}

	if (!finished) 
	{
		// check if register argument
		struct RegisterVariable* regArg = getRegArgByReg(params.currentFunc, targetReg);
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

unsigned char decompileComparison(struct DecompilationParameters params, unsigned char invertOperator, struct JdcStr* result)
{
	struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[params.startInstructionIndex]);

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
	case SETG:
		if (invertOperator) { strcpy(compOperator, "<="); }
		else { strcpy(compOperator, ">"); }
		break;
	case JL_SHORT:
	case JB_SHORT:
	case CMOVL:
	case CMOVB:
	case SETL:
	case SETB:
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
	case CMOVGE:
	case CMOVNB:
	case SETNB:
		if (invertOperator) { strcpy(compOperator, "<"); }
		else { strcpy(compOperator, ">="); }
		break;
	default:
		return 0;
	}

	// looking for instruction that modifies the appropriate flags
	for (int i = params.startInstructionIndex - 1; i >= 0; i--)
	{
		currentInstruction = &(params.currentFunc->instructions[i]);

		params.startInstructionIndex = i;
		if (currentInstruction->opcode == TEST || currentInstruction->opcode == AND)
		{
			if (compareOperands(&currentInstruction->operands[0], &currentInstruction->operands[1]))
			{
				if (params.currentFunc->instructions[i - 1].opcode == SETNZ) // redundant pattern ?
				{
					i--;
					continue;
				}

				struct JdcStr operandStr = initializeJdcStr();
				if (!decompileOperand(params, &currentInstruction->operands[0], getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[0]), &operandStr))
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
				if (!decompileOperand(params, &currentInstruction->operands[0], getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[0]), &operand1Str))
				{
					freeJdcStr(&operand1Str);
					return 0;
				}

				struct JdcStr operand2Str = initializeJdcStr();
				if (!decompileOperand(params, &currentInstruction->operands[1], getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[1]), &operand2Str))
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
			if (!decompileOperand(params, &currentInstruction->operands[0], getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[0]), &operand1Str))
			{
				freeJdcStr(&operand1Str);
				return 0;
			}

			struct JdcStr operand2Str = initializeJdcStr();
			if (!decompileOperand(params, &currentInstruction->operands[1], getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[1]), &operand2Str))
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
			if (!decompileOperand(params, &currentInstruction->operands[0], getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[0]), &operand1Str))
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

unsigned char decompileOperation(struct DecompilationParameters params, struct VarType type, unsigned char getAssignment, struct JdcStr* result)
{
	struct DisassembledInstruction* instruction = &(params.currentFunc->instructions[params.startInstructionIndex]);

	int ogStartInstructionIndex = params.startInstructionIndex;

	struct JdcStr decompiledOperands[4] = { 0 };
	int numOfOperands = 0;
	while(numOfOperands < 4)
	{
		struct Operand* currentOperand = &instruction->operands[numOfOperands];
		if (currentOperand->type == NO_OPERAND) 
		{
			break;
		}

		if (currentOperand->type == REGISTER)
		{
			params.startInstructionIndex = ogStartInstructionIndex - 1; // avoiding infinite recursive loop with decompileRegister. startInstructionIndex could just be decremented for most instructions, but if secondOperand is dependent on the instruction pointer it needs startInstructionIndex to be the current instruction index
		}
		else 
		{
			params.startInstructionIndex = ogStartInstructionIndex;
		}

		struct VarType operandType = getTypeOfOperand(instruction->opcode, currentOperand);
		if (operandType.primitiveType == VOID_TYPE) 
		{
			operandType = type;
		}
		
		decompiledOperands[numOfOperands] = initializeJdcStr();
		if (!decompileOperand(params, currentOperand, operandType, &decompiledOperands[numOfOperands]))
		{
			strcpyJdc(&decompiledOperands[numOfOperands], "ERROR"); // not all operands may be used in the decompilation, often when getAssignment is 0 and this is the first operand
		}

		numOfOperands++;
	}

	params.startInstructionIndex = ogStartInstructionIndex;

	if (isOpcodeXor(instruction->opcode) && compareOperands(&instruction->operands[0], &instruction->operands[1]))
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s = 0", decompiledOperands[0].buffer); }
		else { strcpyJdc(result, "0"); }
		for (int i = 0; i < numOfOperands; i++) { freeJdcStr(&decompiledOperands[i]); }
		return 1;
	}
	else if (instruction->opcode == INC) 
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s++", decompiledOperands[0].buffer); }
		else { strcpyJdc(result, " + 1"); }
		for (int i = 0; i < numOfOperands; i++) { freeJdcStr(&decompiledOperands[i]); }
		return 1;
	}
	else if (instruction->opcode == DEC) 
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s--", decompiledOperands[0].buffer); }
		else { strcpyJdc(result, " - 1"); }
		for (int i = 0; i < numOfOperands; i++) { freeJdcStr(&decompiledOperands[i]); }
		return 1;
	}
	else if (instruction->opcode == IDIV) 
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s /= %s", decompiledOperands[0].buffer, decompiledOperands[0].buffer); }
		else { sprintfJdc(result, 0, " / %s", decompiledOperands[0].buffer); }
		for (int i = 0; i < numOfOperands; i++) { freeJdcStr(&decompiledOperands[i]); }
		return 1;
	}
	else if (instruction->opcode == NEG)
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s = -%s", decompiledOperands[0].buffer, decompiledOperands[0].buffer); }
		else { sprintfJdc(result, 0, "-%s", decompiledOperands[0].buffer); }
		for (int i = 0; i < numOfOperands; i++) { freeJdcStr(&decompiledOperands[i]); }
		return 1;
	}
	else if (instruction->opcode == STMXCSR) 
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s = stmxcsr()", decompiledOperands[0].buffer); }
		else { strcpyJdc(result, "stmxcsr()"); }
		for (int i = 0; i < numOfOperands; i++) { freeJdcStr(&decompiledOperands[i]); }
		return 1;
	}
	else if (isOpcodeAES(instruction->opcode))
	{
		char* mnemonicStrsLowercase = (char*)malloc(strlen(mnemonicStrs[instruction->opcode]) + 1);
		if (!mnemonicStrsLowercase) 
		{ 
			for (int i = 0; i < numOfOperands; i++) { freeJdcStr(&decompiledOperands[i]); }
			return 0; 
		}
		strcpy(mnemonicStrsLowercase, mnemonicStrs[instruction->opcode]);
		for (int i = 0; i < strlen(mnemonicStrsLowercase); i++) 
		{
			mnemonicStrsLowercase[i] = tolower(mnemonicStrsLowercase[i]);
		}
		
		if (getAssignment) { sprintfJdc(result, 0, "%s = %s(%s, %s)", decompiledOperands[0].buffer, mnemonicStrsLowercase, decompiledOperands[0].buffer, decompiledOperands[1].buffer); }
		else { sprintfJdc(result, 0, "%s(%s, %s)", mnemonicStrsLowercase, decompiledOperands[0].buffer, decompiledOperands[1].buffer); }
		free(mnemonicStrsLowercase);
		for (int i = 0; i < numOfOperands; i++) { freeJdcStr(&decompiledOperands[i]); }
		return 1;
	}
	else if (instruction->opcode == IMUL && instruction->operands[2].type != NO_OPERAND)
	{
		if (compareOperands(&instruction->operands[0], &instruction->operands[1]))
		{
			if (getAssignment) { sprintfJdc(result, 0, "%s *= %s", decompiledOperands[0].buffer, decompiledOperands[2].buffer); }
			else { sprintfJdc(result, 0, " * %s", decompiledOperands[2].buffer); }
			for (int i = 0; i < numOfOperands; i++) { freeJdcStr(&decompiledOperands[i]); }
			return 1;
		}
		else 
		{
			if (getAssignment) { sprintfJdc(result, 0, "%s = (%s * %s)", decompiledOperands[0].buffer, decompiledOperands[1].buffer, decompiledOperands[2].buffer); }
			else { sprintfJdc(result, 0, "(%s * %s)", decompiledOperands[1].buffer, decompiledOperands[2].buffer); }
			for (int i = 0; i < numOfOperands; i++) { freeJdcStr(&decompiledOperands[i]); }
			return 1;
		}
	}
	else if (isOpcodeCMOVcc(instruction->opcode))
	{
		struct JdcStr comparisonStr = initializeJdcStr();
		if (!decompileComparison(params, 0, &comparisonStr))
		{
			freeJdcStr(&comparisonStr);
			for (int i = 0; i < numOfOperands; i++) { freeJdcStr(&decompiledOperands[i]); }
			return 0;
		}

		if (getAssignment) { sprintfJdc(result, 0, "%s = (%s ? %s : %s)", decompiledOperands[0].buffer, comparisonStr.buffer, decompiledOperands[1].buffer, decompiledOperands[0].buffer); }
		else { sprintfJdc(result, 0, "(%s ? %s : %s)", comparisonStr.buffer, decompiledOperands[1].buffer, decompiledOperands[0].buffer); }
		freeJdcStr(&comparisonStr);
		for (int i = 0; i < numOfOperands; i++) { freeJdcStr(&decompiledOperands[i]); }
		return 1;
	}
	else if (isOpcodeSETcc(instruction->opcode))
	{
		struct JdcStr comparisonStr = initializeJdcStr();
		if (!decompileComparison(params, 0, &comparisonStr))
		{
			freeJdcStr(&comparisonStr);
			for (int i = 0; i < numOfOperands; i++) { freeJdcStr(&decompiledOperands[i]); }
			return 0;
		}

		if (getAssignment) { sprintfJdc(result, 0, "%s = %s", decompiledOperands[0].buffer, comparisonStr.buffer); }
		else { sprintfJdc(result, 0, "%s", comparisonStr.buffer); }
		for (int i = 0; i < numOfOperands; i++) { freeJdcStr(&decompiledOperands[i]); }
		return 1;
	}
	else if (instruction->group1Prefix == REPZ && instruction->opcode == MOVS) 
	{
		struct JdcStr count = initializeJdcStr();
		struct VarType uIntType = { 0 };
		uIntType.primitiveType = INT_TYPE;
		if (!decompileRegister(params, CX, uIntType, &count, 0))
		{
			for (int i = 0; i < numOfOperands; i++) { freeJdcStr(&decompiledOperands[i]); }
			freeJdcStr(&count);
			return 0;
		}

		switch (instruction->operands[0].memoryAddress.ptrSize) 
		{
		case 1:
			strcatJdc(&count, " * sizeof(char)");
			break;
		case 2:
			strcatJdc(&count, " * sizeof(short)");
			break;
		case 4:
			strcatJdc(&count, " * sizeof(int)");
			break;
		case 8:
			strcatJdc(&count, " * sizeof(long long)");
			break;
		}

		sprintfJdc(result, 0, "memcpy(%s, %s, %s)", decompiledOperands[0].buffer, decompiledOperands[1].buffer, count.buffer);
		freeJdcStr(&count);
		for (int i = 0; i < numOfOperands; i++) { freeJdcStr(&decompiledOperands[i]); }
		return 1;
	}
	else if (instruction->opcode == POP) 
	{
		struct JdcStr value = initializeJdcStr();
		
		int stackOffset = 0;
		unsigned char gotValue = 0;
		for (int i = ogStartInstructionIndex; i >= 0; i--)
		{
			if (params.currentFunc->instructions[i].opcode == PUSH) 
			{
				stackOffset--;
				if (stackOffset == 0) 
				{
					params.startInstructionIndex = i;
					if (!decompileOperand(params, &params.currentFunc->instructions[i].operands[0], type, &value)) 
					{
						for (int i = 0; i < numOfOperands; i++) { freeJdcStr(&decompiledOperands[i]); }
						freeJdcStr(&value);
						return 0;
					}

					gotValue = 1;
					break;
				}
			}
			else if (params.currentFunc->instructions[i].opcode == PUSHF) 
			{
				stackOffset--;
				if (stackOffset == 0) 
				{
					strcpyJdc(&value, "__readeflags()"); // Windows function
					gotValue = 1;
					break;
				}
			}
			else if (params.currentFunc->instructions[i].opcode == POP) 
			{
				stackOffset++;
			}
		}

		if (gotValue) 
		{
			if (getAssignment) { sprintfJdc(result, 0, "%s = %s", decompiledOperands[0].buffer, value.buffer); }
			else { sprintfJdc(result, 0, "%s", value.buffer); }
			return 1;
		}
		else
		{
			if(instruction->operands[0].type == REGISTER)
			{
				if (getAssignment) { sprintfJdc(result, 0, "%s = %s", decompiledOperands[0].buffer, registerStrs[instruction->operands[0].reg]); }
				else { sprintfJdc(result, 0, "%s", registerStrs[instruction->operands[0].reg]); }
				gotValue = 1;
			}

			for (int i = 0; i < numOfOperands; i++) { freeJdcStr(&decompiledOperands[i]); }
			freeJdcStr(&value);
			return gotValue;
		}
	}

	struct Operand* secondOperand = &instruction->operands[1];

	if (isOpcodeMov(instruction->opcode) || 
		(instruction->opcode == LEA && secondOperand->type == MEM_ADDRESS && !compareRegisters(secondOperand->memoryAddress.reg, BP) && !compareRegisters(secondOperand->memoryAddress.reg, SP)))
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s = %s", decompiledOperands[0].buffer, decompiledOperands[1].buffer); }
		else { sprintfJdc(result, 0, "%s", decompiledOperands[1].buffer); }
	}
	else if (instruction->opcode == LEA) 
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s = &%s", decompiledOperands[0].buffer, decompiledOperands[1].buffer); }
		else { sprintfJdc(result, 0, "&%s", decompiledOperands[1].buffer); }
	}
	else if (isOpcodeAdd(instruction->opcode)) 
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s += %s", decompiledOperands[0].buffer, decompiledOperands[1].buffer); }
		else { sprintfJdc(result, 0, " + %s", decompiledOperands[1].buffer); }
	}
	else if (isOpcodeSub(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s -= %s", decompiledOperands[0].buffer, decompiledOperands[1].buffer); }
		else { sprintfJdc(result, 0, " - %s", decompiledOperands[1].buffer); }
	}
	else if (isOpcodeAnd(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s &= %s", decompiledOperands[0].buffer, decompiledOperands[1].buffer); }
		else { sprintfJdc(result, 0, " & %s", decompiledOperands[1].buffer); }
	}
	else if (isOpcodeOr(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s |= %s", decompiledOperands[0].buffer, decompiledOperands[1].buffer); }
		else { sprintfJdc(result, 0, " | %s", decompiledOperands[1].buffer); }
	}
	else if (isOpcodeXor(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s ^= %s", decompiledOperands[0].buffer, decompiledOperands[1].buffer); }
		else { sprintfJdc(result, 0, " ^ %s", decompiledOperands[1].buffer); }
	}
	else if (isOpcodeShl(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s <<= %s", decompiledOperands[0].buffer, decompiledOperands[1].buffer); }
		else { sprintfJdc(result, 0, " << %s", decompiledOperands[1].buffer); }
	}
	else if (isOpcodeShr(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s >>= %s", decompiledOperands[0].buffer, decompiledOperands[1].buffer); }
		else { sprintfJdc(result, 0, " >> %s", decompiledOperands[1].buffer); }
	}
	else if (isOpcodeMul(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s *= %s", decompiledOperands[0].buffer, decompiledOperands[1].buffer); }
		else { sprintfJdc(result, 0, " * %s", decompiledOperands[1].buffer); }
	}
	else if (isOpcodeDiv(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s /= %s", decompiledOperands[0].buffer, decompiledOperands[1].buffer); }
		else { sprintfJdc(result, 0, " / %s", decompiledOperands[1].buffer); }
	}
	else if (isOpcodeCvtToDbl(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s = (double)%s", decompiledOperands[0].buffer, decompiledOperands[1].buffer); }
		else { sprintfJdc(result, 0, "(double)%s", decompiledOperands[1].buffer); }
	}
	else if (isOpcodeCvtToFlt(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s = (float)%s", decompiledOperands[0].buffer, decompiledOperands[1].buffer); }
		else { sprintfJdc(result, 0, "(float)%s", decompiledOperands[1].buffer); }
	}
	else 
	{
		for (int i = 0; i < numOfOperands; i++) { freeJdcStr(&decompiledOperands[i]); }
		return 0;
	}

	for (int i = 0; i < numOfOperands; i++) { freeJdcStr(&decompiledOperands[i]); }
	return 1;
}
