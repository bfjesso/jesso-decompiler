#include "disassembler.h"
#include "disassemblyUtils.h"
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

unsigned long long getJumpTableAddress(struct DisassembledInstruction* instructions, int numOfInstructions, unsigned char* size)
{
	if (numOfInstructions < 2) 
	{
		return 0;
	}
	
	struct DisassembledInstruction* jmpInstruction = &instructions[numOfInstructions - 1];

	if (jmpInstruction->opcode != JMP_NEAR)
	{
		return 0;
	}

	if (jmpInstruction->operands[0].type == REGISTER)
	{
		unsigned long long result = 0;
		
		enum Register targetReg = jmpInstruction->operands[0].reg;
		int i = numOfInstructions - 2;
		struct DisassembledInstruction* instruction = &instructions[i];
		while (!isOpcodeJcc(instruction->opcode) && !isOpcodeReturn(instruction->opcode) && i >= 0)
		{
			if (instruction->operands[0].type == REGISTER && compareRegisters(targetReg, instruction->operands[0].reg)) 
			{
				if (instruction->opcode == ADD) 
				{
					unsigned long long val = 0;
					if (operandToValue(instructions, i, 0, &instruction->operands[1], &val)) 
					{
						result += val;
					}
				}
				else if ((instruction->opcode == MOV || instruction->opcode == LEA) && instruction->operands[1].type == MEM_ADDRESS && instruction->operands[1].memoryAddress.scale > 1)
				{
					unsigned long long regDisplacementVal = 0;
					if (regToValue(instructions, numOfInstructions - 2, 0, jmpInstruction->operands[1].memoryAddress.regDisplacement, &regDisplacementVal))
					{
						result += regDisplacementVal;
					}

					*size = instruction->operands[1].memoryAddress.ptrSize;

					return result + instruction->operands[1].memoryAddress.constDisplacement;
				}
			}

			i--;
			instruction = &instructions[i];
		}
	}
	else if (jmpInstruction->operands[0].type == MEM_ADDRESS && jmpInstruction->operands[0].memoryAddress.scale > 1)
	{
		unsigned long long result = jmpInstruction->operands[0].memoryAddress.constDisplacement;

		unsigned long long regDisplacementVal = 0;
		if (regToValue(instructions, numOfInstructions - 2, 0, jmpInstruction->operands[0].memoryAddress.regDisplacement, &regDisplacementVal)) 
		{
			result += regDisplacementVal;
		}

		*size = jmpInstruction->operands[0].memoryAddress.ptrSize;

		return result;
	}

	return 0;
}

unsigned long long getIndirectTableAddress(struct DisassembledInstruction* instructions, int numOfInstructions)
{
	struct DisassembledInstruction* instruction = &instructions[numOfInstructions - 1];

	if (instruction->opcode == MOVZX && instruction->operands[1].type == MEM_ADDRESS)
	{
		unsigned long long result = instruction->operands[1].memoryAddress.constDisplacement;
		
		unsigned long long regDisplacementVal = 0;
		if (regToValue(instructions, numOfInstructions - 2, 0, instruction->operands[1].memoryAddress.regDisplacement, &regDisplacementVal))
		{
			result += regDisplacementVal;
		}

		return result;
	}

	return 0;
}