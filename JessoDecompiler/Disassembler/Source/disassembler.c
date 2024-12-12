#include "../Headers/disassembler.h"
#include "../Headers/oneByteOpcodeMap.h"

unsigned char disassembleInstruction(unsigned char* bytes, unsigned char* maxBytesAddr, struct DisassemblerOptions* disassemblerOptions, struct DisassembledInstruction* result)
{
	unsigned char* startPoint = bytes;
	
	struct LegacyPrefixes legacyPrefixes = { NO_PREFIX, NO_PREFIX, NO_PREFIX, NO_PREFIX };
	if (!handleLegacyPrefixes(&bytes, maxBytesAddr, &legacyPrefixes))
	{
		return 0;
	}

	struct REXPrefix rexPrefix = { 0, 0, 0, 0, 0 };
	if (disassemblerOptions->is64BitMode && !handleREXPrefix(&bytes, maxBytesAddr, &rexPrefix))
	{
		return 0;
	}

	struct Opcode* opcode = 0;
	if (!handleOpcode(&bytes, maxBytesAddr, disassemblerOptions, &opcode))
	{
		return 0;
	}

	struct OperandsResult operands = { 0, 0, 0 };
	if (!handleOperands(&bytes, maxBytesAddr, disassemblerOptions->is64BitMode, opcode, &legacyPrefixes, &rexPrefix, &operands))
	{
		return 0;
	}

	result->opcode = opcode->mnemonic;
	result->operands = operands;
	result->legacyPrefixes = legacyPrefixes;
	result->numOfBytes = (bytes - startPoint);

	result->str = instructionToStr(result);

	return 1;
}

static const char* instructionToStr(struct DisassembledInstruction* instruction)
{
	return mnemonicStrs[instruction->opcode];
}

static unsigned char handleLegacyPrefixes(unsigned char** bytesPtr, unsigned char* maxBytesAddr, struct LegacyPrefixes* result)
{
	unsigned char* bytes = *bytesPtr;
	
	for (int i = 0; i < 4; i++) // up to four prefixes
	{
		if (bytes > maxBytesAddr) { return 0; }

		unsigned char byte = bytes[0];
		bytes++;

		switch (byte)
		{
		case 0xF0:
			result->group1 = LOCK;
			break;
		case 0xF2:
			result->group1 = REPNZ;
			break;
		case 0xF3:
			result->group1 = REPZ;
			break;
		case 0x2E:
			result->group2 = CSO_BNT;
			break;
		case 0x36:
			result->group2 = SSO;
			break;
		case 0x3E:
			result->group2 = DSO_BT;
			break;
		case 0x26:
			result->group2 = ESO;
			break;
		case 0x64:
			result->group2 = FSO;
			break;
		case 0x65:
			result->group2 = GSO;
			break;
		case 0x66:
			result->group3 = OSO;
			break;
		case 0x67:
			result->group4 = ASO;
			break;
		default:
			bytes--;
			return 1;
		}
	}

	return 1;
}

static unsigned char handleREXPrefix(unsigned char** bytesPtr, unsigned char* maxBytesAddr, struct REXPrefix* result)
{
	unsigned char* bytes = *bytesPtr;
	
	if (bytes > maxBytesAddr) { return 0; }

	unsigned char rexByte = bytes[0];
	bytes++;

	if (rexByte < 0x40 || rexByte > 0x4F) { return 1; }

	result->isValidREX = 1;
	result->w = (rexByte >> 3) & 0x01;
	result->r = (rexByte >> 2) & 0x01;
	result->x = (rexByte >> 1) & 0x01;
	result->b = (rexByte >> 0) & 0x01;

	return 1;
}

static unsigned char handleOpcode(unsigned char** bytesPtr, unsigned char* maxBytesAddr, struct DisassemblerOptions* disassemblerOptions, struct Opcode** result)
{
	unsigned char* bytes = *bytesPtr;

	if (bytes > maxBytesAddr) { return 0; }
	
	// check the opcode map in use
	if (bytes[0] == 0x0F)
	{
		if ((bytes + 2) > maxBytesAddr && bytes[1] == 0x3A) // sequence: 0x0F 0x3A opcode
		{
			//result.opcode = &threeByteMap2[bytes[2]];
			bytes += 3;
		}
		else if ((bytes + 2) > maxBytesAddr && bytes[1] == 0x38) // sequence: 0x0F 0x38 opcode
		{
			//result.opcode = &threeByteMap[bytes[2]];
			bytes += 3;
		}
		else if((bytes + 1) > maxBytesAddr) // sequence: 0x0F opcode
		{
			//result.opcode = &twoByteMap[bytes[1]];
			bytes += 2;
		}
		else 
		{
			return 0;
		}
	}
	else if (bytes > maxBytesAddr) // sequence: opcode
	{
		*result = &oneByteMap[bytes[0]];

		if (disassemblerOptions->is64BitMode && bytes[0] == 0x63)
		{
			*result = &alternateX63;
		}

		bytes++;
	}
	else
	{
		return 0;
	}

	return 1;
}

static unsigned char handleOperands(unsigned char** bytesPtr, unsigned char* maxBytesAddr, unsigned char is64BitMode, struct Opcode* opcode, struct LegacyPrefixes* legPrefixes, struct REXPrefix* rexPrefix, struct OperandsResult* result)
{
	unsigned char* bytes = *bytesPtr;

	for (int i = 0; i < 3; i++)
	{
		enum OperandCode currentOperandCode;
		union Operand* currentOperand;

		if (i == 0)
		{
			currentOperandCode = opcode->operand1;
			currentOperand = &(result->operand1);
		}
		else if (i == 1)
		{
			currentOperandCode = opcode->operand2;
			currentOperand = &(result->operand2);
		}
		else
		{
			currentOperandCode = opcode->operand3;
			currentOperand = &(result->operand3);
		}

		currentOperand->reg = NO_REG;
		currentOperand->memoryAddress.segment = NO_SEGMENT;
		currentOperand->memoryAddress.reg = NO_REG;
		currentOperand->memoryAddress.regDisplacement = NO_REG;
		currentOperand->memoryAddress.constDisplacement = 0;
		currentOperand->memoryAddress.scale = 1;

		unsigned char modRMByte = 0;
		if (bytes > maxBytesAddr) 
		{
			modRMByte = bytes[0]; // may or may not actually be a modrm byte
		}

		char hasGotModRM = 0;

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
			if (!handleModRM(&bytes, maxBytesAddr, hasGotModRM, &modRMByte, 0, 1, legPrefixes->group4 == ASO, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Ev:
			if (!handleModRM(&bytes, maxBytesAddr, hasGotModRM, &modRMByte, 0, legPrefixes->group3 == OSO ? 2 : is64BitMode ? 8 : 4, legPrefixes->group4 == ASO, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Ew:
			if (!handleModRM(&bytes, maxBytesAddr, hasGotModRM, &modRMByte, 0, 2, legPrefixes->group4 == ASO, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Gb:
			if (!handleModRM(&bytes, maxBytesAddr, hasGotModRM, &modRMByte, 1, 1, 0, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Gv:
			if (!handleModRM(&bytes, maxBytesAddr, hasGotModRM, &modRMByte, 1, legPrefixes->group3 == OSO ? 2 : is64BitMode ? 8 : 4, 0, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Gz:
			if(!handleModRM(&bytes, maxBytesAddr, hasGotModRM, &modRMByte, 1, legPrefixes->group3 == OSO ? 2 : 4, 0, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Gw:
			if (!handleModRM(&bytes, maxBytesAddr, hasGotModRM, &modRMByte, 1, 2, 0, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case M:
		case Mp:
		case Ma:
			if (!handleModRM(&bytes, maxBytesAddr, hasGotModRM, &modRMByte, 0, 0, legPrefixes->group4 == ASO, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Ib:
			currentOperand->immediate = getUIntFromBytes(&bytes, maxBytesAddr, 1);
			break;
		case Iv:
			currentOperand->immediate = getUIntFromBytes(&bytes, maxBytesAddr, legPrefixes->group3 == OSO ? 2 : is64BitMode ? 8 : 4);
			break;
		case Iz:
			currentOperand->immediate = getUIntFromBytes(&bytes, maxBytesAddr, legPrefixes->group3 == OSO ? 2 : 4);
			break;
		case Iw:
			currentOperand->immediate = getUIntFromBytes(&bytes, maxBytesAddr, 2);
			break;
		case Yb:
			currentOperand->memoryAddress.segment = ES;
			currentOperand->memoryAddress.reg = DI;
			break;
		case Yv:
			currentOperand->memoryAddress.segment = ES;
			currentOperand->memoryAddress.reg = legPrefixes->group3 == OSO ? DI : is64BitMode ? RDI : EDI;
			break;
		case Yz:
			currentOperand->memoryAddress.segment = ES;
			currentOperand->memoryAddress.reg = legPrefixes->group3 == OSO ? DI : EDI;
			break;
		case Xb:
			currentOperand->memoryAddress.segment = DS;
			currentOperand->memoryAddress.reg = SI;
			break;
		case Xv:
			currentOperand->memoryAddress.segment = DS;
			currentOperand->memoryAddress.reg = legPrefixes->group3 == OSO ? SI : is64BitMode ? RSI : ESI;
			break;
		case Xz:
			currentOperand->memoryAddress.segment = DS;
			currentOperand->memoryAddress.reg = legPrefixes->group3 == OSO ? SI : ESI;
			break;
		case Jb:
			currentOperand->immediate = (unsigned char)(getUIntFromBytes(&bytes, maxBytesAddr, 1) + 2); // add 2 because that is this instruction's size
			break;
		case Jz:
			currentOperand->immediate = (unsigned char)(getUIntFromBytes(&bytes, maxBytesAddr, legPrefixes->group3 == OSO ? 2 : 4) + (legPrefixes->group3 == OSO ? 3 : 5)); // add 3 or 5 because that is this instruction's size
			break;
		case Ob:
			currentOperand->memoryAddress.segment = legPrefixes->group2 == NO_PREFIX ? DS : (enum Segment)legPrefixes->group2;
			currentOperand->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, maxBytesAddr, 1);
			break;
		case Ov:
			currentOperand->memoryAddress.segment = legPrefixes->group2 == NO_PREFIX ? DS : (enum Segment)legPrefixes->group2;
			currentOperand->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, maxBytesAddr, legPrefixes->group3 == OSO ? 2 : is64BitMode ? 8 : 4);
			break;
		case Sw:
			if (!handleModRM(&bytes, maxBytesAddr, hasGotModRM, &modRMByte, 2, 2, 0, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Ap:
			currentOperand->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, maxBytesAddr, 4);
			currentOperand->memoryAddress.constSegment = getUIntFromBytes(&bytes, maxBytesAddr, 2);
			break;
		}
	}
}

static unsigned char handleModRM(unsigned char** bytesPtr, unsigned char* maxBytesAddr, char hasGotModRM, unsigned char* modRMByteRef, char getRegOrSeg, unsigned char operandSize, char addressSizeOverride, union Operand* result)
{
	unsigned char* bytes = *bytesPtr;
	unsigned char modRMByte = *modRMByteRef;

	if (!hasGotModRM) 
	{
		if (bytes > maxBytesAddr) { return 0; }
		modRMByte = bytes[0];
		bytes++;
		(*modRMByteRef) = modRMByte;
	}
	
	unsigned char mod = (((modRMByte >> 7) & 0x01) * 2) + ((modRMByte >> 6) & 0x01);
	unsigned char reg = (((modRMByte >> 5) & 0x01) * 4) + (((modRMByte >> 4) & 0x01) * 2) + ((modRMByte >> 3) & 0x01);
	unsigned char rm = (((modRMByte >> 2) & 0x01) * 4) + (((modRMByte >> 1) & 0x01) * 2) + ((modRMByte >> 0) & 0x01);

	if (getRegOrSeg == 1)
	{
		switch (operandSize)
		{
		case 1:
			result->reg = (enum Register)(reg + AL);
			break;
		case 2:
			result->reg = (enum Register)(reg + AX);
			break;
		case 4:
			result->reg = (enum Register)(reg + EAX);
			break;
		case 8:
			result->reg = (enum Register)(reg + RAX);
			break;
		}

		return result;
	}
	else if (getRegOrSeg == 2)
	{
		result->segment = (enum Segment)reg;

		return result;
	}
	else if (mod == 3)
	{
		switch (rm)
		{
		case 0:
			result->reg = operandSize == 4 ? EAX : operandSize == 2 ? AX : AL;
			break;
		case 1:
			result->reg = operandSize == 4 ? ECX : operandSize == 2 ? CX : CL;
			break;
		case 2:
			result->reg = operandSize == 4 ? EDX : operandSize == 2 ? DX : DL;
			break;
		case 3:
			result->reg = operandSize == 4 ? EBX : operandSize == 2 ? BX : BL;
			break;
		case 4:
			result->reg = operandSize == 4 ? ESP : operandSize == 2 ? SP : AH;
			break;
		case 5:
			result->reg = operandSize == 4 ? EBP : operandSize == 2 ? BP : CH;
			break;
		case 6:
			result->reg = operandSize == 4 ? ESI : operandSize == 2 ? SI : DH;
			break;
		case 7:
			result->reg = operandSize == 4 ? EDI : operandSize == 2 ? DI : BH;
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
				result->memoryAddress.reg = BX;
				result->memoryAddress.regDisplacement = SI;
				break;
			case 1:
				result->memoryAddress.reg = BX;
				result->memoryAddress.regDisplacement = DI;
				break;
			case 2:
				result->memoryAddress.reg = BP;
				result->memoryAddress.regDisplacement = SI;
				break;
			case 3:
				result->memoryAddress.reg = BP;
				result->memoryAddress.regDisplacement = DI;
				break;
			case 4:
				result->memoryAddress.reg = SI;
				break;
			case 5:
				result->memoryAddress.reg = DI;
				break;
			case 6:
				if ((bytes + 1) > maxBytesAddr) { return 0; }
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 2); // disp16
				break;
			case 7:
				result->memoryAddress.reg = BX;
				break;
			}

			break;
		}
		case 1:
		{
			switch (rm)
			{
			case 0:
				if (bytes > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BX;
				result->memoryAddress.regDisplacement = SI;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 1); // disp8
				break;
			case 1:
				if (bytes > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BX;
				result->memoryAddress.regDisplacement = DI;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 1); // disp8
				break;
			case 2:
				if (bytes > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BP;
				result->memoryAddress.regDisplacement = SI;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 1); // disp8
				break;
			case 3:
				if (bytes > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BP;
				result->memoryAddress.regDisplacement = DI;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 1); // disp8
				break;
			case 4:
				if (bytes > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = SI;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 1); // disp8
				break;
			case 5:
				if (bytes > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = DI;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 1); // disp8
				break;
			case 6:
				if (bytes > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BP;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 1); // disp8
				break;
			case 7:
				if (bytes > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BX;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 1); // disp8
				break;
			}

			break;
		}
		case 2:
		{
			switch (rm)
			{
			case 0:
				if ((bytes + 1) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BX;
				result->memoryAddress.regDisplacement = SI;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 2); // disp16
				break;
			case 1:
				if ((bytes + 1) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BX;
				result->memoryAddress.regDisplacement = DI;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 2); // disp16
				break;
			case 2:
				if ((bytes + 1) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BP;
				result->memoryAddress.regDisplacement = SI;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 2); // disp16
				break;
			case 3:
				if ((bytes + 1) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BP;
				result->memoryAddress.regDisplacement = DI;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 2); // disp16
				break;
			case 4:
				if ((bytes + 1) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = SI;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 2); // disp16
				break;
			case 5:
				if ((bytes + 1) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = DI;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 2); // disp16
				break;
			case 6:
				if ((bytes + 1) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BP;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 2); // disp16
				break;
			case 7:
				if ((bytes + 1) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BX;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 2); // disp16
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
				result->memoryAddress.reg = EAX;
				break;
			case 1:
				result->memoryAddress.reg = ECX;
				break;
			case 2:
				result->memoryAddress.reg = EDX;
				break;
			case 3:
				result->memoryAddress.reg = EBX;
				break;
			case 4:
				if (bytes > maxBytesAddr) { return 0; }
				handleSIB(&bytes, result);
				break;
			case 5:
				if ((bytes + 3) > maxBytesAddr) { return 0; }
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 4); // disp32
				break;
			case 6:
				result->memoryAddress.reg = ESI;
				break;
			case 7:
				result->memoryAddress.reg = EDI;
				break;
			}

			break;
		}
		case 1:
		{
			switch (rm)
			{
			case 0:
				if (bytes > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EAX;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 1); // disp8
				break;
			case 1:
				if (bytes > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = ECX;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 1); // disp8
				break;
			case 2:
				if (bytes > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EDX;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 1); // disp8
				break;
			case 3:
				if (bytes > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EBX;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 1); // disp8
				break;
			case 4:
				if ((bytes + 1) > maxBytesAddr) { return 0; }
				handleSIB(&bytes, result);
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 1); // disp8
				break;
			case 5:
				if (bytes > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EBP;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 1); // disp8
				break;
			case 6:
				if (bytes > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = ESI;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 1); // disp8
				break;
			case 7:
				if (bytes > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EDI;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 1); // disp8
				break;
			}

			break;
		}
		case 2:
		{
			switch (rm)
			{
			case 0:
				if ((bytes + 3) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EAX;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 4); // disp32
				break;
			case 1:
				if ((bytes + 3) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = ECX;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 4); // disp32
				break;
			case 2:
				if ((bytes + 3) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EDX;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 4); // disp32
				break;
			case 3:
				if ((bytes + 3) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EBX;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 4); // disp32
				break;
			case 4:
				if ((bytes + 4) > maxBytesAddr) { return 0; }
				handleSIB(&bytes, result);
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 4); // disp32
				break;
			case 5:
				if ((bytes + 3) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EBP;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 4); // disp32
				break;
			case 6:
				if ((bytes + 3) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = ESI;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 4); // disp32
				break;
			case 7:
				if ((bytes + 3) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EDI;
				result->memoryAddress.constDisplacement = getUIntFromBytes(&bytes, 4); // disp32
				break;
			}

			break;
		}
		}
	}

	return 1;
}

static unsigned char* handleSIB(unsigned char** bytesPtr, union Operand* result)
{
	unsigned char* bytes = *bytesPtr;

	unsigned char sibByte = bytes[0];
	bytes++;

	unsigned char scale = (((sibByte >> 7) & 0x01) * 2) + ((sibByte >> 6) & 0x01);
	unsigned char index = (((sibByte >> 5) & 0x01) * 4) + (((sibByte >> 4) & 0x01) * 2) + ((sibByte >> 3) & 0x01);
	unsigned char base = (((sibByte >> 2) & 0x01) * 4) + (((sibByte >> 1) & 0x01) * 2) + ((sibByte >> 0) & 0x01);

	for (int i = 0; i < scale; i++)
	{
		result->memoryAddress.scale *= 2;
	}

	result->memoryAddress.reg = (enum Register)(index + EAX);
	result->memoryAddress.regDisplacement = (enum Register)(base + EAX);

	return result;
}

static unsigned long long getUIntFromBytes(unsigned char** bytesPtr, unsigned char size)
{
	unsigned long long result = 0;

	unsigned char* bytes = *bytesPtr;

	for (int i = 0; i < size; i++)
	{
		result += (bytes[0] << (8 * i));
		bytes++;
	}

	return result;
}