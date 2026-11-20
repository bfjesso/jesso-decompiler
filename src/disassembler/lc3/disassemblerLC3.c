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
	else if (instructionData == 0xF025) // TRAP 0x25
	{
		result->opcode = HALT_LC3;
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
			if (ERROR_JDC == readSignedIntFromLowerBits(instructionData, 9, &currentOperand->memoryAddress.constDisplacement)) { return ERROR_JDC; }
			currentOperand->type = MEMORY_OPERAND;
			currentOperand->memoryAddress.reg = PC_LC3_REG;
			currentOperand->memoryAddress.ptrSize = 2;
			currentOperand->memoryAddress.scale = 1;
			break;
		case BASER_PCOFFSET11_LC3_OC:
			if (instructionData & (1 << 11))
			{
				if (ERROR_JDC == readSignedIntFromLowerBits(instructionData, 11, &currentOperand->memoryAddress.constDisplacement)) { return ERROR_JDC; }
				currentOperand->memoryAddress.reg = PC_LC3_REG;
			}
			else
			{
				if ((instructionData & 0b111000000000) || (instructionData & 0b111111)) { return ERROR_JDC; }
				currentOperand->memoryAddress.reg = (((instructionData & 0b111000000) >> 6) + R0_LC3_REG);
			}

			currentOperand->type = MEMORY_OPERAND;
			currentOperand->memoryAddress.ptrSize = 2;
			currentOperand->memoryAddress.scale = 1;
			break;
		case BASER_LC3_OC:
			if ((instructionData & 0b111000000000) || (instructionData & 0b111111)) { return ERROR_JDC; }
			currentOperand->type = MEMORY_OPERAND;
			currentOperand->memoryAddress.reg = (((instructionData & 0b111000000) >> 6) + R0_LC3_REG);
			currentOperand->memoryAddress.ptrSize = 2;
			currentOperand->memoryAddress.scale = 1;
			break;
		case BASER_OFFSET6_LC3_OC:
			if (ERROR_JDC == readSignedIntFromLowerBits(instructionData, 6, &currentOperand->memoryAddress.constDisplacement)) { return ERROR_JDC; }
			currentOperand->type = MEMORY_OPERAND;
			currentOperand->memoryAddress.reg = (((instructionData & 0b111000000) >> 6) + R0_LC3_REG);
			currentOperand->memoryAddress.ptrSize = 2;
			currentOperand->memoryAddress.scale = 1;
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

enum JdcStatus generateInstructionInfoStrLC3(struct DisassembledInstruction* instruction, uint16_t instructionData, struct JdcStr* result)
{
	uint8_t opcodeBits = (instructionData & 0b1111000000000000) >> 12;
	sprintfJdc(result, true, "Opcode bits: %d%d%d%d\n", (opcodeBits & 0b1000) >> 3, (opcodeBits & 0b100) >> 2, (opcodeBits & 0b10) >> 1, opcodeBits & 0b1);


	const struct Opcode* opcode = &opcodeMapLC3[opcodeBits];
	sprintfJdc(result, true, "\tMnemonic: %s\n", opcodeMnemonicStrs[opcode->mnemonic]);

	if (opcode->mnemonic == BRP_LC3)
	{
		uint8_t nzp = ((instructionData & 0b111000000000) >> 9);
		sprintfJdc(result, true, "nzp: %d%d%d\n", (nzp & 0b100) >> 2, (nzp & 0b10) >> 1, nzp & 0b1);
	}


	for (int32_t i = 0; i < 3; i++)
	{
		if (opcode->operandCodes[i] == NONE_OC)
		{
			break;
		}

		uint16_t tmp = 0;
		switch (opcode->operandCodes[i])
		{
		case DRSR_LC3_OC:
			tmp = ((instructionData & 0b111000000000) >> 9);
			sprintfJdc(result, true, "Destination/source register: %d%d%d\n", (tmp & 0b100) >> 2, (tmp & 0b10) >> 1, tmp & 0b1);
			break;
		case SR1_LC3_OC:
			tmp = ((instructionData & 0b111000000) >> 6);
			sprintfJdc(result, true, "Source register 1: %d%d%d\n", (tmp & 0b100) >> 2, (tmp & 0b10) >> 1, tmp & 0b1);
			break;
		case SR2_IMM5_LC3_OC:
			if (instructionData & (1 << 5))
			{
				strcatJdc(result, "Bit 5: 1\n");
				tmp = (instructionData & 0b11111);
				sprintfJdc(result, true, "5-bit signed immediate: %d%d%d%d%d\n", (tmp & 0b10000) >> 4, (tmp & 0b1000) >> 3, (tmp & 0b100) >> 2, (tmp & 0b10) >> 1, tmp & 0b1);
			}
			else
			{
				strcatJdc(result, "Bit 5: 0\n");
				strcatJdc(result, "Bits 4-3: 00\n");
				tmp = (instructionData & 0b111);
				sprintfJdc(result, true, "Source register 2: %d%d%d\n", (tmp & 0b100) >> 2, (tmp & 0b10) >> 1, tmp & 0b1);
			}

			break;
		case PCOFFSET9_LC3_OC:
			tmp = (instructionData & 0b111111111);
			sprintfJdc(result, true, "9-bit signed PC offset: %d%d%d%d%d%d%d%d%d\n", (tmp & 0b100000000) >> 8, (tmp & 0b10000000) >> 7, (tmp & 0b1000000) >> 6, (tmp & 0b100000) >> 5, (tmp & 0b10000) >> 4, (tmp & 0b1000) >> 3, (tmp & 0b100) >> 2, (tmp & 0b10) >> 1, tmp & 0b1);
			break;
		case BASER_PCOFFSET11_LC3_OC:
			if (instructionData & (1 << 11))
			{
				strcatJdc(result, "Bit 11: 1\n");
				tmp = (instructionData & 0b11111111111);
				sprintfJdc(result, true, "11-bit signed PC offset: %d%d%d%d%d%d%d%d%d%d%d\n", (tmp & 0b10000000000) >> 10, (tmp & 0b1000000000) >> 9, (tmp & 0b100000000) >> 8, (tmp & 0b10000000) >> 7, (tmp & 0b1000000) >> 6, (tmp & 0b100000) >> 5, (tmp & 0b10000) >> 4, (tmp & 0b1000) >> 3, (tmp & 0b100) >> 2, (tmp & 0b10) >> 1, tmp & 0b1);
			}
			else
			{
				strcatJdc(result, "Bit 11: 0\n");
				strcatJdc(result, "Bits 10-9: 00\n");
				tmp = ((instructionData & 0b111000000) >> 6);
				sprintfJdc(result, true, "Base register: %d%d%d\n", (tmp & 0b100) >> 2, (tmp & 0b10) >> 1, tmp & 0b1);
				strcatJdc(result, "Bits 5-0: 000000\n");
			}
			break;
		case BASER_LC3_OC:
			strcatJdc(result, "Bits 11-9: 000\n");
			tmp = ((instructionData & 0b111000000) >> 6);
			sprintfJdc(result, true, "Base register: %d%d%d\n", (tmp & 0b100) >> 2, (tmp & 0b10) >> 1, tmp & 0b1);
			strcatJdc(result, "Bits 5-0: 000000\n");
			break;
		case BASER_OFFSET6_LC3_OC:
			tmp = ((instructionData & 0b111000000) >> 6);
			sprintfJdc(result, true, "Base register: %d%d%d\n", (tmp & 0b100) >> 2, (tmp & 0b10) >> 1, tmp & 0b1);

			tmp = (instructionData & 0b111111);
			sprintfJdc(result, true, "6-bit signed offset: %d%d%d%d%d%d\n", (tmp & 0b100000) >> 5, (tmp & 0b10000) >> 4, (tmp & 0b1000) >> 3, (tmp & 0b100) >> 2, (tmp & 0b10) >> 1, tmp & 0b1);
			break;
		case TRAPVECT8_LC3_OC:
			strcatJdc(result, "Bits 11-8: 0000\n");
			tmp = (instructionData & 0b11111111);
			sprintfJdc(result, true, "TRAP vector: %d%d%d%d%d%d%d%d\n", (tmp & 0b10000000) >> 7, (tmp & 0b1000000) >> 6, (tmp & 0b100000) >> 5, (tmp & 0b10000) >> 4, (tmp & 0b1000) >> 3, (tmp & 0b100) >> 2, (tmp & 0b10) >> 1, tmp & 0b1);
			break;
		}
	}

	switch (opcode->mnemonic)
	{
	case NOT_LC3:
		strcatJdc(result, "Bits 5-0: 111111\n");
		break;
	case RET_LC3:
		strcatJdc(result, "Bits 11-0: 000 111 000000\n");
		break;
	case RTI_LC3:
		strcatJdc(result, "Bits 11-0: 000000000000\n");
		break;
	}

	return SUCCESS_JDC;
}