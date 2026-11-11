#include "expressions.h"
#include "../file-handler/fileHandler.h"
#include "decompilationUtils.h"
#include "conditions.h"
#include "functions.h"
#include "functionCalls.h"
#include "assignment.h"
#include "operations.h"
#include "dataTypes.h"
#include "intrinsics.h"

enum JdcStatus decompileOperand(struct DecompilationParameters* params, int32_t instructionIndex, uint8_t operandNum, bool defaultToReg, struct JdcStr* result)
{
	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];
	if (operandNum >= instruction->numOfOperands)
	{
		return ERROR_JDC;
	}

	struct Operand* operand = &instruction->operands[operandNum];
	if (operand->type == IMMEDIATE_OPERAND)
	{
		if (operand->immediate.value > -10 && operand->immediate.value < 0)
		{
			return sprintfJdc(result, false, "-%lli", -operand->immediate.value);
		}
		else if (operand->immediate.value >= 0 && operand->immediate.value < 10)
		{
			return sprintfJdc(result, false, "%lli", operand->immediate.value);
		}
		else if (operand->immediate.value < 0)
		{
			return sprintfJdc(result, false, "-0x%llX", -operand->immediate.value);
		}

		int32_t calleeIndex = findFunctionByAddress(params, (uint64_t)(operand->immediate.value));
		if (calleeIndex != -1)
		{
			return strcpyJdc(result, params->functions[calleeIndex].name.buffer);
		}

		if (getStringFromDataSection(params, operand->immediate.value, result))
		{
			return SUCCESS_JDC;
		}

		return sprintfJdc(result, false, "0x%llX", operand->immediate.value);
	}
	else if (operand->type == MEMORY_OPERAND)
	{
		return decompileMemoryAddress(params, instructionIndex, operandNum, result);
	}
	else if (operand->type == REGISTER_OPERAND)
	{
		return decompileRegister(params, instructionIndex, operandNum, operand->reg, defaultToReg, 0, result, 0);
	}

	return ERROR_JDC;
}

static enum JdcStatus decompileMemoryAddress(struct DecompilationParameters* params, int32_t instructionIndex, uint8_t operandNum, struct JdcStr* result)
{
	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];
	struct MemoryAddress* memAddress = &instruction->operands[operandNum].memoryAddress;

	int64_t offsetFromInitSP = 0;
	if (isMemAddressStackVar(params, instructionIndex, memAddress, &offsetFromInitSP))
	{
		return decompileStackVar(params, instructionIndex, operandNum, offsetFromInitSP, result);
	}

	struct DataType memAddrType = getMemoryAddressDataType(instruction->opcode, memAddress);

	struct JdcStr memAddrStr = initializeJdcStr();
	bool hasGotFirstTerm = false;

	uint64_t baseRegVal = 0;
	if (isRegisterInstructionPointer(memAddress->reg))
	{
		baseRegVal = (instruction->address + (instruction->numOfBytes / params->addressability)) * memAddress->scale;
	}
	else if (memAddress->reg != NO_REG)
	{
		struct RegisterVariable* regArgVar = 0; // will be set if the register is decompiled to only a regVar or regArg. this is so it can be just dereferenced if it is a pointer type
		struct JdcStr baseRegStr = initializeJdcStr();
		if (ERROR_JDC == decompileRegister(params, instructionIndex, operandNum, memAddress->reg, 1, 0, &baseRegStr, &regArgVar))
		{
			freeJdcStr(&memAddrStr);
			freeJdcStr(&baseRegStr);
			return ERROR_JDC;
		}

		if (regArgVar && regArgVar->dataType.pointerLevel == 1 && regArgVar->dataType.primitiveType == memAddrType.primitiveType && memAddress->regDisplacement == NO_REG && memAddress->constDisplacement == 0 && memAddress->scale == 1)
		{
			if (isOpcodeLEA(instruction->opcode))
			{
				sprintfJdc(result, false, "%s", regArgVar->name.buffer);
			}
			else
			{
				sprintfJdc(result, false, "*%s", regArgVar->name.buffer);
			}

			return SUCCESS_JDC;
		}

		if (memAddress->scale != 1)
		{
			sprintfJdc(&memAddrStr, false, "(%s * %d)", baseRegStr.buffer, memAddress->scale);
		}
		else
		{
			strcpyJdc(&memAddrStr, baseRegStr.buffer);
		}

		freeJdcStr(&baseRegStr);
		hasGotFirstTerm = true;
	}

	bool addedDisplacement = false;

	uint64_t displacementRegVal = 0;
	if (isRegisterInstructionPointer(memAddress->regDisplacement))
	{
		displacementRegVal = instruction->address + (instruction->numOfBytes / params->addressability);
	}
	else if (memAddress->regDisplacement != NO_REG)
	{
		struct JdcStr displacementRegStr = initializeJdcStr();
		if (ERROR_JDC == decompileRegister(params, instructionIndex, operandNum, memAddress->regDisplacement, 1, 0, &displacementRegStr, 0))
		{
			freeJdcStr(&displacementRegStr);
			return ERROR_JDC;
		}

		if (hasGotFirstTerm)
		{
			sprintfJdc(&memAddrStr, true, " + %s", displacementRegStr.buffer);
			addedDisplacement = true;
		}
		else
		{
			strcpyJdc(&memAddrStr, displacementRegStr.buffer);
		}

		freeJdcStr(&displacementRegStr);
		hasGotFirstTerm = true;
	}

	if (!hasGotFirstTerm)
	{
		uint64_t totalDisplacement = baseRegVal + displacementRegVal + memAddress->constDisplacement;

		int32_t calleeIndex = findFunctionByAddress(params, totalDisplacement);
		int32_t importIndex = getImportIndexByAddress(params, totalDisplacement);
		if (calleeIndex != -1)
		{
			strcpyJdc(&memAddrStr, params->functions[calleeIndex].name.buffer);
		}
		else if (importIndex != -1 && !isOpcodeLEA(instruction->opcode))
		{
			strcpyJdc(result, params->imports[importIndex].name.buffer); // the import.address is the address of the table entry for the import, so it is like a ptr
			freeJdcStr(&memAddrStr);
			return SUCCESS_JDC;
		}
		else if (isOpcodeLEA(instruction->opcode) && SUCCESS_JDC == getStringFromDataSection(params, totalDisplacement, &memAddrStr))
		{
			strcatJdc(result, memAddrStr.buffer);
			freeJdcStr(&memAddrStr);
			return SUCCESS_JDC;
		}
		else if (!isOpcodeLEA(instruction->opcode) && SUCCESS_JDC == getValueFromDataSection(params, memAddrType, totalDisplacement, &memAddrStr))
		{
			strcatJdc(result, memAddrStr.buffer);
			freeJdcStr(&memAddrStr);
			return SUCCESS_JDC;
		}
		else
		{
			sprintfJdc(&memAddrStr, false, "0x%llX", totalDisplacement);
		}
	}
	else if (baseRegVal != 0 || displacementRegVal != 0)
	{
		sprintfJdc(&memAddrStr, true, " + 0x%llX", baseRegVal + displacementRegVal + memAddress->constDisplacement); // this has to be positive because baseRegVal and displacementRegVal are either zero or the IP
		addedDisplacement = true;
	}
	else if (memAddress->constDisplacement < 0)
	{
		sprintfJdc(&memAddrStr, true, " - 0x%llX", -memAddress->constDisplacement);
		addedDisplacement = true;
	}
	else if (memAddress->constDisplacement > 0)
	{
		sprintfJdc(&memAddrStr, true, " + 0x%llX", memAddress->constDisplacement);
		addedDisplacement = true;
	}

	if (addedDisplacement)
	{
		wrapJdcStrInParentheses(&memAddrStr);
	}

	if (!isOpcodeLEA(instruction->opcode))
	{
		struct JdcStr typeStr = initializeJdcStr();
		dataTypeToStr(memAddrType, params->useStdInt, &typeStr);
		sprintfJdc(result, false, "*(%s*)(%s)", typeStr.buffer, memAddrStr.buffer);
		freeJdcStr(&typeStr);
	}
	else
	{
		strcatJdc(result, memAddrStr.buffer);
	}

	freeJdcStr(&memAddrStr);
	return SUCCESS_JDC;
}

static enum JdcStatus decompileStackVar(struct DecompilationParameters* params, int32_t instructionIndex, uint8_t operandNum, int64_t offsetFromInitSP, struct JdcStr* result)
{
	struct StackVariable* stackVar = getStackVarByOffset(params->currentFunc, offsetFromInitSP);
	if (!stackVar)
	{
		return ERROR_JDC;
	}

	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];
	struct MemoryAddress* memAddress = &instruction->operands[operandNum].memoryAddress;
	struct DataType memAddrType = getMemoryAddressDataType(instruction->opcode, memAddress);

	struct JdcStr displacementRegStr = initializeJdcStr();
	if (memAddress->regDisplacement != NO_REG &&
		ERROR_JDC == decompileRegister(params, instructionIndex, operandNum, memAddress->regDisplacement, 1, 0, &displacementRegStr, 0))
	{
		freeJdcStr(&displacementRegStr);
		return ERROR_JDC;
	}

	if (isOpcodeLEA(instruction->opcode))
	{
		if (isDataTypePtr(stackVar->dataType))
		{
			if (memAddress->regDisplacement != NO_REG)
			{
				sprintfJdc(result, false, "(%s + %s)", stackVar->name.buffer, displacementRegStr.buffer);
			}
			else
			{
				strcpyJdc(result, stackVar->name.buffer);
			}
		}
		else
		{
			if (memAddress->regDisplacement != NO_REG)
			{
				sprintfJdc(result, false, "(&%s + %s)", stackVar->name.buffer, displacementRegStr.buffer);
			}
			else
			{
				sprintfJdc(result, false, "&%s", stackVar->name.buffer);
			}
		}

		freeJdcStr(&displacementRegStr);
		return SUCCESS_JDC;
	}

	if (memAddrType.primitiveType != stackVar->dataType.primitiveType)
	{
		struct JdcStr newTypeStr = initializeJdcStr();
		dataTypeToStr(memAddrType, params->useStdInt, &newTypeStr);

		if (memAddress->regDisplacement != NO_REG)
		{
			if (isDataTypePtr(stackVar->dataType))
			{
				sprintfJdc(result, true, "(%s*)(%s + %s)", newTypeStr.buffer, stackVar->name.buffer, displacementRegStr.buffer);
			}
			else
			{
				sprintfJdc(result, true, "*(%s*)(&%s + %s)", newTypeStr.buffer, stackVar->name.buffer, displacementRegStr.buffer);
			}
		}
		else
		{
			if (isDataTypePtr(stackVar->dataType))
			{
				sprintfJdc(result, false, "(%s*)%s", newTypeStr.buffer, stackVar->name.buffer);
			}
			else
			{
				sprintfJdc(result, false, "(%s)%s", newTypeStr.buffer, stackVar->name.buffer);
			}
		}

		freeJdcStr(&newTypeStr);
		freeJdcStr(&displacementRegStr);
		return SUCCESS_JDC;
	}

	uint8_t primitiveSize = getPrimitiveTypeSize(stackVar->dataType.primitiveType);
	if (stackVar->dataType.arrayLen > 1 && primitiveSize > 0)
	{
		int64_t arrayIndex = (offsetFromInitSP - stackVar->offsetFromInitSP) / primitiveSize;

		strcpyJdc(result, stackVar->name.buffer);
		if (memAddress->regDisplacement != NO_REG)
		{
			if (arrayIndex > 0)
			{
				if (primitiveSize > 1)
				{
					sprintfJdc(result, true, "[%llu + (%s / %u)]", arrayIndex, displacementRegStr.buffer, primitiveSize);
				}
				else
				{
					sprintfJdc(result, true, "[%llu + %s]", arrayIndex, displacementRegStr.buffer);
				}
			}
			else
			{
				if (primitiveSize > 1)
				{
					sprintfJdc(result, true, "[%s / %u]", displacementRegStr.buffer, primitiveSize);
				}
				else
				{
					sprintfJdc(result, true, "[%s]", displacementRegStr.buffer);
				}
			}
		}
		else
		{
			sprintfJdc(result, true, "[%llu]", arrayIndex);
		}

		freeJdcStr(&displacementRegStr);
		return SUCCESS_JDC;
	}

	if (stackVar->dataType.pointerLevel > 0)
	{
		sprintfJdc(result, false, "*%s", stackVar->name.buffer);
	}
	else
	{
		strcpyJdc(result, stackVar->name.buffer);
	}

	freeJdcStr(&displacementRegStr);
	return SUCCESS_JDC;
}

enum JdcStatus decompileRegister(struct DecompilationParameters* params, int32_t instructionIndex, uint8_t operandNum, enum Register targetReg, bool defaultToReg, bool notStatusFlag, struct JdcStr* result, struct RegisterVariable** regVarRef)
{
	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];

	if (isRegisterInstructionPointer(targetReg))
	{
		return sprintfJdc(result, false, "0x%llX", instruction->address + (instruction->numOfBytes / params->addressability));
	}
	else if (compareRegisters(targetReg, SP_X86_REG))
	{
		struct StackVariable* stackVar = getStackVarByOffset(params->currentFunc, -getStackFrameSizeAtInstruction(params, instructionIndex));
		if (stackVar)
		{
			if (isDataTypePtr(stackVar->dataType))
			{
				return strcpyJdc(result, stackVar->name.buffer);
			}
			else
			{
				return sprintfJdc(result, false, "&%s", stackVar->name.buffer);
			}
		}
	}

	struct RegisterVariable* localRegVar = getLocalRegVarByReg(params->currentFunc, targetReg);
	if (localRegVar)
	{
		if (checkRegVarScope(params, localRegVar, instructionIndex))
		{
			if (regVarRef)
			{
				*regVarRef = localRegVar;
			}

			if (isRegisterStatusFlag(targetReg))
			{
				return sprintfJdc(result, false, "%s%s", notStatusFlag ? "!" : "", localRegVar->name.buffer);
			}

			if (isRegisterHighByte(targetReg) && !isRegisterHighByte(localRegVar->reg))
			{
				return sprintfJdc(result, false, "(%s & 0xF0)", localRegVar->name.buffer);
			}

			struct DataType targetType = getRegisterDataType(&params->instructions[instructionIndex], operandNum, targetReg);
			if (doDataTypesRequireCasting(targetType, localRegVar->dataType, params->is64Bit) &&
				!doesInstructionModifyRegister(params, instructionIndex, targetReg, 0, 0))
			{
				struct JdcStr targetTypeStr = initializeJdcStr();
				dataTypeToStr(targetType, params->useStdInt, &targetTypeStr);

				sprintfJdc(result, false, "(%s)%s", targetTypeStr.buffer, localRegVar->name.buffer);
				freeJdcStr(&targetTypeStr);
				return SUCCESS_JDC;
			}

			return strcpyJdc(result, localRegVar->name.buffer);
		}
	}

	struct Expression* expressions = (struct Expression*)calloc(5, sizeof(struct Expression));
	if (!expressions)
	{
		return ERROR_JDC;
	}

	int32_t expressionsBufferSize = 5;
	int32_t expressionIndex = 0;

	int32_t ogInstructionIndex = instructionIndex;

	bool finished = false;
	for (int32_t i = instructionIndex - 1; i >= params->currentFunc->firstInstructionIndex; i--)
	{
		struct DisassembledInstruction* instruction = &params->instructions[i];

		if (finished)
		{
			break;
		}

		struct Condition* condition = getConditionFromLastBodyInstruction(params, i);
		if (condition &&
			(i != instructionIndex - 1 || condition->conditionType != DO_WHILE_CT)) // this is if decompileReg is called from decompileComparison
		{
			i = getConditionChainFirstBodyInstruction(params, condition);
			continue;
		}

		if (doesInstructionDoNothing(&(params->instructions[i])))
		{
			continue;
		}

		bool overwrites = false;
		if (doesInstructionModifyRegister(params, i, targetReg, 0, &overwrites))
		{
			expressions[expressionIndex].jdcStr = initializeJdcStr();
			if (ERROR_JDC == decompileOperation(params, i, targetReg, 0, notStatusFlag, &expressions[expressionIndex].jdcStr, &expressions[expressionIndex].placeOperatorInfront))
			{
				for (int32_t j = 0; j < expressionIndex; j++)
				{
					freeJdcStr(&expressions[j].jdcStr);
				}
				free(expressions);
				return ERROR_JDC;
			}

			expressionIndex++;

			// if the intrinsic function doesnt initialize the dst/first operand, then the first operand is decompiled as an argument
			finished = overwrites || isInstructionReturningIntrinsic(instruction, 0);
		}

		if (expressionIndex >= expressionsBufferSize)
		{
			expressionsBufferSize += 5;

			struct Expression* newExpressions = (struct Expression*)realloc(expressions, expressionsBufferSize * sizeof(struct JdcStr));
			if (newExpressions)
			{
				expressions = newExpressions;
			}
			else
			{
				for (int32_t j = 0; j < expressionIndex; j++)
				{
					freeJdcStr(&expressions[j].jdcStr);
				}
				free(expressions);
				return ERROR_JDC;
			}
		}
	}

	if (!finished)
	{
		// check if register argument
		struct RegisterVariable* regArg = getRegArgByReg(params->currentFunc, targetReg);
		if (regArg)
		{
			expressions[expressionIndex].jdcStr = initializeJdcStr();

			if (isRegisterHighByte(targetReg) && !isRegisterHighByte(regArg->reg))
			{
				sprintfJdc(&expressions[expressionIndex].jdcStr, false, "(%s & 0xF0)", regArg->name.buffer);
			}
			else
			{
				struct DataType targetType = getRegisterDataType(&params->instructions[ogInstructionIndex], operandNum, targetReg);
				if (doDataTypesRequireCasting(targetType, regArg->dataType, params->is64Bit))
				{
					struct JdcStr targetTypeStr = initializeJdcStr();
					dataTypeToStr(targetType, params->useStdInt, &targetTypeStr);

					sprintfJdc(&expressions[expressionIndex].jdcStr, false, "(%s)%s", targetTypeStr.buffer, regArg->name.buffer);
					freeJdcStr(&targetTypeStr);
				}
				else
				{
					strcpyJdc(&expressions[expressionIndex].jdcStr, regArg->name.buffer);
				}
			}

			expressionIndex++;

			if (regVarRef && expressionIndex == 1)
			{
				*regVarRef = regArg;
			}
		}
		else
		{
			for (int32_t i = 0; i < expressionIndex; i++)
			{
				freeJdcStr(&expressions[i].jdcStr);
			}
			free(expressions);

			if (defaultToReg)
			{
				strcpyJdc(result, registerStrs[targetReg]);
				return SUCCESS_JDC;
			}

			return ERROR_JDC;
		}
	}

	for (int32_t i = expressionIndex - 1; i >= 0; i--)
	{
		if (i < expressionIndex - 2 || expressions[i].placeOperatorInfront)
		{
			wrapJdcStrInParentheses(result);
		}

		if (expressions[i].placeOperatorInfront)
		{
			strcatStartJdc(result, expressions[i].jdcStr.buffer);
		}
		else
		{
			strcatJdc(result, expressions[i].jdcStr.buffer);
		}

		freeJdcStr(&expressions[i].jdcStr);
	}

	free(expressions);

	if (expressionIndex > 1)
	{
		wrapJdcStrInParentheses(result);
	}

	return SUCCESS_JDC;
}

enum JdcStatus decompileComparison(struct DecompilationParameters* params, int32_t conditionalInstructionIndex, bool invertOperator, struct JdcStr* result)
{
	struct DisassembledInstruction* conditionalInstruction = &(params->instructions[conditionalInstructionIndex]);
	enum OpcodeMnemonic cc = conditionalInstruction->opcode;

	char compOperator[3] = { 0 };
	switch (cc)
	{
	case JZ_SHORT_X86:
	case CMOVZ_X86:
	case SETZ_X86:
		if (invertOperator) { strcpy(compOperator, "!="); }
		else { strcpy(compOperator, "=="); }
		break;
	case JNZ_SHORT_X86:
	case CMOVNZ_X86:
	case SETNZ_X86:
		if (invertOperator) { strcpy(compOperator, "=="); }
		else { strcpy(compOperator, "!="); }
		break;
	case JG_SHORT_X86:
	case JA_SHORT_X86:
	case CMOVG_X86:
	case CMOVA_X86:
	case SETG_X86:
	case SETA_X86:
		if (invertOperator) { strcpy(compOperator, "<="); }
		else { strcpy(compOperator, ">"); }
		break;
	case JL_SHORT_X86:
	case JB_SHORT_X86:
	case JS_SHORT_X86:
	case CMOVL_X86:
	case CMOVB_X86:
	case CMOVS_X86:
	case SETL_X86:
	case SETB_X86:
	case SETS_X86:
		if (invertOperator) { strcpy(compOperator, ">="); }
		else { strcpy(compOperator, "<"); }
		break;
	case JLE_SHORT_X86:
	case JBE_SHORT_X86:
	case CMOVLE_X86:
	case CMOVBE_X86:
	case SETLE_X86:
	case SETBE_X86:
		if (invertOperator) { strcpy(compOperator, ">"); }
		else { strcpy(compOperator, "<="); }
		break;
	case JGE_SHORT_X86:
	case JNB_SHORT_X86:
	case JNS_SHORT_X86:
	case CMOVGE_X86:
	case CMOVNB_X86:
	case CMOVNS_X86:
	case SETNB_X86:
	case SETNS_X86:
		if (invertOperator) { strcpy(compOperator, "<"); }
		else { strcpy(compOperator, ">="); }
		break;
	}

	// looking for CMP instruction first before resorting to decompiling the status flags individually
	if (compOperator[0] != 0)
	{
		for (int32_t i = conditionalInstructionIndex - 1; i >= params->currentFunc->firstInstructionIndex; i--)
		{
			struct DisassembledInstruction* instruction = &(params->instructions[i]);
			if (isOpcodeCmp(instruction->opcode))
			{
				struct JdcStr operand1Str = initializeJdcStr();
				if (ERROR_JDC == decompileOperand(params, i, 0, 1, &operand1Str))
				{
					freeJdcStr(&operand1Str);
					return ERROR_JDC;
				}

				struct JdcStr operand2Str = initializeJdcStr();
				if (ERROR_JDC == decompileOperand(params, i, 1, 1, &operand2Str))
				{
					freeJdcStr(&operand1Str);
					freeJdcStr(&operand2Str);
					return ERROR_JDC;
				}

				sprintfJdc(result, false, "%s %s %s", operand1Str.buffer, compOperator, operand2Str.buffer);
				freeJdcStr(&operand1Str);
				freeJdcStr(&operand2Str);

				addAssociatedInstruction(params->currentFunc, i);
				return SUCCESS_JDC;
			}

			bool stop = false;
			for (int32_t j = CF_X86_REG; j <= OF_X86_REG; j++)
			{
				if (doesInstructionAccessRegister(params, conditionalInstructionIndex, j, 0, 0) && doesInstructionModifyRegister(params, i, j, 0, 0))
				{
					stop = true;
					break;
				}
			}
			if (stop)
			{
				break;
			}
		}
	}

	if (cc == JZ_SHORT_X86 || cc == SETZ_X86 || cc == CMOVZ_X86)
	{
		return decompileRegister(params, conditionalInstructionIndex, -1, ZF_X86_REG, 1, invertOperator, result, 0);
	}
	else if (cc == JNZ_SHORT_X86 || cc == SETNZ_X86 || cc == CMOVNZ_X86)
	{
		return decompileRegister(params, conditionalInstructionIndex, -1, ZF_X86_REG, 1, !invertOperator, result, 0);
	}
	else if (cc == JB_SHORT_X86 || cc == SETB_X86 || cc == CMOVB_X86)
	{
		return decompileRegister(params, conditionalInstructionIndex, -1, CF_X86_REG, 1, invertOperator, result, 0);
	}
	else if (cc == JNB_SHORT_X86 || cc == SETNB_X86 || cc == CMOVNB_X86)
	{
		return decompileRegister(params, conditionalInstructionIndex, -1, CF_X86_REG, 1, !invertOperator, result, 0);
	}
	else if (cc == JO_SHORT_X86 || cc == SETO_X86 || cc == CMOVO_X86)
	{
		return decompileRegister(params, conditionalInstructionIndex, -1, OF_X86_REG, 1, invertOperator, result, 0);
	}
	else if (cc == JNO_SHORT_X86 || cc == SETNO_X86 || cc == CMOVNO_X86)
	{
		return decompileRegister(params, conditionalInstructionIndex, -1, OF_X86_REG, 1, !invertOperator, result, 0);
	}
	else if (cc == JS_SHORT_X86 || cc == SETS_X86 || cc == CMOVS_X86)
	{
		return decompileRegister(params, conditionalInstructionIndex, -1, SF_X86_REG, 1, invertOperator, result, 0);
	}
	else if (cc == JNS_SHORT_X86 || cc == SETNS_X86 || cc == CMOVNS_X86)
	{
		return decompileRegister(params, conditionalInstructionIndex, -1, SF_X86_REG, 1, !invertOperator, result, 0);
	}
	else if (cc == JP_SHORT_X86 || cc == SETP_X86 || cc == CMOVP_X86)
	{
		return decompileRegister(params, conditionalInstructionIndex, -1, PF_X86_REG, 1, invertOperator, result, 0);
	}
	else if (cc == JNP_SHORT_X86 || cc == SETNP_X86 || cc == CMOVNP_X86)
	{
		return decompileRegister(params, conditionalInstructionIndex, -1, PF_X86_REG, 1, !invertOperator, result, 0);
	}
	else if (cc >= BRP_LC3 && cc <= BRNZ_LC3)
	{
		int32_t numOfConditions = 0;
		if (cc == BRN_LC3 || cc == BRNP_LC3 || cc == BRNZ_LC3)
		{
			struct JdcStr nExpression = initializeJdcStr();
			if (ERROR_JDC == decompileRegister(params, conditionalInstructionIndex, -1, N_LC3_REG, 1, invertOperator, &nExpression, 0))
			{
				freeJdcStr(&nExpression);
				return ERROR_JDC;
			}

			strcpyJdc(result, nExpression.buffer);
			freeJdcStr(&nExpression);
			numOfConditions++;
		}

		if (cc == BRZ_LC3 || cc == BRZP_LC3 || cc == BRNZ_LC3)
		{
			struct JdcStr zExpression = initializeJdcStr();
			if (ERROR_JDC == decompileRegister(params, conditionalInstructionIndex, -1, Z_LC3_REG, 1, invertOperator, &zExpression, 0))
			{
				freeJdcStr(&zExpression);
				return ERROR_JDC;
			}

			if (numOfConditions > 0)
			{
				sprintfJdc(result, true, " || %s", zExpression.buffer);
			}
			else
			{
				strcpyJdc(result, zExpression.buffer);
			}

			freeJdcStr(&zExpression);
			numOfConditions++;
		}

		if (cc == BRP_LC3 || cc == BRZP_LC3 || cc == BRNP_LC3)
		{
			struct JdcStr pExpression = initializeJdcStr();
			if (ERROR_JDC == decompileRegister(params, conditionalInstructionIndex, -1, P_LC3_REG, 1, invertOperator, &pExpression, 0))
			{
				freeJdcStr(&pExpression);
				return ERROR_JDC;
			}

			if (numOfConditions > 0)
			{
				sprintfJdc(result, true, " || %s", pExpression.buffer);
			}
			else
			{
				strcpyJdc(result, pExpression.buffer);
			}

			freeJdcStr(&pExpression);
			numOfConditions++;
		}

		return SUCCESS_JDC;
	}

	return ERROR_JDC;
}

static enum JdcStatus getValueFromDataSection(struct DecompilationParameters* params, struct DataType dataType, uint64_t address, struct JdcStr* result)
{
	if (address < params->imageBase)
	{
		return ERROR_JDC;
	}

	struct FileSection* section = 0;
	uint64_t fileOffset = rvaToFileOffset(params->sections, params->numOfSections, params->addressability, address - params->imageBase, &section);

	if (fileOffset == 0 || fileOffset >= params->numOfFileBytes || !section || section->type != INIT_DATA_FST || !section->isReadOnly)
	{
		return ERROR_JDC;
	}

	switch (dataType.primitiveType)
	{
	case INT8_TYPE:
	case UINT8_TYPE:
		return sprintfJdc(result, false, "0x%X", *(uint8_t*)(params->fileBytes + fileOffset));
	case INT16_TYPE:
	case UINT16_TYPE:
		return sprintfJdc(result, false, "0x%X", *(uint16_t*)(params->fileBytes + fileOffset));
	case INT32_TYPE:
	case UINT32_TYPE:
		return sprintfJdc(result, false, "0x%X", *(uint32_t*)(params->fileBytes + fileOffset));
	case INT64_TYPE:
	case UINT64_TYPE:
		return sprintfJdc(result, false, "0x%llX", *(uint64_t*)(params->fileBytes + fileOffset));
	case FLOAT_TYPE:
		return sprintfJdc(result, false, "%0.8g", *(float*)(params->fileBytes + fileOffset));
	case DOUBLE_TYPE:
		return sprintfJdc(result, false, "%0.16g", *(double*)(params->fileBytes + fileOffset));
	}

	return ERROR_JDC;
}

static enum JdcStatus getStringFromDataSection(struct DecompilationParameters* params, uint64_t address, struct JdcStr* result)
{
	if (address < params->imageBase)
	{
		return ERROR_JDC;
	}

	struct FileSection* section = 0;
	uint64_t fileOffset = rvaToFileOffset(params->sections, params->numOfSections, params->addressability, address - params->imageBase, &section);

	if (fileOffset == 0 || fileOffset >= params->numOfFileBytes || !section || section->type != INIT_DATA_FST || !section->isReadOnly)
	{
		return ERROR_JDC;
	}

	bool isString = true;

	struct JdcStr tmp = initializeJdcStr();
	int32_t len = 0;
	char byte = 0;
	while (1)
	{
		byte = *(char*)(params->fileBytes + len + fileOffset);

		if (byte == 0)
		{
			break;
		}
		else if (len > 100 || (byte < ' ' && (byte < '\a' || byte > '\r')) || byte > '~') // checking if len isn't too long, and the byte is either an escape char or a character
		{
			isString = false;
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

		sprintfJdc(result, false, "\"%s\"", tmp.buffer);
		return freeJdcStr(&tmp);
	}

	freeJdcStr(&tmp);
	return ERROR_JDC;
}