#include "operations.h"
#include "intrinsics.h"
#include "expressions.h"

unsigned char decompileOperation(struct DecompilationParameters* params, enum Register targetReg, unsigned char getAssignment, struct JdcStr* result)
{
	struct DisassembledInstruction* instruction = &(params->currentFunc->instructions[params->startInstructionIndex]);

	struct IntrinsicFunc* intrinsicFunc;
	if (checkForReturningIntrinsicFunc(instruction->opcode, &intrinsicFunc))
	{
		return decompileReturningIntrinsicFunc(params, intrinsicFunc, getAssignment, result);
	}

	struct Operand* firstOperand = &instruction->operands[0];
	struct JdcStr decompiledFirstOperand = initializeJdcStr();
	if (getAssignment && firstOperand->type != NO_OPERAND)
	{
		if (!decompileOperand(params, firstOperand, &decompiledFirstOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			return 0;
		}
	}

	if (isOpcodeXor(instruction->opcode) && compareOperands(&instruction->operands[0], &instruction->operands[1]))
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s = 0", decompiledFirstOperand.buffer); }
		else { strcpyJdc(result, "0"); }
		freeJdcStr(&decompiledFirstOperand);
		return 1;
	}
	else if (instruction->opcode == INC)
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s++", decompiledFirstOperand.buffer); }
		else { strcpyJdc(result, " + 1"); }
		freeJdcStr(&decompiledFirstOperand);
		return 1;
	}
	else if (instruction->opcode == DEC)
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s--", decompiledFirstOperand.buffer); }
		else { strcpyJdc(result, " - 1"); }
		freeJdcStr(&decompiledFirstOperand);
		return 1;
	}
	else if (instruction->opcode == FLD)
	{
		if (getAssignment) { sprintfJdc(result, 0, " = %s", decompiledFirstOperand.buffer); }
		else 
		{ 
			if (!decompileOperand(params, firstOperand, &decompiledFirstOperand))
			{
				freeJdcStr(&decompiledFirstOperand);
				return 0;
			}
			
			sprintfJdc(result, 0, "%s", decompiledFirstOperand.buffer); 
		}
		freeJdcStr(&decompiledFirstOperand);
		return 1;
	}
	else if (instruction->opcode == IDIV)
	{
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
		}
		else 
		{ 
			if (!decompileOperand(params, firstOperand, &decompiledFirstOperand))
			{
				freeJdcStr(&decompiledFirstOperand);
				return 0;
			}
			
			sprintfJdc(result, 0, " / %s", decompiledFirstOperand.buffer); 
		}

		freeJdcStr(&decompiledFirstOperand);
		return 1;
	}
	else if (instruction->opcode == NEG)
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s = -%s", decompiledFirstOperand.buffer, decompiledFirstOperand.buffer); }
		else { strcatJdc(result, " * -1"); }
		freeJdcStr(&decompiledFirstOperand);
		return 1;
	}
	else if (instruction->opcode == IMUL && instruction->operands[2].type != NO_OPERAND)
	{
		struct JdcStr decompiledThirdOperand = initializeJdcStr();
		if (!decompileOperand(params, &instruction->operands[2], &decompiledThirdOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledThirdOperand);
			return 0;
		}

		if (compareOperands(&instruction->operands[0], &instruction->operands[1]))
		{
			if (getAssignment) { sprintfJdc(result, 0, "%s *= %s", decompiledFirstOperand.buffer, decompiledThirdOperand.buffer); }
			else { sprintfJdc(result, 0, " * %s", decompiledThirdOperand.buffer); }
			freeJdcStr(&decompiledThirdOperand);
			freeJdcStr(&decompiledFirstOperand);
			return 1;
		}
		else
		{
			struct JdcStr decompiledSecondOperand = initializeJdcStr();
			if (!decompileOperand(params, &instruction->operands[1], &decompiledSecondOperand))
			{
				freeJdcStr(&decompiledFirstOperand);
				freeJdcStr(&decompiledSecondOperand);
				freeJdcStr(&decompiledThirdOperand);
				return 0;
			}

			if (getAssignment) { sprintfJdc(result, 0, "%s = (%s * %s)", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer, decompiledThirdOperand.buffer); }
			else { sprintfJdc(result, 0, "(%s * %s)", decompiledSecondOperand.buffer, decompiledThirdOperand.buffer); }
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledSecondOperand);
			freeJdcStr(&decompiledThirdOperand);
			return 1;
		}
	}
	else if (instruction->opcode == IMUL && instruction->operands[1].type == NO_OPERAND)
	{
		struct JdcStr decompiledAX = initializeJdcStr();
		if (!decompileRegister(params, AX, &decompiledAX, 0))
		{
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledAX);
			return 0;
		}

		if (compareRegisters(targetReg, AX))
		{
			if (getAssignment) { sprintfJdc(result, 0, "%s *= %s", decompiledAX.buffer, decompiledFirstOperand.buffer); }
			else
			{
				if (!decompileOperand(params, firstOperand, &decompiledFirstOperand))
				{
					freeJdcStr(&decompiledFirstOperand);
					freeJdcStr(&decompiledAX);
					return 0;
				}

				sprintfJdc(result, 0, " * %s", decompiledFirstOperand.buffer);
			}
		}
		else if (compareRegisters(targetReg, DX))
		{
			if (getAssignment)
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
				freeJdcStr(&decompiledDX);
			}
			else
			{
				if (!decompileOperand(params, firstOperand, &decompiledFirstOperand))
				{
					freeJdcStr(&decompiledFirstOperand);
					freeJdcStr(&decompiledAX);
					return 0;
				}

				sprintfJdc(result, 0, "(%s * %s) >> %d", decompiledAX.buffer, decompiledFirstOperand.buffer, getSizeOfOperand(firstOperand));
			}
		}

		freeJdcStr(&decompiledFirstOperand);
		freeJdcStr(&decompiledAX);
		return 1;
	}
	else if (isOpcodeCMOVcc(instruction->opcode))
	{
		struct JdcStr comparisonStr = initializeJdcStr();
		if (!decompileComparison(params, 0, &comparisonStr))
		{
			freeJdcStr(&comparisonStr);
			freeJdcStr(&decompiledFirstOperand);
			return 0;
		}

		struct JdcStr decompiledSecondOperand = initializeJdcStr();
		if (!decompileOperand(params, &instruction->operands[1], &decompiledSecondOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledSecondOperand);
			return 0;
		}

		if (getAssignment) { sprintfJdc(result, 0, "%s = (%s ? %s : %s)", decompiledFirstOperand.buffer, comparisonStr.buffer, decompiledSecondOperand.buffer, decompiledFirstOperand.buffer); }
		else 
		{ 
			if (!decompileOperand(params, firstOperand, &decompiledFirstOperand))
			{
				freeJdcStr(&decompiledFirstOperand);
				return 0;
			}

			sprintfJdc(result, 0, "(%s ? %s : %s)", comparisonStr.buffer, decompiledSecondOperand.buffer, decompiledFirstOperand.buffer); 
		}
		freeJdcStr(&comparisonStr);
		freeJdcStr(&decompiledFirstOperand);
		return 1;
	}
	else if (isOpcodeSETcc(instruction->opcode))
	{
		struct JdcStr comparisonStr = initializeJdcStr();
		if (!decompileComparison(params, 0, &comparisonStr))
		{
			freeJdcStr(&comparisonStr);
			freeJdcStr(&decompiledFirstOperand);
			return 0;
		}

		if (getAssignment) { sprintfJdc(result, 0, "%s = %s", decompiledFirstOperand.buffer, comparisonStr.buffer); }
		else { sprintfJdc(result, 0, "%s", comparisonStr.buffer); }
		freeJdcStr(&comparisonStr);
		freeJdcStr(&decompiledFirstOperand);
		return 1;
	}
	else if (instruction->group1Prefix == REPZ && instruction->opcode == MOVS)
	{
		struct JdcStr count = initializeJdcStr();
		if (!decompileRegister(params, CX, &count, 0))
		{
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&count);
			return 0;
		}

		struct JdcStr decompiledSecondOperand = initializeJdcStr();
		if (!decompileOperand(params, &instruction->operands[1], &decompiledSecondOperand))
		{
			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&decompiledSecondOperand);
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

		sprintfJdc(result, 0, "memcpy(%s, %s, %s)", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer, count.buffer);
		freeJdcStr(&count);
		freeJdcStr(&decompiledFirstOperand);
		freeJdcStr(&decompiledSecondOperand);
		return 1;
	}
	else if (instruction->opcode == POP)
	{
		struct JdcStr value = initializeJdcStr();

		int stackOffset = 0;
		unsigned char gotValue = 0;
		int ogStartInstructionIndex = params->startInstructionIndex;
		for (int i = ogStartInstructionIndex; i >= 0; i--)
		{
			if (params->currentFunc->instructions[i].opcode == PUSH)
			{
				stackOffset--;
				if (stackOffset == 0)
				{
					params->startInstructionIndex = i;
					if (!decompileOperand(params, &params->currentFunc->instructions[i].operands[0], &value))
					{
						freeJdcStr(&decompiledFirstOperand);
						freeJdcStr(&value);
						return 0;
					}

					gotValue = 1;
					break;
				}
			}
			else if (params->currentFunc->instructions[i].opcode == PUSHF)
			{
				stackOffset--;
				if (stackOffset == 0)
				{
					strcpyJdc(&value, "__readeflags()"); // Windows function
					gotValue = 1;
					break;
				}
			}
			else if (params->currentFunc->instructions[i].opcode == POP)
			{
				stackOffset++;
			}
		}

		if (gotValue)
		{
			if (getAssignment) { sprintfJdc(result, 0, "%s = %s", decompiledFirstOperand.buffer, value.buffer); }
			else { sprintfJdc(result, 0, "%s", value.buffer); }
			return 1;
		}
		else
		{
			if (instruction->operands[0].type == REGISTER)
			{
				if (getAssignment) { sprintfJdc(result, 0, "%s = %s", decompiledFirstOperand.buffer, registerStrs[instruction->operands[0].reg]); }
				else { sprintfJdc(result, 0, "%s", registerStrs[instruction->operands[0].reg]); }
				gotValue = 1;
			}

			freeJdcStr(&decompiledFirstOperand);
			freeJdcStr(&value);
			return gotValue;
		}
	}

	unsigned short ogStartInstructionIndex = params->startInstructionIndex;

	struct Operand* secondOperand = &instruction->operands[1];
	if (secondOperand->type == REGISTER)
	{
		params->startInstructionIndex--; // avoiding infinite recursive loop with decompileRegister. startInstructionIndex could just be decremented for most instructions, but if secondOperand is dependent on the instruction pointer it needs startInstructionIndex to be the current instruction index
	}

	struct JdcStr decompiledSecondOperand = initializeJdcStr();
	if (!decompileOperand(params, secondOperand, &decompiledSecondOperand))
	{
		freeJdcStr(&decompiledSecondOperand);
		return 0;
	}

	if (isOpcodeMov(instruction->opcode) ||
		(instruction->opcode == LEA && secondOperand->type == MEM_ADDRESS && !compareRegisters(secondOperand->memoryAddress.reg, BP) && !compareRegisters(secondOperand->memoryAddress.reg, SP)))
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s = %s", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer); }
		else { sprintfJdc(result, 0, "%s", decompiledSecondOperand.buffer); }
	}
	else if (instruction->opcode == LEA)
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s = &%s", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer); }
		else { sprintfJdc(result, 0, "&%s", decompiledSecondOperand.buffer); }
	}
	else if (isOpcodeAdd(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s += %s", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer); }
		else { sprintfJdc(result, 0, " + %s", decompiledSecondOperand.buffer); }
	}
	else if (isOpcodeSub(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s -= %s", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer); }
		else { sprintfJdc(result, 0, " - %s", decompiledSecondOperand.buffer); }
	}
	else if (isOpcodeAnd(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s &= %s", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer); }
		else { sprintfJdc(result, 0, " & %s", decompiledSecondOperand.buffer); }
	}
	else if (instruction->opcode == IMUL)
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s *= %s", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer); }
		else { sprintfJdc(result, 0, " * %s", decompiledSecondOperand.buffer); }
	}
	else if (isOpcodeOr(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s |= %s", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer); }
		else { sprintfJdc(result, 0, " | %s", decompiledSecondOperand.buffer); }
	}
	else if (isOpcodeXor(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s ^= %s", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer); }
		else { sprintfJdc(result, 0, " ^ %s", decompiledSecondOperand.buffer); }
	}
	else if (isOpcodeShl(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s <<= %s", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer); }
		else { sprintfJdc(result, 0, " << %s", decompiledSecondOperand.buffer); }
	}
	else if (isOpcodeShr(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s >>= %s", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer); }
		else { sprintfJdc(result, 0, " >> %s", decompiledSecondOperand.buffer); }
	}
	else if (isOpcodeCvtToDbl(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s = (double)%s", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer); }
		else { sprintfJdc(result, 0, "(double)%s", decompiledSecondOperand.buffer); }
	}
	else if (isOpcodeCvtToFlt(instruction->opcode))
	{
		if (getAssignment) { sprintfJdc(result, 0, "%s = (float)%s", decompiledFirstOperand.buffer, decompiledSecondOperand.buffer); }
		else { sprintfJdc(result, 0, "(float)%s", decompiledSecondOperand.buffer); }
	}
	else
	{
		freeJdcStr(&decompiledFirstOperand);
		freeJdcStr(&decompiledSecondOperand);
		return 0;
	}

	params->startInstructionIndex = ogStartInstructionIndex;

	freeJdcStr(&decompiledFirstOperand);
	freeJdcStr(&decompiledSecondOperand);
	return 1;
}