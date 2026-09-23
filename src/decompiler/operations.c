#include "operations.h"
#include "assignment.h"
#include "decompilationUtils.h"
#include "functions.h"
#include "intrinsics.h"
#include "expressions.h"

unsigned char decompileOperation(struct DecompilationParameters* params, int instructionIndex, enum Register targetReg, unsigned char getAssignment, unsigned char notStatusFlag, struct JdcStr* result, unsigned char* placeOperatorInfront)
{
	if (placeOperatorInfront) { *placeOperatorInfront = 0; }
	
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
			return 0;
		}

		strcpyJdc(result, returnedVar->name.buffer);
		return 1;
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

	return 0;
}

static unsigned char decompileBinaryOperation(struct DecompilationParameters* params, int instructionIndex, unsigned char getAssignment, const char* regularOperator, const char* assignmentOperator, struct JdcStr* result)
{
	struct JdcStr decompiledSecondOperand = initializeJdcStr();
	if (!decompileOperand(params, instructionIndex, 1, 1, &decompiledSecondOperand))
	{
		freeJdcStr(&decompiledSecondOperand);
		return 0;
	}

	if (getAssignment)
	{
		if (!doesInstructionAssignToOperand(params, instructionIndex, 0)) 
		{
			freeJdcStr(&decompiledSecondOperand);
			return 0;
		}
		
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (!decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledSecondOperand);
			return 0;
		}

		addDecompiledLine(params, result, instructionIndex, "%s%s%s;", decompiledFirstOperand.buffer, assignmentOperator, decompiledSecondOperand.buffer);

		freeJdcStr(&decompiledFirstOperand);
		freeJdcStr(&decompiledSecondOperand);
		return 1;
	}

	sprintfJdc(result, 0, "%s%s", regularOperator, decompiledSecondOperand.buffer);
	freeJdcStr(&decompiledSecondOperand);
	return 1;
}

static unsigned char decompileSumStatusFlags(struct DecompilationParameters* params, int instructionIndex, enum Register targetReg, unsigned char getAssignment, unsigned char notStatusFlag, struct JdcStr* result) 
{
	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];
	enum Mnemonic opcode = instruction->opcode;

	struct JdcStr decompiledFirstOperand = initializeJdcStr();
	if (!decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
	{
		freeJdcStr(&decompiledFirstOperand);
		return 0;
	}

	struct JdcStr decompiledSecondOperand = initializeJdcStr();
	if (!decompileOperand(params, instructionIndex, 1, 1, &decompiledSecondOperand))
	{
		freeJdcStr(&decompiledFirstOperand);
		freeJdcStr(&decompiledSecondOperand);
		return 0;
	}

	if (isOpcodeCmp(opcode) || opcode == SUB) 
	{
		wrapJdcStrInParentheses(&decompiledSecondOperand);
		strcatStartJdc(&decompiledSecondOperand, "-");
	}

	int operandSize = getSizeOfOperand(&instruction->operands[0]);

	struct JdcStr cfExpression = initializeJdcStr();
	struct DataType uintType = getOperandDataType(opcode, &instruction->operands[0]);
	uintType.isUnsigned = 1;
	struct JdcStr uintTypeStr = initializeJdcStr();
	dataTypeToStr(uintType, &uintTypeStr);
	sprintfJdc(&cfExpression, 0, "((%s)(%s) + (%s)(%s)) %s (%s)(%s)", uintTypeStr.buffer, decompiledFirstOperand.buffer, uintTypeStr.buffer, decompiledSecondOperand.buffer, notStatusFlag ? ">=" : "<", uintTypeStr.buffer, decompiledFirstOperand.buffer);
	freeJdcStr(&uintTypeStr);

	struct JdcStr pfExpression = initializeJdcStr();
	sprintfJdc(&pfExpression, 0, "__popcnt((%s + %s) & 0xFF) % 2 %s 0", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer, notStatusFlag ? "!=" : "==");
	
	struct JdcStr afExpression = initializeJdcStr();
	sprintfJdc(&afExpression, 0, "((unsigned char)(%s & 0xF) + (unsigned char)(%s & 0xF)) %s 0x10", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer, notStatusFlag ? "<" : ">=");

	struct JdcStr zfExpression = initializeJdcStr();
	sprintfJdc(&zfExpression, 0, "(%s + %s) %s 0", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer, notStatusFlag ? "!=" : "==");

	struct JdcStr sfExpression = initializeJdcStr();
	sprintfJdc(&sfExpression, 0, "%s(((%s + %s) >> %d) & 1)", notStatusFlag ? "!" : "", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer, (operandSize * 8) - 1);

	struct JdcStr ofExpression = initializeJdcStr();
	sprintfJdc(&ofExpression, 0, "%s((%s < 0 && %s < 0 && (%s + %s) > 0) || (%s > 0 && %s > 0 && (%s + %s) < 0))", notStatusFlag ? "!" : "", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer, decompiledFirstOperand.buffer, decompiledSecondOperand.buffer, decompiledFirstOperand.buffer, decompiledSecondOperand.buffer, decompiledFirstOperand.buffer, decompiledSecondOperand.buffer);

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
	return 1;
}

static unsigned char decompileIncDec(struct DecompilationParameters* params, int instructionIndex, enum Register targetReg, unsigned char getAssignment, unsigned char notStatusFlag, struct JdcStr* result)
{
	unsigned char isInc = params->instructions[instructionIndex].opcode == INC;
	if (getAssignment)
	{
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (!decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			return 0;
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
		return 1;
	}

	if (targetReg == ZF) 
	{
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (!decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			return 0;
		}

		sprintfJdc(result, 0, "%s %s 0",  decompiledFirstOperand.buffer, notStatusFlag ? "!=" : "==");
		freeJdcStr(&decompiledFirstOperand);
		return 1;
	}

	sprintfJdc(result, 0, " %s 1", isInc ? "+" : "-");
	return 1;
}

static unsigned char decompileNeg(struct DecompilationParameters* params, int instructionIndex, enum Register targetReg, unsigned char getAssignment, unsigned char notStatusFlag, struct JdcStr* result, unsigned char* placeOperatorInfront)
{
	if (getAssignment)
	{
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (!decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			return 0;
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
			unsigned char operandSize = getSizeOfOperand(&params->instructions[instructionIndex].operands[0]);
			addDecompiledLine(params, result, instructionIndex, "%s = (%s) %s -(1 << %d);", ofVar->name.buffer, decompiledFirstOperand.buffer, notStatusFlag ? "!=" : "==", ((operandSize * 8) - 1));
		}

		if (doesInstructionAssignToOperand(params, instructionIndex, 0)) 
		{
			addDecompiledLine(params, result, instructionIndex, "%s = -(%s);", decompiledFirstOperand.buffer, decompiledFirstOperand.buffer);
		}
		
		freeJdcStr(&decompiledFirstOperand);
		return 1;
	}

	if (targetReg == CF || targetReg == OF)
	{
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (!decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			return 0;
		}

		if (targetReg == CF) 
		{
			sprintfJdc(result, 0, "%s %s 0", decompiledFirstOperand.buffer, notStatusFlag ? "==" : "!=");
		}
		else 
		{
			unsigned char operandSize = getSizeOfOperand(&params->instructions[instructionIndex].operands[0]);
			sprintfJdc(result, 0, "%s %s -(1 << %d)", decompiledFirstOperand.buffer, notStatusFlag ? "!=" : "==", ((operandSize * 8) - 1));
		}

		freeJdcStr(&decompiledFirstOperand);
	}
	else 
	{
		if (placeOperatorInfront) { *placeOperatorInfront = 1; }
		strcpyJdc(result, "-");
	}
	
	return 1;
}

static unsigned char decompileNot(struct DecompilationParameters* params, int instructionIndex, unsigned char getAssignment, struct JdcStr* result, unsigned char* placeOperatorInfront)
{
	if (getAssignment)
	{
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (!decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			return 0;
		}

		addDecompiledLine(params, result, instructionIndex, "%s = ~(%s);", decompiledFirstOperand.buffer, decompiledFirstOperand.buffer);

		freeJdcStr(&decompiledFirstOperand);
		return 1;
	}

	if (placeOperatorInfront) { *placeOperatorInfront = 1; }
	strcpyJdc(result, "~");
	return 1;
}

static unsigned char decompileOr(struct DecompilationParameters* params, int instructionIndex, unsigned char getAssignment, struct JdcStr* result)
{
	struct Operand* secondOperand = &params->instructions[instructionIndex].operands[1];
	if (secondOperand->type == IMMEDIATE && isImmediateAllOnes(&secondOperand->immediate))
	{
		if (getAssignment)
		{
			struct JdcStr decompiledFirstOperand = initializeJdcStr();
			if (!decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
			{
				freeJdcStr(&decompiledFirstOperand);
				return 0;
			}
			
			addDecompiledLine(params, result, instructionIndex, "%s = 0x%llX;", decompiledFirstOperand.buffer, secondOperand->immediate.value);

			freeJdcStr(&decompiledFirstOperand);
			return 1;
		}

		sprintfJdc(result, 0, "0x%llX", secondOperand->immediate.value);
		return 1;
	}

	return decompileBinaryOperation(params, instructionIndex, getAssignment, " | ", " |= ", result);
}

static unsigned char decompileXor(struct DecompilationParameters* params, int instructionIndex, unsigned char getAssignment, struct JdcStr* result)
{
	struct Operand* firstOperand = &params->instructions[instructionIndex].operands[0];
	struct Operand* secondOperand = &params->instructions[instructionIndex].operands[1];
	if (compareOperands(firstOperand, secondOperand))
	{
		if (getAssignment) 
		{
			struct JdcStr decompiledFirstOperand = initializeJdcStr();
			if (!decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
			{
				freeJdcStr(&decompiledFirstOperand);
				return 0;
			}
			
			addDecompiledLine(params, result, instructionIndex, "%s = 0;", decompiledFirstOperand.buffer);

			freeJdcStr(&decompiledFirstOperand);
			return 1;
		}
		
		strcpyJdc(result, "0");
		return 1;
	}
	
	return decompileBinaryOperation(params, instructionIndex, getAssignment, " ^ ", " ^= ", result);
}

static unsigned char decompileBitTest(struct DecompilationParameters* params, int instructionIndex, unsigned char getAssignment, unsigned char notStatusFlag, struct JdcStr* result)
{
	struct JdcStr decompiledFirstOperand = initializeJdcStr();
	if (!decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
	{
		freeJdcStr(&decompiledFirstOperand);
		return 0;
	}

	struct JdcStr expression = initializeJdcStr();

	struct Operand* secondOperand = &params->instructions[instructionIndex].operands[1];
	if (secondOperand->type == IMMEDIATE)
	{
		unsigned long long bitMask = (unsigned long long)(1) << (unsigned char)(secondOperand->immediate.value);
		sprintfJdc(&expression, 0, "%s & 0x%llX", decompiledFirstOperand.buffer, bitMask);
	}
	else
	{
		struct JdcStr decompiledSecondOperand = initializeJdcStr();
		if (!decompileOperand(params, instructionIndex, 1, 1, &decompiledSecondOperand))
		{
			freeJdcStr(&decompiledSecondOperand);
			return 0;
		}

		sprintfJdc(&expression, 0, "%s & (1 << %s)", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer);
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
			return 0;
		}

		addDecompiledLine(params, result, instructionIndex, "%s = %s;", cfVar->name.buffer, expression.buffer);
	}
	else 
	{
		strcatJdc(result, expression.buffer);
	}

	freeJdcStr(&expression);
	return 1;
}

static unsigned char decompileBitSet(struct DecompilationParameters* params, int instructionIndex, enum Register targetReg, unsigned char getAssignment, unsigned char notStatusFlag, struct JdcStr* result)
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
	if (!decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
	{
		freeJdcStr(&decompiledFirstOperand);
		return 0;
	}

	struct Operand* secondOperand = &params->instructions[instructionIndex].operands[1];
	if (secondOperand->type == IMMEDIATE)
	{
		unsigned long long bitMask = (unsigned long long)(1) << (unsigned char)(secondOperand->immediate.value);
		if (getAssignment) 
		{
			addDecompiledLine(params, result, instructionIndex, "%s |= 0x%llX;", decompiledFirstOperand.buffer, bitMask);
		}
		else 
		{
			sprintfJdc(result, 0, " | 0x%llX", bitMask);
		}
	}
	else
	{
		struct JdcStr decompiledSecondOperand = initializeJdcStr();
		if (!decompileOperand(params, instructionIndex, 1, 1, &decompiledSecondOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledSecondOperand);
			return 0;
		}

		if (getAssignment) 
		{
			addDecompiledLine(params, result, instructionIndex, "%s |= (1 << %s);", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer);
		}
		else 
		{
			sprintfJdc(result, 0, " | (1 << %s)", decompiledSecondOperand.buffer);
		}

		freeJdcStr(&decompiledSecondOperand);
	}

	freeJdcStr(&decompiledFirstOperand);
	return 1;
}

static unsigned char decompileBitReset(struct DecompilationParameters* params, int instructionIndex, enum Register targetReg, unsigned char getAssignment, unsigned char notStatusFlag, struct JdcStr* result)
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
	if (!decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
	{
		freeJdcStr(&decompiledFirstOperand);
		return 0;
	}

	struct Operand* secondOperand = &params->instructions[instructionIndex].operands[1];
	if (secondOperand->type == IMMEDIATE)
	{
		unsigned long long bitMask = (unsigned long long)(1) << (unsigned char)(secondOperand->immediate.value);
		if (getAssignment)
		{
			addDecompiledLine(params, result, instructionIndex, "%s &= ~0x%llX;", decompiledFirstOperand.buffer, bitMask);
		}
		else
		{
			sprintfJdc(result, 0, " & ~0x%llX", bitMask);
		}
	}
	else
	{
		struct JdcStr decompiledSecondOperand = initializeJdcStr();
		if (!decompileOperand(params, instructionIndex, 1, 1, &decompiledSecondOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledSecondOperand);
			return 0;
		}

		if (getAssignment)
		{
			addDecompiledLine(params, result, instructionIndex, "%s &= ~(1 << %s);", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer);
		}
		else
		{
			sprintfJdc(result, 0, " & ~(1 << %s)", decompiledSecondOperand.buffer);
		}

		freeJdcStr(&decompiledSecondOperand);
	}

	freeJdcStr(&decompiledFirstOperand);
	return 1;
}

static unsigned char decompileSBB(struct DecompilationParameters* params, int instructionIndex, unsigned char getAssignment, struct JdcStr* result)
{
	struct JdcStr decompiledCF = initializeJdcStr();
	if (!decompileRegister(params, instructionIndex, -1, CF, 1, 0, &decompiledCF, 0))
	{
		freeJdcStr(&decompiledCF);
		return 0;
	}
	
	struct Operand* firstOperand = &params->instructions[instructionIndex].operands[0];
	struct Operand* secondOperand = &params->instructions[instructionIndex].operands[1];
	if (compareOperands(firstOperand, secondOperand)) 
	{
		if (getAssignment)
		{
			struct JdcStr decompiledFirstOperand = initializeJdcStr();
			if (!decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
			{
				freeJdcStr(&decompiledCF);
				freeJdcStr(&decompiledFirstOperand);
				return 0;
			}
			
			addDecompiledLine(params, result, instructionIndex, "%s = -%s;", decompiledFirstOperand.buffer, decompiledCF.buffer);

			freeJdcStr(&decompiledFirstOperand);
		}
		else
		{
			sprintfJdc(result, 0, "-%s", decompiledCF.buffer);
		}

		freeJdcStr(&decompiledCF);
		return 1;
	}

	struct JdcStr decompiledSecondOperand = initializeJdcStr();
	if (!decompileOperand(params, instructionIndex, 1, 1, &decompiledSecondOperand))
	{
		freeJdcStr(&decompiledSecondOperand);
		freeJdcStr(&decompiledCF);
		return 0;
	}

	if (getAssignment) 
	{
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (!decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledSecondOperand);
			freeJdcStr(&decompiledCF);
			return 0;
		}
		
		addDecompiledLine(params, result, instructionIndex, "%s -= (%s + %s);", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer, decompiledCF.buffer);

		freeJdcStr(&decompiledFirstOperand);
	}
	else 
	{
		sprintfJdc(result, 0, " - (%s + %s)", decompiledSecondOperand.buffer, decompiledCF.buffer);
	}

	freeJdcStr(&decompiledSecondOperand);
	freeJdcStr(&decompiledCF);
	return 1;
}

static unsigned char decompileFLD(struct DecompilationParameters* params, int instructionIndex, unsigned char getAssignment, struct JdcStr* result)
{
	struct JdcStr decompiledFirstOperand = initializeJdcStr();
	if (!decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
	{
		freeJdcStr(&decompiledFirstOperand);
		return 0;
	}
	
	if (getAssignment) 
	{ 
		struct JdcStr decompiledST0 = initializeJdcStr();
		if (!decompileRegister(params, instructionIndex, -1, ST0, 1, 0, &decompiledST0, 0))
		{
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledST0);
			return 0;
		}
		
		addDecompiledLine(params, result, instructionIndex, "%s = %s;", decompiledST0.buffer, decompiledFirstOperand.buffer);

		freeJdcStr(&decompiledFirstOperand);
		freeJdcStr(&decompiledST0);
		return 1;
	}

	sprintfJdc(result, 0, "%s", decompiledFirstOperand.buffer);
	freeJdcStr(&decompiledFirstOperand);
	return 1;
}

static unsigned char decompileIDIV(struct DecompilationParameters* params, int instructionIndex, enum Register targetReg, unsigned char getAssignment, struct JdcStr* result)
{
	struct JdcStr decompiledFirstOperand = initializeJdcStr();
	if (!decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
	{
		freeJdcStr(&decompiledFirstOperand);
		return 0;
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
				return 0;
			}

			addDecompiledLine(params, result, instructionIndex, "%s = %s %% %s;", dxVar->name.buffer, decompiledAX.buffer, decompiledFirstOperand.buffer);

			freeJdcStr(&decompiledAX);
		}

		freeJdcStr(&decompiledFirstOperand);
		return 1;
	}

	if (compareRegisters(targetReg, AX))
	{
		sprintfJdc(result, 0, " / %s", decompiledFirstOperand.buffer);
		freeJdcStr(&decompiledFirstOperand);
		return 1;
	}
	else if (compareRegisters(targetReg, DX))
	{
		struct JdcStr decompiledAX = initializeJdcStr();
		if (!decompileRegister(params, instructionIndex, -1, AX, 1, 0, &decompiledAX, 0))
		{
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledAX);
			return 0;
		}
		
		sprintfJdc(result, 0, "%s % %s", decompiledAX.buffer, decompiledFirstOperand.buffer);
		freeJdcStr(&decompiledAX);
		freeJdcStr(&decompiledFirstOperand);
		return 1;
	}
	
	return 0;
}

static unsigned char decompileIMUL(struct DecompilationParameters* params, int instructionIndex, enum Register targetReg, unsigned char getAssignment, struct JdcStr* result)
{
	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];
	struct Operand* firstOperand = &instruction->operands[0];
	
	if (instruction->numOfOperands == 3)
	{
		struct Operand* secondOperand = &instruction->operands[1];
		struct Operand* thirdOperand = &instruction->operands[2];
		
		struct JdcStr decompiledThirdOperand = initializeJdcStr();
		if (!decompileOperand(params, instructionIndex, 2, 1, &decompiledThirdOperand))
		{
			freeJdcStr(&decompiledThirdOperand);
			return 0;
		}

		if (getAssignment) 
		{
			struct JdcStr decompiledFirstOperand = initializeJdcStr();
			if (!decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
			{
				freeJdcStr(&decompiledFirstOperand);
				freeJdcStr(&decompiledThirdOperand);
				return 0;
			}

			if (compareOperands(firstOperand, secondOperand))
			{
				addDecompiledLine(params, result, instructionIndex, "%s *= %s;", decompiledFirstOperand.buffer, decompiledThirdOperand.buffer);

				freeJdcStr(&decompiledThirdOperand);
				freeJdcStr(&decompiledFirstOperand);
				return 1;
			}

			struct JdcStr decompiledSecondOperand = initializeJdcStr();
			if (!decompileOperand(params, instructionIndex, 1, 1, &decompiledSecondOperand))
			{
				freeJdcStr(&decompiledFirstOperand);
				freeJdcStr(&decompiledSecondOperand);
				freeJdcStr(&decompiledThirdOperand);
				return 0;
			}

			addDecompiledLine(params, result, instructionIndex, "%s = (%s * %s);", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer, decompiledThirdOperand.buffer);

			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledSecondOperand);
			freeJdcStr(&decompiledThirdOperand);
			return 1;
		}

		if (compareOperands(firstOperand, secondOperand))
		{
			sprintfJdc(result, 0, " * %s", decompiledThirdOperand.buffer);
			freeJdcStr(&decompiledThirdOperand);
			return 1;
		}

		struct JdcStr decompiledSecondOperand = initializeJdcStr();
		if (!decompileOperand(params, instructionIndex, 1, 1, &decompiledSecondOperand))
		{
			freeJdcStr(&decompiledSecondOperand);
			freeJdcStr(&decompiledThirdOperand);
			return 0;
		}

		sprintfJdc(result, 0, "(%s * %s)", decompiledSecondOperand.buffer, decompiledThirdOperand.buffer);
		freeJdcStr(&decompiledSecondOperand);
		freeJdcStr(&decompiledThirdOperand);
		return 1;
	}
	else if (instruction->numOfOperands == 2) 
	{
		return decompileBinaryOperation(params, instructionIndex, getAssignment, " * ", " *= ", result);
	}

	// one operand form
	
	struct JdcStr decompiledFirstOperand = initializeJdcStr();
	if (!decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
	{
		freeJdcStr(&decompiledFirstOperand);
		return 0;
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
				return 0;
			}
			
			addDecompiledLine(params, result, instructionIndex, "%s = %s >> %d;", dxVar->name.buffer, decompiledAX.buffer, decompiledFirstOperand.buffer, 8 * getSizeOfOperand(firstOperand));

			freeJdcStr(&decompiledAX);
		}

		freeJdcStr(&decompiledFirstOperand);
		return 1;
	}

	if (compareRegisters(targetReg, AX))
	{
		sprintfJdc(result, 0, " * %s", decompiledFirstOperand.buffer);
		freeJdcStr(&decompiledFirstOperand);
		return 1;
	}
	else if (compareRegisters(targetReg, DX))
	{
		if (!decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			return 0;
		}

		struct JdcStr decompiledAX = initializeJdcStr();
		if (!decompileRegister(params, instructionIndex, -1, AX, 1, 0, &decompiledAX, 0))
		{
			freeJdcStr(&decompiledAX);
			return 0;
		}

		sprintfJdc(result, 0, "(%s * %s) >> %d", decompiledAX.buffer, decompiledFirstOperand.buffer, getSizeOfOperand(firstOperand));
	}

	freeJdcStr(&decompiledFirstOperand);
	return 1;
}

static unsigned char decompileCMOVcc(struct DecompilationParameters* params, int instructionIndex, unsigned char getAssignment, struct JdcStr* result)
{
	struct JdcStr comparisonStr = initializeJdcStr();
	if (!decompileComparison(params, instructionIndex, 0, &comparisonStr))
	{
		freeJdcStr(&comparisonStr);
		return 0;
	}

	struct JdcStr decompiledFirstOperand = initializeJdcStr();
	if (!decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
	{
		freeJdcStr(&comparisonStr);
		freeJdcStr(&decompiledFirstOperand);
		return 0;
	}

	struct JdcStr decompiledSecondOperand = initializeJdcStr();
	if (!decompileOperand(params, instructionIndex, 1, 1, &decompiledSecondOperand))
	{
		freeJdcStr(&comparisonStr);
		freeJdcStr(&decompiledFirstOperand);
		freeJdcStr(&decompiledSecondOperand);
		return 0;
	}

	if (getAssignment) 
	{ 
		addDecompiledLine(params, result, instructionIndex, "%s = (%s ? %s : %s);", decompiledFirstOperand.buffer, comparisonStr.buffer, decompiledSecondOperand.buffer, decompiledFirstOperand.buffer);
	}
	else 
	{
		sprintfJdc(result, 0, "(%s ? %s : %s)", comparisonStr.buffer, decompiledSecondOperand.buffer, decompiledFirstOperand.buffer);
	}

	freeJdcStr(&comparisonStr);
	freeJdcStr(&decompiledFirstOperand);
	freeJdcStr(&decompiledSecondOperand);
	return 1;
}

static unsigned char decompileSETcc(struct DecompilationParameters* params, int instructionIndex, unsigned char getAssignment, struct JdcStr* result)
{
	struct JdcStr comparisonStr = initializeJdcStr();
	if (!decompileComparison(params, instructionIndex, 0, &comparisonStr))
	{
		freeJdcStr(&comparisonStr);
		return 0;
	}

	if (getAssignment) 
	{ 
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (!decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
		{
			freeJdcStr(&comparisonStr);
			freeJdcStr(&decompiledFirstOperand);
			return 0;
		}

		addDecompiledLine(params, result, instructionIndex, "%s = %s;", decompiledFirstOperand.buffer, comparisonStr.buffer);

		freeJdcStr(&comparisonStr);
		freeJdcStr(&decompiledFirstOperand);
		return 1;
	}

	sprintfJdc(result, 0, "%s", comparisonStr.buffer);
	freeJdcStr(&comparisonStr);
	return 1;
}

static unsigned char decompilePop(struct DecompilationParameters* params, int instructionIndex, unsigned char getAssignment, struct JdcStr* result)
{
	struct JdcStr value = initializeJdcStr();

	int stackOffset = 0;
	unsigned char gotValue = 0;
	for (int i = instructionIndex; i >= 0; i--)
	{
		if (params->instructions[i].opcode == PUSH)
		{
			stackOffset--;
			if (stackOffset == 0)
			{
				if (!decompileOperand(params, i, 0, 1, &value))
				{
					freeJdcStr(&value);
					return 0;
				}

				addAssociatedInstruction(params->currentFunc, i);
				gotValue = 1;
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
				gotValue = 1;
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
		if (!decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			return 0;
		}

		if (gotValue) 
		{
			addDecompiledLine(params, result, instructionIndex, "%s = %s;", decompiledFirstOperand.buffer, value.buffer);

			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&value);
			return 1;
		}
		else if (firstOperand->type == REGISTER)
		{
			addDecompiledLine(params, result, instructionIndex, "%s = %s;", decompiledFirstOperand.buffer, registerStrs[firstOperand->reg]);

			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&value);
			return 1;
		}
		
		freeJdcStr(&decompiledFirstOperand);
		freeJdcStr(&value);
		return 0;
	}

	if (gotValue)
	{
		sprintfJdc(result, 0, "%s", value.buffer);
		freeJdcStr(&value);
		return 1;
	}
	else if (firstOperand->type == REGISTER)
	{
		sprintfJdc(result, 0, "%s", registerStrs[firstOperand->reg]);
		freeJdcStr(&value);
		return 1;
	}

	freeJdcStr(&value);
	return 0;
}

static unsigned char decompileXCHG(struct DecompilationParameters* params, int instructionIndex, enum Register targetReg, unsigned char getAssignment, struct JdcStr* result)
{
	struct Operand* firstOperand = &params->instructions[instructionIndex].operands[0];
	
	if (getAssignment) 
	{
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (!decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			return 0;
		}

		struct JdcStr decompiledSecondOperand = initializeJdcStr();
		if (!decompileOperand(params, instructionIndex, 1, 1, &decompiledSecondOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledSecondOperand);
			return 0;
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
		return 1;
	}

	if (firstOperand->type == REGISTER && compareRegisters(firstOperand->reg, targetReg))
	{
		if (!decompileOperand(params, instructionIndex, 1, 1, result))
		{
			return 0;
		}
	}
	else 
	{
		if (!decompileOperand(params, instructionIndex, 0, 1, result))
		{
			return 0;
		}
	}

	return 1;
}

static unsigned char decompileCMPXCHG(struct DecompilationParameters* params, int instructionIndex, unsigned char getAssignment, struct JdcStr* result) 
{
	if (!getAssignment) 
	{
		return 0;
	}

	struct RegisterVariable* axVar = doesInstructionAssignToRegVar(params, instructionIndex, AX);
	if (!axVar) 
	{
		return 0;
	}

	struct JdcStr decompiledAX = initializeJdcStr();
	if (!decompileRegister(params, instructionIndex, -1, AX, 1, 0, &decompiledAX, 0))
	{
		freeJdcStr(&decompiledAX);
		return 0;
	}

	struct JdcStr decompiledFirstOperand = initializeJdcStr();
	if (!decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
	{
		freeJdcStr(&decompiledAX);
		freeJdcStr(&decompiledFirstOperand);
		return 0;
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
	if (!decompileOperand(params, instructionIndex, 1, 1, &decompiledSecondOperand))
	{
		freeJdcStr(&decompiledAX);
		freeJdcStr(&decompiledFirstOperand);
		freeJdcStr(&decompiledSecondOperand);
		return 0;
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
	return 1;
}