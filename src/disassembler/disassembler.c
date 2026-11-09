#include "disassembler.h"
#include "x86/disassemblerX86.h"
#include "lc3/disassemblerLC3.h"

enum JdcStatus disassembleInstruction(enum InstructionSetArchitecture isa, uint8_t* bytes, uint8_t* maxBytesAddr, struct DisassemblerOptions* disassemblerOptions, struct DisassembledInstruction* result)
{
	switch (isa)
	{
	case X86_ISA:
		return disassembleX86(bytes, maxBytesAddr, disassemblerOptions, result);
	case LC3_ISA:
		return disassembleLC3(bytes, maxBytesAddr, result);
	}

	return ERROR_JDC;
}

enum JdcStatus instructionToStr(struct DisassembledInstruction* instruction, bool evaluateIP, struct JdcStr* result) // this will be in intel syntax
{
	strcpyJdc(result, "");

	if (instruction->group1Prefix != NONE_LP)
	{
		strcatJdc(result, getGroup1PrefixStr(instruction));
		strcatJdc(result, " ");
	}

	strcatJdc(result, opcodeMnemonicStrs[instruction->opcode]);

	for (int32_t i = 0; i < instruction->numOfOperands; i++)
	{
		if (i != 0)
		{
			strcatJdc(result, ", ");
		}
		else
		{
			strcatJdc(result, " ");
		}

		struct Operand* currentOperand = &instruction->operands[i];
		if (currentOperand->type == REGISTER_OPERAND)
		{
			if (evaluateIP && compareRegisters(currentOperand->reg, IP_X86_REG))
			{
				sprintfJdc(result, true, "0x%llX", instruction->address + instruction->numOfBytes);
			}
			else
			{
				strcatJdc(result, registerStrs[currentOperand->reg]);
			}
		}
		else if (currentOperand->type == MEMORY_OPERAND)
		{
			if (memAddressToStr(instruction, &currentOperand->memoryAddress, evaluateIP, result) == ERROR_JDC)
			{
				return ERROR_JDC;
			}
		}
		else if (currentOperand->type == IMMEDIATE_OPERAND)
		{
			sprintfJdc(result, true, "0x%llX", currentOperand->immediate.value);
		}

		if (i == 0 && instruction->opmask != NO_REG)
		{
			sprintfJdc(result, true, " {%s}", registerStrs[instruction->opmask]);
			if (instruction->zeroMasking)
			{
				strcatJdc(result, "{Z}");
			}
		}
	}

	return SUCCESS_JDC;
}

static enum JdcStatus memAddressToStr(struct DisassembledInstruction* instruction, struct MemoryAddress* memAddr, bool evaluateIP, struct JdcStr* result)
{
	if (memAddr->ptrSize != 0)
	{
		strcatJdc(result, getPtrSizeStr(memAddr->ptrSize));
		strcatJdc(result, " ");
	}

	if (memAddr->regSegment != NO_REG)
	{
		strcatJdc(result, registerStrs[memAddr->regSegment]);
		strcatJdc(result, ":");
	}
	else if (memAddr->constSegment != 0)
	{
		sprintfJdc(result, true, "0x%04X", memAddr->constSegment);
		strcatJdc(result, ":");
	}

	strcatJdc(result, "[");

	uint64_t totalDisplacement = 0;
	bool gotFirstTerm = false;

	if (evaluateIP && compareRegisters(memAddr->reg, IP_X86_REG))
	{
		totalDisplacement = (instruction->address + instruction->numOfBytes) * memAddr->scale;
	}
	else if (memAddr->reg != NO_REG)
	{
		strcatJdc(result, registerStrs[memAddr->reg]);
		if (memAddr->scale > 1)
		{
			sprintfJdc(result, true, " * 0x%X", memAddr->scale);
		}

		gotFirstTerm = true;
	}

	if (evaluateIP && compareRegisters(memAddr->regDisplacement, IP_X86_REG))
	{
		totalDisplacement += instruction->address + instruction->numOfBytes;
	}
	else if (memAddr->regDisplacement != NO_REG)
	{
		if (gotFirstTerm)
		{
			strcatJdc(result, " + ");
		}

		strcatJdc(result, registerStrs[memAddr->regDisplacement]);
		gotFirstTerm = true;
	}

	if (gotFirstTerm)
	{
		if (totalDisplacement != 0 || memAddr->constDisplacement > 0)
		{
			sprintfJdc(result, true, " + 0x%llX", totalDisplacement + memAddr->constDisplacement);
		}
		else if (memAddr->constDisplacement < 0)
		{
			sprintfJdc(result, true, " - 0x%llX", -memAddr->constDisplacement);
		}
	}
	else
	{
		sprintfJdc(result, true, "0x%llX", totalDisplacement + memAddr->constDisplacement);
	}

	strcatJdc(result, "]");
	return SUCCESS_JDC;
}

const char* getGroup1PrefixStr(struct DisassembledInstruction* instruction)
{
	if (instruction->group1Prefix == REPNZ_BND_LP &&
		(instruction->opcode == CALL_NEAR_X86 || instruction->opcode == RET_NEAR_X86 || instruction->opcode == JMP_NEAR_X86 || isOpcodeJcc(instruction->opcode)))
	{
		return "BND";
	}

	switch (instruction->group1Prefix)
	{
	case LOCK_LP:
		return "LOCK";
	case REPNZ_BND_LP:
		return "REPNZ";
	case REPZ_LP:
		return "REP";
	}

	return "";
}

const char* getPtrSizeStr(int32_t ptrSize)
{
	switch (ptrSize)
	{
	case 1:
		return "BYTE PTR";
	case 2:
		return "WORD PTR";
	case 4:
		return "DWORD PTR";
	case 6:
		return "FWORD PTR";
	case 8:
		return "QWORD PTR";
	case 10:
		return "TBYTE PTR";
	case 16:
		return "XMMWORD PTR";
	case 32:
		return "YMMWORD PTR";
	case 64:
		return "ZMMWORD PTR";
	}

	return "";
}

enum JdcStatus generateDataInstruction(struct DisassembledInstruction* instruction, uint64_t address, uint64_t data, uint8_t dataSize)
{
	memset(instruction, 0, sizeof(struct DisassembledInstruction));
	instruction->opcode = DATA_OPCODE;
	instruction->numOfBytes = dataSize;

	instruction->address = address;
	instruction->operands = (struct Operand*)calloc(1, sizeof(struct Operand));
	if (!instruction->operands)
	{
		return ERROR_JDC;
	}

	instruction->operands[0].type = IMMEDIATE_OPERAND;
	instruction->operands[0].immediate.value = data;
	instruction->operands[0].immediate.size = dataSize;
	instruction->numOfOperands = 1;

	return SUCCESS_JDC;
}