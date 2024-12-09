#include "../Headers/disassembler.h"
#include "../Headers/opcodeTables.h"

struct DisassembledInstruction disassembleInstruction(unsigned char* bytes, struct DisassemblerOptions* disassemblerOptions)
{
	struct DisassembledInstruction result = { 0, 0 };

	struct LegacyPrefixes legacyPrefixes = handleLegacyPrefixes(bytes);
	bytes += legacyPrefixes.numOfPrefixes;
	result.numOfBytes += legacyPrefixes.numOfPrefixes;

	struct REXPrefix rexPrefix;
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

struct LegacyPrefixes handleLegacyPrefixes(unsigned char* bytes)
{
	struct LegacyPrefixes result = { NO_PREFIX, NO_PREFIX, NO_PREFIX, NO_PREFIX, 0 };

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

static struct REXPrefix handleREXPrefix(unsigned char* bytes)
{
	struct REXPrefix result = { 0, 0, 0, 0, 0 };
	
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

static struct OpcodeResult handleOpcode(unsigned char* bytes, struct DisassemblerOptions* disassemblerOptions)
{
	struct OpcodeResult result = { 0, 0 };
	
	// check the opcode map in use
	if (bytes[0] == 0x0F) 
	{
		if (bytes[1] == 0x3A) // sequence: 0x0F 0x3A opcode
		{
			//result.opcode = &threeByteMap2[bytes[2]];
			result.numOfBytes = 3;
		}
		else if (bytes[1] == 0x38) // sequence: 0x0F 0x38 opcode
		{
			//result.opcode = &threeByteMap[bytes[2]];
			result.numOfBytes = 3;
		}
		else // sequence: 0x0F opcode
		{
			//result.opcode = &twoByteMap[bytes[1]];
			result.numOfBytes = 2;
		}
	}
	else // sequence: opcode
	{
		result.opcode = &oneByteMap[bytes[0]];

		if (disassemblerOptions->is64BitMode && bytes[0] == 0x63)
		{
			result.opcode = &alternateX63;
		}

		result.numOfBytes = 1;
	}

	return result;
}

static struct OperandsResult handleOperands(unsigned char* bytes, unsigned char is64BitMode, struct OpcodeResult* opcode, struct LegacyPrefixes* legPrefixes, struct REXPrefix* rexPrefix)
{
	struct OperandsResult result = { 0, 0, 0 };

	for (int i = 0; i < 3; i++) 
	{
		enum OperandCode currentOperandCode = opcode->opcode->operand1;
		const char* currentOperandStr = result.operand1;
		if (i == 1) 
		{
			currentOperandCode = opcode->opcode->operand2;
			currentOperandStr = result.operand2;
		}
		else 
		{
			currentOperandCode = opcode->opcode->operand3;
			currentOperandStr = result.operand3;
		}
		
		switch (currentOperandCode)
		{
		case NO_OPERAND_CODE:
			i = 3; // no more operands; done handling them all
			break;
		case ONE:
			currentOperandStr = "0x1";
			break;
		case AL:
			currentOperandStr = "AL";
			break;
		case CL:
			currentOperandStr = "CL";
			break;
		case DX:
			currentOperandStr = "DX";
			break;
		case rAX:
			currentOperandStr = rexPrefix->w ? "RAX" : (legPrefixes->group3 == OSO ? "AX" : "EAX");
			break;
		case rCX:
			currentOperandStr = rexPrefix->w ? "RCX" : (legPrefixes->group3 == OSO ? "CX" : "ECX");
			break;
		case rDX:
			currentOperandStr = rexPrefix->w ? "RDX" : (legPrefixes->group3 == OSO ? "DX" : "EDX");
			break;
		case rBX:
			currentOperandStr = rexPrefix->w ? "RBX" : (legPrefixes->group3 == OSO ? "BX" : "EBX");
			break;
		case rSP:
			currentOperandStr = rexPrefix->w ? "RSP" : (legPrefixes->group3 == OSO ? "SP" : "ESP");
			break;
		case rBP:
			currentOperandStr = rexPrefix->w ? "RBP" : (legPrefixes->group3 == OSO ? "BP" : "EBP");
			break;
		case rSI:
			currentOperandStr = rexPrefix->w ? "RSI" : (legPrefixes->group3 == OSO ? "SI" : "ESI");
			break;
		case rDI:
			currentOperandStr = rexPrefix->w ? "RDI" : (legPrefixes->group3 == OSO ? "DI" : "EDI");
			break;
		case rAX_r8:
			currentOperandStr = rexPrefix->b ? "R8" : (legPrefixes->group3 == OSO ? "AX" : (is64BitMode ? "RAX" : "EAX"));
			break;
		case rCX_r9:
			currentOperandStr = rexPrefix->b ? "R9" : (legPrefixes->group3 == OSO ? "CX" : (is64BitMode ? "RCX" : "ECX"));
			break;
		case rDX_r10:
			currentOperandStr = rexPrefix->b ? "R10" : (legPrefixes->group3 == OSO ? "DX" : (is64BitMode ? "RDX" : "EDX"));
			break;
		case rBX_r11:
			currentOperandStr = rexPrefix->b ? "R11" : (legPrefixes->group3 == OSO ? "BX" : (is64BitMode ? "RBX" : "EBX"));
			break;
		case rSP_r12:
			currentOperandStr = rexPrefix->b ? "R12" : (legPrefixes->group3 == OSO ? "SP" : (is64BitMode ? "RSP" : "ESP"));
			break;
		case rBP_r13:
			currentOperandStr = rexPrefix->b ? "R13" : (legPrefixes->group3 == OSO ? "BP" : (is64BitMode ? "RBP" : "EBP"));
			break;
		case rSI_r14:
			currentOperandStr = rexPrefix->b ? "R14" : (legPrefixes->group3 == OSO ? "SI" : (is64BitMode ? "RSI" : "ESI"));
			break;
		case rDI_r15:
			currentOperandStr = rexPrefix->b ? "R15" : (legPrefixes->group3 == OSO ? "DI" : (is64BitMode ? "RDI" : "EDI"));
			break;
		case AL_R8B:
			currentOperandStr = rexPrefix->b ? "R8B" : "AL";
			break;
		case CL_R9B:
			currentOperandStr = rexPrefix->b ? "R9B" : "CL";
			break;
		case DL_R10B:
			currentOperandStr = rexPrefix->b ? "R10B" : "DL";
			break;
		case BL_R11B:
			currentOperandStr = rexPrefix->b ? "R11B" : "BL";
			break;
		case AH_R12B:
			currentOperandStr = rexPrefix->b ? "R12B" : "AH";
			break;
		case CH_R13B:
			currentOperandStr = rexPrefix->b ? "R13B" : "CH";
			break;
		case DH_R14B:
			currentOperandStr = rexPrefix->b ? "R14B" : "DH";
			break;
		case BH_R15B:
			currentOperandStr = rexPrefix->b ? "R15B" : "BH";
			break;
		}
	}
}

const char* handleModRM(unsigned char* bytes, ) 
{
	unsigned char modRMByte = bytes[0];

	unsigned char mod = (((modRMByte >> 7) & 0x01) * 2) + ((modRMByte >> 6) & 0x01);
	unsigned char reg = (((modRMByte >> 5) & 0x01) * 4) + (((modRMByte >> 4) & 0x01) * 2) + ((modRMByte >> 3) & 0x01);
	unsigned char rm = (((modRMByte >> 2) & 0x01) * 4) + (((modRMByte >> 1) & 0x01) * 2) + ((modRMByte >> 0) & 0x01);

	// figure out what information is needed from the modrm byte and the size of it
}