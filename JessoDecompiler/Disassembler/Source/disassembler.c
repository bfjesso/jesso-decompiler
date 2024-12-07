#include "../Headers/disassembler.h"
#include "../Headers/opcodeTables.h"

struct DisassembledInstruction disassembleInstruction(unsigned char* bytes, struct DisassemblerOptions* disassemblerOptions)
{
	struct DisassembledInstruction result = { 0, 0 };

	struct LegacyPrefixesResult legacyPrefixes = handleLegacyPrefixes(bytes);
	bytes += legacyPrefixes.numOfPrefixes;
	result.numOfBytes += legacyPrefixes.numOfPrefixes;

	struct REXPrefixResult rexPrefix;
	if (disassemblerOptions->is64BitMode) 
	{
		rexPrefix = handleREXPrefix(bytes);
		if (rexPrefix.isValidREX) 
		{
			bytes++;
			result.numOfBytes++;
		}
	}

	struct OpcodeResult opcodeInfo = handleOpcode(bytes);
	bytes += opcodeInfo.numOfBytes;
	result.numOfBytes += opcodeInfo.numOfBytes;

	// handle operands

	return result;
}

struct LegacyPrefixesResult handleLegacyPrefixes(unsigned char* bytes)
{
	struct LegacyPrefixesResult result = { NO_PREFIX, NO_PREFIX, NO_PREFIX, NO_PREFIX, 0 };

	for (int i = 0; i < 4; i++) // up to four prefixes
	{
		unsigned char byte = bytes[i];
		result.numOfPrefixes++;

		switch (byte) 
		{
		case 0xF0:
			result.group1 = LOCK;
			break;
		case 0xF2:
			result.group1 = REPNZ;
			break;
		case 0xF3:
			result.group1 = REPZ;
			break;
		case 0x2E:
			result.group2 = CS_BNT;
			break;
		case 0x36:
			result.group2 = SS;
			break;
		case 0x3E:
			result.group2 = DS_BT;
			break;
		case 0x26:
			result.group2 = ES;
			break;
		case 0x64:
			result.group2 = FS;
			break;
		case 0x65:
			result.group2 = GS;
			break;
		case 0x66:
			result.group3 = OSO;
			break;
		case 0x67:
			result.group4 = ASO;
			break;
		default:
			result.numOfPrefixes--;
			break;
		}
	}

	return result;
}

static struct REXPrefixResult handleREXPrefix(unsigned char* bytes) 
{
	struct REXPrefixResult result = { 0, 0, 0, 0, 0 };
	
	unsigned char rexByte = bytes[0];

	if (rexByte < 0x40 || rexByte > 0x4F)
	{
		return result;
	}

	result.isValidREX = 1;
	result.w = (rexByte >> 3) & 0x01;
	result.w = (rexByte >> 2) & 0x01;
	result.w = (rexByte >> 1) & 0x01;
	result.w = (rexByte >> 0) & 0x01;

	return result;
}

static struct OpcodeResult handleOpcode(unsigned char* bytes)
{
	struct OpcodeResult result = { 0, 0 };
	
	// check the opcode map in use
	if (bytes[0] == 0x0F) 
	{
		if (bytes[1] == 0x3A) // sequence: 0x0F 0x3A opcode
		{
			//result.opcode = threeByteMap2[bytes[2]];
			result.numOfBytes = 3;
		}
		else if (bytes[1] == 0x38) // sequence: 0x0F 0x38 opcode
		{
			//result.opcode = threeByteMap[bytes[2]];
			result.numOfBytes = 3;
		}
		else // sequence: 0x0F opcode
		{
			//result.opcode = twoByteMap[bytes[1]];
			result.numOfBytes = 2;
		}
	}
	else // sequence: opcode
	{
		result.opcode = oneByteMap[bytes[0]];
		result.numOfBytes = 1;
	}

	return result;
}