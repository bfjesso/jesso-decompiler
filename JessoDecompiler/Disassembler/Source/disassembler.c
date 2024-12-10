#include "../Headers/disassembler.h"
#include "../Headers/oneByteOpcodeMap.h"

struct DisassembledInstruction disassembleInstruction(unsigned char* bytes, struct DisassemblerOptions* disassemblerOptions)
{
	struct DisassembledInstruction result = { 0, 0 };

	struct LegacyPrefixes legacyPrefixes = handleLegacyPrefixes(bytes);

	struct REXPrefix rexPrefix;
	if (disassemblerOptions->is64BitMode) 
	{
		rexPrefix = handleREXPrefix(bytes);
	}

	struct Opcode* opcode = handleOpcode(bytes, disassemblerOptions);

	// handle operands

	return result;
}

struct LegacyPrefixes handleLegacyPrefixes(unsigned char* bytes)
{
	struct LegacyPrefixes result = { NO_PREFIX, NO_PREFIX, NO_PREFIX, NO_PREFIX };

	for (int i = 0; i < 4; i++) // up to four prefixes
	{
		unsigned char byte = bytes[0];
		bytes++;

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
			bytes--;
			return result;
		}
	}

	return result;
}

static struct REXPrefix handleREXPrefix(unsigned char* bytes)
{
	struct REXPrefix result = { 0, 0, 0, 0, 0 };
	
	unsigned char rexByte = bytes[0];
	bytes++;

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

static struct Opcode* handleOpcode(unsigned char* bytes, struct DisassemblerOptions* disassemblerOptions)
{
	struct Opcode* result;
	
	// check the opcode map in use
	if (bytes[0] == 0x0F) 
	{
		if (bytes[1] == 0x3A) // sequence: 0x0F 0x3A opcode
		{
			//result.opcode = &threeByteMap2[bytes[2]];
			bytes += 3;
		}
		else if (bytes[1] == 0x38) // sequence: 0x0F 0x38 opcode
		{
			//result.opcode = &threeByteMap[bytes[2]];
			bytes += 3;
		}
		else // sequence: 0x0F opcode
		{
			//result.opcode = &twoByteMap[bytes[1]];
			bytes += 2;
		}
	}
	else // sequence: opcode
	{
		result = &oneByteMap[bytes[0]];

		if (disassemblerOptions->is64BitMode && bytes[0] == 0x63)
		{
			result = &alternateX63;
		}

		bytes++;
	}

	return result;
}

static struct OperandsResult handleOperands(unsigned char* bytes, unsigned char is64BitMode, struct Opcode* opcode, struct LegacyPrefixes* legPrefixes, struct REXPrefix* rexPrefix)
{
	struct OperandsResult result = { 0, 0, 0 };

	for (int i = 0; i < 3; i++) 
	{
		enum OperandCode currentOperandCode;
		union Operand* currentOperand;

		if (i == 0) 
		{
			currentOperandCode = opcode->operand1;
			currentOperand = &result.operand1;
		}
		else if(i == 1)
		{
			currentOperandCode = opcode->operand2;
			currentOperand = &result.operand2;
		}
		else 
		{
			currentOperandCode = opcode->operand3;
			currentOperand = &result.operand3;
		}

		currentOperand->reg = NO_REG;
		
		switch (currentOperandCode)
		{
		case NO_OPERAND_CODE:
			i = 3; // no more operands; done handling them all
			break;
		case ONE:
			currentOperand->immediate = 1;
			break;
		case AL_CODE:
			currentOperand->reg = AL;
			break;
		case CL_CODE:
			currentOperand->reg = CL;
			break;
		case DX_CODE:
			currentOperand->reg = DX;
			break;
		case rAX:
			currentOperand->reg = rexPrefix->w ? RAX : (legPrefixes->group3 == OSO ? AX : EAX);
			break;
		case rCX:
			currentOperand->reg = rexPrefix->w ? RCX : (legPrefixes->group3 == OSO ? CX : ECX);
			break;
		case rDX:
			currentOperand->reg = rexPrefix->w ? RDX : (legPrefixes->group3 == OSO ? DX : EDX);
			break;
		case rBX:
			currentOperand->reg = rexPrefix->w ? RBX : (legPrefixes->group3 == OSO ? BX : EBX);
			break;
		case rSP:
			currentOperand->reg = rexPrefix->w ? RSP : (legPrefixes->group3 == OSO ? SP : ESP);
			break;
		case rBP:
			currentOperand->reg = rexPrefix->w ? RBP : (legPrefixes->group3 == OSO ? BP : EBP);
			break;
		case rSI:
			currentOperand->reg = rexPrefix->w ? RSI : (legPrefixes->group3 == OSO ? SI : ESI);
			break;
		case rDI:
			currentOperand->reg = rexPrefix->w ? RDI : (legPrefixes->group3 == OSO ? DI : EDI);
			break;
		case rAX_r8:
			currentOperand->reg = rexPrefix->b ? R8 : (legPrefixes->group3 == OSO ? AX : (is64BitMode ? RAX : EAX));
			break;
		case rCX_r9:
			currentOperand->reg = rexPrefix->b ? R9 : (legPrefixes->group3 == OSO ? CX : (is64BitMode ? RCX : ECX));
			break;
		case rDX_r10:
			currentOperand->reg = rexPrefix->b ? R10 : (legPrefixes->group3 == OSO ? DX : (is64BitMode ? RDX : EDX));
			break;
		case rBX_r11:
			currentOperand->reg = rexPrefix->b ? R11 : (legPrefixes->group3 == OSO ? BX : (is64BitMode ? RBX : EBX));
			break;
		case rSP_r12:
			currentOperand->reg = rexPrefix->b ? R12 : (legPrefixes->group3 == OSO ? SP : (is64BitMode ? RSP : ESP));
			break;
		case rBP_r13:
			currentOperand->reg = rexPrefix->b ? R13 : (legPrefixes->group3 == OSO ? BP : (is64BitMode ? RBP : EBP));
			break;
		case rSI_r14:
			currentOperand->reg = rexPrefix->b ? R14 : (legPrefixes->group3 == OSO ? SI : (is64BitMode ? RSI : ESI));
			break;
		case rDI_r15:
			currentOperand->reg = rexPrefix->b ? R15 : (legPrefixes->group3 == OSO ? DI : (is64BitMode ? RDI : EDI));
			break;
		case AL_R8B:
			currentOperand->reg = rexPrefix->b ? R8B : AL;
			break;
		case CL_R9B:
			currentOperand->reg = rexPrefix->b ? R9B : CL;
			break;
		case DL_R10B:
			currentOperand->reg = rexPrefix->b ? R10B : DL;
			break;
		case BL_R11B:
			currentOperand->reg = rexPrefix->b ? R11B : BL;
			break;
		case AH_R12B:
			currentOperand->reg = rexPrefix->b ? R12B : AH;
			break;
		case CH_R13B:
			currentOperand->reg = rexPrefix->b ? R13B : CH;
			break;
		case DH_R14B:
			currentOperand->reg = rexPrefix->b ? R14B : DH;
			break;
		case BH_R15B:
			currentOperand->reg = rexPrefix->b ? R15B : BH;
			break;
		case Eb:
			*currentOperand = handleModRM(bytes, 0, 1, legPrefixes->group4 == ASO);
			break;
		case Ev:
			*currentOperand = handleModRM(bytes, 0, legPrefixes->group3 == OSO ? 2 : is64BitMode ? 8 : 4, legPrefixes->group4 == ASO);
			break;
		case Ew:
			*currentOperand = handleModRM(bytes, 0, 2, legPrefixes->group4 == ASO);
			break;
		case Gb:
			*currentOperand = handleModRM(bytes, 1, 1, 0);
			break;
		case Gv:
			*currentOperand = handleModRM(bytes, 1, legPrefixes->group3 == OSO ? 2 : is64BitMode ? 8 : 4, 0);
			break;
		case Gz:
			*currentOperand = handleModRM(bytes, 1, legPrefixes->group3 == OSO ? 2 : 4, 0);
			break;
		case Gw:
			*currentOperand = handleModRM(bytes, 1, 2, 0);
			break;
		}
	}
}

union Operand handleModRM(unsigned char* bytes, char getReg, unsigned char operandSize, char addressSizeOverride)
{
	union Operand result = { 0 };
	struct MemoryAddress memAddr = { 0, 0 };

	unsigned char modRMByte = bytes[0];
	bytes++;

	unsigned char mod = (((modRMByte >> 7) & 0x01) * 2) + ((modRMByte >> 6) & 0x01);
	unsigned char reg = (((modRMByte >> 5) & 0x01) * 4) + (((modRMByte >> 4) & 0x01) * 2) + ((modRMByte >> 3) & 0x01);
	unsigned char rm = (((modRMByte >> 2) & 0x01) * 4) + (((modRMByte >> 1) & 0x01) * 2) + ((modRMByte >> 0) & 0x01);

	if (getReg)
	{
		switch (operandSize)
		{
		case 1:
			result.reg = (enum Register)reg;
			break;
		case 2:
			result.reg = (enum Register)(reg + 16);
			break;
		case 4:
			result.reg = (enum Register)(reg + 24);
			break;
		case 8:
			result.reg = (enum Register)(reg + 32);
			break;
		}

		return result;
	}
	else if (mod == 3) 
	{
		switch (rm)
		{
		case 0:
			result.reg = operandSize == 4 ? EAX : operandSize == 2 ? AX : AL;
			break;
		case 1:
			result.reg = operandSize == 4 ? ECX : operandSize == 2 ? CX : CL;
			break;
		case 2:
			result.reg = operandSize == 4 ? EDX : operandSize == 2 ? DX : DL;
			break;
		case 3:
			result.reg = operandSize == 4 ? EBX : operandSize == 2 ? BX : BL;
			break;
		case 4:
			result.reg = operandSize == 4 ? ESP : operandSize == 2 ? SP : AH;
			break;
		case 5:
			result.reg = operandSize == 4 ? EBP : operandSize == 2 ? BP : CH;
			break;
		case 6:
			result.reg = operandSize == 4 ? ESI : operandSize == 2 ? SI : DH;
			break;
		case 7:
			result.reg = operandSize == 4 ? EDI : operandSize == 2 ? DI : BH;
			break;
		}

		return result;
	}

	if (addressSizeOverride)
	{
		switch (mod)
		{
		case 0: 
		{
			switch (rm)
			{
			case 0:
				result.memoryAddress.reg = BX;
				result.memoryAddress.regDisplacement = SI;
				break;
			case 1:
				result.memoryAddress.reg = BX;
				result.memoryAddress.regDisplacement = DI;
				break;
			case 2:
				result.memoryAddress.reg = BP;
				result.memoryAddress.regDisplacement = SI;
				break;
			case 3:
				result.memoryAddress.reg = BP;
				result.memoryAddress.regDisplacement = DI;
				break;
			case 4:
				result.memoryAddress.reg = SI;
				break;
			case 5:
				result.memoryAddress.reg = DI;
				break;
			case 6:
				result.memoryAddress.constDisplacement = bytes[0] + (bytes[1] << 8); // disp16
				bytes += 2;
				break;
			case 7:
				result.memoryAddress.reg = BX;
				break;
			}

			break;
		}
		case 1: 
		{
			switch (rm)
			{
			case 0:
				result.memoryAddress.reg = BX;
				result.memoryAddress.regDisplacement = SI;
				result.memoryAddress.constDisplacement = bytes[0]; // disp8
				bytes++;
				break;
			case 1:
				result.memoryAddress.reg = BX;
				result.memoryAddress.regDisplacement = DI;
				result.memoryAddress.constDisplacement = bytes[0]; // disp8
				bytes++;
				break;
			case 2:
				result.memoryAddress.reg = BP;
				result.memoryAddress.regDisplacement = SI;
				result.memoryAddress.constDisplacement = bytes[0]; // disp8
				bytes++;
				break;
			case 3:
				result.memoryAddress.reg = BP;
				result.memoryAddress.regDisplacement = DI;
				result.memoryAddress.constDisplacement = bytes[0]; // disp8
				bytes++;
				break;
			case 4:
				result.memoryAddress.reg = SI;
				result.memoryAddress.constDisplacement = bytes[0]; // disp8
				bytes++;
				break;
			case 5:
				result.memoryAddress.reg = DI;
				result.memoryAddress.constDisplacement = bytes[0]; // disp8
				bytes++;
				break;
			case 6:
				result.memoryAddress.reg = BP;
				result.memoryAddress.constDisplacement = bytes[0]; // disp8
				bytes++;
				break;
			case 7:
				result.memoryAddress.reg = BX;
				result.memoryAddress.constDisplacement = bytes[0]; // disp8
				bytes++;
				break;
			}

			break;
		}
		case 2: 
		{
			switch (rm)
			{
			case 0:
				result.memoryAddress.reg = BX;
				result.memoryAddress.regDisplacement = SI;
				result.memoryAddress.constDisplacement = bytes[0] + (bytes[1] << 8); // disp16
				bytes += 2;
				break;
			case 1:
				result.memoryAddress.reg = BX;
				result.memoryAddress.regDisplacement = DI;
				result.memoryAddress.constDisplacement = bytes[0] + (bytes[1] << 8); // disp16
				bytes += 2;
				break;
			case 2:
				result.memoryAddress.reg = BP;
				result.memoryAddress.regDisplacement = SI;
				result.memoryAddress.constDisplacement = bytes[0] + (bytes[1] << 8); // disp16
				bytes += 2;
				break;
			case 3:
				result.memoryAddress.reg = BP;
				result.memoryAddress.regDisplacement = DI;
				result.memoryAddress.constDisplacement = bytes[0] + (bytes[1] << 8); // disp16
				bytes += 2;
				break;
			case 4:
				result.memoryAddress.reg = SI;
				result.memoryAddress.constDisplacement = bytes[0] + (bytes[1] << 8); // disp16
				bytes += 2;
				break;
			case 5:
				result.memoryAddress.reg = DI;
				result.memoryAddress.constDisplacement = bytes[0] + (bytes[1] << 8); // disp16
				bytes += 2;
				break;
			case 6:
				result.memoryAddress.reg = BP;
				result.memoryAddress.constDisplacement = bytes[0] + (bytes[1] << 8); // disp16
				bytes += 2;
				break;
			case 7:
				result.memoryAddress.reg = BX;
				result.memoryAddress.constDisplacement = bytes[0] + (bytes[1] << 8); // disp16
				bytes += 2;
				break;
			}

			break;
		}
		}
	}
	else
	{
		switch (mod)
		{
		case 0:
		{
			switch (rm)
			{
			case 0:
				result.memoryAddress.reg = EAX;
				break;
			case 1:
				result.memoryAddress.reg = ECX;
				break;
			case 2:
				result.memoryAddress.reg = EDX;
				break;
			case 3:
				result.memoryAddress.reg = EBX;
				break;
			case 4:
				handleSIB()
				break;
			case 5:
				result.memoryAddress.constDisplacement = bytes[0] + (bytes[1] << 8) + (bytes[2] << 16) + (bytes[3] << 24); // disp32
				bytes += 4;
				break;
			case 6:
				result.memoryAddress.reg = ESI;
				break;
			case 7:
				result.memoryAddress.reg = EDI;
				break;
			}

			break;
		}
		case 1:
		{
			switch (rm)
			{
			case 0:
				result.memoryAddress.reg = EAX;
				result.memoryAddress.constDisplacement = bytes[0]; // disp8
				bytes++;
				break;
			case 1:
				result.memoryAddress.reg = ECX;
				result.memoryAddress.constDisplacement = bytes[0]; // disp8
				bytes++;
				break;
			case 2:
				result.memoryAddress.reg = EDX;
				result.memoryAddress.constDisplacement = bytes[0]; // disp8
				bytes++;
				break;
			case 3:
				result.memoryAddress.reg = EBX;
				result.memoryAddress.constDisplacement = bytes[0]; // disp8
				bytes++;
				break;
			case 4:
				handleSIB()
				result.memoryAddress.constDisplacement = bytes[0]; // disp8
				bytes++;
				break;
			case 5:
				result.memoryAddress.reg = EBP;
				result.memoryAddress.constDisplacement = bytes[0]; // disp8
				bytes++;
				break;
			case 6:
				result.memoryAddress.reg = ESI;
				result.memoryAddress.constDisplacement = bytes[0]; // disp8
				bytes++;
				break;
			case 7:
				result.memoryAddress.reg = EDI;
				result.memoryAddress.constDisplacement = bytes[0]; // disp8
				bytes++;
				break;
			}

			break;
		}
		case 2:
		{
			switch (rm)
			{
			case 0:
				result.memoryAddress.reg = EAX;
				result.memoryAddress.constDisplacement = bytes[0] + (bytes[1] << 8) + (bytes[2] << 16) + (bytes[3] << 24); // disp32
				bytes += 4;
				break;
			case 1:
				result.memoryAddress.reg = ECX;
				result.memoryAddress.constDisplacement = bytes[0] + (bytes[1] << 8) + (bytes[2] << 16) + (bytes[3] << 24); // disp32
				bytes += 4;
				break;
			case 2:
				result.memoryAddress.reg = EDX;
				result.memoryAddress.constDisplacement = bytes[0] + (bytes[1] << 8) + (bytes[2] << 16) + (bytes[3] << 24); // disp32
				bytes += 4;
				break;
			case 3:
				result.memoryAddress.reg = EBX;
				result.memoryAddress.constDisplacement = bytes[0] + (bytes[1] << 8) + (bytes[2] << 16) + (bytes[3] << 24); // disp32
				bytes += 4;
				break;
			case 4:
				handleSIB()
				result.memoryAddress.constDisplacement = bytes[0] + (bytes[1] << 8) + (bytes[2] << 16) + (bytes[3] << 24); // disp32
				bytes += 4;
				break;
			case 5:
				result.memoryAddress.reg = EBP;
				result.memoryAddress.constDisplacement = bytes[0] + (bytes[1] << 8) + (bytes[2] << 16) + (bytes[3] << 24); // disp32
				bytes += 4;
				break;
			case 6:
				result.memoryAddress.reg = ESI;
				result.memoryAddress.constDisplacement = bytes[0] + (bytes[1] << 8) + (bytes[2] << 16) + (bytes[3] << 24); // disp32
				bytes += 4;
				break;
			case 7:
				result.memoryAddress.reg = EDI;
				result.memoryAddress.constDisplacement = bytes[0] + (bytes[1] << 8) + (bytes[2] << 16) + (bytes[3] << 24); // disp32
				bytes += 4;
				break;
			}

			break;
		}
		}
	}

	return result;
}