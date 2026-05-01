#include "disassemblyUtils.h"

unsigned char doesInstructionModifyOperand(struct DisassembledInstruction* instruction, unsigned char operandNum, unsigned char* srcOperandNum, unsigned char* overwrites)
{
	if (overwrites != 0) 
	{
		*overwrites = 0;
	}

	if (srcOperandNum != 0) 
	{
		for (int i = 3; i >= 0; i--) 
		{
			if (instruction->operands[i].type != NO_OPERAND) 
			{
				*srcOperandNum = i;
				break;
			}
		}
	}

	if (operandNum == 0)
	{
		if (isOpcodeXor(instruction->opcode) && compareOperands(&instruction->operands[0], &instruction->operands[1]))
		{
			if (overwrites != 0) { *overwrites = 1; }
			return 1;
		}
		else if (isOpcodeOr(instruction->opcode) && instruction->operands[1].type == IMMEDIATE)
		{
			if (overwrites != 0) { *overwrites = isImmediateAllOnes(&instruction->operands[1].immediate); }
			return 1;
		}

		if (instruction->opcode == IMUL)
		{
			if (instruction->operands[2].type != NO_OPERAND) 
			{
				if (overwrites != 0)
				{
					*overwrites = !compareOperands(&instruction->operands[0], &instruction->operands[1]);
				}
				return 1;
			}
			else if (instruction->operands[1].type == NO_OPERAND) 
			{
				return 0;
			}
		}
		
		if (isOpcodeMov(instruction->opcode) || 
			instruction->opcode == LEA || 
			instruction->opcode == POP || 
			isOpcodeCvtToDbl(instruction->opcode) ||
			isOpcodeCvtToFlt(instruction->opcode) ||
			isOpcodeCMOVcc(instruction->opcode) || 
			isOpcodeSETcc(instruction->opcode) ||
			instruction->opcode == STMXCSR ||
			isOpcodeAES(instruction->opcode) ||
			isOpcodeShuf(instruction->opcode) ||
			instruction->opcode == XCHG)
		{
			if (overwrites != 0) { *overwrites = 1; }
			return 1;
		}

		if (isOpcodeAdd(instruction->opcode) ||
			isOpcodeSub(instruction->opcode) ||
			isOpcodeAnd(instruction->opcode) ||
			isOpcodeOr(instruction->opcode) ||
			isOpcodeXor(instruction->opcode) ||
			isOpcodeShl(instruction->opcode) ||
			isOpcodeShr(instruction->opcode) ||
			instruction->opcode == IMUL || instruction->opcode == IDIV ||
			instruction->opcode == INC || instruction->opcode == DEC || instruction->opcode == NEG)
		{
			return 1;
		}
	}
	else if (operandNum == 1) 
	{
		if (instruction->opcode == XCHG) 
		{
			if (overwrites != 0) { *overwrites = 1; }
			return 1;
		}
		else if (isOpcodeXor(instruction->opcode))
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

unsigned char doesInstructionAccessRegister(struct DisassembledInstruction* instruction, enum Register reg, unsigned char* regOperandNum) // this returns 1 if it only reads the register, not writes
{
	for (int i = 0; i < 4; i++)
	{
		if (regOperandNum != 0)
		{
			*regOperandNum = i;
		}
		
		struct Operand* op = &(instruction->operands[i]);
		if (op->type == MEM_ADDRESS && (compareRegisters(op->memoryAddress.reg, reg) || compareRegisters(op->memoryAddress.regDisplacement, reg)))
		{
			return 1;
		}
		else if (!doesInstructionModifyOperand(instruction, i, 0, 0) && op->type == REGISTER && compareRegisters(op->reg, reg))
		{
			return 1;
		}
	}

	return 0;
}

unsigned char doesInstructionModifyRegister(struct DisassembledInstruction* instruction, enum Register reg, unsigned char* regOperandNum, unsigned char* srcOperandNum, unsigned char* overwrites)
{
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
				if(regOperandNum != 0) { *regOperandNum = i; }
				return 1;
			}
		}
	}

	return 0;
}

unsigned char doesInstructionModifyZF(struct DisassembledInstruction* instruction)
{
	return !isOpcodeMov(instruction->opcode) && instruction->opcode != LEA && !isOpcodeAES(instruction->opcode) && doesInstructionModifyOperand(instruction, 0, 0, 0); // this isn't a full check
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

	return 0;
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