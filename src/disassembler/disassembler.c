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
	"REPZ"
};

extern const char* ptrSizeStrs[] =
{
	"BYTE PTR",
	"WORD PTR",
	"DWORD PTR",
	"FWORD PTR",
	"QWORD PTR",
	"TBYTE PTR"
};

unsigned char disassembleInstruction(unsigned char* bytes, unsigned char* maxBytesAddr, struct DisassemblerOptions* disassemblerOptions, struct DisassembledInstruction* result, unsigned char* numOfBytes)
{
	unsigned char* startPoint = bytes;

	struct LegacyPrefixes legacyPrefixes = { NO_PREFIX, NO_PREFIX, NO_PREFIX, NO_PREFIX };
	if (!handleLegacyPrefixes(&bytes, maxBytesAddr, &legacyPrefixes))
	{
		return 0;
	}

	struct REXPrefix rexPrefix = { 0 };
	if (disassemblerOptions->is64BitMode && !handleREXPrefix(&bytes, maxBytesAddr, &rexPrefix))
	{
		return 0;
	}

	struct VEXPrefix vexPrefix = { 0 };
	if (disassemblerOptions->is64BitMode && !rexPrefix.isValidREX && !handleVEXPrefix(&bytes, maxBytesAddr, &vexPrefix))
	{
		return 0;
	}

	// if the opcode is an extended one then modRM will be retrieved
	unsigned char modRMByte = 0;
	char hasGotModRM = 0;
	struct Opcode opcode = { NO_MNEMONIC, -1, 0, 0, 0, 0, 0 };
	if (!handleOpcode(&bytes, maxBytesAddr, &hasGotModRM, &modRMByte, disassemblerOptions, &legacyPrefixes, &rexPrefix, &opcode))
	{
		return 0;
	}

	if (!handleOperands(&bytes, maxBytesAddr, startPoint, hasGotModRM, &modRMByte, disassemblerOptions->is64BitMode, &opcode, &legacyPrefixes, &rexPrefix, &vexPrefix, (struct Operand*)(&result->operands)))
	{
		return 0;
	}

	result->opcode = opcode.mnemonic;
	result->group1Prefix = legacyPrefixes.group1;
	result->isInvalid = (disassemblerOptions->is64BitMode && opcode.opcodeSuperscript == i64) || (!disassemblerOptions->is64BitMode && opcode.opcodeSuperscript == o64);

	if (numOfBytes) 
	{
		*numOfBytes = (unsigned char)(bytes - startPoint);
	}

	return 1;
}

unsigned char instructionToStr(struct DisassembledInstruction* instruction, char* buffer, unsigned char bufferSize) // this will be in intel syntax
{
	if (instruction->group1Prefix != NO_PREFIX) 
	{
		strcpy(buffer, group1PrefixStrs[instruction->group1Prefix - LOCK]);
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
			sprintf(immediateBuffer, "0x%llX", currentOperand->immediate);
			strcat(buffer, immediateBuffer);
			break;
		}
	}

	return strlen(buffer) < bufferSize;
}

static unsigned char memAddressToStr(struct MemoryAddress* memAddr, char* buffer, unsigned char bufferSize)
{
	if (memAddr->ptrSize != 0 && memAddr->ptrSize <= 10)
	{
		strcat(buffer, ptrSizeStrs[memAddr->ptrSize / 2]);
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

unsigned char doesInstructionModifyOperand(struct DisassembledInstruction* instruction, unsigned char operandNum, unsigned char* overwrites)
{
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
			instruction->opcode == STMXCSR)
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
			isOpcodeDiv(instruction->opcode))
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
