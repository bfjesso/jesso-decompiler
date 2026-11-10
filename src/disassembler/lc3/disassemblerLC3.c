#include "disassemblerLC3.h"
#include "opcodeMapLC3.h"

enum JdcStatus disassembleLC3(uint8_t* bytes, uint8_t* maxBytesAddr, struct DisassembledInstruction* result)
{
	if ((bytes + 1) > maxBytesAddr)
	{
		return ERROR_JDC;
	}

	memset(result, 0, sizeof(struct DisassembledInstruction));

	uint16_t instructionData = (bytes[0] << 8) + bytes[1];
	if (instructionData == 0)
	{
		result->opcode = NOP_LC3;
		result->numOfOperands = 0;
		result->numOfBytes = 2;
		return SUCCESS_JDC;
	}

	uint8_t opcodeBits = (instructionData & 0b1111000000000000) >> 12;
	if (opcodeBits > 0xF)
	{
		return ERROR_JDC;
	}

	const struct Opcode* opcode = &opcodeMapLC3[opcodeBits];
	if (opcode->mnemonic == NO_OPCODE ||
		(opcode->mnemonic == NOT_LC3 && (instructionData & 0b111111) != 0b111111) ||
		(opcode->mnemonic == RET_LC3 && (instructionData & 0b111111111111) != 0b000111000000) ||
		(opcode->mnemonic == RTI_LC3 && (instructionData & 0b111111111111) != 0b000000000000))
	{
		return ERROR_JDC;
	}

	result->opcode = opcode->mnemonic;

	if (result->opcode == BRP_LC3)
	{
		result->opcode = (((instructionData & 0b111000000000) >> 9) + NOP_LC3);
		if (result->opcode == NOP_LC3)
		{
			return ERROR_JDC;
		}
	}
	else if (result->opcode == JSRR_LC3 && (instructionData & (1 << 11)))
	{
		result->opcode = JSR_LC3;
	}

	struct Operand operands[4] = { 0 };

	int32_t numOfOperands;
	for (numOfOperands = 0; numOfOperands < 3; numOfOperands++)
	{
		if (opcode->operandCodes[numOfOperands] == NONE_OC)
		{
			break;
		}

		struct Operand* currentOperand = &(operands[numOfOperands]);
		switch (opcode->operandCodes[numOfOperands])
		{
		case DRSR_LC3_OC:
			currentOperand->type = REGISTER_OPERAND;
			currentOperand->reg = (((instructionData & 0b111000000000) >> 9) + R0_LC3_REG);
			break;
		case SR1_LC3_OC:
			currentOperand->type = REGISTER_OPERAND;
			currentOperand->reg = (((instructionData & 0b111000000) >> 6) + R0_LC3_REG);
			break;
		case SR2_IMM5_LC3_OC:
			if (instructionData & (1 << 5))
			{
				if (ERROR_JDC == readSignedIntFromLowerBits(instructionData, 5, &currentOperand->immediate.value)) { return ERROR_JDC; }
				currentOperand->type = IMMEDIATE_OPERAND;
				currentOperand->immediate.size = 2;
			}
			else
			{
				if (instructionData & 0b11000) { return ERROR_JDC; }
				currentOperand->type = REGISTER_OPERAND;
				currentOperand->reg = ((instructionData & 0b111) + R0_LC3_REG);
			}

			break;
		case PCOFFSET9_LC3_OC:
			if (ERROR_JDC == readSignedIntFromLowerBits(instructionData, 9, &currentOperand->immediate.value)) { return ERROR_JDC; }
			currentOperand->type = IMMEDIATE_OPERAND;
			currentOperand->immediate.size = 2;
			break;
		case BASER_PCOFFSET11_LC3_OC:
			if (instructionData & (1 << 11))
			{
				if (ERROR_JDC == readSignedIntFromLowerBits(instructionData, 11, &currentOperand->immediate.value)) { return ERROR_JDC; }
				currentOperand->type = IMMEDIATE_OPERAND;
				currentOperand->immediate.size = 2;
			}
			else
			{
				if ((instructionData & 0b111000000000) || (instructionData & 0b111111)) { return ERROR_JDC; }
				currentOperand->type = MEMORY_OPERAND;
				currentOperand->memoryAddress.reg = (((instructionData & 0b111000000) >> 6) + R0_LC3_REG);
			}

			break;
		case BASER_LC3_OC:
			if ((instructionData & 0b111000000000) || (instructionData & 0b111111)) { return ERROR_JDC; }
			currentOperand->type = MEMORY_OPERAND;
			currentOperand->memoryAddress.reg = (((instructionData & 0b111000000) >> 6) + R0_LC3_REG);
			break;
		case BASER_OFFSET6_LC3_OC:
			if (ERROR_JDC == readSignedIntFromLowerBits(instructionData, 6, &currentOperand->memoryAddress.constDisplacement)) { return ERROR_JDC; }
			currentOperand->type = MEMORY_OPERAND;
			currentOperand->memoryAddress.reg = (((instructionData & 0b111000000) >> 6) + R0_LC3_REG);
			break;
		case TRAPVECT8_LC3_OC:
			if (instructionData & 0b111100000000) { return ERROR_JDC; }
			if (ERROR_JDC == readSignedIntFromLowerBits(instructionData, 8, &currentOperand->immediate.value)) { return ERROR_JDC; }
			currentOperand->type = IMMEDIATE_OPERAND;
			currentOperand->immediate.size = 2;
			break;
		}
	}

	if (numOfOperands > 0)
	{
		result->operands = (struct Operand*)calloc(numOfOperands, sizeof(struct Operand));
		if (!result->operands)
		{
			return ERROR_JDC;
		}

		for (int32_t i = 0; i < numOfOperands; i++)
		{
			result->operands[i] = operands[i];
		}
	}

	result->numOfOperands = numOfOperands;
	result->numOfBytes = 2;

	return SUCCESS_JDC;
}

static enum JdcStatus readSignedIntFromLowerBits(uint16_t instructionData, uint8_t numOfBits, int64_t* result)
{
	if (!result || numOfBits > 16 || numOfBits == 0)
	{
		return ERROR_JDC;
	}

	bool isNegative = instructionData & (1 << (numOfBits - 1));

	*result = isNegative ? 0xFFFFFFFFFFFFFFFF : 0;
	for (int32_t i = 0; i < numOfBits; i++)
	{
		int64_t bit = (instructionData & (1 << i));
		if (isNegative && !bit)
		{
			*result &= (~((unsigned long long)1 << i));
		}
		else if (!isNegative && bit)
		{
			*result |= ((unsigned long long)1 << i);
		}
	}

	return SUCCESS_JDC;
}