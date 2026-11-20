#include "disassemblerX86.h"
#include "prefixes.h"
#include "opcodes.h"
#include "operands.h"

enum JdcStatus disassembleX86(uint8_t* bytes, uint8_t* maxBytesAddr, bool is64BitMode, struct DisassembledInstruction* result)
{
	struct DisassemblyParameters params = { 0 };
	params.bytes = bytes;
	params.maxBytesAddr = maxBytesAddr;
	params.startBytePtr = bytes;
	params.is64BitMode = is64BitMode;

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

enum JdcStatus generateInstructionInfoStrX86(uint8_t* bytes, uint8_t numOfBytes, bool is64BitMode, struct JdcStr* result)
{
	struct DisassemblyParameters params = { 0 };
	params.bytes = bytes;
	params.maxBytesAddr = bytes + numOfBytes - 1;
	params.startBytePtr = bytes;
	params.is64BitMode = is64BitMode;

	struct DisassembledInstruction instruction;
	memset(&instruction, 0, sizeof(struct DisassembledInstruction));

	if (ERROR_JDC == handleLegacyPrefixes(&params))
	{
		return ERROR_JDC;
	}

	strcatJdc(result, "Legacy prefixes:\n");
	switch (params.legPrefixes.group1)
	{
	case LOCK_LP:
		sprintfJdc(result, true, "\tGroup 1: LOCK (0xF0)\n");
		break;
	case REPNZ_BND_LP:
		sprintfJdc(result, true, "\tGroup 1: REPNZ/BND (0xF2)\n");
		break;
	case REPZ_LP:
		sprintfJdc(result, true, "\tGroup 1: REPZ (0xF3)\n");
		break;
	default:
		sprintfJdc(result, true, "\tGroup 1: none\n");
		break;
	}
	switch (params.legPrefixes.group2)
	{
	case CSO_BNT_LP:
		sprintfJdc(result, true, "\tGroup 2: CS segment override or branch not taken (0x2E)\n");
		break;
	case SSO_LP:
		sprintfJdc(result, true, "\tGroup 2: SS segment overide (0x36)\n");
		break;
	case DSO_BT_LP:
		sprintfJdc(result, true, "\tGroup 2: DS segment override or branch taken (0x3E)\n");
		break;
	case ESO_LP:
		sprintfJdc(result, true, "\tGroup 2: ES segment override (0x26)\n");
		break;
	case FSO_LP:
		sprintfJdc(result, true, "\tGroup 2: FS segment override (0x64)\n");
		break;
	case GSO_LP:
		sprintfJdc(result, true, "\tGroup 2: GS segment override (0x65)\n");
		break;
	default:
		sprintfJdc(result, true, "\tGroup 2: none\n");
		break;
	}
	switch (params.legPrefixes.group3)
	{
	case OSO_LP:
		sprintfJdc(result, true, "\tGroup 3: operand-size override (0x66)\n");
		break;
	default:
		sprintfJdc(result, true, "\tGroup 3: none\n");
		break;
	}
	switch (params.legPrefixes.group4)
	{
	case ASO_LP:
		sprintfJdc(result, true, "\tGroup 4: address-size override (0x67)\n");
		break;
	default:
		sprintfJdc(result, true, "\tGroup 4: none\n");
		break;
	}

	if (params.is64BitMode && ERROR_JDC == handleREXPrefix(&params))
	{
		return ERROR_JDC;
	}

	if (params.rexPrefix.isValidREX)
	{
		sprintfJdc(result, true, "REX prefix: 0x%02X (0100WRXB)\n", *(params.bytes - 1));
		sprintfJdc(result, true, "\tW: %d\n", params.rexPrefix.W);
		sprintfJdc(result, true, "\tR: %d\n", params.rexPrefix.R);
		sprintfJdc(result, true, "\tX: %d\n", params.rexPrefix.X);
		sprintfJdc(result, true, "\tB: %d\n", params.rexPrefix.B);
	}
	else
	{
		strcatJdc(result, "No REX prefix\n");
	}

	if (!params.rexPrefix.isValidREX && ERROR_JDC == handleVEXPrefix(&params))
	{
		return ERROR_JDC;
	}


	if (params.vexPrefix.isValidVEX)
	{
		if (!params.vexPrefix.isThreeByteForm)
		{
			sprintfJdc(result, true, "VEX prefix (2-byte): 0xC5 0x%02X (11000101 RvvvvLpp)\n", *(params.bytes - 1));
		}
		else
		{
			sprintfJdc(result, true, "VEX prefix (3-byte): 0xC4 0x%02X 0x%02X (11000100 RXBmmmmm WvvvvLpp)\n", *(params.bytes - 2), *(params.bytes - 1));
		}

		sprintfJdc(result, true, "\tW: %d\n", params.vexPrefix.W);
		sprintfJdc(result, true, "\tR: %d\n", params.vexPrefix.R);
		sprintfJdc(result, true, "\tX: %d\n", params.vexPrefix.X);
		sprintfJdc(result, true, "\tB: %d\n", params.vexPrefix.B);
		sprintfJdc(result, true, "\tm-mmmm: %0xX", params.vexPrefix.m_mmmm);
		switch (params.vexPrefix.m_mmmm)
		{
		case 1:
			strcatJdc(result, " (0x0F implied leading opcode byte)\n");
			break;
		case 2:
			strcatJdc(result, " (0x0F 0x38 implied leading opcode bytes)\n");
			break;
		case 3:
			strcatJdc(result, " (0x0F 0x3A implied leading opcode bytes)\n");
			break;
		default:
			strcatJdc(result, " (reserved)\n");
			break;
		}
		sprintfJdc(result, true, "\tvvvv: %0xX\n", params.vexPrefix.vvvv);
		sprintfJdc(result, true, "\tL: %d", params.vexPrefix.L);
		switch (params.vexPrefix.L)
		{
		case 0:
			strcatJdc(result, " (scalar or 128-bit vector)\n");
			break;
		case 1:
			strcatJdc(result, " 256-bit vector)\n");
			break;
		}
		sprintfJdc(result, true, "\tpp: %d", params.vexPrefix.pp);
		switch (params.vexPrefix.pp)
		{
		case 0:
			strcatJdc(result, " (no implied legacy prefix)\n");
			break;
		case 1:
			strcatJdc(result, " (0x66 implied legacy prefix)\n");
			break;
		case 2:
			strcatJdc(result, " (0xF3 implied legacy prefix)\n");
			break;
		case 3:
			strcatJdc(result, " (0xF2 implied legacy prefix)\n");
			break;
		}
	}
	else
	{
		strcatJdc(result, "No VEX prefix\n");
	}

	if (!params.rexPrefix.isValidREX && !params.vexPrefix.isValidVEX && ERROR_JDC == handleEVEXPrefix(&params))
	{
		return ERROR_JDC;
	}

	if (params.evexPrefix.isValidEVEX)
	{
		sprintfJdc(result, true, "EVEX prefix: 0x62 0x%02X 0x%02X 0x%02X (01100010 RXBR'0mmm Wvvvv1pp zL'LbV'aaa)\n", *(params.bytes - 3), *(params.bytes - 2), *(params.bytes - 1));
		sprintfJdc(result, true, "\tR: %d\n", params.evexPrefix.R);
		sprintfJdc(result, true, "\tX: %d\n", params.evexPrefix.X);
		sprintfJdc(result, true, "\tB: %d\n", params.evexPrefix.B);
		sprintfJdc(result, true, "\tR': %d\n", params.evexPrefix.R_prime);
		sprintfJdc(result, true, "\tmmm: 0x%X", params.evexPrefix.mmm);
		switch (params.evexPrefix.mmm)
		{
		case 1:
			strcatJdc(result, " (0x0F implied leading opcode byte)\n");
			break;
		case 2:
			strcatJdc(result, " (0x0F 0x38 implied leading opcode bytes)\n");
			break;
		case 3:
			strcatJdc(result, " (0x0F 0x3A implied leading opcode bytes)\n");
			break;
		default:
			strcatJdc(result, " (reserved)\n");
			break;
		}
		sprintfJdc(result, true, "\tW: %d", params.evexPrefix.W);
		sprintfJdc(result, true, "\tvvvv: 0x%X", params.evexPrefix.vvvv);
		sprintfJdc(result, true, "\tpp: %d", params.evexPrefix.pp);
		switch (params.evexPrefix.pp)
		{
		case 0:
			strcatJdc(result, " (no implied legacy prefix)\n");
			break;
		case 1:
			strcatJdc(result, " (0x66 implied legacy prefix)\n");
			break;
		case 2:
			strcatJdc(result, " (0xF3 implied legacy prefix)\n");
			break;
		case 3:
			strcatJdc(result, " (0xF2 implied legacy prefix)\n");
			break;
		}
		sprintfJdc(result, true, "\tz: %d", params.evexPrefix.z);
		sprintfJdc(result, true, "\tL'L: %d", params.evexPrefix.LL);
		switch (params.evexPrefix.LL)
		{
		case 0:
			strcatJdc(result, " (128-bit vector)\n");
			break;
		case 1:
			strcatJdc(result, " (256-bit vector)\n");
			break;
		case 2:
			strcatJdc(result, " (512-bit vector)\n");
			break;
		}
		sprintfJdc(result, true, "\tb: %d", params.evexPrefix.b);
		sprintfJdc(result, true, "\tV': %d", params.evexPrefix.V_prime);
		sprintfJdc(result, true, "\taaa: 0x%X", params.evexPrefix.aaa);
	}
	else
	{
		strcatJdc(result, "No EVEX prefix\n");
	}

	uint8_t* opcodeBytesStart = params.bytes;
	if (ERROR_JDC == handleOpcode(&params))
	{
		return ERROR_JDC;
	}

	strcatJdc(result, "Opcode bytes: ");
	for (uint8_t* i = opcodeBytesStart; i < params.bytes; i++)
	{
		sprintfJdc(result, true, "0x%02X ", *i);
	}
	result->buffer[strlen(result->buffer) - 1] = '\n';
	sprintfJdc(result, true, "\tMnemonic: %s\n", opcodeMnemonicStrs[params.opcode.mnemonic]);

	if (ERROR_JDC == handleOperands(&params, &instruction))
	{
		return ERROR_JDC;
	}

	if (params.modRM.hasGotModRM)
	{
		sprintfJdc(result, true, "ModR/M: 0x%02X (mod reg r/m)\n", (params.modRM.mod << 6) + (params.modRM.reg << 3) + params.modRM.rm);
		sprintfJdc(result, true, "\tmod: %d%d\n", (params.modRM.mod & 0b10) >> 1, params.modRM.mod & 0b1);
		sprintfJdc(result, true, "\treg: %d%d%d\n", (params.modRM.reg & 0b100) >> 2, (params.modRM.reg & 0b10) >> 1, params.modRM.reg & 0b1);
		sprintfJdc(result, true, "\tr/m: %d%d%d\n", (params.modRM.rm & 0b100) >> 2, (params.modRM.rm & 0b10) >> 1, params.modRM.rm & 0b1);
	}
	else
	{
		strcatJdc(result, "No ModR/M byte\n");
	}

	if (params.sib.hasGotSIB)
	{
		sprintfJdc(result, true, "SIB: 0x%02X (scale index base)\n", (params.sib.scale << 6) + (params.sib.index << 3) + params.sib.base);
		sprintfJdc(result, true, "\tscale: %d%d\n", (params.sib.scale & 0b10) >> 1, params.sib.scale & 0b1);
		sprintfJdc(result, true, "\tindex: %d%d%d\n", (params.sib.index & 0b100) >> 2, (params.sib.index & 0b10) >> 1, params.sib.index & 0b1);
		sprintfJdc(result, true, "\tbase: %d%d%d\n", (params.sib.base & 0b100) >> 2, (params.sib.base & 0b10) >> 1, params.sib.base & 0b1);
	}
	else
	{
		strcatJdc(result, "No SIB byte\n");
	}

	return SUCCESS_JDC;
}