#include "disassembler.h"
#include "disassemblyUtils.h"
#include "prefixes.h"
#include "opcodes.h"
#include "operands.h"

unsigned char disassembleInstruction(unsigned char* bytes, unsigned char* maxBytesAddr, struct DisassemblerOptions* disassemblerOptions, struct DisassembledInstruction* result)
{
	struct DisassemblyParameters params = { 0 };
	params.bytes = bytes;
	params.maxBytesAddr = maxBytesAddr;
	params.startBytePtr = bytes;
	params.is64BitMode = disassemblerOptions->is64BitMode;

	memset(result, 0, sizeof(struct DisassembledInstruction));
	
	if (!handleLegacyPrefixes(&params))
	{
		result->numOfBytes = (unsigned char)(params.bytes - params.startBytePtr);
		return 0;
	}

	if (params.is64BitMode) 
	{
		if (!handleREXPrefix(&params))
		{
			result->numOfBytes = (unsigned char)(params.bytes - params.startBytePtr);
			return 0;
		}

		if (!params.rexPrefix.isValidREX && !handleVEXPrefix(&params))
		{
			result->numOfBytes = (unsigned char)(params.bytes - params.startBytePtr);
			return 0;
		}

		if (!params.rexPrefix.isValidREX && !params.vexPrefix.isValidVEX && !handleEVEXPrefix(&params))
		{
			result->numOfBytes = (unsigned char)(params.bytes - params.startBytePtr);
			return 0;
		}
	}

	// if the opcode is an extended one then modRM will be retrieved
	if (!handleOpcode(&params))
	{
		result->numOfBytes = (unsigned char)(params.bytes - params.startBytePtr);
		return 0;
	}

	if (!handleOperands(&params, result))
	{
		result->numOfBytes = (unsigned char)(params.bytes - params.startBytePtr);
		return 0;
	}

	result->opcode = params.opcode.mnemonic;
	result->group1Prefix = params.legPrefixes.group1;
	result->numOfBytes = (unsigned char)(params.bytes - params.startBytePtr);
	result->isInvalid = (disassemblerOptions->is64BitMode && params.opcode.opcodeSuperscript == i64) || (!disassemblerOptions->is64BitMode && params.opcode.opcodeSuperscript == o64);

	return result->numOfBytes != 0;
}

unsigned char instructionToStr(struct DisassembledInstruction* instruction, struct JdcStr* result) // this will be in intel syntax
{
	strcpyJdc(result, "");

	if (instruction->group1Prefix != NO_PREFIX) 
	{
		strcatJdc(result, getGroup1PrefixStr(instruction));
		strcatJdc(result, " ");
	}

	strcatJdc(result, mnemonicStrs[instruction->opcode]);

	for (int i = 0; i < instruction->numOfOperands; i++)
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
		switch (currentOperand->type)
		{
		case SEGMENT:
			strcatJdc(result, segmentStrs[currentOperand->segment]);
			break;
		case REGISTER:
			strcatJdc(result, registerStrs[currentOperand->reg]);
			break;
		case MEM_ADDRESS:
			if (!memAddressToStr(&currentOperand->memoryAddress, result)) { return 0; }
			break;
		case IMMEDIATE:
			sprintfJdc(result, 1, "0x%llX", currentOperand->immediate.value);
			break;
		}
	}

	return 1;
}

static unsigned char memAddressToStr(struct MemoryAddress* memAddr, struct JdcStr* result)
{
	if (memAddr->ptrSize != 0)
	{
		strcatJdc(result, getPtrSizeStr(memAddr->ptrSize));
		strcatJdc(result, " ");
	}
	
	if (memAddr->segment != NO_SEGMENT) 
	{
		strcatJdc(result, segmentStrs[memAddr->segment]);
		strcatJdc(result, ":");
	}
	else if (memAddr->constSegment != 0) 
	{
		sprintfJdc(result, 1, "0x%X", memAddr->constSegment);
		strcatJdc(result, ":");
	}

	strcatJdc(result, "[");

	if (memAddr->reg != NO_REG) 
	{
		strcatJdc(result, registerStrs[memAddr->reg]);
	}

	if (memAddr->scale > 1) 
	{
		sprintfJdc(result, 1, "*0x%X", memAddr->scale);
	}

	if (memAddr->regDisplacement != NO_REG)
	{
		strcatJdc(result, "+");
		strcatJdc(result, registerStrs[memAddr->regDisplacement]);
	}

	if (memAddr->reg != NO_REG)
	{
		if (memAddr->constDisplacement > 0)
		{
			sprintfJdc(result, 1, "+0x%llX", memAddr->constDisplacement);
		}
		else if (memAddr->constDisplacement < 0)
		{
			sprintfJdc(result, 1, "-0x%llX", -memAddr->constDisplacement);
		}
	}
	else
	{
		sprintfJdc(result, 1, "0x%llX", memAddr->constDisplacement);
	}

	strcatJdc(result, "]");

	return 1;
}

const char* getPtrSizeStr(int ptrSize)
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

const char* getGroup1PrefixStr(struct DisassembledInstruction* instruction)
{
	if (instruction->group1Prefix == REPNZ_BND &&
		(instruction->opcode == CALL_NEAR || instruction->opcode == RET_NEAR || instruction->opcode == JMP_NEAR || isOpcodeJcc(instruction->opcode))) 
	{
		return "BND";
	}

	switch (instruction->group1Prefix) 
	{
	case LOCK:
		return "LOCK";
	case REPNZ_BND:
		return "REPNZ";
	case REPZ:
		return "REP";
	}
	
	return "";
}