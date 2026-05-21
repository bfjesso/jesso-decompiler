#include "decompilationUtils.h"
#include "functionCalls.h"
#include "../disassembler/disassemblyUtils.h"

void addIndents(struct JdcStr* result, int numOfIndents)
{
	for (int i = 0; i < numOfIndents; i++)
	{
		strcatJdc(result, "\t");
	}
}

unsigned long long resolveJmpChain(struct DecompilationParameters* params)
{
	struct DisassembledInstruction* instruction = &params->instructions[params->startInstructionIndex];
	int ogStartInstructionIndex = params->startInstructionIndex;

	unsigned long long jmpAddress = 0;
	if (!operandToValue(params->instructions, params->startInstructionIndex, params->currentFunc ? params->currentFunc->firstInstructionIndex : 0, &instruction->operands[0], &jmpAddress))
	{
		params->startInstructionIndex = ogStartInstructionIndex;
		return 0;
	}

	if (instruction->operands[0].type == IMMEDIATE) 
	{
		jmpAddress += instruction->address;
	}

	int instructionIndex = findInstructionByAddress(params->instructions, 0, params->numOfInstructions - 1, jmpAddress);
	if (instructionIndex != -1)
	{
		struct DisassembledInstruction* jmpInstruction = &(params->instructions[instructionIndex]);
		if (instructionIndex != params->startInstructionIndex && isOpcodeJmp(jmpInstruction->opcode))
		{
			params->startInstructionIndex = instructionIndex;
			unsigned long long result =  resolveJmpChain(params);
			params->startInstructionIndex = ogStartInstructionIndex;
			return result;
		}
	}

	params->startInstructionIndex = ogStartInstructionIndex;
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

unsigned char doesInstructionModifyRegister(struct DecompilationParameters* params, struct DisassembledInstruction* instruction, enum Register reg, unsigned char* regOperandNum, unsigned char* srcOperandNum, unsigned char* overwrites)
{
	struct Function* callee;
	if ((checkForKnownFunctionCall(params, &callee) && callee && compareRegisters(callee->returnReg, reg)) ||
		(checkForUnknownFunctionCall(params) && compareRegisters(reg, AX)))
	{
		if (overwrites) { *overwrites = 1; }
		return 1;
	}
	
	if (compareRegisters(reg, AX)) // some opcodes may modify a register even if it isn't an operand
	{
		switch (instruction->opcode)
		{
		case IDIV:
			return 1;
		}

		if (instruction->opcode == IMUL && instruction->operands[1].type == NO_OPERAND)
		{
			return 1;
		}
	}
	else if (compareRegisters(reg, DX))
	{
		if (instruction->opcode == IMUL && instruction->operands[1].type == NO_OPERAND)
		{
			if (overwrites) { *overwrites = 1; }
			return 1;
		}
	}
	else if (compareRegisters(reg, ST0))
	{
		switch (instruction->opcode)
		{
		case FLD:
			if (overwrites) { *overwrites = 1; }
			return 1;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		struct Operand* op = &(instruction->operands[i]);
		if (op->type == REGISTER && compareRegisters(op->reg, reg))
		{
			if (doesInstructionModifyOperand(instruction, i, srcOperandNum, overwrites))
			{
				if (regOperandNum != 0) { *regOperandNum = i; }
				return 1;
			}
		}
	}

	return 0;
}