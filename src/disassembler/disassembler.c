#include "disassembler.h"
#include "prefixes.h"
#include "opcodes.h"
#include "operands.h"

enum JdcStatus disassembleInstruction(uint8_t* bytes, uint8_t* maxBytesAddr, struct DisassemblerOptions* disassemblerOptions, struct DisassembledInstruction* result)
{
	struct DisassemblyParameters params = { 0 };
	params.bytes = bytes;
	params.maxBytesAddr = maxBytesAddr;
	params.startBytePtr = bytes;
	params.is64BitMode = disassemblerOptions->is64BitMode;

	memset(result, 0, sizeof(struct DisassembledInstruction));
	
	if (ERROR_JDC == handleLegacyPrefixes(&params))
	{
		result->numOfBytes = (uint8_t)(params.bytes - params.startBytePtr);
		return ERROR_JDC;
	}

	if (params.is64BitMode) 
	{
		if (ERROR_JDC == handleREXPrefix(&params))
		{
			result->numOfBytes = (uint8_t)(params.bytes - params.startBytePtr);
			return ERROR_JDC;
		}

		if (!params.rexPrefix.isValidREX && ERROR_JDC == handleVEXPrefix(&params))
		{
			result->numOfBytes = (uint8_t)(params.bytes - params.startBytePtr);
			return ERROR_JDC;
		}

		if (!params.rexPrefix.isValidREX && !params.vexPrefix.isValidVEX && ERROR_JDC == handleEVEXPrefix(&params))
		{
			result->numOfBytes = (uint8_t)(params.bytes - params.startBytePtr);
			return ERROR_JDC;
		}
	}

	// if the opcode is an extended one then modRM will be retrieved
	if (ERROR_JDC == handleOpcode(&params))
	{
		result->numOfBytes = (uint8_t)(params.bytes - params.startBytePtr);
		return ERROR_JDC;
	}

	if (ERROR_JDC == handleOperands(&params, result))
	{
		result->numOfBytes = (uint8_t)(params.bytes - params.startBytePtr);
		return ERROR_JDC;
	}

	result->opcode = params.opcode.mnemonic;
	result->group1Prefix = params.legPrefixes.group1;
	result->numOfBytes = (uint8_t)(params.bytes - params.startBytePtr);
	result->isInvalid = (disassemblerOptions->is64BitMode && params.opcode.opcodeSuperscript == i64) || (!disassemblerOptions->is64BitMode && params.opcode.opcodeSuperscript == o64);

	if (result->numOfBytes == 0) 
	{
		return ERROR_JDC;
	}

	return SUCCESS_JDC;
}

enum JdcStatus instructionToStr(struct DisassembledInstruction* instruction, struct JdcStr* result) // this will be in intel syntax
{
	strcpyJdc(result, "");

	if (instruction->group1Prefix != NO_PREFIX) 
	{
		strcatJdc(result, getGroup1PrefixStr(instruction));
		strcatJdc(result, " ");
	}

	strcatJdc(result, mnemonicStrs[instruction->opcode]);

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
		switch (currentOperand->type)
		{
		case SEGMENT:
			strcatJdc(result, segmentStrs[currentOperand->segment]);
			break;
		case REGISTER:
			strcatJdc(result, registerStrs[currentOperand->reg]);
			break;
		case MEM_ADDRESS:
			if (memAddressToStr(&currentOperand->memoryAddress, result) == ERROR_JDC) { return ERROR_JDC; }
			break;
		case IMMEDIATE:
			sprintfJdc(result, true, "0x%llX", currentOperand->immediate.value);
			break;
		}
	}

	return SUCCESS_JDC;
}

static enum JdcStatus memAddressToStr(struct MemoryAddress* memAddr, struct JdcStr* result)
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
		sprintfJdc(result, true, "0x%X", memAddr->constSegment);
		strcatJdc(result, ":");
	}

	strcatJdc(result, "[");

	if (memAddr->reg != NO_REG) 
	{
		strcatJdc(result, registerStrs[memAddr->reg]);
	}

	if (memAddr->scale > 1) 
	{
		sprintfJdc(result, true, " * 0x%X", memAddr->scale);
	}

	if (memAddr->regDisplacement != NO_REG)
	{
		strcatJdc(result, " + ");
		strcatJdc(result, registerStrs[memAddr->regDisplacement]);
	}

	if (memAddr->reg != NO_REG)
	{
		if (memAddr->constDisplacement > 0)
		{
			sprintfJdc(result, true, " + 0x%llX", memAddr->constDisplacement);
		}
		else if (memAddr->constDisplacement < 0)
		{
			sprintfJdc(result, true, " - 0x%llX", -memAddr->constDisplacement);
		}
	}
	else
	{
		sprintfJdc(result, true, "0x%llX", memAddr->constDisplacement);
	}

	strcatJdc(result, "]");
	return SUCCESS_JDC;
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