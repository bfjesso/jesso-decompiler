#include "disassemblerLC3.h"

enum JdcStatus disassembleLC3Instruction(uint16_t instructionData, struct DisassembledInstruction* result)
{
	memset(result, 0, sizeof(struct DisassembledInstruction));

	uint8_t opcodeBits = (instructionData & 0b1111000000000000) >> 12;
	struct Opcode* opcode = &opcodeMap[opcodeBits];

	struct Operand operands[4] = { 0 };

	uint32_t numOfOperands;
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
		case PCOFFSET11_LC3_OC:
			if (!(instructionData & (1 << 11))) { return ERROR_JDC; }
			if (ERROR_JDC == readSignedIntFromLowerBits(instructionData, 11, &currentOperand->immediate.value)) { return ERROR_JDC; }
			currentOperand->type = IMMEDIATE_OPERAND;
			currentOperand->immediate.size = 2;
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

	result->opcode = opcode->mnemonic;
	result->numOfBytes = 2;

	return SUCCESS_JDC;
}

enum JdcStatus readSignedIntFromLowerBits(uint16_t instructionData, uint8_t numOfBits, int16_t* result)
{
	if (!result || numOfBits > 16)
	{
		return ERROR_JDC;
	}

	*result = 0xFFFF;
	for (int32_t i = 0; i < numOfBits; i++)
	{
		int16_t mask = (instructionData & (1 << i));
		if (!mask)
		{
			*result &= (~mask);
		}
	}

	return SUCCESS_JDC;
}