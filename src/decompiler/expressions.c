#include "expressions.h"
#include "../disassembler/operands.h"
#include "decompilationUtils.h"
#include "conditions.h"
#include "directJmps.h"
#include "functions.h"
#include "functionCalls.h"
#include "assignment.h"
#include "operations.h"
#include "dataTypes.h"

unsigned char decompileOperand(struct DecompilationParameters* params, struct Operand* operand, unsigned char defaultToReg, struct JdcStr* result)
{
	if (operand->type == IMMEDIATE)
	{
		if (!getStringFromDataSection(params, operand->immediate.value, result))
		{
			if (operand->immediate.value > -10 && operand->immediate.value < 0)
			{
				return sprintfJdc(result, 0, "-%lli", -operand->immediate.value);
			}
			else if (operand->immediate.value >= 0 && operand->immediate.value < 10)
			{
				return sprintfJdc(result, 0, "%lli", operand->immediate.value);
			}
			else if (operand->immediate.value < 0)
			{
				return sprintfJdc(result, 0, "-0x%llX", -operand->immediate.value);
			}

			return sprintfJdc(result, 0, "0x%llX", operand->immediate.value);
		}
		
		return 1;
	}
	else if (operand->type == MEM_ADDRESS)
	{
		return decompileMemoryAddress(params, &operand->memoryAddress, result);
	}
	else if (operand->type == REGISTER)
	{
		return decompileRegister(params, operand->reg, defaultToReg, result, 0);
	}
	else if (operand->type == SEGMENT) 
	{
		return strcpyJdc(result, segmentStrs[operand->segment]);
	}

	return 0;
}

static unsigned char decompileMemoryAddress(struct DecompilationParameters* params, struct MemoryAddress* memAddress, struct JdcStr* result)
{
	if (compareRegisters(memAddress->reg, BP) || compareRegisters(memAddress->reg, SP))
	{
		int stackOffset = (int)(memAddress->constDisplacement);
		if (compareRegisters(memAddress->reg, SP))
		{
			stackOffset -= getStackFrameSizeAtInstruction(params);
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
				return 0;
			}
		}

		return 1;
	}

	struct DisassembledInstruction* instruction = &(params->instructions[params->startInstructionIndex]);
	struct VarType memAddrType = getTypeOfMemoryAddress(instruction->opcode, memAddress);

	struct JdcStr memAddrStr = initializeJdcStr();
	unsigned char hasGotFirstTerm = 0;

	if (memAddress->reg != NO_REG) 
	{
		struct RegisterVariable* regArgVar = 0; // will be set if the register is decompiled to only a regVar or regArg. this is so it can be just dereferenced if it is a pointer type
		struct JdcStr baseRegStr = initializeJdcStr();
		if (!decompileRegister(params, memAddress->reg, 1, &baseRegStr, &regArgVar))
		{
			freeJdcStr(&memAddrStr);
			freeJdcStr(&baseRegStr);
			return 0;
		}

		strcpyJdc(&memAddrStr, baseRegStr.buffer);
		freeJdcStr(&baseRegStr);

		if (regArgVar && regArgVar->type.pointerLevel == 1 && regArgVar->type.primitiveType == memAddrType.primitiveType && regArgVar->type.isUnsigned == memAddrType.isUnsigned && memAddress->regDisplacement == NO_REG && memAddress->constDisplacement == 0 && memAddress->scale == 1)
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

		if (memAddress->scale != 1)
		{
			sprintfJdc(&memAddrStr, 1, " * %d", memAddress->scale);
		}

		hasGotFirstTerm = 1;
	}

	if (memAddress->regDisplacement != NO_REG)
	{
		struct JdcStr displacementRegStr = initializeJdcStr();
		if (!decompileRegister(params, memAddress->regDisplacement, 1, &displacementRegStr, 0))
		{
			freeJdcStr(&displacementRegStr);
			return 0;
		}

		if (hasGotFirstTerm) 
		{
			sprintfJdc(&memAddrStr, 1, " + %s", displacementRegStr.buffer);
		}
		else 
		{
			strcpyJdc(&memAddrStr, displacementRegStr.buffer);
		}
		
		freeJdcStr(&displacementRegStr);
		hasGotFirstTerm = 1;
	}

	if (!hasGotFirstTerm) 
	{
		sprintfJdc(&memAddrStr, 0, "0x%llX", memAddress->constDisplacement);
	}
	else if (memAddress->constDisplacement < 0)
	{
		sprintfJdc(&memAddrStr, 1, " - 0x%llX", -memAddress->constDisplacement);
	}
	else if(memAddress->constDisplacement > 0)
	{
		sprintfJdc(&memAddrStr, 1, " + 0x%llX", memAddress->constDisplacement);
	}

	if (instruction->opcode != LEA)
	{
		struct JdcStr typeStr = initializeJdcStr();
		varTypeToStr(memAddrType, &typeStr);
		sprintfJdc(result, 0, "*(%s*)(%s)", typeStr.buffer, memAddrStr.buffer);
		freeJdcStr(&typeStr);
	}
	else
	{
		strcatJdc(result, memAddrStr.buffer);
	}
	
	freeJdcStr(&memAddrStr);
	return 1;
}

unsigned char decompileRegister(struct DecompilationParameters* params, enum Register targetReg, unsigned char defaultToReg, struct JdcStr* result, struct RegisterVariable** regArgVarRef)
{
	if (compareRegisters(targetReg, BP) || compareRegisters(targetReg, SP))
	{
		return strcpyJdc(result, registerStrs[targetReg]);
	}
	else if (compareRegisters(targetReg, IP))
	{
		return sprintfJdc(result, 0, "0x%llX", params->instructions[params->startInstructionIndex + 1].address);
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
	int ogStartInstructionIndex = params->startInstructionIndex;

	int conditionIndex = checkForConditionEnd(params);
	if (conditionIndex != -1)
	{
		params->startInstructionIndex = params->currentFunc->conditions[conditionIndex].startIndex;
	}
	else 
	{
		params->startInstructionIndex--;
	}

	for (int i = params->startInstructionIndex; i >= params->currentFunc->firstInstructionIndex; i--)
	{
		if (finished)
		{
			break;
		}

		struct DisassembledInstruction* currentInstruction = &(params->instructions[i]);
		params->startInstructionIndex = i;

		if (doesInstructionDoNothing(currentInstruction))
		{
			continue;
		}

		unsigned char srcOperandNum = 0;
		if (doesInstructionModifyRegister(currentInstruction, targetReg, 0, &srcOperandNum, &finished))
		{
			expressions[expressionIndex] = initializeJdcStr();
			if (!decompileOperation(params, targetReg, 0, 0, &expressions[expressionIndex]))
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
			if ((checkForKnownFunctionCall(params, &callee) && callee && compareRegisters(callee->returnReg, targetReg)) || 
				(checkForUnknownFunctionCall(params) && compareRegisters(targetReg, AX)))
			{
				struct ReturnedVariable* returnedVar = findReturnedVar(params->currentFunc, currentInstruction->address);
				if (!returnedVar)
				{
					for (int j = 0; j < expressionIndex; j++)
					{
						freeJdcStr(&expressions[j]);
					}
					free(expressions);
					return 0;
				}

				expressions[expressionIndex] = initializeJdcStrWithVal(returnedVar->name.buffer);
				expressionIndex++;
				finished = 1;
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

		int conditionIndex = checkForConditionEnd(params);
		if (conditionIndex != -1)
		{
			i = params->currentFunc->conditions[conditionIndex].startIndex + 1;
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
		else if (defaultToReg) 
		{
			expressions[expressionIndex] = initializeJdcStr();
			strcpyJdc(&expressions[expressionIndex], registerStrs[targetReg]);
			expressionIndex++;
		}
		else
		{
			for (int i = 0; i < expressionIndex; i++)
			{
				freeJdcStr(&expressions[i]);
			}
			free(expressions);
			return 0;
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
	struct DisassembledInstruction* currentInstruction = &(params->instructions[params->startInstructionIndex]);
	int ogStartInstructionIndex = params->startInstructionIndex;

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
	for (int i = params->startInstructionIndex - 1; i >= params->currentFunc->firstInstructionIndex; i--)
	{
		currentInstruction = &(params->instructions[i]);

		params->startInstructionIndex = i;
		if (currentInstruction->opcode == TEST || currentInstruction->opcode == AND)
		{
			struct JdcStr operand1Str = initializeJdcStr();
			if (!decompileOperand(params, &currentInstruction->operands[0], 1, &operand1Str))
			{
				freeJdcStr(&operand1Str);
				return 0;
			}
			
			if (compareOperands(&currentInstruction->operands[0], &currentInstruction->operands[1]) || (currentInstruction->opcode == AND && checkForAssignment(params)))
			{
				if (params->instructions[i - 1].opcode == SETNZ) // redundant pattern ?
				{
					i--;
					freeJdcStr(&operand1Str);
					continue;
				}

				sprintfJdc(result, 0, "%s %s 0", operand1Str.buffer, compOperator);
				freeJdcStr(&operand1Str);
				params->startInstructionIndex = ogStartInstructionIndex;
				return 1;
			}

			struct JdcStr operand2Str = initializeJdcStr();
			if (!decompileOperand(params, &currentInstruction->operands[1], 1, &operand2Str))
			{
				freeJdcStr(&operand1Str);
				freeJdcStr(&operand2Str);
				return 0;
			}

			sprintfJdc(result, 0, "(%s & %s) %s 0", operand1Str.buffer, operand2Str.buffer, compOperator);
			freeJdcStr(&operand1Str);
			freeJdcStr(&operand2Str);
			params->startInstructionIndex = ogStartInstructionIndex;
			return 1;
		}
		else if (isOpcodeCmp(currentInstruction->opcode) || currentInstruction->opcode == SUB)
		{
			struct JdcStr operand1Str = initializeJdcStr();
			if (!decompileOperand(params, &currentInstruction->operands[0], 1, &operand1Str))
			{
				freeJdcStr(&operand1Str);
				return 0;
			}
			
			if (currentInstruction->opcode == SUB && checkForAssignment(params))
			{
				sprintfJdc(result, 0, "%s %s 0", operand1Str.buffer, compOperator);
				freeJdcStr(&operand1Str);
				params->startInstructionIndex = ogStartInstructionIndex;
				return 1;
			}

			struct JdcStr operand2Str = initializeJdcStr();
			if (!decompileOperand(params, &currentInstruction->operands[1], 1, &operand2Str))
			{
				freeJdcStr(&operand1Str);
				freeJdcStr(&operand2Str);
				return 0;
			}

			sprintfJdc(result, 0, "%s %s %s", operand1Str.buffer, compOperator, operand2Str.buffer);
			freeJdcStr(&operand1Str);
			freeJdcStr(&operand2Str);
			params->startInstructionIndex = ogStartInstructionIndex;
			return 1;
		}
		else if ((jcc == JZ_SHORT || jcc == JNZ_SHORT) && doesInstructionModifyZF(currentInstruction)) 
		{
			struct JdcStr operand1Str = initializeJdcStr();
			if (!decompileOperand(params, &currentInstruction->operands[0], 1, &operand1Str))
			{
				freeJdcStr(&operand1Str);
				return 0;
			}

			sprintfJdc(result, 0, "%s %s 0", operand1Str.buffer, compOperator);
			freeJdcStr(&operand1Str);
			params->startInstructionIndex = ogStartInstructionIndex;
			return 1;
		}
	}

	return 0;
}

static unsigned char getValueFromDataSection(struct DecompilationParameters* params, struct VarType type, unsigned long long address, struct JdcStr* result)
{
	if (address < params->imageBase + params->sections[0].virtualAddress)
	{
		return 0;
	}

	int dataSectionIndex = -1;
	int totalSize = 0;
	for (int i = 0; i < params->numOfSections; i++)
	{
		if (params->sections[i].type == INIT_DATA_FST && params->sections[i].isReadOnly)
		{
			if (address > params->imageBase + params->sections[i].virtualAddress && address < params->imageBase + params->sections[i].virtualAddress + params->sections[i].size)
			{
				dataSectionIndex = (int)((totalSize + address) - (params->sections[i].virtualAddress + params->imageBase));
			}
		}

		totalSize += params->sections[i].size;
	}

	if (dataSectionIndex == -1 || dataSectionIndex >= totalSize)
	{
		return 0;
	}

	if (type.primitiveType == FLOAT_TYPE)
	{
		sprintfJdc(result, 0, "%0.8g", *(float*)(params->fileBytes + dataSectionIndex));
	}
	else if (type.primitiveType == DOUBLE_TYPE)
	{
		sprintfJdc(result, 0, "%0.16g", *(double*)(params->fileBytes + dataSectionIndex));
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
				sprintfJdc(result, 0, "%u", *(unsigned char*)(params->fileBytes + dataSectionIndex));
				break;
			case SHORT_TYPE:
				sprintfJdc(result, 0, "%u", *(unsigned short*)(params->fileBytes + dataSectionIndex));
				break;
			case INT_TYPE:
				sprintfJdc(result, 0, "%u", *(unsigned int*)(params->fileBytes + dataSectionIndex));
				break;
			case LONG_LONG_TYPE:
				sprintfJdc(result, 0, "%llu", *(unsigned long long*)(params->fileBytes + dataSectionIndex));
				break;
			}
		}
		else
		{
			switch (type.primitiveType)
			{
			case CHAR_TYPE:
				sprintfJdc(result, 0, "%d", *(char*)(params->fileBytes + dataSectionIndex));
				break;
			case SHORT_TYPE:
				sprintfJdc(result, 0, "%d", *(short*)(params->fileBytes + dataSectionIndex));
				break;
			case INT_TYPE:
				sprintfJdc(result, 0, "%d", *(int*)(params->fileBytes + dataSectionIndex));
				break;
			case LONG_LONG_TYPE:
				sprintfJdc(result, 0, "%lld", *(long long*)(params->fileBytes + dataSectionIndex));
				break;
			}
		}
	}

	return 1;
}

static unsigned char getStringFromDataSection(struct DecompilationParameters* params, unsigned long long address, struct JdcStr* result)
{
	if (address < params->imageBase + params->sections[0].virtualAddress)
	{
		return 0;
	}

	int dataSectionIndex = -1;
	int totalSize = 0;
	for (int i = 0; i < params->numOfSections; i++)
	{
		if (params->sections[i].type == INIT_DATA_FST && params->sections[i].isReadOnly)
		{
			if (address > params->imageBase + params->sections[i].virtualAddress && address < params->imageBase + params->sections[i].virtualAddress + params->sections[i].size)
			{
				dataSectionIndex = (int)((totalSize + address) - (params->sections[i].virtualAddress + params->imageBase));
			}
		}

		totalSize += params->sections[i].size;
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
		byte = *(char*)(params->fileBytes + len + dataSectionIndex);

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