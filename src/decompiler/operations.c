#include "operations.h"
#include "assignment.h"
#include "decompilationUtils.h"
#include "functions.h"
#include "intrinsics.h"
#include "expressions.h"

enum JdcStatus decompileOperation(struct DecompilationParameters* params, int32_t instructionIndex, enum Register targetReg, bool getAssignment, bool notStatusFlag, struct JdcStr* result, bool* placeOperatorInfront)
{
	if (placeOperatorInfront) { *placeOperatorInfront = false; }
	
	if (!getAssignment) 
	{
		addAssociatedInstruction(params->currentFunc, instructionIndex);
	}
	
	struct DisassembledInstruction* instruction = &(params->instructions[instructionIndex]);
	if (isOpcodeCall(instruction->opcode) || isOpcodeJmp(instruction->opcode))
	{
		struct ReturnedVariable* returnedVar = findReturnedVar(params->currentFunc, instruction->address);
		if (!returnedVar) 
		{
			return ERROR_JDC;
		}

		strcpyJdc(result, returnedVar->name.buffer);
		return SUCCESS_JDC;
	}

	struct Intrinsic* intrinsic = 0;
	if (isInstructionReturningIntrinsic(instruction, &intrinsic))
	{
		return decompileReturningIntrinsic(params, instructionIndex, intrinsic, getAssignment, result);
	}
	else if (isOpcodeMov(instruction->opcode) || instruction->opcode == LEA) // LEA is handled when decompiling memory addresses 
	{
		return decompileBinaryOperation(params, instructionIndex, getAssignment, "", " = ", result);
	}
	else if (isOpcodeAdd(instruction->opcode))
	{
		if (instruction->opcode == ADD) 
		{
			if (getAssignment) 
			{
				decompileSumStatusFlags(params, instructionIndex, NO_REG, 1, notStatusFlag, result);
			}
			else if (isRegisterStatusFlag(targetReg)) 
			{
				return decompileSumStatusFlags(params, instructionIndex, targetReg, 0, notStatusFlag, result);
			}
		}
		
		return decompileBinaryOperation(params, instructionIndex, getAssignment, " + ", " += ", result);
	}
	else if (isOpcodeSub(instruction->opcode))
	{
		if (instruction->opcode == SUB)
		{
			if (getAssignment)
			{
				decompileSumStatusFlags(params, instructionIndex, NO_REG, 1, notStatusFlag, result);
			}
			else if (isRegisterStatusFlag(targetReg))
			{
				return decompileSumStatusFlags(params, instructionIndex, targetReg, 0, notStatusFlag, result);
			}
		}
		
		return decompileBinaryOperation(params, instructionIndex, getAssignment, " - ", " -= ", result);
	}
	else if (isOpcodeCmp(instruction->opcode))
	{
		return decompileSumStatusFlags(params, instructionIndex, targetReg, getAssignment, notStatusFlag, result);
	}
	else if (isOpcodeAnd(instruction->opcode))
	{
		return decompileBinaryOperation(params, instructionIndex, getAssignment, " & ", " &= ", result);
	}
	else if (isOpcodeOr(instruction->opcode))
	{
		return decompileOr(params, instructionIndex, getAssignment, result);
	}
	else if (isOpcodeShl(instruction->opcode))
	{
		return decompileBinaryOperation(params, instructionIndex, getAssignment, " << ", " <<= ", result);
	}
	else if (isOpcodeShr(instruction->opcode))
	{
		return decompileBinaryOperation(params, instructionIndex, getAssignment, " >> ", " >>= ", result);
	}
	else if (isOpcodeMul(instruction->opcode))
	{
		return decompileBinaryOperation(params, instructionIndex, getAssignment, " * ", " *= ", result);
	}
	else if (isOpcodeDiv(instruction->opcode))
	{
		return decompileBinaryOperation(params, instructionIndex, getAssignment, " / ", " /= ", result);
	}
	else if (isOpcodeCvtToDbl(instruction->opcode))
	{
		return decompileBinaryOperation(params, instructionIndex, getAssignment, "(double)", " = (double)", result);
	}
	else if (isOpcodeCvtToFlt(instruction->opcode))
	{
		return decompileBinaryOperation(params, instructionIndex, getAssignment, "(float)", " = (float)", result);
	}
	else if (instruction->opcode == INC || instruction->opcode == DEC)
	{
		return decompileIncDec(params, instructionIndex, targetReg, getAssignment, notStatusFlag, result);
	}
	else if (instruction->opcode == NEG)
	{
		return decompileNeg(params, instructionIndex, targetReg, getAssignment, notStatusFlag, result, placeOperatorInfront);
	}
	else if (instruction->opcode == NOT)
	{
		return decompileNot(params, instructionIndex, getAssignment, result, placeOperatorInfront);
	}
	else if (isOpcodeXor(instruction->opcode))
	{
		return decompileXor(params, instructionIndex, getAssignment, result);
	}
	else if (instruction->opcode == BT)
	{
		return decompileBitTest(params, instructionIndex, getAssignment, notStatusFlag, result);
	}
	else if (instruction->opcode == BTS)
	{
		return decompileBitSet(params, instructionIndex, targetReg, getAssignment, notStatusFlag, result);
	}
	else if (instruction->opcode == BTR)
	{
		return decompileBitReset(params, instructionIndex, targetReg, getAssignment, notStatusFlag, result);
	}
	else if (instruction->opcode == ADC)
	{
		return decompileADC(params, instructionIndex, getAssignment, result);
		}
	else if (instruction->opcode == SBB)
	{
		return decompileSBB(params, instructionIndex, getAssignment, result);
	}
	else if (instruction->opcode == FLD)
	{
		return decompileFLD(params, instructionIndex, getAssignment, result);
	}
	else if (instruction->opcode == IDIV || instruction->opcode == DIV)
	{
		return decompileIDIV(params, instructionIndex, targetReg, getAssignment, result);
	}
	else if (instruction->opcode == IMUL || instruction->opcode == MUL)
	{
		return decompileIMUL(params, instructionIndex, targetReg, getAssignment, result);
	}
	else if (instruction->opcode == POP)
	{
		return decompilePop(params, instructionIndex, getAssignment, result);
	}
	else if (isOpcodeCMOVcc(instruction->opcode)) 
	{
		return decompileCMOVcc(params, instructionIndex, getAssignment, result);
	}
	else if (isOpcodeSETcc(instruction->opcode))
	{
		return decompileSETcc(params, instructionIndex, getAssignment, result);
	}
	else if (instruction->opcode == XCHG)
	{
		return decompileXCHG(params, instructionIndex, targetReg, getAssignment, result);
	}
	else if (instruction->opcode == CMPXCHG)
	{
		return decompileCMPXCHG(params, instructionIndex, getAssignment, result);
	}

	return ERROR_JDC;
}

static enum JdcStatus decompileBinaryOperation(struct DecompilationParameters* params, int32_t instructionIndex, bool getAssignment, const char* regularOperator, const char* assignmentOperator, struct JdcStr* result)
{
	struct JdcStr decompiledSecondOperand = initializeJdcStr();
	if (ERROR_JDC == decompileOperand(params, instructionIndex, 1, 1, &decompiledSecondOperand))
	{
		freeJdcStr(&decompiledSecondOperand);
		return ERROR_JDC;
	}

	if (getAssignment)
	{
		if (!doesInstructionAssignToOperand(params, instructionIndex, 0)) 
		{
			freeJdcStr(&decompiledSecondOperand);
			return ERROR_JDC;
		}
		
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledSecondOperand);
			return ERROR_JDC;
		}

		addDecompiledLine(params, result, instructionIndex, "%s%s%s;", decompiledFirstOperand.buffer, assignmentOperator, decompiledSecondOperand.buffer);

		freeJdcStr(&decompiledFirstOperand);
		freeJdcStr(&decompiledSecondOperand);
		return SUCCESS_JDC;
	}

	sprintfJdc(result, false, "%s%s", regularOperator, decompiledSecondOperand.buffer);
	freeJdcStr(&decompiledSecondOperand);
	return SUCCESS_JDC;
}

static enum JdcStatus decompileSumStatusFlags(struct DecompilationParameters* params, int32_t instructionIndex, enum Register targetReg, bool getAssignment, bool notStatusFlag, struct JdcStr* result) 
{
	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];
	enum Mnemonic opcode = instruction->opcode;

	struct JdcStr decompiledFirstOperand = initializeJdcStr();
	if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
	{
		freeJdcStr(&decompiledFirstOperand);
		return ERROR_JDC;
	}

	struct JdcStr decompiledSecondOperand = initializeJdcStr();
	if (ERROR_JDC == decompileOperand(params, instructionIndex, 1, 1, &decompiledSecondOperand))
	{
		freeJdcStr(&decompiledFirstOperand);
		freeJdcStr(&decompiledSecondOperand);
		return ERROR_JDC;
	}

	if (isOpcodeCmp(opcode) || opcode == SUB) 
	{
		wrapJdcStrInParentheses(&decompiledSecondOperand);
		strcatStartJdc(&decompiledSecondOperand, "-");
	}

	int32_t operandSize = getSizeOfOperand(&instruction->operands[0]);

	struct JdcStr cfExpression = initializeJdcStr();
	struct DataType uintType = getOperandDataType(opcode, &instruction->operands[0]);
	uintType.isUnsigned = 1;
	struct JdcStr uintTypeStr = initializeJdcStr();
	dataTypeToStr(uintType, params->useStdInt, &uintTypeStr);
	sprintfJdc(&cfExpression, false, "((%s)(%s) + (%s)(%s)) %s (%s)(%s)", uintTypeStr.buffer, decompiledFirstOperand.buffer, uintTypeStr.buffer, decompiledSecondOperand.buffer, notStatusFlag ? ">=" : "<", uintTypeStr.buffer, decompiledFirstOperand.buffer);
	freeJdcStr(&uintTypeStr);

	struct JdcStr pfExpression = initializeJdcStr();
	sprintfJdc(&pfExpression, false, "__popcnt((%s + %s) & 0xFF) % 2 %s 0", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer, notStatusFlag ? "!=" : "==");
	
	struct JdcStr afExpression = initializeJdcStr();
	sprintfJdc(&afExpression, false, "((%s)(%s & 0xF) + (%s)(%s & 0xF)) %s 0x10", primitiveTypeToStr(INT8_TYPE, params->useStdInt), decompiledFirstOperand.buffer, primitiveTypeToStr(INT8_TYPE, params->useStdInt), decompiledSecondOperand.buffer, notStatusFlag ? "<" : ">=");

	struct JdcStr zfExpression = initializeJdcStr();
	sprintfJdc(&zfExpression, false, "(%s + %s) %s 0", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer, notStatusFlag ? "!=" : "==");

	struct JdcStr sfExpression = initializeJdcStr();
	sprintfJdc(&sfExpression, false, "%s(((%s + %s) >> %d) & 1)", notStatusFlag ? "!" : "", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer, (operandSize * 8) - 1);

	struct JdcStr ofExpression = initializeJdcStr();
	sprintfJdc(&ofExpression, false, "%s((%s < 0 && %s < 0 && (%s + %s) > 0) || (%s > 0 && %s > 0 && (%s + %s) < 0))", notStatusFlag ? "!" : "", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer, decompiledFirstOperand.buffer, decompiledSecondOperand.buffer, decompiledFirstOperand.buffer, decompiledSecondOperand.buffer, decompiledFirstOperand.buffer, decompiledSecondOperand.buffer);

	freeJdcStr(&decompiledFirstOperand);
	freeJdcStr(&decompiledSecondOperand);

	if (getAssignment) 
	{
		struct RegisterVariable* cfVar = doesInstructionAssignToRegVar(params, instructionIndex, CF);
		if (cfVar)
		{
			addDecompiledLine(params, result, instructionIndex, "%s = %s;", cfVar->name.buffer, cfExpression.buffer);
		}

		struct RegisterVariable* pfVar = doesInstructionAssignToRegVar(params, instructionIndex, PF);
		if (pfVar)
		{
			addDecompiledLine(params, result, instructionIndex, "%s = %s;", pfVar->name.buffer, pfExpression.buffer);
		}

		struct RegisterVariable* afVar = doesInstructionAssignToRegVar(params, instructionIndex, AF);
		if (afVar)
		{
			addDecompiledLine(params, result, instructionIndex, "%s = %s;", afVar->name.buffer, afExpression.buffer);
		}

		struct RegisterVariable* zfVar = doesInstructionAssignToRegVar(params, instructionIndex, ZF);
		if (zfVar)
		{
			addDecompiledLine(params, result, instructionIndex, "%s = %s;", zfVar->name.buffer, zfExpression.buffer);
		}

		struct RegisterVariable* sfVar = doesInstructionAssignToRegVar(params, instructionIndex, SF);
		if (sfVar)
		{
			addDecompiledLine(params, result, instructionIndex, "%s = %s;", sfVar->name.buffer, sfExpression.buffer);
		}

		struct RegisterVariable* ofVar = doesInstructionAssignToRegVar(params, instructionIndex, OF);
		if (ofVar)
		{
			addDecompiledLine(params, result, instructionIndex, "%s = %s;", ofVar->name.buffer, ofExpression.buffer);
		}
	}
	else 
	{
		switch (targetReg)
		{
		case CF:
			strcpyJdc(result, cfExpression.buffer);
			break;
		case PF:
			strcpyJdc(result, pfExpression.buffer);
			break;
		case AF:
			strcpyJdc(result, afExpression.buffer);
			break;
		case ZF:
			strcpyJdc(result, zfExpression.buffer);
			break;
		case SF:
			strcpyJdc(result, sfExpression.buffer);
			break;
		case OF:
			strcpyJdc(result, ofExpression.buffer);
			break;
		}
	}
	
	freeJdcStr(&cfExpression);
	freeJdcStr(&pfExpression);
	freeJdcStr(&afExpression);
	freeJdcStr(&zfExpression);
	freeJdcStr(&sfExpression);
	freeJdcStr(&ofExpression);
	return SUCCESS_JDC;
}

static enum JdcStatus decompileIncDec(struct DecompilationParameters* params, int32_t instructionIndex, enum Register targetReg, bool getAssignment, bool notStatusFlag, struct JdcStr* result)
{
	bool isInc = params->instructions[instructionIndex].opcode == INC;
	if (getAssignment)
	{
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			return ERROR_JDC;
		}

		if (doesInstructionAssignToOperand(params, instructionIndex, 0)) 
		{
			addDecompiledLine(params, result, instructionIndex, "%s%s;", decompiledFirstOperand.buffer, isInc ? "++" : "--");
		}

		struct RegisterVariable* zfVar = doesInstructionAssignToRegVar(params, instructionIndex, ZF);
		if (zfVar)
		{
			addDecompiledLine(params, result, instructionIndex, "%s = %s %s 0;", zfVar->name.buffer, decompiledFirstOperand.buffer, notStatusFlag ? "!=" : "==");
		}

		freeJdcStr(&decompiledFirstOperand);
		return SUCCESS_JDC;
	}

	if (targetReg == ZF) 
	{
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			return ERROR_JDC;
		}

		sprintfJdc(result, false, "%s %s 0",  decompiledFirstOperand.buffer, notStatusFlag ? "!=" : "==");
		freeJdcStr(&decompiledFirstOperand);
		return SUCCESS_JDC;
	}

	sprintfJdc(result, false, " %s 1", isInc ? "+" : "-");
	return SUCCESS_JDC;
}

static enum JdcStatus decompileNeg(struct DecompilationParameters* params, int32_t instructionIndex, enum Register targetReg, bool getAssignment, bool notStatusFlag, struct JdcStr* result, bool* placeOperatorInfront)
{
	if (getAssignment)
	{
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			return ERROR_JDC;
		}

		struct RegisterVariable* cfVar = doesInstructionAssignToRegVar(params, instructionIndex, CF);
		if (cfVar)
		{
			addDecompiledLine(params, result, instructionIndex, "%s = (%s) %s 0;", cfVar->name.buffer, decompiledFirstOperand.buffer, notStatusFlag ? "==" : "!=");
		}

		struct RegisterVariable* ofVar = doesInstructionAssignToRegVar(params, instructionIndex, OF);
		if (ofVar)
		{
			// NEG is the same as (0 - firstOperand). it will only overflow if the firstOperand is equal to the smallest 2's complement integer
			uint8_t operandSize = getSizeOfOperand(&params->instructions[instructionIndex].operands[0]);
			addDecompiledLine(params, result, instructionIndex, "%s = (%s) %s -(1 << %d);", ofVar->name.buffer, decompiledFirstOperand.buffer, notStatusFlag ? "!=" : "==", ((operandSize * 8) - 1));
		}

		if (doesInstructionAssignToOperand(params, instructionIndex, 0)) 
		{
			addDecompiledLine(params, result, instructionIndex, "%s = -(%s);", decompiledFirstOperand.buffer, decompiledFirstOperand.buffer);
		}
		
		freeJdcStr(&decompiledFirstOperand);
		return SUCCESS_JDC;
	}

	if (targetReg == CF || targetReg == OF)
	{
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			return ERROR_JDC;
		}

		if (targetReg == CF) 
		{
			sprintfJdc(result, false, "%s %s 0", decompiledFirstOperand.buffer, notStatusFlag ? "==" : "!=");
		}
		else 
		{
			uint8_t operandSize = getSizeOfOperand(&params->instructions[instructionIndex].operands[0]);
			sprintfJdc(result, false, "%s %s -(1 << %d)", decompiledFirstOperand.buffer, notStatusFlag ? "!=" : "==", ((operandSize * 8) - 1));
		}

		freeJdcStr(&decompiledFirstOperand);
	}
	else 
	{
		if (placeOperatorInfront) { *placeOperatorInfront = true; }
		strcpyJdc(result, "-");
	}
	
	return SUCCESS_JDC;
}

static enum JdcStatus decompileNot(struct DecompilationParameters* params, int32_t instructionIndex, bool getAssignment, struct JdcStr* result, bool* placeOperatorInfront)
{
	if (getAssignment)
	{
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			return ERROR_JDC;
		}

		addDecompiledLine(params, result, instructionIndex, "%s = ~(%s);", decompiledFirstOperand.buffer, decompiledFirstOperand.buffer);

		freeJdcStr(&decompiledFirstOperand);
		return SUCCESS_JDC;
	}

	if (placeOperatorInfront) { *placeOperatorInfront = true; }
	strcpyJdc(result, "~");
	return SUCCESS_JDC;
}

static enum JdcStatus decompileOr(struct DecompilationParameters* params, int32_t instructionIndex, bool getAssignment, struct JdcStr* result)
{
	struct Operand* secondOperand = &params->instructions[instructionIndex].operands[1];
	if (secondOperand->type == IMMEDIATE && isImmediateAllOnes(&secondOperand->immediate))
	{
		if (getAssignment)
		{
			struct JdcStr decompiledFirstOperand = initializeJdcStr();
			if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
			{
				freeJdcStr(&decompiledFirstOperand);
				return ERROR_JDC;
			}
			
			addDecompiledLine(params, result, instructionIndex, "%s = 0x%llX;", decompiledFirstOperand.buffer, secondOperand->immediate.value);

			freeJdcStr(&decompiledFirstOperand);
			return SUCCESS_JDC;
		}

		sprintfJdc(result, false, "0x%llX", secondOperand->immediate.value);
		return SUCCESS_JDC;
	}

	return decompileBinaryOperation(params, instructionIndex, getAssignment, " | ", " |= ", result);
}

static enum JdcStatus decompileXor(struct DecompilationParameters* params, int32_t instructionIndex, bool getAssignment, struct JdcStr* result)
{
	struct Operand* firstOperand = &params->instructions[instructionIndex].operands[0];
	struct Operand* secondOperand = &params->instructions[instructionIndex].operands[1];
	if (compareOperands(firstOperand, secondOperand))
	{
		if (getAssignment) 
		{
			struct JdcStr decompiledFirstOperand = initializeJdcStr();
			if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
			{
				freeJdcStr(&decompiledFirstOperand);
				return ERROR_JDC;
			}
			
			addDecompiledLine(params, result, instructionIndex, "%s = 0;", decompiledFirstOperand.buffer);

			freeJdcStr(&decompiledFirstOperand);
			return SUCCESS_JDC;
		}
		
		strcpyJdc(result, "0");
		return SUCCESS_JDC;
	}
	
	return decompileBinaryOperation(params, instructionIndex, getAssignment, " ^ ", " ^= ", result);
}

static enum JdcStatus decompileBitTest(struct DecompilationParameters* params, int32_t instructionIndex, bool getAssignment, bool notStatusFlag, struct JdcStr* result)
{
	struct JdcStr decompiledFirstOperand = initializeJdcStr();
	if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
	{
		freeJdcStr(&decompiledFirstOperand);
		return ERROR_JDC;
	}

	struct JdcStr expression = initializeJdcStr();

	struct Operand* secondOperand = &params->instructions[instructionIndex].operands[1];
	if (secondOperand->type == IMMEDIATE)
	{
		uint64_t bitMask = (uint64_t)(1) << (uint8_t)(secondOperand->immediate.value);
		sprintfJdc(&expression, false, "%s & 0x%llX", decompiledFirstOperand.buffer, bitMask);
	}
	else
	{
		struct JdcStr decompiledSecondOperand = initializeJdcStr();
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 1, 1, &decompiledSecondOperand))
		{
			freeJdcStr(&decompiledSecondOperand);
			return ERROR_JDC;
		}

		sprintfJdc(&expression, false, "%s & (1 << %s)", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer);
		freeJdcStr(&decompiledSecondOperand);
	}

	freeJdcStr(&decompiledFirstOperand);

	if (notStatusFlag)
	{
		strcatJdc(&expression, " == 0");
	}
	else
	{
		strcatJdc(&expression, " != 0");
	}

	if (getAssignment) 
	{
		struct RegisterVariable* cfVar = doesInstructionAssignToRegVar(params, instructionIndex, CF);
		if (!cfVar)
		{
			return ERROR_JDC;
		}

		addDecompiledLine(params, result, instructionIndex, "%s = %s;", cfVar->name.buffer, expression.buffer);
	}
	else 
	{
		strcatJdc(result, expression.buffer);
	}

	freeJdcStr(&expression);
	return SUCCESS_JDC;
}

static enum JdcStatus decompileBitSet(struct DecompilationParameters* params, int32_t instructionIndex, enum Register targetReg, bool getAssignment, bool notStatusFlag, struct JdcStr* result)
{
	if (getAssignment && doesInstructionAssignToRegVar(params, instructionIndex, CF))
	{
		decompileBitTest(params, instructionIndex, 1, notStatusFlag, result);
	}
	else if (!getAssignment && targetReg == CF)
	{
		return decompileBitTest(params, instructionIndex, 0, notStatusFlag, result);
	}
	
	struct JdcStr decompiledFirstOperand = initializeJdcStr();
	if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
	{
		freeJdcStr(&decompiledFirstOperand);
		return ERROR_JDC;
	}

	struct Operand* secondOperand = &params->instructions[instructionIndex].operands[1];
	if (secondOperand->type == IMMEDIATE)
	{
		uint64_t bitMask = (uint64_t)(1) << (uint8_t)(secondOperand->immediate.value);
		if (getAssignment) 
		{
			addDecompiledLine(params, result, instructionIndex, "%s |= 0x%llX;", decompiledFirstOperand.buffer, bitMask);
		}
		else 
		{
			sprintfJdc(result, false, " | 0x%llX", bitMask);
		}
	}
	else
	{
		struct JdcStr decompiledSecondOperand = initializeJdcStr();
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 1, 1, &decompiledSecondOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledSecondOperand);
			return ERROR_JDC;
		}

		if (getAssignment) 
		{
			addDecompiledLine(params, result, instructionIndex, "%s |= (1 << %s);", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer);
		}
		else 
		{
			sprintfJdc(result, false, " | (1 << %s)", decompiledSecondOperand.buffer);
		}

		freeJdcStr(&decompiledSecondOperand);
	}

	freeJdcStr(&decompiledFirstOperand);
	return SUCCESS_JDC;
}

static enum JdcStatus decompileBitReset(struct DecompilationParameters* params, int32_t instructionIndex, enum Register targetReg, bool getAssignment, bool notStatusFlag, struct JdcStr* result)
{
	if (getAssignment && doesInstructionAssignToRegVar(params, instructionIndex, CF))
	{
		decompileBitTest(params, instructionIndex, 1, notStatusFlag, result);
	}
	else if (!getAssignment && targetReg == CF)
	{
		return decompileBitTest(params, instructionIndex, 0, notStatusFlag, result);
	}
	
	struct JdcStr decompiledFirstOperand = initializeJdcStr();
	if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
	{
		freeJdcStr(&decompiledFirstOperand);
		return ERROR_JDC;
	}

	struct Operand* secondOperand = &params->instructions[instructionIndex].operands[1];
	if (secondOperand->type == IMMEDIATE)
	{
		uint64_t bitMask = (uint64_t)(1) << (uint8_t)(secondOperand->immediate.value);
		if (getAssignment)
		{
			addDecompiledLine(params, result, instructionIndex, "%s &= ~0x%llX;", decompiledFirstOperand.buffer, bitMask);
		}
		else
		{
			sprintfJdc(result, false, " & ~0x%llX", bitMask);
		}
	}
	else
	{
		struct JdcStr decompiledSecondOperand = initializeJdcStr();
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 1, 1, &decompiledSecondOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledSecondOperand);
			return ERROR_JDC;
		}

		if (getAssignment)
		{
			addDecompiledLine(params, result, instructionIndex, "%s &= ~(1 << %s);", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer);
		}
		else
		{
			sprintfJdc(result, false, " & ~(1 << %s)", decompiledSecondOperand.buffer);
		}

		freeJdcStr(&decompiledSecondOperand);
	}

	freeJdcStr(&decompiledFirstOperand);
	return SUCCESS_JDC;
}

static enum JdcStatus decompileADC(struct DecompilationParameters* params, int32_t instructionIndex, bool getAssignment, struct JdcStr* result)
{
	struct JdcStr decompiledCF = initializeJdcStr();
	if (!decompileRegister(params, instructionIndex, -1, CF, 1, 0, &decompiledCF, 0))
	{
		freeJdcStr(&decompiledCF);
		return ERROR_JDC;
	}

	struct Operand* secondOperand = &params->instructions[instructionIndex].operands[1];
	if (secondOperand->type == IMMEDIATE && secondOperand->immediate.value == 0)
	{
		if (getAssignment)
		{
			struct JdcStr decompiledFirstOperand = initializeJdcStr();
			if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
			{
				freeJdcStr(&decompiledCF);
				freeJdcStr(&decompiledFirstOperand);
				return ERROR_JDC;
			}

			addDecompiledLine(params, result, instructionIndex, "%s += %s;", decompiledFirstOperand.buffer, decompiledCF.buffer);

			freeJdcStr(&decompiledFirstOperand);
		}
		else
		{
			sprintfJdc(result, false, " + %s", decompiledCF.buffer);
		}

		freeJdcStr(&decompiledCF);
		return SUCCESS_JDC;
	}

	struct JdcStr decompiledSecondOperand = initializeJdcStr();
	if (ERROR_JDC == decompileOperand(params, instructionIndex, 1, 1, &decompiledSecondOperand))
	{
		freeJdcStr(&decompiledSecondOperand);
		freeJdcStr(&decompiledCF);
		return ERROR_JDC;
	}

	if (getAssignment)
	{
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledSecondOperand);
			freeJdcStr(&decompiledCF);
			return ERROR_JDC;
		}

		addDecompiledLine(params, result, instructionIndex, "%s += (%s) + (%s);", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer, decompiledCF.buffer);

		freeJdcStr(&decompiledFirstOperand);
	}
	else
	{
		sprintfJdc(result, false, " + (%s) + (%s)", decompiledSecondOperand.buffer, decompiledCF.buffer);
	}

	freeJdcStr(&decompiledSecondOperand);
	freeJdcStr(&decompiledCF);
	return SUCCESS_JDC;
}

static enum JdcStatus decompileSBB(struct DecompilationParameters* params, int32_t instructionIndex, bool getAssignment, struct JdcStr* result)
{
	struct JdcStr decompiledCF = initializeJdcStr();
	if (!decompileRegister(params, instructionIndex, -1, CF, 1, 0, &decompiledCF, 0))
	{
		freeJdcStr(&decompiledCF);
		return ERROR_JDC;
	}
	
	struct Operand* firstOperand = &params->instructions[instructionIndex].operands[0];
	struct Operand* secondOperand = &params->instructions[instructionIndex].operands[1];
	if (compareOperands(firstOperand, secondOperand)) 
	{
		if (getAssignment)
		{
			struct JdcStr decompiledFirstOperand = initializeJdcStr();
			if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
			{
				freeJdcStr(&decompiledCF);
				freeJdcStr(&decompiledFirstOperand);
				return ERROR_JDC;
			}
			
			addDecompiledLine(params, result, instructionIndex, "%s = -(%s);", decompiledFirstOperand.buffer, decompiledCF.buffer);

			freeJdcStr(&decompiledFirstOperand);
		}
		else
		{
			sprintfJdc(result, false, "-(%s)", decompiledCF.buffer);
		}

		freeJdcStr(&decompiledCF);
		return SUCCESS_JDC;
	}

	struct JdcStr decompiledSecondOperand = initializeJdcStr();
	if (ERROR_JDC == decompileOperand(params, instructionIndex, 1, 1, &decompiledSecondOperand))
	{
		freeJdcStr(&decompiledSecondOperand);
		freeJdcStr(&decompiledCF);
		return ERROR_JDC;
	}

	if (getAssignment) 
	{
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledSecondOperand);
			freeJdcStr(&decompiledCF);
			return ERROR_JDC;
		}
		
		addDecompiledLine(params, result, instructionIndex, "%s -= (%s + %s);", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer, decompiledCF.buffer);

		freeJdcStr(&decompiledFirstOperand);
	}
	else 
	{
		sprintfJdc(result, false, " - (%s + %s)", decompiledSecondOperand.buffer, decompiledCF.buffer);
	}

	freeJdcStr(&decompiledSecondOperand);
	freeJdcStr(&decompiledCF);
	return SUCCESS_JDC;
}

static enum JdcStatus decompileFLD(struct DecompilationParameters* params, int32_t instructionIndex, bool getAssignment, struct JdcStr* result)
{
	struct JdcStr decompiledFirstOperand = initializeJdcStr();
	if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
	{
		freeJdcStr(&decompiledFirstOperand);
		return ERROR_JDC;
	}
	
	if (getAssignment) 
	{ 
		struct JdcStr decompiledST0 = initializeJdcStr();
		if (!decompileRegister(params, instructionIndex, -1, ST0, 1, 0, &decompiledST0, 0))
		{
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledST0);
			return ERROR_JDC;
		}
		
		addDecompiledLine(params, result, instructionIndex, "%s = %s;", decompiledST0.buffer, decompiledFirstOperand.buffer);

		freeJdcStr(&decompiledFirstOperand);
		freeJdcStr(&decompiledST0);
		return SUCCESS_JDC;
	}

	sprintfJdc(result, false, "%s", decompiledFirstOperand.buffer);
	freeJdcStr(&decompiledFirstOperand);
	return SUCCESS_JDC;
}

static enum JdcStatus decompileIDIV(struct DecompilationParameters* params, int32_t instructionIndex, enum Register targetReg, bool getAssignment, struct JdcStr* result)
{
	struct JdcStr decompiledFirstOperand = initializeJdcStr();
	if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
	{
		freeJdcStr(&decompiledFirstOperand);
		return ERROR_JDC;
	}

	if (getAssignment) 
	{
		struct RegisterVariable* axVar = doesInstructionAssignToRegVar(params, instructionIndex, AX);
		if (axVar) 
		{
			addDecompiledLine(params, result, instructionIndex, "%s /= %s;", axVar->name.buffer, decompiledFirstOperand.buffer);
		}

		struct RegisterVariable* dxVar = doesInstructionAssignToRegVar(params, instructionIndex, DX);
		if (dxVar)
		{
			struct JdcStr decompiledAX = initializeJdcStr();
			if (!decompileRegister(params, instructionIndex, -1, AX, 1, 0, &decompiledAX, 0))
			{
				freeJdcStr(&decompiledFirstOperand);
				freeJdcStr(&decompiledAX);
				return ERROR_JDC;
			}

			addDecompiledLine(params, result, instructionIndex, "%s = %s %% %s;", dxVar->name.buffer, decompiledAX.buffer, decompiledFirstOperand.buffer);

			freeJdcStr(&decompiledAX);
		}

		freeJdcStr(&decompiledFirstOperand);
		return SUCCESS_JDC;
	}

	if (compareRegisters(targetReg, AX))
	{
		sprintfJdc(result, false, " / %s", decompiledFirstOperand.buffer);
		freeJdcStr(&decompiledFirstOperand);
		return SUCCESS_JDC;
	}
	else if (compareRegisters(targetReg, DX))
	{
		struct JdcStr decompiledAX = initializeJdcStr();
		if (!decompileRegister(params, instructionIndex, -1, AX, 1, 0, &decompiledAX, 0))
		{
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledAX);
			return ERROR_JDC;
		}
		
		sprintfJdc(result, false, "%s % %s", decompiledAX.buffer, decompiledFirstOperand.buffer);
		freeJdcStr(&decompiledAX);
		freeJdcStr(&decompiledFirstOperand);
		return SUCCESS_JDC;
	}
	
	return ERROR_JDC;
}

static enum JdcStatus decompileIMUL(struct DecompilationParameters* params, int32_t instructionIndex, enum Register targetReg, bool getAssignment, struct JdcStr* result)
{
	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];
	struct Operand* firstOperand = &instruction->operands[0];
	
	if (instruction->numOfOperands == 3)
	{
		struct Operand* secondOperand = &instruction->operands[1];
		struct Operand* thirdOperand = &instruction->operands[2];
		
		struct JdcStr decompiledThirdOperand = initializeJdcStr();
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 2, 1, &decompiledThirdOperand))
		{
			freeJdcStr(&decompiledThirdOperand);
			return ERROR_JDC;
		}

		if (getAssignment) 
		{
			struct JdcStr decompiledFirstOperand = initializeJdcStr();
			if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
			{
				freeJdcStr(&decompiledFirstOperand);
				freeJdcStr(&decompiledThirdOperand);
				return ERROR_JDC;
			}

			if (compareOperands(firstOperand, secondOperand))
			{
				addDecompiledLine(params, result, instructionIndex, "%s *= %s;", decompiledFirstOperand.buffer, decompiledThirdOperand.buffer);

				freeJdcStr(&decompiledThirdOperand);
				freeJdcStr(&decompiledFirstOperand);
				return SUCCESS_JDC;
			}

			struct JdcStr decompiledSecondOperand = initializeJdcStr();
			if (ERROR_JDC == decompileOperand(params, instructionIndex, 1, 1, &decompiledSecondOperand))
			{
				freeJdcStr(&decompiledFirstOperand);
				freeJdcStr(&decompiledSecondOperand);
				freeJdcStr(&decompiledThirdOperand);
				return ERROR_JDC;
			}

			addDecompiledLine(params, result, instructionIndex, "%s = (%s * %s);", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer, decompiledThirdOperand.buffer);

			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledSecondOperand);
			freeJdcStr(&decompiledThirdOperand);
			return SUCCESS_JDC;
		}

		if (compareOperands(firstOperand, secondOperand))
		{
			sprintfJdc(result, false, " * %s", decompiledThirdOperand.buffer);
			freeJdcStr(&decompiledThirdOperand);
			return SUCCESS_JDC;
		}

		struct JdcStr decompiledSecondOperand = initializeJdcStr();
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 1, 1, &decompiledSecondOperand))
		{
			freeJdcStr(&decompiledSecondOperand);
			freeJdcStr(&decompiledThirdOperand);
			return ERROR_JDC;
		}

		sprintfJdc(result, false, "(%s * %s)", decompiledSecondOperand.buffer, decompiledThirdOperand.buffer);
		freeJdcStr(&decompiledSecondOperand);
		freeJdcStr(&decompiledThirdOperand);
		return SUCCESS_JDC;
	}
	else if (instruction->numOfOperands == 2) 
	{
		return decompileBinaryOperation(params, instructionIndex, getAssignment, " * ", " *= ", result);
	}

	// one operand form
	
	struct JdcStr decompiledFirstOperand = initializeJdcStr();
	if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
	{
		freeJdcStr(&decompiledFirstOperand);
		return ERROR_JDC;
	}

	if (getAssignment)
	{
		struct RegisterVariable* axVar = doesInstructionAssignToRegVar(params, instructionIndex, AX);
		if (axVar)
		{
			addDecompiledLine(params, result, instructionIndex, "%s *= %s;", axVar->name.buffer, decompiledFirstOperand.buffer);
		}
		
		struct RegisterVariable* dxVar = doesInstructionAssignToRegVar(params, instructionIndex, DX);
		if (dxVar)
		{
			struct JdcStr decompiledAX = initializeJdcStr();
			if (!decompileRegister(params, instructionIndex, -1, AX, 1, 0, &decompiledAX, 0))
			{
				freeJdcStr(&decompiledAX);
				freeJdcStr(&decompiledFirstOperand);
				return ERROR_JDC;
			}
			
			addDecompiledLine(params, result, instructionIndex, "%s = %s >> %d;", dxVar->name.buffer, decompiledAX.buffer, decompiledFirstOperand.buffer, 8 * getSizeOfOperand(firstOperand));

			freeJdcStr(&decompiledAX);
		}

		freeJdcStr(&decompiledFirstOperand);
		return SUCCESS_JDC;
	}

	if (compareRegisters(targetReg, AX))
	{
		sprintfJdc(result, false, " * %s", decompiledFirstOperand.buffer);
		freeJdcStr(&decompiledFirstOperand);
		return SUCCESS_JDC;
	}
	else if (compareRegisters(targetReg, DX))
	{
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			return ERROR_JDC;
		}

		struct JdcStr decompiledAX = initializeJdcStr();
		if (!decompileRegister(params, instructionIndex, -1, AX, 1, 0, &decompiledAX, 0))
		{
			freeJdcStr(&decompiledAX);
			return ERROR_JDC;
		}

		sprintfJdc(result, false, "(%s * %s) >> %d", decompiledAX.buffer, decompiledFirstOperand.buffer, getSizeOfOperand(firstOperand));
	}

	freeJdcStr(&decompiledFirstOperand);
	return SUCCESS_JDC;
}

static enum JdcStatus decompileCMOVcc(struct DecompilationParameters* params, int32_t instructionIndex, bool getAssignment, struct JdcStr* result)
{
	struct JdcStr comparisonStr = initializeJdcStr();
	if (!decompileComparison(params, instructionIndex, 0, &comparisonStr))
	{
		freeJdcStr(&comparisonStr);
		return ERROR_JDC;
	}

	struct JdcStr decompiledFirstOperand = initializeJdcStr();
	if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
	{
		freeJdcStr(&comparisonStr);
		freeJdcStr(&decompiledFirstOperand);
		return ERROR_JDC;
	}

	struct JdcStr decompiledSecondOperand = initializeJdcStr();
	if (ERROR_JDC == decompileOperand(params, instructionIndex, 1, 1, &decompiledSecondOperand))
	{
		freeJdcStr(&comparisonStr);
		freeJdcStr(&decompiledFirstOperand);
		freeJdcStr(&decompiledSecondOperand);
		return ERROR_JDC;
	}

	if (getAssignment) 
	{ 
		addDecompiledLine(params, result, instructionIndex, "%s = (%s ? %s : %s);", decompiledFirstOperand.buffer, comparisonStr.buffer, decompiledSecondOperand.buffer, decompiledFirstOperand.buffer);
	}
	else 
	{
		sprintfJdc(result, false, "(%s ? %s : %s)", comparisonStr.buffer, decompiledSecondOperand.buffer, decompiledFirstOperand.buffer);
	}

	freeJdcStr(&comparisonStr);
	freeJdcStr(&decompiledFirstOperand);
	freeJdcStr(&decompiledSecondOperand);
	return SUCCESS_JDC;
}

static enum JdcStatus decompileSETcc(struct DecompilationParameters* params, int32_t instructionIndex, bool getAssignment, struct JdcStr* result)
{
	struct JdcStr comparisonStr = initializeJdcStr();
	if (!decompileComparison(params, instructionIndex, 0, &comparisonStr))
	{
		freeJdcStr(&comparisonStr);
		return ERROR_JDC;
	}

	if (getAssignment) 
	{ 
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
		{
			freeJdcStr(&comparisonStr);
			freeJdcStr(&decompiledFirstOperand);
			return ERROR_JDC;
		}

		addDecompiledLine(params, result, instructionIndex, "%s = %s;", decompiledFirstOperand.buffer, comparisonStr.buffer);

		freeJdcStr(&comparisonStr);
		freeJdcStr(&decompiledFirstOperand);
		return SUCCESS_JDC;
	}

	sprintfJdc(result, false, "%s", comparisonStr.buffer);
	freeJdcStr(&comparisonStr);
	return SUCCESS_JDC;
}

static enum JdcStatus decompilePop(struct DecompilationParameters* params, int32_t instructionIndex, bool getAssignment, struct JdcStr* result)
{
	struct JdcStr value = initializeJdcStr();

	int32_t stackOffset = 0;
	bool gotValue = false;
	for (int32_t i = instructionIndex; i >= 0; i--)
	{
		if (params->instructions[i].opcode == PUSH)
		{
			stackOffset--;
			if (stackOffset == 0)
			{
				if (ERROR_JDC == decompileOperand(params, i, 0, 1, &value))
				{
					freeJdcStr(&value);
					return ERROR_JDC;
				}

				addAssociatedInstruction(params->currentFunc, i);
				gotValue = true;
				break;
			}
		}
		else if (params->instructions[i].opcode == PUSHF)
		{
			stackOffset--;
			if (stackOffset == 0)
			{
				strcpyJdc(&value, "__readeflags()"); // Windows function

				addAssociatedInstruction(params->currentFunc, i);
				gotValue = true;
				break;
			}
		}
		else if (params->instructions[i].opcode == POP)
		{
			stackOffset++;
		}
	}

	struct Operand* firstOperand = &params->instructions[instructionIndex].operands[0];
	if (getAssignment)
	{
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			return ERROR_JDC;
		}

		if (gotValue) 
		{
			addDecompiledLine(params, result, instructionIndex, "%s = %s;", decompiledFirstOperand.buffer, value.buffer);

			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&value);
			return SUCCESS_JDC;
		}
		else if (firstOperand->type == REGISTER)
		{
			addDecompiledLine(params, result, instructionIndex, "%s = %s;", decompiledFirstOperand.buffer, registerStrs[firstOperand->reg]);

			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&value);
			return SUCCESS_JDC;
		}
		
		freeJdcStr(&decompiledFirstOperand);
		freeJdcStr(&value);
		return ERROR_JDC;
	}

	if (gotValue)
	{
		sprintfJdc(result, false, "%s", value.buffer);
		freeJdcStr(&value);
		return SUCCESS_JDC;
	}
	else if (firstOperand->type == REGISTER)
	{
		sprintfJdc(result, false, "%s", registerStrs[firstOperand->reg]);
		freeJdcStr(&value);
		return SUCCESS_JDC;
	}

	freeJdcStr(&value);
	return ERROR_JDC;
}

static enum JdcStatus decompileXCHG(struct DecompilationParameters* params, int32_t instructionIndex, enum Register targetReg, bool getAssignment, struct JdcStr* result)
{
	struct Operand* firstOperand = &params->instructions[instructionIndex].operands[0];
	
	if (getAssignment) 
	{
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			return ERROR_JDC;
		}

		struct JdcStr decompiledSecondOperand = initializeJdcStr();
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 1, 1, &decompiledSecondOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledSecondOperand);
			return ERROR_JDC;
		}
		
		if ((firstOperand->type == REGISTER && compareRegisters(firstOperand->reg, targetReg)) || firstOperand->type == MEM_ADDRESS)
		{
			addDecompiledLine(params, result, instructionIndex, "%s = %s;", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer);
		}
		else 
		{
			addDecompiledLine(params, result, instructionIndex, "%s = %s;", decompiledSecondOperand.buffer, decompiledFirstOperand.buffer);
		}

		freeJdcStr(&decompiledFirstOperand);
		freeJdcStr(&decompiledSecondOperand);
		return SUCCESS_JDC;
	}

	if (firstOperand->type == REGISTER && compareRegisters(firstOperand->reg, targetReg))
	{
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 1, 1, result))
		{
			return ERROR_JDC;
		}
	}
	else 
	{
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, result))
		{
			return ERROR_JDC;
		}
	}

	return SUCCESS_JDC;
}

static enum JdcStatus decompileCMPXCHG(struct DecompilationParameters* params, int32_t instructionIndex, bool getAssignment, struct JdcStr* result) 
{
	if (!getAssignment) 
	{
		return ERROR_JDC;
	}

	struct RegisterVariable* axVar = doesInstructionAssignToRegVar(params, instructionIndex, AX);
	if (!axVar) 
	{
		return ERROR_JDC;
	}

	struct JdcStr decompiledAX = initializeJdcStr();
	if (!decompileRegister(params, instructionIndex, -1, AX, 1, 0, &decompiledAX, 0))
	{
		freeJdcStr(&decompiledAX);
		return ERROR_JDC;
	}

	struct JdcStr decompiledFirstOperand = initializeJdcStr();
	if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
	{
		freeJdcStr(&decompiledAX);
		freeJdcStr(&decompiledFirstOperand);
		return ERROR_JDC;
	}

	addDecompiledLine(params, result, instructionIndex, "if (%s == %s)", decompiledAX.buffer, decompiledFirstOperand.buffer);
	addDecompiledLine(params, result, instructionIndex, "{");
	params->numOfIndents++;

	struct RegisterVariable* zfVar = doesInstructionAssignToRegVar(params, instructionIndex, ZF);
	if (zfVar) 
	{
		addDecompiledLine(params, result, instructionIndex, "%s = 1;", zfVar->name.buffer);
	}

	struct JdcStr decompiledSecondOperand = initializeJdcStr();
	if (ERROR_JDC == decompileOperand(params, instructionIndex, 1, 1, &decompiledSecondOperand))
	{
		freeJdcStr(&decompiledAX);
		freeJdcStr(&decompiledFirstOperand);
		freeJdcStr(&decompiledSecondOperand);
		return ERROR_JDC;
	}

	addDecompiledLine(params, result, instructionIndex, "%s = %s;", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer);

	params->numOfIndents--;
	addDecompiledLine(params, result, instructionIndex, "}");
	addDecompiledLine(params, result, instructionIndex, "else");
	addDecompiledLine(params, result, instructionIndex, "{");
	params->numOfIndents++;

	if (zfVar)
	{
		addDecompiledLine(params, result, instructionIndex, "%s = 0;", zfVar->name.buffer);
	}

	addDecompiledLine(params, result, instructionIndex, "%s = %s;", axVar->name.buffer, decompiledFirstOperand.buffer);

	params->numOfIndents--;
	addDecompiledLine(params, result, instructionIndex, "}");

	freeJdcStr(&decompiledAX);
	freeJdcStr(&decompiledFirstOperand);
	freeJdcStr(&decompiledSecondOperand);
	return SUCCESS_JDC;
}