#include "disassemblyUtils.h"
#include "operands.h"

unsigned char doesInstructionModifyOperand(struct DisassembledInstruction* instruction, unsigned char operandNum, unsigned char* overwrites)
{
	if (overwrites != 0) 
	{
		*overwrites = 0;
	}

	if (instruction->group1Prefix != NO_PREFIX) // REPZ instructions are decompiled as void intrinsics and I have not handled the other prefixes yet
	{
		return 0;
	}

	enum Mnemonic opcode = instruction->opcode;

	if (operandNum == 0)
	{
		if (isOpcodeXor(opcode) && compareOperands(&instruction->operands[0], &instruction->operands[1]))
		{
			if (overwrites != 0) { *overwrites = 1; }
			return 1;
		}
		else if (isOpcodeOr(opcode) && instruction->operands[1].type == IMMEDIATE)
		{
			if (overwrites != 0) { *overwrites = isImmediateAllOnes(&instruction->operands[1].immediate); }
			return 1;
		}

		if (opcode == IMUL)
		{
			if (instruction->numOfOperands == 3) 
			{
				if (overwrites != 0)
				{
					*overwrites = !compareOperands(&instruction->operands[0], &instruction->operands[1]);
				}
				return 1;
			}
			else if (instruction->numOfOperands == 2)
			{
				return 1;
			}
			else if (instruction->numOfOperands == 1) 
			{
				return 0;
			}
		}
		
		if (doesOpcodeOverwriteFirstOperand(opcode))
		{
			if (overwrites != 0) { *overwrites = 1; }
			return 1;
		}
		else if (doesOpcodeModifyFirstOperand(opcode))
		{
			return 1;
		}
	}
	else if (operandNum == 1) 
	{
		if (opcode == XCHG) 
		{
			if (overwrites != 0) { *overwrites = 1; }
			return 1;
		}
		else if (isOpcodeXor(opcode))
		{
			if (compareOperands(&instruction->operands[0], &instruction->operands[1]))
			{
				if (overwrites != 0) { *overwrites = 1; }
				return 1;
			}
			return 0;
		}
	}

	return 0;
}

unsigned char doesInstructionModifyZF(struct DisassembledInstruction* instruction)
{
	return !isOpcodeMov(instruction->opcode) && instruction->opcode != LEA && doesInstructionModifyOperand(instruction, 0, 0); // this isn't a full check
}

unsigned char doesInstructionDoNothing(struct DisassembledInstruction* instruction)
{
	if (instruction->opcode == NOP)
	{
		return 1;
	}
	else if (isOpcodeMov(instruction->opcode) && compareOperands(&instruction->operands[0], &instruction->operands[1]))
	{
		return 1;
	}
	else if (instruction->opcode == LEA &&
		instruction->operands[0].type == REGISTER && instruction->operands[1].type == MEM_ADDRESS &&
		compareRegisters(instruction->operands[0].reg, instruction->operands[1].memoryAddress.reg) &&
		instruction->operands[1].memoryAddress.constDisplacement == 0 && instruction->operands[1].memoryAddress.regDisplacement == NO_REG && instruction->operands[1].memoryAddress.scale == 1)
	{
		return 1;
	}
	else if ((isOpcodeAdd(instruction->opcode) || isOpcodeSub(instruction->opcode)) && instruction->operands[1].type == IMMEDIATE && instruction->operands[1].immediate.value == 0)
	{
		return 1;
	}
	else if ((instruction->opcode == JMP_NEAR || instruction->opcode == JMP_SHORT) && instruction->operands[0].type == IMMEDIATE && instruction->operands[0].immediate.value == 0)
	{
		return 1;
	}

	return 0;
}

unsigned char doesInstructionGenerateInterruptOrException(struct DisassembledInstruction* instruction)
{
	switch (instruction->opcode)
	{
	case INT3:
	case _INT:
	case HLT:
	case UD0:
	case UD1:
	case UD2:
	case DATA:
		return 1;
	}

	return instruction->isInvalid;
}

unsigned char isImmediateAllOnes(struct Immediate* immediate) 
{
	switch (immediate->size)
	{
	case 1:
		return immediate->value == 0xFF;
	case 2:
		return immediate->value == 0xFFFF;
	case 4:
		return immediate->value == 0xFFFFFFFF;
	case 8:
		return immediate->value == 0xFFFFFFFFFFFFFFFF;
	}

	return 0;
}