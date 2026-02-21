#include "disassembler.h"
#include "prefixes.h"
#include "opcodes.h"
#include "operands.h"

#include <stdio.h>
#include <string.h>

static const char* group1PrefixStrs[] =
{
	"LOCK",
	"REPNZ",
	"REP"
};

static const char* ptrSizeStrs[] =
{
	"BYTE PTR",
	"WORD PTR",
	"DWORD PTR",
	"FWORD PTR",
	"QWORD PTR",
	"TBYTE PTR",
	"XMMWORD PTR",
	"YMMWORD PTR",
	"ZMMWORD PTR"
};

unsigned char disassembleInstruction(unsigned char* bytes, unsigned char* maxBytesAddr, struct DisassemblerOptions* disassemblerOptions, struct DisassembledInstruction* result, unsigned char* numOfBytes)
{
	struct DisassemblyParameters params = { 0 };
	params.bytes = bytes;
	params.maxBytesAddr = maxBytesAddr;
	params.startBytePtr = bytes;
	params.is64BitMode = disassemblerOptions->is64BitMode;
	
	if (!handleLegacyPrefixes(&params))
	{
		return 0;
	}

	if (params.is64BitMode) 
	{
		if (!handleREXPrefix(&params))
		{
			return 0;
		}

		if (!params.rexPrefix.isValidREX && !handleVEXPrefix(&params))
		{
			return 0;
		}

		if (!params.rexPrefix.isValidREX && !params.vexPrefix.isValidVEX && !handleEVEXPrefix(&params))
		{
			return 0;
		}
	}

	// if the opcode is an extended one then modRM will be retrieved
	if (!handleOpcode(&params))
	{
		return 0;
	}

	if (!handleOperands(&params, (struct Operand*)(&result->operands)))
	{
		return 0;
	}

	result->opcode = params.opcode.mnemonic;
	result->group1Prefix = params.legPrefixes.group1;
	result->isInvalid = (disassemblerOptions->is64BitMode && params.opcode.opcodeSuperscript == i64) || (!disassemblerOptions->is64BitMode && params.opcode.opcodeSuperscript == o64);

	if (numOfBytes) 
	{
		*numOfBytes = (unsigned char)(params.bytes - params.startBytePtr);
	}

	return 1;
}

unsigned char instructionToStr(struct DisassembledInstruction* instruction, char* buffer, unsigned char bufferSize) // this will be in intel syntax
{
	if (instruction->group1Prefix != NO_PREFIX) 
	{
		strcpy(buffer, getGroup1PrefixStr(instruction->group1Prefix));
		strcat(buffer, " ");
	}

	strcat(buffer, mnemonicStrs[instruction->opcode]);

	for (int i = 0; i < 4; i++) 
	{
		if (instruction->operands[i].type == NO_OPERAND) { continue; }

		if (i != 0) 
		{
			strcat(buffer, ", ");
		}
		else 
		{
			strcat(buffer, " ");
		}

		struct Operand* currentOperand = &instruction->operands[i];

		char immediateBuffer[20] = { 0 };
		switch (currentOperand->type)
		{
		case SEGMENT:
			strcat(buffer, segmentStrs[currentOperand->segment]);
			break;
		case REGISTER:
			strcat(buffer , registerStrs[currentOperand->reg]);
			break;
		case MEM_ADDRESS:
			if (!memAddressToStr(&currentOperand->memoryAddress, buffer, bufferSize)) { return 0; }
			break;
		case IMMEDIATE:
			sprintf(immediateBuffer, "0x%llX", currentOperand->immediate.value);
			strcat(buffer, immediateBuffer);
			break;
		}
	}

	return strlen(buffer) < bufferSize;
}

static unsigned char memAddressToStr(struct MemoryAddress* memAddr, char* buffer, unsigned char bufferSize)
{
	if (memAddr->ptrSize != 0)
	{
		strcat(buffer, getPtrSizeStr(memAddr->ptrSize));
		strcat(buffer, " ");
	}
	
	if (memAddr->segment != NO_SEGMENT) 
	{
		strcat(buffer, segmentStrs[memAddr->segment]);
		strcat(buffer, ":");
	}
	else if (memAddr->constSegment != 0) 
	{
		char hexSeg[20] = { 0 };
		sprintf(hexSeg, "0x%X", memAddr->constSegment);

		strcat(buffer, hexSeg);
		strcat(buffer, ":");
	}

	strcat(buffer, "[");

	if (memAddr->reg != NO_REG) 
	{
		strcat(buffer, registerStrs[memAddr->reg]);
	}

	if (memAddr->scale > 1) 
	{
		strcat(buffer, "*");

		char hexScale[20] = { 0 };
		sprintf(hexScale, "0x%X", memAddr->scale);

		strcat(buffer, hexScale);
	}

	if (memAddr->regDisplacement != NO_REG)
	{
		strcat(buffer, "+");
		strcat(buffer, registerStrs[memAddr->regDisplacement]);
	}

	if (memAddr->constDisplacement != 0) 
	{
		char constDisp[20] = { 0 };
		if (memAddr->constDisplacement < 0) 
		{
			sprintf(constDisp, "-0x%llX", -memAddr->constDisplacement);
		}
		else 
		{
			sprintf(constDisp, "0x%llX", memAddr->constDisplacement);
		}

		if (buffer[strlen(buffer) - 1] != '[' && memAddr->constDisplacement >= 0)
		{
			strcat(buffer, "+");
		}

		strcat(buffer, constDisp);
	}

	strcat(buffer, "]");

	return strlen(buffer) < bufferSize;
}

const char* getPtrSizeStr(int ptrSize) 
{
	return ptrSizeStrs[ptrSize <= 10 ? ptrSize / 2 : ptrSize == 16 ? 6 : ptrSize == 32 ? 7 : ptrSize == 64 ? 8 : 0];
}

const char* getGroup1PrefixStr(enum LegacyPrefix prefix)
{
	return group1PrefixStrs[prefix - LOCK];
}

unsigned long long getJumpTableAddress(struct DisassembledInstruction* instructions, int numOfInstructions)
{
	struct DisassembledInstruction* currentInstruction = &instructions[numOfInstructions - 1];

	if (currentInstruction->opcode == JMP_NEAR && currentInstruction->operands[0].type == REGISTER)
	{
		enum Register targetReg = currentInstruction->operands[0].reg;
		int i = numOfInstructions - 2;
		struct DisassembledInstruction* instruction = &instructions[i];
		while (!isOpcodeJcc(instruction->opcode) && !isOpcodeReturn(instruction->opcode))
		{
			if (instruction->opcode == MOV && instruction->operands[0].type == REGISTER && compareRegisters(targetReg, instruction->operands[0].reg) && instruction->operands[1].type == MEM_ADDRESS && instruction->operands[1].memoryAddress.scale > 1)
			{
				return instruction->operands[1].memoryAddress.constDisplacement;
			}

			i--;
			instruction = &instructions[i];
		}
	}

	return 0;
}

unsigned char doesInstructionModifyOperand(struct DisassembledInstruction* instruction, unsigned char operandNum, unsigned char* overwrites)
{
	if (overwrites != 0) 
	{
		*overwrites = 0;
	}
	
	if (instruction->operands[operandNum].type == REGISTER && compareRegisters(instruction->operands[operandNum].reg, AX)) // some opcodes may modify a register even if it isn't an operand
	{
		switch (instruction->opcode)
		{
		case IDIV:
			return 1;
		}
	}

	if (operandNum == 0)
	{
		if (isOpcodeXor(instruction->opcode) && areOperandsEqual(&instruction->operands[0], &instruction->operands[1]))
		{
			if (overwrites != 0) { *overwrites = 1; }
			return 1;
		}
		else if (isOpcodeOr(instruction->opcode) && instruction->operands[1].type == IMMEDIATE && instruction->operands[1].immediate.value == 0xFF)
		{
			if (overwrites != 0) { *overwrites = 1; }
			return 1;
		}

		if (instruction->opcode == IMUL && instruction->operands[2].type != NO_OPERAND)
		{
			if (overwrites != 0) { *overwrites = 1; }
			return 1;
		}
		
		if (isOpcodeMov(instruction->opcode) || 
			instruction->opcode == LEA || 
			isOpcodeCvtToDbl(instruction->opcode) ||
			isOpcodeCvtToFlt(instruction->opcode) ||
			isOpcodeCMOVcc(instruction->opcode) || 
			instruction->opcode == STMXCSR ||
			isOpcodeAES(instruction->opcode))
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
			isOpcodeMul(instruction->opcode) ||
			isOpcodeDiv(instruction->opcode) || 
			instruction->opcode == INC || instruction->opcode == DEC || instruction->opcode == NEG)
		{
			return 1;
		}
	}
	else if (operandNum == 1) 
	{
		if (isOpcodeXor(instruction->opcode))
		{
			if (areOperandsEqual(&instruction->operands[0], &instruction->operands[1]))
			{
				if (overwrites != 0) { *overwrites = 1; }
				return 1;
			}
			return 0;
		}
	}

	return 0;
}

unsigned char doesInstructionAccessRegister(struct DisassembledInstruction* instruction, enum Register reg, unsigned char* operandNum)
{
	for (int i = 0; i < 4; i++)
	{
		struct Operand* op = &(instruction->operands[i]);
		if (!doesInstructionModifyOperand(instruction, i, 0)) 
		{
			if (operandNum != 0)
			{
				*operandNum = i;
			}
			
			if (op->type == REGISTER && compareRegisters(op->reg, reg))
			{
				return 1;
			}
			else if (op->type == MEM_ADDRESS && compareRegisters(op->memoryAddress.reg, reg))
			{
				return 1;
			}
		}
	}

	return 0;
}

unsigned char doesInstructionModifyRegister(struct DisassembledInstruction* instruction, enum Register reg, unsigned char* operandNum, unsigned char* overwrites)
{
	for (int i = 0; i < 4; i++)
	{
		struct Operand* op = &(instruction->operands[i]);
		if (op->type == REGISTER && compareRegisters(op->reg, reg))
		{
			if (doesInstructionModifyOperand(instruction, i, overwrites))
			{
				if(operandNum != 0) { *operandNum = i; }
				return 1;
			}
		}
	}

	return 0;
}

unsigned char doesInstructionModifyZF(struct DisassembledInstruction* instruction)
{
	return !isOpcodeMov(instruction->opcode) && instruction->opcode != LEA && !isOpcodeAES(instruction->opcode) && doesInstructionModifyOperand(instruction, 0, 0); // this isn't a full check
}

unsigned char areOperandsEqual(struct Operand* op1, struct Operand* op2)
{
	if (op1->type == op2->type)
	{
		struct MemoryAddress* m1 = &op1->memoryAddress;
		struct MemoryAddress* m2 = &op2->memoryAddress;

		switch (op1->type)
		{
		case NO_OPERAND:
			return 1;
		case SEGMENT:
			return op1->segment == op2->segment;
		case REGISTER:
			return op1->reg == op2->reg;
		case MEM_ADDRESS:
			return m1->reg == m2->reg && m1->constDisplacement == m2->constDisplacement && m1->ptrSize == m2->ptrSize && m1->scale == m2->scale && m1->constSegment == m2->constSegment && m1->regDisplacement == m2->regDisplacement && m1->segment == m2->segment;
		case IMMEDIATE:
			return op1->immediate.value == op2->immediate.value;
		}
	}

	return 0;
}
