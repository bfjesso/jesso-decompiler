#include "operations.h"
#include "intrinsics.h"
#include "../disassembler/operands.h"
#include "expressions.h"

unsigned char decompileOperation(struct DecompilationParameters* params, enum Register targetReg, unsigned char getAssignment, int dstOperandIndex, struct JdcStr* result)
{
	struct DisassembledInstruction* instruction = &(params->instructions[params->startInstructionIndex]);

	struct IntrinsicFunc* intrinsicFunc;
	if (checkForReturningIntrinsicFunc(instruction->opcode, &intrinsicFunc))
	{
		return decompileReturningIntrinsicFunc(params, intrinsicFunc, getAssignment, result);
	}
	else if (isOpcodeMov(instruction->opcode)) 
	{
		return decompileBinaryOperation(params, getAssignment, "", " = ", result);
	}
	else if (isOpcodeAdd(instruction->opcode))
	{
		return decompileBinaryOperation(params, getAssignment, " + ", " += ", result);
	}
	else if (isOpcodeSub(instruction->opcode))
	{
		return decompileBinaryOperation(params, getAssignment, " - ", " -= ", result);
	}
	else if (isOpcodeAnd(instruction->opcode))
	{
		return decompileBinaryOperation(params, getAssignment, " & ", " &= ", result);
	}
	else if (isOpcodeOr(instruction->opcode))
	{
		return decompileBinaryOperation(params, getAssignment, " | ", " |= ", result);
	}
	else if (isOpcodeShl(instruction->opcode))
	{
		return decompileBinaryOperation(params, getAssignment, " << ", " <<= ", result);
	}
	else if (isOpcodeShr(instruction->opcode))
	{
		return decompileBinaryOperation(params, getAssignment, " >> ", " >>= ", result);
	}
	else if (isOpcodeCvtToDbl(instruction->opcode))
	{
		return decompileBinaryOperation(params, getAssignment, "(double)", " = (double)", result);
	}
	else if (isOpcodeCvtToFlt(instruction->opcode))
	{
		return decompileBinaryOperation(params, getAssignment, "(float)", " = (float)", result);
	}
	else if (instruction->opcode == LEA)
	{
		return decompileLEA(params, getAssignment, result);
	}
	else if (instruction->opcode == INC)
	{
		return decompileInc(params, getAssignment, result);
	}
	else if (instruction->opcode == DEC)
	{
		return decompileDec(params, getAssignment, result);
	}
	else if (instruction->opcode == NEG)
	{
		return decompileNeg(params, getAssignment, result);
	}
	else if (isOpcodeXor(instruction->opcode))
	{
		return decompileXor(params, getAssignment, result);
	}
	else if (instruction->opcode == FLD)
	{
		return decompileFLD(params, getAssignment, result);
	}
	else if (instruction->opcode == IDIV)
	{
		return decompileIDIV(params, getAssignment, result);
	}
	else if (instruction->opcode == IMUL)
	{
		return decompileIMUL(params, targetReg, getAssignment, result);
	}
	else if (instruction->opcode == POP)
	{
		return decompilePop(params, getAssignment, result);
	}
	else if (isOpcodeCMOVcc(instruction->opcode)) 
	{
		return decompileCMOVcc(params, getAssignment, result);
	}
	else if (isOpcodeSETcc(instruction->opcode))
	{
		return decompileCMOVcc(params, getAssignment, result);
	}
	else if (instruction->opcode == XCHG)
	{
		return decompileXCHG(params, targetReg, getAssignment, dstOperandIndex, result);
	}

	return 0;
}

static unsigned char decompileBinaryOperation(struct DecompilationParameters* params, unsigned char getAssignment, const char* regularOperator, const char* assignmentOperator, struct JdcStr* result)
{
	int ogStartInstructionIndex = params->startInstructionIndex;
	struct Operand* secondOperand = &params->instructions[params->startInstructionIndex].operands[1];
	if (secondOperand->type == REGISTER)
	{
		params->startInstructionIndex--;
	}

	struct JdcStr decompiledSecondOperand = initializeJdcStr();
	if (!decompileOperand(params, secondOperand, &decompiledSecondOperand))
	{
		freeJdcStr(&decompiledSecondOperand);
		return 0;
	}

	params->startInstructionIndex = ogStartInstructionIndex;

	if (getAssignment)
	{
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (!decompileOperand(params, &params->instructions[params->startInstructionIndex].operands[0], &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledSecondOperand);
			return 0;
		}

		sprintfJdc(result, 0, "%s%s%s", decompiledFirstOperand.buffer, assignmentOperator, decompiledSecondOperand.buffer);
		freeJdcStr(&decompiledFirstOperand);
		freeJdcStr(&decompiledSecondOperand);
		return 1;
	}

	sprintfJdc(result, 0, "%s%s", regularOperator, decompiledSecondOperand.buffer);
	freeJdcStr(&decompiledSecondOperand);
	return 1;
}

static unsigned char decompileLEA(struct DecompilationParameters* params, unsigned char getAssignment, struct JdcStr* result)
{
	struct Operand* secondOperand = &params->instructions[params->startInstructionIndex].operands[1];
	if (secondOperand->type == MEM_ADDRESS && !compareRegisters(secondOperand->memoryAddress.reg, BP) && !compareRegisters(secondOperand->memoryAddress.reg, SP))
	{
		return decompileBinaryOperation(params, getAssignment, "", " = ", result);
	}

	return decompileBinaryOperation(params, getAssignment, "&", " = &", result);
}

static unsigned char decompileInc(struct DecompilationParameters* params, unsigned char getAssignment, struct JdcStr* result)
{
	if (getAssignment) 
	{ 
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (!decompileOperand(params, &params->instructions[params->startInstructionIndex].operands[0], &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			return 0;
		}

		sprintfJdc(result, 0, "%s++", decompiledFirstOperand.buffer); 
		freeJdcStr(&decompiledFirstOperand);
		return 1;
	}

	strcpyJdc(result, " + 1");
	return 1;
}

static unsigned char decompileDec(struct DecompilationParameters* params, unsigned char getAssignment, struct JdcStr* result)
{
	if (getAssignment)
	{
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (!decompileOperand(params, &params->instructions[params->startInstructionIndex].operands[0], &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			return 0;
		}

		sprintfJdc(result, 0, "%s--", decompiledFirstOperand.buffer);
		freeJdcStr(&decompiledFirstOperand);
		return 1;
	}

	strcpyJdc(result, " - 1");
	return 1;
}

static unsigned char decompileNeg(struct DecompilationParameters* params, unsigned char getAssignment, struct JdcStr* result)
{
	if (getAssignment)
	{
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (!decompileOperand(params, &params->instructions[params->startInstructionIndex].operands[0], &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			return 0;
		}

		sprintfJdc(result, 0, "%s = -%s", decompiledFirstOperand.buffer, decompiledFirstOperand.buffer);
		freeJdcStr(&decompiledFirstOperand);
		return 1;
	}

	strcpyJdc(result, " * -1");
	return 1;
}

static unsigned char decompileXor(struct DecompilationParameters* params, unsigned char getAssignment, struct JdcStr* result)
{
	struct Operand* firstOperand = &params->instructions[params->startInstructionIndex].operands[0];
	struct Operand* secondOperand = &params->instructions[params->startInstructionIndex].operands[1];
	if (compareOperands(firstOperand, secondOperand))
	{
		if (getAssignment) 
		{
			struct JdcStr decompiledFirstOperand = initializeJdcStr();
			if (!decompileOperand(params, firstOperand, &decompiledFirstOperand))
			{
				freeJdcStr(&decompiledFirstOperand);
				return 0;
			}
			
			sprintfJdc(result, 0, "%s = 0", decompiledFirstOperand.buffer);
			freeJdcStr(&decompiledFirstOperand);
			return 1;
		}
		
		strcpyJdc(result, "0");
		return 1;
	}
	
	return decompileBinaryOperation(params, getAssignment, " ^ ", " ^= ", result);
}

static unsigned char decompileFLD(struct DecompilationParameters* params, unsigned char getAssignment, struct JdcStr* result) 
{
	struct JdcStr decompiledFirstOperand = initializeJdcStr();
	if (!decompileOperand(params, &params->instructions[params->startInstructionIndex].operands[0], &decompiledFirstOperand))
	{
		freeJdcStr(&decompiledFirstOperand);
		return 0;
	}
	
	if (getAssignment) 
	{ 
		struct JdcStr decompiledST0 = initializeJdcStr();
		if (!decompileRegister(params, ST0, &decompiledST0, 0))
		{
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledST0);
			return 0;
		}
		
		sprintfJdc(result, 0, "%s = %s", decompiledST0.buffer, decompiledFirstOperand.buffer);
		freeJdcStr(&decompiledFirstOperand);
		freeJdcStr(&decompiledST0);
		return 1;
	}

	sprintfJdc(result, 0, "%s", decompiledFirstOperand.buffer);
	freeJdcStr(&decompiledFirstOperand);
	return 1;
}

static unsigned char decompileIDIV(struct DecompilationParameters* params, unsigned char getAssignment, struct JdcStr* result) 
{
	struct JdcStr decompiledFirstOperand = initializeJdcStr();
	if (!decompileOperand(params, &params->instructions[params->startInstructionIndex].operands[0], &decompiledFirstOperand))
	{
		freeJdcStr(&decompiledFirstOperand);
		return 0;
	}
	
	if (getAssignment)
	{
		struct JdcStr decompiledAX = initializeJdcStr();
		if (!decompileRegister(params, AX, &decompiledAX, 0))
		{
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledAX);
			return 0;
		}

		sprintfJdc(result, 0, "%s /= %s", decompiledAX.buffer, decompiledFirstOperand.buffer);
		freeJdcStr(&decompiledAX);
		freeJdcStr(&decompiledFirstOperand);
		return 1;
	}

	sprintfJdc(result, 0, " / %s", decompiledFirstOperand.buffer);
	freeJdcStr(&decompiledFirstOperand);
	return 1;
}

static unsigned char decompileIMUL(struct DecompilationParameters* params, enum Register targetReg, unsigned char getAssignment, struct JdcStr* result)
{
	struct Operand* firstOperand = &params->instructions[params->startInstructionIndex].operands[0];
	struct Operand* secondOperand = &params->instructions[params->startInstructionIndex].operands[1];
	struct Operand* thirdOperand = &params->instructions[params->startInstructionIndex].operands[2];
	
	if (thirdOperand->type != NO_OPERAND)
	{
		struct JdcStr decompiledThirdOperand = initializeJdcStr();
		if (!decompileOperand(params, thirdOperand, &decompiledThirdOperand))
		{
			freeJdcStr(&decompiledThirdOperand);
			return 0;
		}

		if (getAssignment) 
		{
			struct JdcStr decompiledFirstOperand = initializeJdcStr();
			if (!decompileOperand(params, firstOperand, &decompiledFirstOperand))
			{
				freeJdcStr(&decompiledFirstOperand);
				freeJdcStr(&decompiledThirdOperand);
				return 0;
			}

			if (compareOperands(firstOperand, secondOperand))
			{
				sprintfJdc(result, 0, "%s *= %s", decompiledFirstOperand.buffer, decompiledThirdOperand.buffer);
				freeJdcStr(&decompiledThirdOperand);
				freeJdcStr(&decompiledFirstOperand);
				return 1;
			}

			struct JdcStr decompiledSecondOperand = initializeJdcStr();
			if (!decompileOperand(params, secondOperand, &decompiledSecondOperand))
			{
				freeJdcStr(&decompiledFirstOperand);
				freeJdcStr(&decompiledSecondOperand);
				freeJdcStr(&decompiledThirdOperand);
				return 0;
			}

			sprintfJdc(result, 0, "%s = (%s * %s)", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer, decompiledThirdOperand.buffer);
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
		if (!decompileOperand(params, secondOperand, &decompiledSecondOperand))
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
	else if (secondOperand->type != NO_OPERAND) 
	{
		return decompileBinaryOperation(params, getAssignment, " * ", " *= ", result);
	}

	// one operand form
	
	struct JdcStr decompiledFirstOperand = initializeJdcStr();
	if (!decompileOperand(params, firstOperand, &decompiledFirstOperand))
	{
		freeJdcStr(&decompiledFirstOperand);
		return 0;
	}

	if (getAssignment)
	{
		struct JdcStr decompiledAX = initializeJdcStr();
		if (!decompileRegister(params, AX, &decompiledAX, 0))
		{
			freeJdcStr(&decompiledAX);
			return 0;
		}

		if (compareRegisters(targetReg, AX))
		{
			sprintfJdc(result, 0, "%s *= %s", decompiledAX.buffer, decompiledFirstOperand.buffer);
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledAX);
			return 1;
		}
		else if (compareRegisters(targetReg, DX))
		{
			struct JdcStr decompiledDX = initializeJdcStr();
			if (!decompileRegister(params, DX, &decompiledDX, 0))
			{
				freeJdcStr(&decompiledFirstOperand);
				freeJdcStr(&decompiledAX);
				freeJdcStr(&decompiledDX);
				return 0;
			}

			sprintfJdc(result, 0, "%s = %s >> %d", decompiledDX.buffer, decompiledAX.buffer, decompiledFirstOperand.buffer, 8 * getSizeOfOperand(firstOperand));
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledAX);
			freeJdcStr(&decompiledDX);
			return 1;
		}

		freeJdcStr(&decompiledAX);
		return 0;
	}

	if (compareRegisters(targetReg, AX))
	{
		sprintfJdc(result, 0, " * %s", decompiledFirstOperand.buffer);
		freeJdcStr(&decompiledFirstOperand);
		return 1;
	}
	else if (compareRegisters(targetReg, DX))
	{
		if (!decompileOperand(params, firstOperand, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			return 0;
		}

		struct JdcStr decompiledAX = initializeJdcStr();
		if (!decompileRegister(params, AX, &decompiledAX, 0))
		{
			freeJdcStr(&decompiledAX);
			return 0;
		}

		sprintfJdc(result, 0, "(%s * %s) >> %d", decompiledAX.buffer, decompiledFirstOperand.buffer, getSizeOfOperand(firstOperand));
	}

	freeJdcStr(&decompiledFirstOperand);
	return 1;
}

static unsigned char decompileCMOVcc(struct DecompilationParameters* params, unsigned char getAssignment, struct JdcStr* result) 
{
	struct JdcStr comparisonStr = initializeJdcStr();
	if (!decompileComparison(params, 0, &comparisonStr))
	{
		freeJdcStr(&comparisonStr);
		return 0;
	}

	struct JdcStr decompiledFirstOperand = initializeJdcStr();
	if (!decompileOperand(params, &params->instructions[params->startInstructionIndex].operands[0], &decompiledFirstOperand))
	{
		freeJdcStr(&comparisonStr);
		freeJdcStr(&decompiledFirstOperand);
		return 0;
	}

	struct JdcStr decompiledSecondOperand = initializeJdcStr();
	if (!decompileOperand(params, &params->instructions[params->startInstructionIndex].operands[1], &decompiledSecondOperand))
	{
		freeJdcStr(&comparisonStr);
		freeJdcStr(&decompiledFirstOperand);
		freeJdcStr(&decompiledSecondOperand);
		return 0;
	}

	if (getAssignment) 
	{ 
		sprintfJdc(result, 0, "%s = (%s ? %s : %s)", decompiledFirstOperand.buffer, comparisonStr.buffer, decompiledSecondOperand.buffer, decompiledFirstOperand.buffer);
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

static unsigned char decompileSETcc(struct DecompilationParameters* params, unsigned char getAssignment, struct JdcStr* result)
{
	struct JdcStr comparisonStr = initializeJdcStr();
	if (!decompileComparison(params, 0, &comparisonStr))
	{
		freeJdcStr(&comparisonStr);
		return 0;
	}

	if (getAssignment) 
	{ 
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (!decompileOperand(params, &params->instructions[params->startInstructionIndex].operands[0], &decompiledFirstOperand))
		{
			freeJdcStr(&comparisonStr);
			freeJdcStr(&decompiledFirstOperand);
			return 0;
		}

		sprintfJdc(result, 0, "%s = %s", decompiledFirstOperand.buffer, comparisonStr.buffer); 
		freeJdcStr(&comparisonStr);
		freeJdcStr(&decompiledFirstOperand);
		return 1;
	}

	sprintfJdc(result, 0, "%s", comparisonStr.buffer);
	freeJdcStr(&comparisonStr);
	return 1;
}

static unsigned char decompilePop(struct DecompilationParameters* params, unsigned char getAssignment, struct JdcStr* result) 
{
	struct JdcStr value = initializeJdcStr();

	int stackOffset = 0;
	unsigned char gotValue = 0;
	int ogStartInstructionIndex = params->startInstructionIndex;
	for (int i = ogStartInstructionIndex; i >= 0; i--)
	{
		if (params->instructions[i].opcode == PUSH)
		{
			stackOffset--;
			if (stackOffset == 0)
			{
				params->startInstructionIndex = i;
				if (!decompileOperand(params, &params->instructions[i].operands[0], &value))
				{
					freeJdcStr(&value);
					return 0;
				}

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
				gotValue = 1;
				break;
			}
		}
		else if (params->instructions[i].opcode == POP)
		{
			stackOffset++;
		}
	}

	struct Operand* firstOperand = &params->instructions[params->startInstructionIndex].operands[0];
	if (getAssignment)
	{
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (!decompileOperand(params, firstOperand, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			return 0;
		}

		if (gotValue) 
		{
			sprintfJdc(result, 0, "%s = %s", decompiledFirstOperand.buffer, value.buffer);
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&value);
			return 1;
		}
		else if (firstOperand->type == REGISTER)
		{
			sprintfJdc(result, 0, "%s = %s", decompiledFirstOperand.buffer, registerStrs[firstOperand->reg]);
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

static unsigned char decompileXCHG(struct DecompilationParameters* params, enum Register targetReg, unsigned char getAssignment, int dstOperandIndex, struct JdcStr* result)
{
	struct Operand* firstOperand = &params->instructions[params->startInstructionIndex].operands[0];
	struct Operand* secondOperand = &params->instructions[params->startInstructionIndex].operands[1];
	
	if (getAssignment) 
	{
		struct JdcStr decompiledFirstOperand = initializeJdcStr();
		if (!decompileOperand(params, firstOperand, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			return 0;
		}

		struct JdcStr decompiledSecondOperand = initializeJdcStr();
		if (!decompileOperand(params, secondOperand, &decompiledSecondOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledSecondOperand);
			return 0;
		}
		
		if (dstOperandIndex == 0) 
		{
			sprintfJdc(result, 0, "%s = %s", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer);
		}
		else 
		{
			sprintfJdc(result, 0, "%s = %s", decompiledSecondOperand.buffer, decompiledFirstOperand.buffer);
		}

		freeJdcStr(&decompiledFirstOperand);
		freeJdcStr(&decompiledSecondOperand);
		return 1;
	}

	if (firstOperand->type == REGISTER && compareRegisters(firstOperand->reg, targetReg))
	{
		if (!decompileOperand(params, secondOperand, result))
		{
			return 0;
		}
	}
	else 
	{
		if (!decompileOperand(params, firstOperand, result))
		{
			return 0;
		}
	}

	return 1;
}