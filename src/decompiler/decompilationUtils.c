#include "decompilationUtils.h"
#include "functions.h"
#include "functionCalls.h"
#include "../disassembler/disassemblyUtils.h"

void addIndents(struct JdcStr* result, int numOfIndents)
{
	for (int i = 0; i < numOfIndents; i++)
	{
		strcatJdc(result, "\t");
	}
}

unsigned long long resolveJmpChain(struct DecompilationParameters* params, int startInstructionIndex)
{
	struct DisassembledInstruction* instruction = &params->instructions[startInstructionIndex];
	if (!isOpcodeJmp(instruction->opcode) && !isOpcodeJcc(instruction->opcode) && !isOpcodeCall(instruction->opcode)) 
	{
		return 0;
	}

	unsigned long long jmpAddress = 0;
	if (!operandToValue(params->instructions, startInstructionIndex, params->currentFunc ? params->currentFunc->firstInstructionIndex : startInstructionIndex - 0x1000, &instruction->operands[0], &jmpAddress))
	{
		return 0;
	}

	if (instruction->opcode != JMP_FAR && instruction->opcode != CALL_FAR && instruction->operands[0].type == IMMEDIATE)
	{
		jmpAddress += params->instructions[startInstructionIndex + 1].address;
	}

	int instructionIndex = findInstructionByAddress(params->instructions, 0, params->numOfInstructions - 1, jmpAddress);
	if (instructionIndex != -1)
	{
		struct DisassembledInstruction* jmpInstruction = &(params->instructions[instructionIndex]);
		if (instructionIndex != startInstructionIndex && isOpcodeJmp(jmpInstruction->opcode))
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

unsigned char doesInstructionAccessRegister(struct DecompilationParameters* params, int instructionIndex, enum Register reg, enum Register* specificReg, struct DataType* dataTypeRef)
{
	struct Function* callee;
	if (checkForKnownFunctionCall(params, instructionIndex, &callee) && callee)
	{
		struct RegisterVariable* regArg = getRegArgByReg(callee, reg);
		if (regArg)
		{
			if (specificReg)
			{
				*specificReg = regArg->reg;
			}

			if (dataTypeRef) 
			{ 
				*dataTypeRef = regArg->dataType; 
			}

			return 1;
		}
	}
	
	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];
	for (int i = 0; i < 4; i++)
	{
		struct Operand* op = &(instruction->operands[i]);
		if (op->type == MEM_ADDRESS)
		{
			if (compareRegisters(op->memoryAddress.reg, reg))
			{
				if (specificReg)
				{
					*specificReg = op->memoryAddress.reg;
				}

				if (dataTypeRef) 
				{ 
					*dataTypeRef = getOperandDataType(instruction->opcode, op);
					dataTypeRef->pointerLevel = 1;
				}

				return 1;
			}
			else if (compareRegisters(op->memoryAddress.regDisplacement, reg))
			{
				if (specificReg)
				{
					*specificReg = op->memoryAddress.regDisplacement;
				}

				if (dataTypeRef)
				{
					*dataTypeRef = getOperandDataType(instruction->opcode, op);
					dataTypeRef->pointerLevel = 1;
				}

				return 1;
			}
		}
		else if (!doesInstructionModifyOperand(instruction, i, 0, 0) && op->type == REGISTER && compareRegisters(op->reg, reg))
		{
			if (specificReg)
			{
				*specificReg = op->reg;
			}

			if (dataTypeRef)
			{
				*dataTypeRef = getOperandDataType(instruction->opcode, op);
			}

			return 1;
		}
	}

	return 0;
}

unsigned char doesInstructionModifyRegister(struct DecompilationParameters* params, int instructionIndex, enum Register reg, unsigned char* regOperandNum, unsigned char* srcOperandNum, unsigned char* overwrites)
{
	struct Function* callee;
	if ((checkForKnownFunctionCall(params, instructionIndex, &callee) && callee && compareRegisters(callee->returnReg, reg)) ||
		(checkForUnknownFunctionCall(params, instructionIndex) && compareRegisters(reg, AX)))
	{
		if (overwrites) { *overwrites = 1; }
		return 1;
	}

	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];

	if (instruction->opcode == POP && instruction->operands[0].type == REGISTER && compareRegisters(instruction->operands[0].reg, reg))
	{
		int stackOffset = 0;
		for (int i = instructionIndex; i >= params->currentFunc->firstInstructionIndex; i--) 
		{
			if (params->instructions[i].opcode == PUSH) 
			{
				stackOffset++;
				if (stackOffset == 0)
				{
					if (params->instructions[i].operands[0].type == REGISTER && instruction->operands[0].reg == params->instructions[i].operands[0].reg)
					{
						return 0;
					}

					if (overwrites) { *overwrites = 1; }
					return 1;
				}
			}
			else if (params->instructions[i].opcode == POP)
			{
				stackOffset--;
			}
		}

		return 0;
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