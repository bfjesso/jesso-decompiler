#include "disassemblerX86.h"
#include "prefixes.h"
#include "opcodes.h"
#include "operands.h"

enum JdcStatus disassembleX86(uint8_t* bytes, uint8_t* maxBytesAddr, struct DisassemblerOptions* disassemblerOptions, struct DisassembledInstruction* result)
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

	if (params.is64BitMode && ERROR_JDC == handleREXPrefix(&params))
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

	if (result->numOfBytes == 0)
	{
		return ERROR_JDC;
	}

	return SUCCESS_JDC;
}