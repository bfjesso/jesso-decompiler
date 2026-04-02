#include "decompilationUtils.h"

void addIndents(struct JdcStr* result, int numOfIndents)
{
	for (int i = 0; i < numOfIndents; i++)
	{
		strcatJdc(result, "    ");
	}
}

unsigned long long resolveJmpChain(struct DecompilationParameters* params, int startInstructionIndex)
{
	struct DisassembledInstruction* instruction = &params->allInstructions[startInstructionIndex];

	unsigned long long jmpAddress = 0;
	if (!operandToValue(params, startInstructionIndex, &instruction->operands[0], &jmpAddress))
	{
		return 0;
	}

	int instructionIndex = findInstructionByAddress(params->allInstructions, 0, params->totalNumOfInstructions - 1, jmpAddress);
	if (instructionIndex != -1)
	{
		struct DisassembledInstruction* jmpInstruction = &(params->allInstructions[instructionIndex]);
		if (instructionIndex != startInstructionIndex && (jmpInstruction->opcode == JMP_FAR || jmpInstruction->opcode == JMP_NEAR))
		{
			return resolveJmpChain(params, instructionIndex);
		}
	}

	return jmpAddress;
}

int findInstructionByAddress(struct DisassembledInstruction* instructions, int low, int high, unsigned long long address)
{
	while (low <= high)
	{
		int mid = low + (high - low) / 2;

		if (instructions[mid].address == address) { return mid; }

		if (instructions[mid].address < address) { low = mid + 1; }
		else { high = mid - 1; }
	}

	return -1;
}

int findAddressInArr(unsigned long long* addresses, int low, int high, unsigned long long address)
{
	while (low <= high)
	{
		int mid = low + (high - low) / 2;

		if (addresses[mid] == address) { return mid; }

		if (addresses[mid] < address) { low = mid + 1; }
		else { high = mid - 1; }
	}

	return -1;
}

unsigned char checkForAddressInArrInRange(unsigned long long* addresses, int low, int high, unsigned long long minAddress, unsigned long long maxAddress)
{
	while (low <= high)
	{
		int mid = low + (high - low) / 2;

		if (addresses[mid] >= minAddress && addresses[mid] <= maxAddress) { return 1; }

		if (addresses[mid] < minAddress) { low = mid + 1; }
		else { high = mid - 1; }
	}

	return 0;
}

static unsigned char operandToValue(struct DecompilationParameters* params, int startInstructionIndex, struct Operand* operand, unsigned long long* result)
{
	if (operand->type == IMMEDIATE)
	{
		*result = operand->immediate.value;
		return 1;
	}
	else if (operand->type == MEM_ADDRESS)
	{
		if (compareRegisters(operand->memoryAddress.reg, IP))
		{
			*result = params->allInstructions[startInstructionIndex + 1].address + operand->memoryAddress.constDisplacement;
			return 1;
		}
		else if (operand->memoryAddress.reg == NO_REG)
		{
			unsigned long long address = operand->memoryAddress.constDisplacement;
			if (!getNumFromData(params, address, result))
			{
				*result = address;
			}
			return 1;
		}
		else
		{
			struct Operand baseReg = { 0 };
			baseReg.type = REGISTER;
			baseReg.reg = operand->memoryAddress.reg;

			unsigned long long regValue = 0;
			if (!operandToValue(params, startInstructionIndex, &baseReg, &regValue))
			{
				return 0;
			}

			unsigned long long address = regValue + operand->memoryAddress.constDisplacement;
			if (!getNumFromData(params, address, result))
			{
				*result = address;
			}
		}

		return 1;
	}
	else if (operand->type == REGISTER)
	{
		for (int i = startInstructionIndex - 1; i >= 0; i--)
		{
			if (params->allInstructions[i].opcode == MOV && compareRegisters(params->allInstructions[i].operands[0].reg, operand->reg))
			{
				return operandToValue(params, i, &(params->allInstructions[i].operands[1]), result);
			}
		}

		return 0;
	}

	return 0;
}

static unsigned char getNumFromData(struct DecompilationParameters* params, unsigned long long address, unsigned long long* result)
{
	if (address < params->imageBase + params->dataSections[0].virtualAddress)
	{
		return 0;
	}

	int dataSectionIndex = -1;
	int totalSize = 0;
	for (int i = 0; i < params->numOfDataSections; i++)
	{
		if (address > params->imageBase + params->dataSections[i].virtualAddress && address < params->imageBase + params->dataSections[i].virtualAddress + params->dataSections[i].size)
		{
			dataSectionIndex = (int)((totalSize + address) - (params->dataSections[i].virtualAddress + params->imageBase));
		}

		totalSize += params->dataSections[i].size;
	}

	if (dataSectionIndex == -1 || dataSectionIndex >= totalSize)
	{
		return 0;
	}

	if (params->is64Bit)
	{
		*result = *(unsigned long long*)(params->dataSectionByte + dataSectionIndex);
	}
	else
	{
		*result = *(unsigned int*)(params->dataSectionByte + dataSectionIndex);
	}

	return 1;
}
