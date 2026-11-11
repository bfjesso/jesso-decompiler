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
	else if (isOpcodeMov(instruction->opcode) || isOpcodeLEA(instruction->opcode)) // LEA is handled when decompiling memory addresses 
	{
		return decompileBinaryOperation(params, instructionIndex, getAssignment, "", " = ", result);
	}
	else if (isOpcodeLD(instruction->opcode))
	{
		return decompileLD(params, instructionIndex, targetReg, getAssignment, notStatusFlag, result);
	}
	else if (isOpcodeST(instruction->opcode))
	{
		return decompileST(params, instructionIndex, getAssignment, result);
	}
	else if (isOpcodeAdd(instruction->opcode) || isOpcodeSub(instruction->opcode) || isOpcodeCmp(instruction->opcode))
	{
		return decompileArithmetic(params, instructionIndex, targetReg, getAssignment, notStatusFlag, result);
	}
	else if (isOpcodeAnd(instruction->opcode) || instruction->opcode == TEST_X86)
	{
		return decompileAnd(params, instructionIndex, targetReg, getAssignment, notStatusFlag, result);
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
	else if (instruction->opcode == INC_X86 || instruction->opcode == DEC_X86)
	{
		return decompileIncDec(params, instructionIndex, targetReg, getAssignment, notStatusFlag, result);
	}
	else if (instruction->opcode == NEG_X86)
	{
		return decompileNeg(params, instructionIndex, targetReg, getAssignment, notStatusFlag, result, placeOperatorInfront);
	}
	else if (isOpcodeNot(instruction->opcode))
	{
		return decompileNot(params, instructionIndex, targetReg, getAssignment, notStatusFlag, result, placeOperatorInfront);
	}
	else if (isOpcodeXor(instruction->opcode))
	{
		return decompileXor(params, instructionIndex, getAssignment, result);
	}
	else if (instruction->opcode == BT_X86)
	{
		return decompileBitTest(params, instructionIndex, getAssignment, notStatusFlag, result);
	}
	else if (instruction->opcode == BTS_X86)
	{
		return decompileBitSet(params, instructionIndex, targetReg, getAssignment, notStatusFlag, result);
	}
	else if (instruction->opcode == BTR_X86)
	{
		return decompileBitReset(params, instructionIndex, targetReg, getAssignment, notStatusFlag, result);
	}
	else if (instruction->opcode == ADC_X86)
	{
		return decompileADC(params, instructionIndex, getAssignment, result);
	}
	else if (instruction->opcode == SBB_X86)
	{
		return decompileSBB(params, instructionIndex, getAssignment, result);
	}
	else if (instruction->opcode == FLD_X86)
	{
		return decompileFLD(params, instructionIndex, getAssignment, result);
	}
	else if (instruction->opcode == IDIV_X86 || instruction->opcode == DIV_X86)
	{
		return decompileIDIV(params, instructionIndex, targetReg, getAssignment, result);
	}
	else if (instruction->opcode == IMUL_X86 || instruction->opcode == MUL_X86)
	{
		return decompileIMUL(params, instructionIndex, targetReg, getAssignment, result);
	}
	else if (instruction->opcode == POP_X86)
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
	else if (instruction->opcode == XCHG_X86)
	{
		return decompileXCHG(params, instructionIndex, targetReg, getAssignment, result);
	}
	else if (instruction->opcode == CMPXCHG_X86)
	{
		return decompileCMPXCHG(params, instructionIndex, getAssignment, result);
	}

	return ERROR_JDC;
}

static enum JdcStatus decompileBinaryOperation(struct DecompilationParameters* params, int32_t instructionIndex, bool getAssignment, const char* regularOperator, const char* assignmentOperator, struct JdcStr* result)
{
	struct JdcStr decompiledSecondOperand = initializeJdcStr();
	if (ERROR_JDC == decompileOperand(params, instructionIndex, 1, true, &decompiledSecondOperand))
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
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, true, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledSecondOperand);
			return ERROR_JDC;
		}

		if (params->instructions[instructionIndex].numOfOperands == 3)
		{
			struct JdcStr decompiledThirdOperand = initializeJdcStr();
			if (ERROR_JDC == decompileOperand(params, instructionIndex, 2, true, &decompiledThirdOperand))
			{
				freeJdcStr(&decompiledFirstOperand);
				freeJdcStr(&decompiledSecondOperand);
				freeJdcStr(&decompiledThirdOperand);
				return ERROR_JDC;
			}

			addDecompiledLine(params, result, instructionIndex, "%s = %s%s%s;", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer, regularOperator, decompiledThirdOperand.buffer);
			freeJdcStr(&decompiledThirdOperand);
		}
		else
		{
			addDecompiledLine(params, result, instructionIndex, "%s%s%s;", decompiledFirstOperand.buffer, assignmentOperator, decompiledSecondOperand.buffer);
		}


		freeJdcStr(&decompiledFirstOperand);
		freeJdcStr(&decompiledSecondOperand);
		return SUCCESS_JDC;
	}

	if (params->instructions[instructionIndex].numOfOperands == 3)
	{
		struct JdcStr decompiledThirdOperand = initializeJdcStr();
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 2, true, &decompiledThirdOperand))
		{
			freeJdcStr(&decompiledSecondOperand);
			freeJdcStr(&decompiledThirdOperand);
			return ERROR_JDC;
		}

		sprintfJdc(result, false, "%s%s%s", decompiledSecondOperand.buffer, regularOperator, decompiledThirdOperand.buffer);
		freeJdcStr(&decompiledThirdOperand);
	}
	else
	{
		sprintfJdc(result, false, "%s%s", regularOperator, decompiledSecondOperand.buffer);
	}

	freeJdcStr(&decompiledSecondOperand);
	return SUCCESS_JDC;
}

static enum JdcStatus decompilePF(struct DecompilationParameters* params, bool notStatusFlag, const char* instructionResult, struct JdcStr* flagExpression)
{
	return sprintfJdc(flagExpression, false, "__popcnt((%s) & 0xFF) % 2 %s 0", instructionResult, notStatusFlag ? "!=" : "==");
}

static enum JdcStatus decompileZF(struct DecompilationParameters* params, bool notStatusFlag, const char* instructionResult, struct JdcStr* flagExpression)
{
	return sprintfJdc(flagExpression, false, "(%s) %s 0", instructionResult, notStatusFlag ? "!=" : "==");
}

static enum JdcStatus decompileSF(struct DecompilationParameters* params, bool notStatusFlag, const char* instructionResult, uint8_t operandSize, struct JdcStr* flagExpression)
{
	return sprintfJdc(flagExpression, false, "%s(((%s) >> %d) & 1)", notStatusFlag ? "!" : "", instructionResult, (operandSize * 8) - 1);
}

static enum JdcStatus decompileNZP(struct DecompilationParameters* params, int32_t instructionIndex, enum Register targetReg, bool getAssignment, bool notStatusFlag, const char* instructionResult, struct JdcStr* result)
{
	struct JdcStr nExpression = initializeJdcStr();
	sprintfJdc(&nExpression, false, "%s < 0", instructionResult);

	struct JdcStr zExpression = initializeJdcStr();
	sprintfJdc(&zExpression, false, "%s == 0", instructionResult);

	struct JdcStr pExpression = initializeJdcStr();
	sprintfJdc(&pExpression, false, "%s > 0", instructionResult);

	enum JdcStatus status = SUCCESS_JDC;
	if (getAssignment)
	{
		struct RegisterVariable* nVar = doesInstructionAssignToRegVar(params, instructionIndex, N_LC3_REG);
		if (nVar)
		{
			addDecompiledLine(params, result, instructionIndex, "%s = %s;", nVar->name.buffer, nExpression.buffer);
		}

		struct RegisterVariable* zVar = doesInstructionAssignToRegVar(params, instructionIndex, Z_LC3_REG);
		if (zVar)
		{
			addDecompiledLine(params, result, instructionIndex, "%s = %s;", zVar->name.buffer, zExpression.buffer);
		}

		struct RegisterVariable* pVar = doesInstructionAssignToRegVar(params, instructionIndex, P_LC3_REG);
		if (pVar)
		{
			addDecompiledLine(params, result, instructionIndex, "%s = %s;", pVar->name.buffer, pExpression.buffer);
		}
	}
	else
	{
		switch (targetReg)
		{
		case N_LC3_REG:
			strcpyJdc(result, nExpression.buffer);
			break;
		case Z_LC3_REG:
			strcpyJdc(result, zExpression.buffer);
			break;
		case P_LC3_REG:
			strcpyJdc(result, pExpression.buffer);
			break;
		default:
			status = ERROR_JDC;
			break;
		}
	}

	freeJdcStr(&nExpression);
	freeJdcStr(&zExpression);
	freeJdcStr(&pExpression);

	return status;
}

static enum JdcStatus decompileLD(struct DecompilationParameters* params, int32_t instructionIndex, enum Register targetReg, bool getAssignment, bool notStatusFlag, struct JdcStr* result)
{
	struct JdcStr decompiledSecondOperand = initializeJdcStr();
	if (ERROR_JDC == decompileOperand(params, instructionIndex, 1, true, &decompiledSecondOperand))
	{
		freeJdcStr(&decompiledSecondOperand);
		return ERROR_JDC;
	}

	if (getAssignment)
	{
		if (ERROR_JDC == decompileNZP(params, instructionIndex, targetReg, true, notStatusFlag, decompiledSecondOperand.buffer, result))
		{
			freeJdcStr(&decompiledSecondOperand);
			return ERROR_JDC;
		}
	}
	else if (!getAssignment && isRegNZP(targetReg))
	{
		enum JdcStatus status = decompileNZP(params, instructionIndex, targetReg, false, notStatusFlag, decompiledSecondOperand.buffer, result);
		freeJdcStr(&decompiledSecondOperand);
		return status;
	}

	freeJdcStr(&decompiledSecondOperand);

	return decompileBinaryOperation(params, instructionIndex, getAssignment, "", " = ", result);
}

static enum JdcStatus decompileST(struct DecompilationParameters* params, int32_t instructionIndex, bool getAssignment, struct JdcStr* result)
{
	if (!getAssignment)
	{
		return ERROR_JDC;
	}

	struct JdcStr decompiledFirstOperand = initializeJdcStr();
	if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, true, &decompiledFirstOperand))
	{
		freeJdcStr(&decompiledFirstOperand);
		return ERROR_JDC;
	}

	struct JdcStr decompiledSecondOperand = initializeJdcStr();
	if (ERROR_JDC == decompileOperand(params, instructionIndex, 1, true, &decompiledSecondOperand))
	{
		freeJdcStr(&decompiledFirstOperand);
		freeJdcStr(&decompiledSecondOperand);
		return ERROR_JDC;
	}

	addDecompiledLine(params, result, instructionIndex, "%s = %s;", decompiledSecondOperand.buffer, decompiledFirstOperand.buffer);

	freeJdcStr(&decompiledFirstOperand);
	freeJdcStr(&decompiledSecondOperand);
	return SUCCESS_JDC;
}

static enum JdcStatus decompileArithmetic(struct DecompilationParameters* params, int32_t instructionIndex, enum Register targetReg, bool getAssignment, bool notStatusFlag, struct JdcStr* result)
{
	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];
	enum OpcodeMnemonic opcode = instruction->opcode;

	if (isOpcodeCmp(opcode) || opcode == ADD_X86 || opcode == SUB_X86)
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

		if (isOpcodeCmp(opcode) || opcode == SUB_X86)
		{
			wrapJdcStrInParentheses(&decompiledSecondOperand);
			strcatStartJdc(&decompiledSecondOperand, "-");
		}

		struct JdcStr sumExpression = initializeJdcStr();
		sprintfJdc(&sumExpression, false, "%s + %s", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer);

		int32_t operandSize = getSizeOfOperand(&instruction->operands[0]);

		struct JdcStr cfExpression = initializeJdcStr();
		enum PrimitiveType uintType = getIntType(getSizeOfOperand(&instruction->operands[0]), false);
		sprintfJdc(&cfExpression, false, "((%s)(%s) + (%s)(%s)) %s (%s)(%s)", primitiveTypeToStr(uintType, params->useStdInt), decompiledFirstOperand.buffer, primitiveTypeToStr(uintType, params->useStdInt), decompiledSecondOperand.buffer, notStatusFlag ? ">=" : "<", primitiveTypeToStr(uintType, params->useStdInt), decompiledFirstOperand.buffer);

		struct JdcStr pfExpression = initializeJdcStr();
		decompilePF(params, notStatusFlag, sumExpression.buffer, &pfExpression);

		struct JdcStr afExpression = initializeJdcStr();
		sprintfJdc(&afExpression, false, "((%s)(%s & 0xF) + (%s)(%s & 0xF)) %s 0x10", primitiveTypeToStr(INT8_TYPE, params->useStdInt), decompiledFirstOperand.buffer, primitiveTypeToStr(INT8_TYPE, params->useStdInt), decompiledSecondOperand.buffer, notStatusFlag ? "<" : ">=");

		struct JdcStr zfExpression = initializeJdcStr();
		decompileZF(params, notStatusFlag, sumExpression.buffer, &zfExpression);

		struct JdcStr sfExpression = initializeJdcStr();
		decompileSF(params, notStatusFlag, sumExpression.buffer, operandSize, &sfExpression);

		struct JdcStr ofExpression = initializeJdcStr();
		sprintfJdc(&ofExpression, false, "%s((%s < 0 && %s < 0 && (%s + %s) > 0) || (%s > 0 && %s > 0 && (%s + %s) < 0))", notStatusFlag ? "!" : "", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer, decompiledFirstOperand.buffer, decompiledSecondOperand.buffer, decompiledFirstOperand.buffer, decompiledSecondOperand.buffer, decompiledFirstOperand.buffer, decompiledSecondOperand.buffer);

		freeJdcStr(&decompiledFirstOperand);
		freeJdcStr(&decompiledSecondOperand);
		freeJdcStr(&sumExpression);

		bool finished = false;
		if (getAssignment)
		{
			struct RegisterVariable* cfVar = doesInstructionAssignToRegVar(params, instructionIndex, CF_X86_REG);
			if (cfVar)
			{
				addDecompiledLine(params, result, instructionIndex, "%s = %s;", cfVar->name.buffer, cfExpression.buffer);
			}

			struct RegisterVariable* pfVar = doesInstructionAssignToRegVar(params, instructionIndex, PF_X86_REG);
			if (pfVar)
			{
				addDecompiledLine(params, result, instructionIndex, "%s = %s;", pfVar->name.buffer, pfExpression.buffer);
			}

			struct RegisterVariable* afVar = doesInstructionAssignToRegVar(params, instructionIndex, AF_X86_REG);
			if (afVar)
			{
				addDecompiledLine(params, result, instructionIndex, "%s = %s;", afVar->name.buffer, afExpression.buffer);
			}

			struct RegisterVariable* zfVar = doesInstructionAssignToRegVar(params, instructionIndex, ZF_X86_REG);
			if (zfVar)
			{
				addDecompiledLine(params, result, instructionIndex, "%s = %s;", zfVar->name.buffer, zfExpression.buffer);
			}

			struct RegisterVariable* sfVar = doesInstructionAssignToRegVar(params, instructionIndex, SF_X86_REG);
			if (sfVar)
			{
				addDecompiledLine(params, result, instructionIndex, "%s = %s;", sfVar->name.buffer, sfExpression.buffer);
			}

			struct RegisterVariable* ofVar = doesInstructionAssignToRegVar(params, instructionIndex, OF_X86_REG);
			if (ofVar)
			{
				addDecompiledLine(params, result, instructionIndex, "%s = %s;", ofVar->name.buffer, ofExpression.buffer);
			}
		}
		else
		{
			switch (targetReg)
			{
			case CF_X86_REG:
				strcpyJdc(result, cfExpression.buffer);
				finished = true;
				break;
			case PF_X86_REG:
				strcpyJdc(result, pfExpression.buffer);
				finished = true;
				break;
			case AF_X86_REG:
				strcpyJdc(result, afExpression.buffer);
				finished = true;
				break;
			case ZF_X86_REG:
				strcpyJdc(result, zfExpression.buffer);
				finished = true;
				break;
			case SF_X86_REG:
				strcpyJdc(result, sfExpression.buffer);
				finished = true;
				break;
			case OF_X86_REG:
				strcpyJdc(result, ofExpression.buffer);
				finished = true;
				break;
			}
		}

		freeJdcStr(&cfExpression);
		freeJdcStr(&pfExpression);
		freeJdcStr(&afExpression);
		freeJdcStr(&zfExpression);
		freeJdcStr(&sfExpression);
		freeJdcStr(&ofExpression);

		if (finished)
		{
			return SUCCESS_JDC;
		}
		else if (!getAssignment && isOpcodeCmp(opcode))
		{
			return ERROR_JDC;
		}
	}
	else if (opcode == ADD_LC3)
	{
		struct JdcStr decompiledSecondOperand = initializeJdcStr();
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 1, 1, &decompiledSecondOperand))
		{
			freeJdcStr(&decompiledSecondOperand);
			return ERROR_JDC;
		}

		struct JdcStr decompiledThirdOperand = initializeJdcStr();
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 2, 1, &decompiledThirdOperand))
		{
			freeJdcStr(&decompiledSecondOperand);
			freeJdcStr(&decompiledThirdOperand);
			return ERROR_JDC;
		}

		struct JdcStr addExpression = initializeJdcStr();
		sprintfJdc(&addExpression, false, "%s + %s", decompiledSecondOperand.buffer, decompiledThirdOperand.buffer);
		freeJdcStr(&decompiledSecondOperand);
		freeJdcStr(&decompiledThirdOperand);

		if (getAssignment)
		{
			if (ERROR_JDC == decompileNZP(params, instructionIndex, targetReg, true, notStatusFlag, addExpression.buffer, result))
			{
				freeJdcStr(&addExpression);
				return ERROR_JDC;
			}
		}
		else if (isRegNZP(targetReg))
		{
			enum JdcStatus status = decompileNZP(params, instructionIndex, targetReg, false, notStatusFlag, addExpression.buffer, result);
			freeJdcStr(&addExpression);
			return status;
		}

		freeJdcStr(&addExpression);
	}

	if (isOpcodeAdd(opcode))
	{
		return decompileBinaryOperation(params, instructionIndex, getAssignment, " + ", " += ", result);
	}
	else if (isOpcodeSub(opcode))
	{
		return decompileBinaryOperation(params, instructionIndex, getAssignment, " - ", " -= ", result);
	}
	else if (isOpcodeCmp(opcode))
	{
		return SUCCESS_JDC;
	}

	return ERROR_JDC;
}

static enum JdcStatus decompileAnd(struct DecompilationParameters* params, int32_t instructionIndex, enum Register targetReg, bool getAssignment, bool notStatusFlag, struct JdcStr* result)
{
	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];
	enum OpcodeMnemonic opcode = instruction->opcode;

	if (opcode == AND_X86 || opcode == TEST_X86)
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

		struct JdcStr andExpression = initializeJdcStr();
		if (compareOperands(&instruction->operands[0], &instruction->operands[1]))
		{
			strcpyJdc(&andExpression, decompiledFirstOperand.buffer);
		}
		else
		{
			sprintfJdc(&andExpression, false, "%s & %s", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer);
		}

		struct JdcStr pfExpression = initializeJdcStr();
		decompilePF(params, notStatusFlag, andExpression.buffer, &pfExpression);

		struct JdcStr zfExpression = initializeJdcStr();
		decompileZF(params, notStatusFlag, andExpression.buffer, &zfExpression);

		struct JdcStr sfExpression = initializeJdcStr();
		decompileSF(params, notStatusFlag, andExpression.buffer, getSizeOfOperand(&instruction->operands[0]), &sfExpression);

		freeJdcStr(&decompiledFirstOperand);
		freeJdcStr(&decompiledSecondOperand);
		freeJdcStr(&andExpression);

		bool finished = false;
		if (getAssignment)
		{
			struct RegisterVariable* cfVar = doesInstructionAssignToRegVar(params, instructionIndex, CF_X86_REG);
			if (cfVar)
			{
				addDecompiledLine(params, result, instructionIndex, "%s = %d;", cfVar->name.buffer, notStatusFlag ? 1 : 0);
			}

			struct RegisterVariable* ofVar = doesInstructionAssignToRegVar(params, instructionIndex, OF_X86_REG);
			if (ofVar)
			{
				addDecompiledLine(params, result, instructionIndex, "%s = %d;", ofVar->name.buffer, notStatusFlag ? 1 : 0);
			}

			struct RegisterVariable* pfVar = doesInstructionAssignToRegVar(params, instructionIndex, PF_X86_REG);
			if (pfVar)
			{
				addDecompiledLine(params, result, instructionIndex, "%s = %s;", pfVar->name.buffer, pfExpression.buffer);
			}

			struct RegisterVariable* zfVar = doesInstructionAssignToRegVar(params, instructionIndex, ZF_X86_REG);
			if (zfVar)
			{
				addDecompiledLine(params, result, instructionIndex, "%s = %s;", zfVar->name.buffer, zfExpression.buffer);
			}

			struct RegisterVariable* sfVar = doesInstructionAssignToRegVar(params, instructionIndex, SF_X86_REG);
			if (sfVar)
			{
				addDecompiledLine(params, result, instructionIndex, "%s = %s;", sfVar->name.buffer, sfExpression.buffer);
			}
		}
		else
		{
			switch (targetReg)
			{
			case CF_X86_REG:
			case OF_X86_REG:
				strcpyJdc(result, notStatusFlag ? "1" : "0");
				finished = true;
				break;
			case PF_X86_REG:
				strcpyJdc(result, pfExpression.buffer);
				finished = true;
				break;
			case ZF_X86_REG:
				strcpyJdc(result, zfExpression.buffer);
				finished = true;
				break;
			case SF_X86_REG:
				strcpyJdc(result, sfExpression.buffer);
				finished = true;
				break;
			}
		}

		freeJdcStr(&pfExpression);
		freeJdcStr(&zfExpression);
		freeJdcStr(&sfExpression);

		if (finished)
		{
			return SUCCESS_JDC;
		}
		else if (!getAssignment && opcode == TEST_X86)
		{
			return ERROR_JDC;
		}
	}
	else if (opcode == AND_LC3)
	{
		struct JdcStr decompiledSecondOperand = initializeJdcStr();
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 1, 1, &decompiledSecondOperand))
		{
			freeJdcStr(&decompiledSecondOperand);
			return ERROR_JDC;
		}

		struct JdcStr decompiledThirdOperand = initializeJdcStr();
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 2, 1, &decompiledThirdOperand))
		{
			freeJdcStr(&decompiledSecondOperand);
			freeJdcStr(&decompiledThirdOperand);
			return ERROR_JDC;
		}

		struct JdcStr andExpression = initializeJdcStr();
		sprintfJdc(&andExpression, false, "%s & %s", decompiledSecondOperand.buffer, decompiledThirdOperand.buffer);
		freeJdcStr(&decompiledSecondOperand);
		freeJdcStr(&decompiledThirdOperand);

		if (getAssignment)
		{
			if (ERROR_JDC == decompileNZP(params, instructionIndex, targetReg, true, notStatusFlag, andExpression.buffer, result))
			{
				freeJdcStr(&andExpression);
				return ERROR_JDC;
			}
		}
		else if (isRegNZP(targetReg))
		{
			enum JdcStatus status = decompileNZP(params, instructionIndex, targetReg, false, notStatusFlag, andExpression.buffer, result);
			freeJdcStr(&andExpression);
			return status;
		}

		freeJdcStr(&andExpression);
	}

	if (isOpcodeAnd(opcode))
	{
		return decompileBinaryOperation(params, instructionIndex, getAssignment, " & ", " &= ", result);
	}
	else if (opcode == TEST_X86)
	{
		return SUCCESS_JDC;
	}

	return ERROR_JDC;
}

static enum JdcStatus decompileIncDec(struct DecompilationParameters* params, int32_t instructionIndex, enum Register targetReg, bool getAssignment, bool notStatusFlag, struct JdcStr* result)
{
	bool isInc = params->instructions[instructionIndex].opcode == INC_X86;
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

		struct RegisterVariable* zfVar = doesInstructionAssignToRegVar(params, instructionIndex, ZF_X86_REG);
		if (zfVar)
		{
			addDecompiledLine(params, result, instructionIndex, "%s = %s %s 0;", zfVar->name.buffer, decompiledFirstOperand.buffer, notStatusFlag ? "!=" : "==");
		}

		freeJdcStr(&decompiledFirstOperand);
		return SUCCESS_JDC;
	}

	if (targetReg == ZF_X86_REG)
	{
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			return ERROR_JDC;
		}

		sprintfJdc(result, false, "%s %s 0", decompiledFirstOperand.buffer, notStatusFlag ? "!=" : "==");
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

		struct RegisterVariable* cfVar = doesInstructionAssignToRegVar(params, instructionIndex, CF_X86_REG);
		if (cfVar)
		{
			addDecompiledLine(params, result, instructionIndex, "%s = (%s) %s 0;", cfVar->name.buffer, decompiledFirstOperand.buffer, notStatusFlag ? "==" : "!=");
		}

		struct RegisterVariable* ofVar = doesInstructionAssignToRegVar(params, instructionIndex, OF_X86_REG);
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

	if (targetReg == CF_X86_REG || targetReg == OF_X86_REG)
	{
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			return ERROR_JDC;
		}

		if (targetReg == CF_X86_REG)
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

static enum JdcStatus decompileNot(struct DecompilationParameters* params, int32_t instructionIndex, enum Register targetReg, bool getAssignment, bool notStatusFlag, struct JdcStr* result, bool* placeOperatorInfront)
{
	if (params->instructions[instructionIndex].opcode == NOT_LC3)
	{
		struct JdcStr decompiledSecondOperand = initializeJdcStr();
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 1, true, &decompiledSecondOperand))
		{
			freeJdcStr(&decompiledSecondOperand);
			return ERROR_JDC;
		}

		struct JdcStr notExpression = initializeJdcStr();
		sprintfJdc(&notExpression, false, "~(%s)", decompiledSecondOperand.buffer);
		freeJdcStr(&decompiledSecondOperand);

		if (getAssignment)
		{
			if (ERROR_JDC == decompileNZP(params, instructionIndex, targetReg, true, notStatusFlag, notExpression.buffer, result))
			{
				return ERROR_JDC;
			}

			struct JdcStr decompiledFirstOperand = initializeJdcStr();
			if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, true, &decompiledFirstOperand))
			{
				freeJdcStr(&decompiledFirstOperand);
				freeJdcStr(&notExpression);
				return ERROR_JDC;
			}

			addDecompiledLine(params, result, instructionIndex, "%s = %s;", decompiledFirstOperand.buffer, notExpression.buffer);
			freeJdcStr(&decompiledFirstOperand);
		}
		else if (isRegNZP(targetReg))
		{
			enum JdcStatus status = decompileNZP(params, instructionIndex, targetReg, false, notStatusFlag, notExpression.buffer, result);
			freeJdcStr(&notExpression);
			return status;
		}

		strcpyJdc(result, notExpression.buffer);
		freeJdcStr(&notExpression);
		return SUCCESS_JDC;
	}

	if (getAssignment)
	{
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, true, &decompiledFirstOperand))
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
	if (secondOperand->type == IMMEDIATE_OPERAND && isImmediateAllOnes(&secondOperand->immediate))
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
	if (secondOperand->type == IMMEDIATE_OPERAND)
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
		struct RegisterVariable* cfVar = doesInstructionAssignToRegVar(params, instructionIndex, CF_X86_REG);
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
	if (getAssignment && doesInstructionAssignToRegVar(params, instructionIndex, CF_X86_REG))
	{
		decompileBitTest(params, instructionIndex, 1, notStatusFlag, result);
	}
	else if (!getAssignment && targetReg == CF_X86_REG)
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
	if (secondOperand->type == IMMEDIATE_OPERAND)
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
	if (getAssignment && doesInstructionAssignToRegVar(params, instructionIndex, CF_X86_REG))
	{
		decompileBitTest(params, instructionIndex, 1, notStatusFlag, result);
	}
	else if (!getAssignment && targetReg == CF_X86_REG)
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
	if (secondOperand->type == IMMEDIATE_OPERAND)
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
	if (!decompileRegister(params, instructionIndex, -1, CF_X86_REG, 1, 0, &decompiledCF, 0))
	{
		freeJdcStr(&decompiledCF);
		return ERROR_JDC;
	}

	struct Operand* secondOperand = &params->instructions[instructionIndex].operands[1];
	if (secondOperand->type == IMMEDIATE_OPERAND && secondOperand->immediate.value == 0)
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
	if (!decompileRegister(params, instructionIndex, -1, CF_X86_REG, 1, 0, &decompiledCF, 0))
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
		if (!decompileRegister(params, instructionIndex, -1, ST0_X86_REG, 1, 0, &decompiledST0, 0))
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
		struct RegisterVariable* axVar = doesInstructionAssignToRegVar(params, instructionIndex, AX_X86_REG);
		if (axVar)
		{
			addDecompiledLine(params, result, instructionIndex, "%s /= %s;", axVar->name.buffer, decompiledFirstOperand.buffer);
		}

		struct RegisterVariable* dxVar = doesInstructionAssignToRegVar(params, instructionIndex, DX_X86_REG);
		if (dxVar)
		{
			struct JdcStr decompiledAX = initializeJdcStr();
			if (!decompileRegister(params, instructionIndex, -1, AX_X86_REG, 1, 0, &decompiledAX, 0))
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

	if (compareRegisters(targetReg, AX_X86_REG))
	{
		sprintfJdc(result, false, " / %s", decompiledFirstOperand.buffer);
		freeJdcStr(&decompiledFirstOperand);
		return SUCCESS_JDC;
	}
	else if (compareRegisters(targetReg, DX_X86_REG))
	{
		struct JdcStr decompiledAX = initializeJdcStr();
		if (!decompileRegister(params, instructionIndex, -1, AX_X86_REG, 1, 0, &decompiledAX, 0))
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
		struct RegisterVariable* axVar = doesInstructionAssignToRegVar(params, instructionIndex, AX_X86_REG);
		if (axVar)
		{
			addDecompiledLine(params, result, instructionIndex, "%s *= %s;", axVar->name.buffer, decompiledFirstOperand.buffer);
		}

		struct RegisterVariable* dxVar = doesInstructionAssignToRegVar(params, instructionIndex, DX_X86_REG);
		if (dxVar)
		{
			struct JdcStr decompiledAX = initializeJdcStr();
			if (!decompileRegister(params, instructionIndex, -1, AX_X86_REG, 1, 0, &decompiledAX, 0))
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

	if (compareRegisters(targetReg, AX_X86_REG))
	{
		sprintfJdc(result, false, " * %s", decompiledFirstOperand.buffer);
		freeJdcStr(&decompiledFirstOperand);
		return SUCCESS_JDC;
	}
	else if (compareRegisters(targetReg, DX_X86_REG))
	{
		if (ERROR_JDC == decompileOperand(params, instructionIndex, 0, 1, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			return ERROR_JDC;
		}

		struct JdcStr decompiledAX = initializeJdcStr();
		if (!decompileRegister(params, instructionIndex, -1, AX_X86_REG, 1, 0, &decompiledAX, 0))
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
		if (params->instructions[i].opcode == PUSH_X86)
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
		else if (params->instructions[i].opcode == PUSHF_X86)
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
		else if (params->instructions[i].opcode == POP_X86)
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
		else if (firstOperand->type == REGISTER_OPERAND)
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
	else if (firstOperand->type == REGISTER_OPERAND)
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

		if ((firstOperand->type == REGISTER_OPERAND && compareRegisters(firstOperand->reg, targetReg)) || firstOperand->type == MEMORY_OPERAND)
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

	if (firstOperand->type == REGISTER_OPERAND && compareRegisters(firstOperand->reg, targetReg))
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

	struct RegisterVariable* axVar = doesInstructionAssignToRegVar(params, instructionIndex, AX_X86_REG);
	if (!axVar)
	{
		return ERROR_JDC;
	}

	struct JdcStr decompiledAX = initializeJdcStr();
	if (!decompileRegister(params, instructionIndex, -1, AX_X86_REG, 1, 0, &decompiledAX, 0))
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

	struct RegisterVariable* zfVar = doesInstructionAssignToRegVar(params, instructionIndex, ZF_X86_REG);
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