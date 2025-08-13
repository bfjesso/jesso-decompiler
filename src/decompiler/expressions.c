#include "expressions.h"
#include "dataTypes.h"

unsigned char decompileOperand(struct DecompilationParameters params, struct Operand* operand, unsigned char type, char* resultBuffer, unsigned char resultBufferSize)
{
	if (operand->type == IMMEDIATE)
	{
		sprintf(resultBuffer, "%llu", operand->immediate);
		return 1;
	}
	else if (operand->type == MEM_ADDRESS)
	{
		if (compareRegisters(operand->memoryAddress.reg, BP) || compareRegisters(operand->memoryAddress.reg, SP))
		{
			if (operand->memoryAddress.constDisplacement < 0)
			{
				struct StackVariable* localVar = getLocalVarByOffset(params.currentFunc, operand->memoryAddress.constDisplacement);
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
				struct StackVariable* stackArg = getStackArgByOffset(params.currentFunc, operand->memoryAddress.constDisplacement);
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
			sprintf(resultBuffer, "*(%s*)(0x%llX)", primitiveTypeStrs[type], operand->memoryAddress.constDisplacement);
		}
		else
		{
			struct Operand baseReg = { 0 };
			baseReg.type = REGISTER;
			baseReg.reg = operand->memoryAddress.reg;

			char baseOperandStr[100] = { 0 };
			if (!decompileOperand(params, &baseReg, type, baseOperandStr, 100))
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
		if (compareRegisters(operand->reg, BP) || compareRegisters(operand->reg, SP) || compareRegisters(operand->reg, IP))
		{
			strcpy(resultBuffer, registerStrs[operand->reg]);
			return 1;
		}

		if (!decompileExpression(params, operand->reg, type, resultBuffer, resultBufferSize))
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

unsigned char decompileExpression(struct DecompilationParameters params, unsigned char targetReg, unsigned char type, char* resultBuffer, unsigned char resultBufferSize)
{
	char expressions[5][100] = { 0 };
	int expressionIndex = 0;

	unsigned char finished = 0;

	for (int i = params.startInstructionIndex; i >= 0; i--)
	{
		if (finished)
		{
			break;
		}

		struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[i]);

		if ((currentInstruction->operands[0].type == REGISTER &&
			compareRegisters(currentInstruction->operands[0].reg, targetReg) &&
			doesInstructionModifyOperand(currentInstruction, 0, &finished))
			|| doesOpcodeModifyRegister(currentInstruction->opcode, targetReg, &finished))
		{
			char targetOperand = getLastOperand(currentInstruction);

			params.startInstructionIndex = i - 1;

			if (currentInstruction->opcode == XOR && compareRegisters(currentInstruction->operands[1].reg, targetReg))
			{
				strcpy(expressions[expressionIndex], "0");
				expressionIndex++;

				break;
			}
			else if (currentInstruction->opcode == IMUL && currentInstruction->operands[2].type != NO_OPERAND)
			{
				char operandStr1[100] = { 0 };
				if (!decompileOperand(params, &currentInstruction->operands[1], type, operandStr1, 100))
				{
					return 0;
				}
				char operandStr2[100] = { 0 };
				if (!decompileOperand(params, &currentInstruction->operands[2], type, operandStr2, 100))
				{
					return 0;
				}

				sprintf(expressions[expressionIndex], "(%s * %s)", operandStr1, operandStr2);
				expressionIndex++;

				break;
			}
			else
			{
				char operandStr[100] = { 0 };
				if (!decompileOperand(params, &currentInstruction->operands[targetOperand], type, operandStr, 100))
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
		else if (currentInstruction->opcode == CALL_NEAR)
		{
			unsigned long long calleeAddress = params.currentFunc->addresses[i] + currentInstruction->operands[0].immediate;
			int calleeIndex = findFunctionByAddress(params.functions, 0, params.numOfFunctions - 1, calleeAddress);

			if (calleeIndex != -1)
			{
				if (params.functions[calleeIndex].returnType == VOID_TYPE) { continue; }

				int callNum = getFunctionCallNumber(params, calleeAddress);
				sprintf(expressions[expressionIndex], "%sRetVal%d", params.functions[calleeIndex].name, callNum);
				expressionIndex++;
				finished = 1;
				break;
			}
			else
			{
				// checking for imported function call
				params.startInstructionIndex = i;
				int importIndex = checkForImportCall(params);

				if (importIndex != -1)
				{
					calleeAddress = currentInstruction->operands[0].memoryAddress.constDisplacement;
					int callNum = getFunctionCallNumber(params, calleeAddress);
					sprintf(expressions[expressionIndex], "%sRetVal%d", params.imports[importIndex].name, callNum);
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