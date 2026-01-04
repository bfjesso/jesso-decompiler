#include "disassembler.h"
#include "registers.h"
#include "oneByteOpcodeMap.h"
#include "twoByteOpcodeMap.h"
#include "extendedOpcodeMap.h"
#include "escapeOpcodeMaps.h"

#include <stdio.h>
#include <string.h>

static const char* group1PrefixStrs[] =
{
	"LOCK",
	"REPNZ",
	"REPZ"
};

extern const char* ptrSizeStrs[] =
{
	"BYTE PTR",
	"WORD PTR",
	"DWORD PTR",
	"FWORD PTR",
	"QWORD PTR",
	"TBYTE PTR"
};

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

	struct VEXPrefix vexPrefix = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	if (disassemblerOptions->is64BitMode && !vexPrefix.isValidVEX && !handleVEXPrefix(&bytes, maxBytesAddr, &vexPrefix))
	{
		return 0;
	}

	// if the opcode is an extended one then modRM will be retrieved
	unsigned char modRMByte = 0;
	char hasGotModRM = 0;
	struct Opcode opcode = { NO_MNEMONIC, -1, 0, 0, 0 };
	if (!handleOpcode(&bytes, maxBytesAddr, &hasGotModRM, &modRMByte, disassemblerOptions, &legacyPrefixes, &rexPrefix, &opcode))
	{
		return 0;
	}

	if (!handleOperands(&bytes, maxBytesAddr, startPoint, hasGotModRM, &modRMByte, disassemblerOptions->is64BitMode, &opcode, &legacyPrefixes, &rexPrefix, &vexPrefix, (struct Operand*)(&result->operands)))
	{
		return 0;
	}

	result->opcode = opcode.mnemonic;
	result->numOfBytes = (unsigned char)(bytes - startPoint);
	result->group1Prefix = legacyPrefixes.group1;

	return 1;
}

unsigned char instructionToStr(struct DisassembledInstruction* instruction, char* buffer, unsigned char bufferSize) // this will be in intel syntax
{
	if (instruction->group1Prefix != NO_PREFIX) 
	{
		strcpy(buffer, group1PrefixStrs[instruction->group1Prefix - LOCK]);
		strcat(buffer, " ");
	}

	strcat(buffer, mnemonicStrs[instruction->opcode]);

	for (int i = 0; i < 3; i++) 
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

		char immediateBuffer[20];
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
			sprintf(immediateBuffer, "0x%llX", currentOperand->immediate);
			strcat(buffer, immediateBuffer);
			break;
		}
	}

	return strlen(buffer) < bufferSize;
}

static unsigned char memAddressToStr(struct MemoryAddress* memAddr, char* buffer, unsigned char bufferSize)
{
	if (memAddr->ptrSize != 0 && memAddr->ptrSize <= 10)
	{
		strcat(buffer, ptrSizeStrs[memAddr->ptrSize / 2]);
		strcat(buffer, " ");
	}
	
	if (memAddr->segment != NO_SEGMENT) 
	{
		strcat(buffer, segmentStrs[memAddr->segment]);
		strcat(buffer, ":");
	}
	else if (memAddr->constSegment != 0) 
	{
		char hexSeg[20];
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

		char hexScale[20];
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
		char constDisp[20];
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

static unsigned char handleLegacyPrefixes(unsigned char** bytesPtr, unsigned char* maxBytesAddr, struct LegacyPrefixes* result)
{
	result->group1 = NO_PREFIX;
	result->group2 = NO_PREFIX;
	result->group3 = NO_PREFIX;
	result->group4 = NO_PREFIX;
	
	while(1)
	{
		if ((*bytesPtr) > maxBytesAddr) { return 0; }

		unsigned char byte = (*bytesPtr)[0];
		(*bytesPtr)++;

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
			(*bytesPtr)--;
			return 1;
		}
	}

	return 1;
}

static unsigned char handleREXPrefix(unsigned char** bytesPtr, unsigned char* maxBytesAddr, struct REXPrefix* result)
{
	if ((*bytesPtr) > maxBytesAddr) { return 0; }

	unsigned char rexByte = (*bytesPtr)[0];

	if (rexByte < 0x40 || rexByte > 0x4F) { return 1; }

	result->isValidREX = 1;
	result->w = (rexByte >> 3) & 0x01;
	result->r = (rexByte >> 2) & 0x01;
	result->x = (rexByte >> 1) & 0x01;
	result->b = (rexByte >> 0) & 0x01;

	(*bytesPtr)++;

	return 1;
}

static unsigned char handleVEXPrefix(unsigned char** bytesPtr, unsigned char* maxBytesAddr, struct VEXPrefix* result)
{
	if ((*bytesPtr) > maxBytesAddr) { return 0; }

	unsigned char byte0 = (*bytesPtr)[0];
	unsigned char byte1 = (*bytesPtr)[1];
	unsigned char byte2 = (*bytesPtr)[2];

	if (byte0 == 0xC5) // two-byte form
	{ 
		result->r = (byte1 >> 7) & 0x01;
		result->vvvv = (((byte1 >> 6) & 0x01) * 8) + (((byte1 >> 5) & 0x01) * 4) + (((byte1 >> 4) & 0x01) * 2) + ((byte1 >> 3) & 0x01);
		result->l = (byte1 >> 2) & 0x01;
		result->pp = (((byte1 >> 1) & 0x01) * 2) + ((byte1 >> 0) & 0x01);
		
		(*bytesPtr) += 2;
		return 1; 
	}
	else if (byte0 == 0xC4) // three-byte form
	{
		result->r = (byte1 >> 7) & 0x01;
		result->x = (byte1 >> 6) & 0x01;
		result->b = (byte1 >> 5) & 0x01;
		result->mmmmm = (((byte1 >> 4) & 0x01) * 16) + (((byte1 >> 3) & 0x01) * 8) + (((byte1 >> 2) & 0x01) * 4) + (((byte1 >> 1) & 0x01) * 2) + ((byte1 >> 0) & 0x01);

		result->w = (byte2 >> 7) & 0x01;
		result->vvvv = (((byte2 >> 6) & 0x01) * 8) + (((byte2 >> 5) & 0x01) * 4) + (((byte2 >> 4) & 0x01) * 2) + ((byte2 >> 3) & 0x01);
		result->l = (byte2 >> 2) & 0x01;
		result->pp = (((byte2 >> 1) & 0x01) * 2) + ((byte2 >> 0) & 0x01);
		
		(*bytesPtr) += 3;
		return 1;
	}

	return 1;
}

static unsigned char handleOpcode(unsigned char** bytesPtr, unsigned char* maxBytesAddr, char* hasGotModRMRef, unsigned char* modRMByteRef, struct DisassemblerOptions* disassemblerOptions, struct LegacyPrefixes* legPrefixes, struct REXPrefix* rexPrefix, struct Opcode* result)
{
	if ((*bytesPtr) > maxBytesAddr) { return 0; }

	unsigned char opcodeByte = 0;
	
	// check the opcode map in use
	if ((*bytesPtr)[0] == 0x0F)
	{
		if (((*bytesPtr) + 2) <= maxBytesAddr && (*bytesPtr)[1] == 0x3A) // sequence: 0x0F 0x3A opcode
		{
			opcodeByte = (*bytesPtr)[2];
			//result.opcode = &threeByteMap2[opcodeByte];
			(*bytesPtr) += 3;
		}
		else if (((*bytesPtr) + 2) <= maxBytesAddr && (*bytesPtr)[1] == 0x38) // sequence: 0x0F 0x38 opcode
		{
			opcodeByte = (*bytesPtr)[2];
			//result.opcode = &threeByteMap[opcodeByte];
			(*bytesPtr) += 3;
		}
		else if(((*bytesPtr) + 1) <= maxBytesAddr) // sequence: 0x0F opcode
		{
			opcodeByte = (*bytesPtr)[1];
			unsigned char prefixByte = ((*bytesPtr) - 1)[0];
			unsigned char prefixIndex = prefixByte == 0x66 ? 1 : prefixByte == 0xF3 ? 2 : prefixByte == 0xF2 ? 3 : 0;
			if (prefixIndex != 0)
			{ 
				legPrefixes->group1 = NO_PREFIX; 
			}
			
			if (twoByteOpcodeMap[opcodeByte][prefixIndex].mnemonic == NO_MNEMONIC) 
			{
				*result = twoByteOpcodeMap[opcodeByte][0];
			}
			else 
			{
				*result = twoByteOpcodeMap[opcodeByte][prefixIndex];
			}

			(*bytesPtr) += 2;
		}
		else 
		{
			return 0;
		}
	}
	else if ((*bytesPtr) <= maxBytesAddr) // sequence: opcode
	{
		opcodeByte = (*bytesPtr)[0];
		*result = oneByteOpcodeMap[opcodeByte];

		if (disassemblerOptions->is64BitMode && opcodeByte == 0x63)
		{
			*result = alternateX63;
		}

		if (result->mnemonic == CWDE)
		{
			if (legPrefixes->group1 == OSO) 
			{
				result->mnemonic = CBW;
			}
			else if (rexPrefix->w) 
			{
				result->mnemonic = CDQE;
			}
		}

		(*bytesPtr)++;
	}
	else
	{
		return 0;
	}

	// check compatibility with 64-bit mode
	if ((disassemblerOptions->is64BitMode && result->opcodeSuperscript == i64) || (!disassemblerOptions->is64BitMode && result->opcodeSuperscript == o64))
	{
		return 0;
	}

	// handle extended opcodes or escape opcodes
	if (result->mnemonic == EXTENDED_OPCODE)
	{
		if ((*bytesPtr) > maxBytesAddr) { return 0; }
		
		unsigned char modRMByte = (*bytesPtr)[0];
		unsigned char mod = (((modRMByte >> 7) & 0x01) * 2) + ((modRMByte >> 6) & 0x01);
		unsigned char reg = (((modRMByte >> 5) & 0x01) * 4) + (((modRMByte >> 4) & 0x01) * 2) + ((modRMByte >> 3) & 0x01);
		unsigned char rm = (((modRMByte >> 2) & 0x01) * 4) + (((modRMByte >> 1) & 0x01) * 2) + ((modRMByte >> 0) & 0x01);

		const struct Opcode* extendedOpcode = 0;

		if (result->extensionGroup < 7) 
		{
			extendedOpcode = &extendedOpcodeMapThroughGroupSix[result->extensionGroup][reg];
		}
		else if (result->extensionGroup == 7) 
		{
			if (mod == 3) // 11B
			{
				extendedOpcode = &extendedOpcodeMapGroup7With11B[reg][rm];
			}
		}
		else if (result->extensionGroup == 8)
		{
			extendedOpcode = &extendedOpcodeMapGroup8[reg];
		}
		else if (result->extensionGroup == 11)
		{
			if (opcodeByte == 0xC6) 
			{
				extendedOpcode = &extendedOpcodeMapGroup11C6[reg];
			}
			else if (opcodeByte == 0xC7)
			{
				extendedOpcode = &extendedOpcodeMapGroup11C7[reg];
			}
		}

		if (extendedOpcode == 0) 
		{
			return 0;
		}

		result->mnemonic = extendedOpcode->mnemonic;

		for (int i = 0; i < 3; i++)
		{
			if (extendedOpcode->operands[i] != NO_OPERAND_CODE)
			{
				result->operands[i] = extendedOpcode->operands[i];
			}
		}

		if (extendedOpcode->opcodeSuperscript != NO_SUPERSCRIPT)
		{
			result->opcodeSuperscript = extendedOpcode->opcodeSuperscript;
		}

		if (result->extensionGroup == 3 && opcodeByte == 0xF7 && result->mnemonic == TEST)
		{
			result->operands[1] = Iz;
		}

		(*hasGotModRMRef) = 1;
		(*modRMByteRef) = modRMByte;
		(*bytesPtr)++;
	}
	else if(opcodeByte > 0xD7 && opcodeByte < 0xE0) // escape to coprocessor instruction set
	{
		if ((*bytesPtr) > maxBytesAddr) { return 0; }

		unsigned char modRMByte = (*bytesPtr)[0];
		unsigned char reg = (((modRMByte >> 5) & 0x01) * 4) + (((modRMByte >> 4) & 0x01) * 2) + ((modRMByte >> 3) & 0x01);
		
		switch (opcodeByte) 
		{
		case 0xD8:
			*result = modRMByte < 0xC0 ? escapeD8OpcodeMapBits[reg] : escapeD8OpcodeMapByte[modRMByte - 0xC0];
			break;
		case 0xD9:
			*result = modRMByte < 0xC0 ? escapeD9OpcodeMapBits[reg] : escapeD9OpcodeMapByte[modRMByte - 0xC0];
			break;
		case 0xDA:
			*result = modRMByte < 0xC0 ? escapeDAOpcodeMapBits[reg] : escapeDAOpcodeMapByte[modRMByte - 0xC0];
			break;
		case 0xDB:
			*result = modRMByte < 0xC0 ? escapeDBOpcodeMapBits[reg] : escapeDBOpcodeMapByte[modRMByte - 0xC0];
			break;
		case 0xDC:
			*result = modRMByte < 0xC0 ? escapeDCOpcodeMapBits[reg] : escapeDCOpcodeMapByte[modRMByte - 0xC0];
			break;
		case 0xDD:
			*result = modRMByte < 0xC0 ? escapeDDOpcodeMapBits[reg] : escapeDDOpcodeMapByte[modRMByte - 0xC0];
			break;
		case 0xDE:
			*result = modRMByte < 0xC0 ? escapeDEOpcodeMapBits[reg] : escapeDEOpcodeMapByte[modRMByte - 0xC0];
			break;
		case 0xDF:
			*result = modRMByte < 0xC0 ? escapeDFOpcodeMapBits[reg] : escapeDFOpcodeMapByte[modRMByte - 0xC0];
			break;
		}

		(*hasGotModRMRef) = 1;
		(*modRMByteRef) = modRMByte;
		(*bytesPtr)++;
	}

	return 1;
}

static unsigned char handleOperands(unsigned char** bytesPtr, unsigned char* maxBytesAddr, unsigned char* startBytePtr, char hasGotModRM, unsigned char* modRMByteRef, unsigned char is64BitMode, struct Opcode* opcode, struct LegacyPrefixes* legPrefixes, struct REXPrefix* rexPrefix, struct VEXPrefix* vexPrefix, struct Operand* result)
{
	for (int i = 0; i < 3; i++)
	{
		enum OperandCode currentOperandCode = opcode->operands[i];
		struct Operand* currentOperand = &(result[i]);

		currentOperand->type = NO_OPERAND;
		currentOperand->reg = NO_REG;
		currentOperand->immediate = 0;
		currentOperand->memoryAddress.ptrSize = 0;
		currentOperand->memoryAddress.segment = NO_SEGMENT;
		currentOperand->memoryAddress.reg = NO_REG;
		currentOperand->memoryAddress.regDisplacement = NO_REG;
		currentOperand->memoryAddress.constDisplacement = 0;
		currentOperand->memoryAddress.constSegment = 0;
		currentOperand->memoryAddress.scale = 1;

		if (opcode->mnemonic == NO_MNEMONIC) { break; }

		char is64BitOperandSize = 0;
		if (is64BitMode && opcode->opcodeSuperscript == d64 && legPrefixes->group3 != OSO) { is64BitOperandSize = 1; }
		else if (is64BitMode && opcode->opcodeSuperscript == f64) { is64BitOperandSize = 1; }
		else if (rexPrefix->w) { is64BitOperandSize = 1; }

		unsigned char operandSize;
		switch (currentOperandCode)
		{
		case NO_OPERAND_CODE:
			break;
		case ONE:
			currentOperand->type = IMMEDIATE;
			currentOperand->immediate = 1;
			break;
		case AL_CODE:
			currentOperand->type = REGISTER;
			currentOperand->reg = AL;
			break;
		case CL_CODE:
			currentOperand->type = REGISTER;
			currentOperand->reg = CL;
			break;
		case AX_CODE:
			currentOperand->type = REGISTER;
			currentOperand->reg = AX;
			break;
		case DX_CODE:
			currentOperand->type = REGISTER;
			currentOperand->reg = DX;
			break;
		case rAX:
			currentOperand->type = REGISTER;
			currentOperand->reg = is64BitOperandSize ? RAX : (legPrefixes->group3 == OSO ? AX : EAX);
			break;
		case rCX:
			currentOperand->type = REGISTER;
			currentOperand->reg = is64BitOperandSize ? RCX : (legPrefixes->group3 == OSO ? CX : ECX);
			break;
		case rDX:
			currentOperand->type = REGISTER;
			currentOperand->reg = is64BitOperandSize ? RDX : (legPrefixes->group3 == OSO ? DX : EDX);
			break;
		case rBX:
			currentOperand->type = REGISTER;
			currentOperand->reg = is64BitOperandSize ? RBX : (legPrefixes->group3 == OSO ? BX : EBX);
			break;
		case rSP:
			currentOperand->type = REGISTER;
			currentOperand->reg = is64BitOperandSize ? RSP : (legPrefixes->group3 == OSO ? SP : ESP);
			break;
		case rBP:
			currentOperand->type = REGISTER;
			currentOperand->reg = is64BitOperandSize ? RBP : (legPrefixes->group3 == OSO ? BP : EBP);
			break;
		case rSI:
			currentOperand->type = REGISTER;
			currentOperand->reg = is64BitOperandSize ? RSI : (legPrefixes->group3 == OSO ? SI : ESI);
			break;
		case rDI:
			currentOperand->type = REGISTER;
			currentOperand->reg = is64BitOperandSize ? RDI : (legPrefixes->group3 == OSO ? DI : EDI);
			break;
		case rAX_r8:
			currentOperand->type = REGISTER;
			currentOperand->reg = rexPrefix->b ? (is64BitOperandSize ? R8 : R8D) : (legPrefixes->group3 == OSO ? AX : (is64BitOperandSize ? RAX : EAX));
			break;
		case rCX_r9:
			currentOperand->type = REGISTER;
			currentOperand->reg = rexPrefix->b ? (is64BitOperandSize ? R9 : R9D) : (legPrefixes->group3 == OSO ? CX : (is64BitOperandSize ? RCX : ECX));
			break;
		case rDX_r10:
			currentOperand->type = REGISTER;
			currentOperand->reg = rexPrefix->b ? (is64BitOperandSize ? R10 : R10D) : (legPrefixes->group3 == OSO ? DX : (is64BitOperandSize ? RDX : EDX));
			break;
		case rBX_r11:
			currentOperand->type = REGISTER;
			currentOperand->reg = rexPrefix->b ? (is64BitOperandSize ? R11 : R11D) : (legPrefixes->group3 == OSO ? BX : (is64BitOperandSize ? RBX : EBX));
			break;
		case rSP_r12:
			currentOperand->type = REGISTER;
			currentOperand->reg = rexPrefix->b ? (is64BitOperandSize ? R12 : R12D) : (legPrefixes->group3 == OSO ? SP : (is64BitOperandSize ? RSP : ESP));
			break;
		case rBP_r13:
			currentOperand->type = REGISTER;
			currentOperand->reg = rexPrefix->b ? (is64BitOperandSize ? R13 : R13D) : (legPrefixes->group3 == OSO ? BP : (is64BitOperandSize ? RBP : EBP));
			break;
		case rSI_r14:
			currentOperand->type = REGISTER;
			currentOperand->reg = rexPrefix->b ? (is64BitOperandSize ? R14 : R14D) : (legPrefixes->group3 == OSO ? SI : (is64BitOperandSize ? RSI : ESI));
			break;
		case rDI_r15:
			currentOperand->type = REGISTER;
			currentOperand->reg = rexPrefix->b ? (is64BitOperandSize ? R15 : R15D) : (legPrefixes->group3 == OSO ? DI : (is64BitOperandSize ? RDI : EDI));
			break;
		case AL_R8B:
			currentOperand->type = REGISTER;
			currentOperand->reg = rexPrefix->b ? R8B : AL;
			break;
		case CL_R9B:
			currentOperand->type = REGISTER;
			currentOperand->reg = rexPrefix->b ? R9B : CL;
			break;
		case DL_R10B:
			currentOperand->type = REGISTER;
			currentOperand->reg = rexPrefix->b ? R10B : DL;
			break;
		case BL_R11B:
			currentOperand->type = REGISTER;
			currentOperand->reg = rexPrefix->b ? R11B : BL;
			break;
		case AH_R12B:
			currentOperand->type = REGISTER;
			currentOperand->reg = rexPrefix->b ? R12B : AH;
			break;
		case CH_R13B:
			currentOperand->type = REGISTER;
			currentOperand->reg = rexPrefix->b ? R13B : CH;
			break;
		case DH_R14B:
			currentOperand->type = REGISTER;
			currentOperand->reg = rexPrefix->b ? R14B : DH;
			break;
		case BH_R15B:
			currentOperand->type = REGISTER;
			currentOperand->reg = rexPrefix->b ? R15B : BH;
			break;
		case ST0_CODE:
			currentOperand->type = REGISTER;
			currentOperand->reg = ST0;
			break;
		case ST1_CODE:
			currentOperand->type = REGISTER;
			currentOperand->reg = ST1;
			break;
		case ST2_CODE:
			currentOperand->type = REGISTER;
			currentOperand->reg = ST2;
			break;
		case ST3_CODE:
			currentOperand->type = REGISTER;
			currentOperand->reg = ST3;
			break;
		case ST4_CODE:
			currentOperand->type = REGISTER;
			currentOperand->reg = ST4;
			break;
		case ST5_CODE:
			currentOperand->type = REGISTER;
			currentOperand->reg = ST5;
			break;
		case ST6_CODE:
			currentOperand->type = REGISTER;
			currentOperand->reg = ST6;
			break;
		case ST7_CODE:
			currentOperand->type = REGISTER;
			currentOperand->reg = ST7;
			break;
		case Eb:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_MEM_ADDRESS, 1, legPrefixes->group4 == ASO, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Ev:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_MEM_ADDRESS, legPrefixes->group3 == OSO ? 2 : is64BitOperandSize ? 8 : 4, legPrefixes->group4 == ASO, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Ew:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_MEM_ADDRESS, 2, legPrefixes->group4 == ASO, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Ep:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_MEM_ADDRESS, legPrefixes->group3 == OSO ? 4 : 6, legPrefixes->group4 == ASO, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Ey:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_MEM_ADDRESS, legPrefixes->group3 != OSO && is64BitMode ? 8 : 4, legPrefixes->group4 == ASO, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Gb:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_REGISTER, 1, 0, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Gv:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_REGISTER, legPrefixes->group3 == OSO ? 2 : is64BitOperandSize ? 8 : 4, 0, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Gz:
			if(!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_REGISTER, legPrefixes->group3 == OSO ? 2 : 4, 0, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Gw:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_REGISTER, 2, 0, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case M:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_MEM_ADDRESS, is64BitMode ? 8 : 4, legPrefixes->group4 == ASO, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Mw:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_MEM_ADDRESS, 2, legPrefixes->group4 == ASO, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Md:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_MEM_ADDRESS, 4, legPrefixes->group4 == ASO, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Ma:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_MEM_ADDRESS, legPrefixes->group3 == OSO ? 4 : 8, legPrefixes->group4 == ASO, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Mp:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_MEM_ADDRESS, legPrefixes->group3 == OSO ? 4 : 6, legPrefixes->group4 == ASO, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Mq:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_MEM_ADDRESS, 8, legPrefixes->group4 == ASO, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Mt:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_MEM_ADDRESS, 10, legPrefixes->group4 == ASO, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Ib:
			if ((*bytesPtr) > maxBytesAddr) { return 0; }
			currentOperand->type = IMMEDIATE;
			currentOperand->immediate = (char)getUIntFromBytes(bytesPtr, 1);
			break;
		case Iv:
			operandSize = legPrefixes->group3 == OSO ? 2 : is64BitOperandSize ? 8 : 4;
			if (((*bytesPtr) + operandSize - 1) > maxBytesAddr) { return 0; }
			currentOperand->type = IMMEDIATE;
			currentOperand->immediate = (long long)getUIntFromBytes(bytesPtr, operandSize);
			break;
		case Iz:
			operandSize = legPrefixes->group3 == OSO ? 2 : 4;
			if (((*bytesPtr) + operandSize - 1) > maxBytesAddr) { return 0; }
			currentOperand->type = IMMEDIATE;
			currentOperand->immediate = (int)getUIntFromBytes(bytesPtr, operandSize);
			break;
		case Iw:
			if (((*bytesPtr) + 1) > maxBytesAddr) { return 0; }
			currentOperand->type = IMMEDIATE;
			currentOperand->immediate = (short)getUIntFromBytes(bytesPtr, 2);
			break;
		case Yb:
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.segment = ES;
			currentOperand->memoryAddress.reg = DI;
			break;
		case Yv:
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.segment = ES;
			currentOperand->memoryAddress.reg = legPrefixes->group3 == OSO ? DI : is64BitOperandSize ? RDI : EDI;
			break;
		case Yz:
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.segment = ES;
			currentOperand->memoryAddress.reg = legPrefixes->group3 == OSO ? DI : EDI;
			break;
		case Xb:
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.segment = DS;
			currentOperand->memoryAddress.reg = SI;
			break;
		case Xv:
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.segment = DS;
			currentOperand->memoryAddress.reg = legPrefixes->group3 == OSO ? SI : is64BitOperandSize ? RSI : ESI;
			break;
		case Xz:
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.segment = DS;
			currentOperand->memoryAddress.reg = legPrefixes->group3 == OSO ? SI : ESI;
			break;
		case Jb:
			if ((*bytesPtr) > maxBytesAddr) { return 0; }
			currentOperand->type = IMMEDIATE;
			currentOperand->immediate = (char)(getUIntFromBytes(bytesPtr, 1) + ((*bytesPtr) - startBytePtr)); // add instruction size
			break;
		case Jz:
			operandSize = legPrefixes->group3 == OSO ? 2 : 4;
			if (((*bytesPtr) + operandSize - 1) > maxBytesAddr) { return 0; }
			currentOperand->type = IMMEDIATE;
			currentOperand->immediate = (int)getUIntFromBytes(bytesPtr, operandSize) + ((*bytesPtr) - startBytePtr); // add instruction size
			break;
		case Ob:
			if ((*bytesPtr) > maxBytesAddr) { return 0; }
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.segment = legPrefixes->group2 == NO_PREFIX ? DS : (enum Segment)legPrefixes->group2;
			currentOperand->memoryAddress.constDisplacement = (char)getUIntFromBytes(bytesPtr, 1);
			break;
		case Ov:
			operandSize = legPrefixes->group3 == OSO ? 2 : is64BitOperandSize ? 8 : 4;
			if (((*bytesPtr) + operandSize - 1) > maxBytesAddr) { return 0; }
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.segment = legPrefixes->group2 == NO_PREFIX ? DS : (enum Segment)legPrefixes->group2;
			currentOperand->memoryAddress.constDisplacement = (long long)getUIntFromBytes(bytesPtr, operandSize);
			break;
		case Sw:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_SEGMENT, 2, 0, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Ap:
			if (((*bytesPtr) + 5) > maxBytesAddr) { return 0; }
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.constDisplacement = (int)getUIntFromBytes(bytesPtr, 4);
			currentOperand->memoryAddress.constSegment = (unsigned short)getUIntFromBytes(bytesPtr, 2);
			break;
		case Pd:
			if(!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_MMX_REG, 4, 0, 0, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Qpi:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_MMX_REG, 8, 0, 0, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Vps:
		case Vpd:
		case Vx:
		case Vy:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_REGISTER, opcode->opcodeSuperscript == f256 ? 32 : 16, legPrefixes->group4 == ASO, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Vss:
		case Vsd:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_REGISTER, 16, legPrefixes->group4 == ASO, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Wps:
		case Wpd:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_MEM_ADDRESS, opcode->opcodeSuperscript == f256 ? 32 : 16, legPrefixes->group4 == ASO, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Wss:
		case Wsd:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_MEM_ADDRESS, 16, legPrefixes->group4 == ASO, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Hps:
		case Hpd:
		case Hx:
			if (!vexPrefix->isValidVEX) { break; }
			currentOperand->type = REGISTER;
			currentOperand->reg = opcode->opcodeSuperscript == f256 ? (YMM0 + vexPrefix->vvvv) : (XMM0 + vexPrefix->vvvv);
			break;
		case Hss:
		case Hsd:
			if (!vexPrefix->isValidVEX) { break; }
			currentOperand->type = REGISTER;
			currentOperand->reg = (XMM0 + vexPrefix->vvvv);
			break;
		case A_BYTE:
			(*bytesPtr)++;
			break;
		}
	}

	return 1;
}

static unsigned char handleModRM(unsigned char** bytesPtr, unsigned char* maxBytesAddr, char hasGotModRM, unsigned char* modRMByteRef, enum ModRMSelection selection, unsigned char operandSize, char addressSizeOverride, unsigned char is64bitMode, struct REXPrefix* rexPrefix, struct Operand* result)
{
	unsigned char modRMByte = *modRMByteRef;

	if (!hasGotModRM) 
	{
		if ((*bytesPtr) > maxBytesAddr) { return 0; }
		modRMByte = (*bytesPtr)[0];
		(*bytesPtr)++;
		(*modRMByteRef) = modRMByte;
	}
	
	unsigned char mod = (((modRMByte >> 7) & 0x01) * 2) + ((modRMByte >> 6) & 0x01);
	unsigned char reg = (((modRMByte >> 5) & 0x01) * 4) + (((modRMByte >> 4) & 0x01) * 2) + ((modRMByte >> 3) & 0x01);
	unsigned char rm = (((modRMByte >> 2) & 0x01) * 4) + (((modRMByte >> 1) & 0x01) * 2) + ((modRMByte >> 0) & 0x01);

	if (selection == GET_REGISTER)
	{
		result->type = REGISTER;
		
		switch (operandSize)
		{
		case 1:
			result->reg = (reg + AL);
			break;
		case 2:
			result->reg = (reg + AX);
			break;
		case 4:
			result->reg = (reg + EAX);
			break;
		case 8:
			result->reg = (reg + RAX);
			break;
		case 16:
			result->reg = (reg + XMM0);
			break;
		case 32:
			result->reg = (reg + YMM0);
			break;
		}

		if (rexPrefix->r)
		{
			result->reg = extendRegister(result->reg);
		}

		return 1;
	}
	else if (selection == GET_SEGMENT)
	{
		result->type = SEGMENT;
		result->segment = reg;

		return 1;
	}
	else if(selection == GET_MMX_REG)
	{
		result->reg = (reg + MM0);
		return 1;
	}
	else if (mod == 3)
	{
		result->type = REGISTER;

		switch (rm)
		{
		case 0:
			result->reg = operandSize == 32 ? YMM0 : operandSize == 16 ? XMM0 : operandSize == 8 ? RAX : operandSize == 4 ? EAX : operandSize == 2 ? AX : AL;
			break;
		case 1:
			result->reg = operandSize == 32 ? YMM1 : operandSize == 16 ? XMM1 : operandSize == 8 ? RCX : operandSize == 4 ? ECX : operandSize == 2 ? CX : CL;
			break;
		case 2:
			result->reg = operandSize == 32 ? YMM2 : operandSize == 16 ? XMM2 : operandSize == 8 ? RDX : operandSize == 4 ? EDX : operandSize == 2 ? DX : DL;
			break;
		case 3:
			result->reg = operandSize == 32 ? YMM3 : operandSize == 16 ? XMM3 : operandSize == 8 ? RBX : operandSize == 4 ? EBX : operandSize == 2 ? BX : BL;
			break;
		case 4:
			result->reg = operandSize == 32 ? YMM4 : operandSize == 16 ? XMM4 : operandSize == 8 ? RSP : operandSize == 4 ? ESP : operandSize == 2 ? SP : AH;
			break;
		case 5:
			result->reg = operandSize == 32 ? YMM5 : operandSize == 16 ? XMM5 : operandSize == 8 ? RBP : operandSize == 4 ? EBP : operandSize == 2 ? BP : CH;
			break;
		case 6:
			result->reg = operandSize == 32 ? YMM6 : operandSize == 16 ? XMM6 : operandSize == 8 ? RSI : operandSize == 4 ? ESI : operandSize == 2 ? SI : DH;
			break;
		case 7:
			result->reg = operandSize == 32 ? YMM7 : operandSize == 16 ? XMM7 : operandSize == 8 ? RDI : operandSize == 4 ? EDI : operandSize == 2 ? DI : BH;
			break;
		}

		if (rexPrefix->b)
		{
			result->reg = extendRegister(result->reg);
		}

		return 1;
	}

	result->type = MEM_ADDRESS;
	result->memoryAddress.ptrSize = operandSize;

	unsigned char usedSIB = 0;

	if (addressSizeOverride && !is64bitMode)
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
				if (((*bytesPtr) + 1) > maxBytesAddr) { return 0; }
				result->memoryAddress.constDisplacement = (short)getUIntFromBytes(bytesPtr, 2); // disp16
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
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BX;
				result->memoryAddress.regDisplacement = SI;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			case 1:
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BX;
				result->memoryAddress.regDisplacement = DI;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			case 2:
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BP;
				result->memoryAddress.regDisplacement = SI;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			case 3:
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BP;
				result->memoryAddress.regDisplacement = DI;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			case 4:
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = SI;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			case 5:
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = DI;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			case 6:
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BP;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			case 7:
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BX;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			}

			break;
		}
		case 2:
		{
			switch (rm)
			{
			case 0:
				if (((*bytesPtr) + 1) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BX;
				result->memoryAddress.regDisplacement = SI;
				result->memoryAddress.constDisplacement = (short)getUIntFromBytes(bytesPtr, 2); // disp16
				break;
			case 1:
				if (((*bytesPtr) + 1) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BX;
				result->memoryAddress.regDisplacement = DI;
				result->memoryAddress.constDisplacement = (short)getUIntFromBytes(bytesPtr, 2); // disp16
				break;
			case 2:
				if (((*bytesPtr) + 1) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BP;
				result->memoryAddress.regDisplacement = SI;
				result->memoryAddress.constDisplacement = (short)getUIntFromBytes(bytesPtr, 2); // disp16
				break;
			case 3:
				if (((*bytesPtr) + 1) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BP;
				result->memoryAddress.regDisplacement = DI;
				result->memoryAddress.constDisplacement = (short)getUIntFromBytes(bytesPtr, 2); // disp16
				break;
			case 4:
				if (((*bytesPtr) + 1) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = SI;
				result->memoryAddress.constDisplacement = (short)getUIntFromBytes(bytesPtr, 2); // disp16
				break;
			case 5:
				if (((*bytesPtr) + 1) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = DI;
				result->memoryAddress.constDisplacement = (short)getUIntFromBytes(bytesPtr, 2); // disp16
				break;
			case 6:
				if (((*bytesPtr) + 1) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BP;
				result->memoryAddress.constDisplacement = (short)getUIntFromBytes(bytesPtr, 2); // disp16
				break;
			case 7:
				if (((*bytesPtr) + 1) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BX;
				result->memoryAddress.constDisplacement = (short)getUIntFromBytes(bytesPtr, 2); // disp16
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
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				handleSIB(bytesPtr, mod, is64bitMode, rexPrefix, result);
				usedSIB = 1;
				break;
			case 5:
				if (((*bytesPtr) + 3) > maxBytesAddr) { return 0; }
				if (is64bitMode) { result->memoryAddress.reg = RIP; }
				else { result->memoryAddress.segment = DS; }
				result->memoryAddress.constDisplacement = (int)getUIntFromBytes(bytesPtr, 4); // disp32
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
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EAX;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			case 1:
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = ECX;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			case 2:
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EDX;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			case 3:
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EBX;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			case 4:
				if (((*bytesPtr) + 1) > maxBytesAddr) { return 0; }
				handleSIB(bytesPtr, mod, is64bitMode, rexPrefix, result);
				if (result->memoryAddress.constDisplacement == 0) { result->memoryAddress.constDisplacement = (char)getUIntFromBytes(bytesPtr, 1); } // disp8
				usedSIB = 1;
				break;
			case 5:
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EBP;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			case 6:
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = ESI;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			case 7:
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EDI;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			}

			break;
		}
		case 2:
		{
			switch (rm)
			{
			case 0:
				if (((*bytesPtr) + 3) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EAX;
				result->memoryAddress.constDisplacement = (int)getUIntFromBytes(bytesPtr, 4); // disp32
				break;
			case 1:
				if (((*bytesPtr) + 3) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = ECX;
				result->memoryAddress.constDisplacement = (int)getUIntFromBytes(bytesPtr, 4); // disp32
				break;
			case 2:
				if (((*bytesPtr) + 3) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EDX;
				result->memoryAddress.constDisplacement = (int)getUIntFromBytes(bytesPtr, 4); // disp32
				break;
			case 3:
				if (((*bytesPtr) + 3) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EBX;
				result->memoryAddress.constDisplacement = (int)getUIntFromBytes(bytesPtr, 4); // disp32
				break;
			case 4:
				if (((*bytesPtr) + 4) > maxBytesAddr) { return 0; }
				handleSIB(bytesPtr, mod, is64bitMode, rexPrefix, result);
				if (result->memoryAddress.constDisplacement == 0) { result->memoryAddress.constDisplacement = (int)getUIntFromBytes(bytesPtr, 4); } // disp32
				usedSIB = 1;
				break;
			case 5:
				if (((*bytesPtr) + 3) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EBP;
				result->memoryAddress.constDisplacement = (int)getUIntFromBytes(bytesPtr, 4); // disp32
				break;
			case 6:
				if (((*bytesPtr) + 3) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = ESI;
				result->memoryAddress.constDisplacement = (int)getUIntFromBytes(bytesPtr, 4); // disp32
				break;
			case 7:
				if (((*bytesPtr) + 3) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EDI;
				result->memoryAddress.constDisplacement = (int)getUIntFromBytes(bytesPtr, 4); // disp32
				break;
			}

			break;
		}
		}
	}

	if (!usedSIB && is64bitMode && !addressSizeOverride && result->memoryAddress.reg != NO_REG && result->memoryAddress.reg != RIP)
	{
		result->memoryAddress.reg = increaseRegisterSize(result->memoryAddress.reg);

		if (rexPrefix->b)
		{
			result->memoryAddress.reg = extendRegister(result->memoryAddress.reg);
		}
	}

	return 1;
}

static unsigned char handleSIB(unsigned char** bytesPtr, unsigned char mod, unsigned char is64Bit, struct REXPrefix* rexPrefix, struct Operand* result)
{
	unsigned char sibByte = (*bytesPtr)[0];
	(*bytesPtr)++;

	unsigned char scale = (((sibByte >> 7) & 0x01) * 2) + ((sibByte >> 6) & 0x01);
	unsigned char index = (((sibByte >> 5) & 0x01) * 4) + (((sibByte >> 4) & 0x01) * 2) + ((sibByte >> 3) & 0x01);
	unsigned char base = (((sibByte >> 2) & 0x01) * 4) + (((sibByte >> 1) & 0x01) * 2) + ((sibByte >> 0) & 0x01);

	for (int i = 0; i < scale; i++)
	{
		result->memoryAddress.scale *= 2;
	}

	if (index != 4) 
	{ 
		result->memoryAddress.reg = (enum Register)(index + EAX); 
		result->memoryAddress.regDisplacement = (enum Register)(base + EAX);
	}
	else 
	{
		result->memoryAddress.reg = (enum Register)(base + EAX);
	}

	if (is64Bit)
	{
		result->memoryAddress.reg = increaseRegisterSize(result->memoryAddress.reg);
		if (result->memoryAddress.regDisplacement != NO_REG) 
		{ 
			result->memoryAddress.regDisplacement = increaseRegisterSize(result->memoryAddress.regDisplacement);
		}

		if (rexPrefix->x)
		{
			result->memoryAddress.reg = extendRegister(result->memoryAddress.reg);
		}
		if (rexPrefix->b && result->memoryAddress.regDisplacement != NO_REG)
		{
			result->memoryAddress.regDisplacement = extendRegister(result->memoryAddress.regDisplacement);
		}
	}
	
	if (base == 5)
	{
		switch (mod)
		{
		case 0:
			if (index != 4) { result->memoryAddress.regDisplacement = NO_REG; }
			else { result->memoryAddress.reg = NO_REG; }
			result->memoryAddress.constDisplacement = (int)getUIntFromBytes(bytesPtr, 4); // disp32
			break;
		case 1:
			result->memoryAddress.constDisplacement = (char)getUIntFromBytes(bytesPtr, 1); // disp8
			break;
		case 2:
			result->memoryAddress.constDisplacement = (int)getUIntFromBytes(bytesPtr, 4); // disp32
			break;
		}
	}

	return 1;
}

static unsigned long long getUIntFromBytes(unsigned char** bytesPtr, unsigned char resultSize)
{
	unsigned long long result = 0;

	for (int i = 0; i < resultSize; i++)
	{
		result += ((unsigned long long)(*bytesPtr)[0] << (8 * i));
		(*bytesPtr)++;
	}

	return result;
}

unsigned char getLastOperand(struct DisassembledInstruction* instruction)
{
	unsigned char result = 0;

	for (int i = 2; i >= 0; i--)
	{
		if (instruction->operands[i].type != NO_OPERAND)
		{
			result = i;
			break;
		}
	}

	return result;
}

unsigned char isOperandStackArgument(struct Operand* operand)
{
	return operand->type == MEM_ADDRESS && operand->memoryAddress.constDisplacement > 0 && 
		(compareRegisters(operand->memoryAddress.reg, BP) || compareRegisters(operand->memoryAddress.reg, SP));
}

unsigned char isOperandLocalVariable(struct Operand* operand)
{
	return operand->type == MEM_ADDRESS && operand->memoryAddress.constDisplacement < 0 && 
		(compareRegisters(operand->memoryAddress.reg, BP) || compareRegisters(operand->memoryAddress.reg, SP));
}

unsigned char doesInstructionModifyOperand(struct DisassembledInstruction* instruction, unsigned char operandNum, unsigned char* overwrites)
{
	if (operandNum == 0)
	{
		switch (instruction->opcode)
		{
		case MOV:
		case MOVUPS:
		case MOVUPD:
		case MOVSS:
		case MOVSD:
		case MOVSX:
		case MOVZX:
		case LEA:
		case CVTPS2PD:
		case CVTPD2PS:
		case CVTSS2SD:
		case CVTSD2SS:
			if (overwrites != 0) { *overwrites = 1; }
			return 1;
		case ADD:
		case ADDPS:
		case ADDPD:
		case ADDSS:
		case ADDSD:
		case SUB:
		case AND:
		case OR:
		case SHL:
		case SHR:
		case INC:
		case DEC:
			return 1;
		case IMUL:
			if (overwrites != 0 && instruction->operands[2].type != NO_OPERAND) { *overwrites = 1; }
			return 1;
		case XOR:
			if (overwrites != 0 && areOperandsEqual(&instruction->operands[0], &instruction->operands[1])) { *overwrites = 1; }
			return 1;
		}
	}
	else if (operandNum == 1) 
	{
		switch (instruction->opcode)
		{
		case XOR:
			if (areOperandsEqual(&instruction->operands[0], &instruction->operands[1])) 
			{ 
				if (overwrites != 0) { *overwrites = 1; }
				return 1;
			}
			return 0;
		}
	}

	return 0;
}

unsigned char doesInstructionAccessRegister(struct DisassembledInstruction* instruction, enum Register reg, unsigned char* operandNum)
{
	for (int i = 0; i < 3; i++)
	{
		struct Operand* op = &(instruction->operands[i]);
		if (!doesInstructionModifyOperand(instruction, i, 0)) 
		{
			if (operandNum != 0)
			{
				*operandNum = i;
			}
			
			if (op->type == REGISTER && compareRegisters(op->reg, reg))
			{
				return 1;
			}
			else if (op->type == MEM_ADDRESS && compareRegisters(op->memoryAddress.reg, reg))
			{
				return 1;
			}
		}
	}

	return 0;
}

unsigned char doesInstructionModifyRegister(struct DisassembledInstruction* instruction, enum Register reg)
{
	for (int i = 0; i < 3; i++)
	{
		struct Operand* op = &(instruction->operands[i]);
		if (op->type == REGISTER && compareRegisters(op->reg, reg))
		{
			if (doesInstructionModifyOperand(instruction, i, 0))
			{
				return 1;
			}
		}
	}

	return 0;
}

unsigned char areOperandsEqual(struct Operand* op1, struct Operand* op2)
{
	if (op1->type == op2->type)
	{
		struct MemoryAddress* m1 = &op1->memoryAddress;
		struct MemoryAddress* m2 = &op2->memoryAddress;

		switch (op1->type)
		{
		case NO_OPERAND:
			return 1;
		case SEGMENT:
			return op1->segment == op2->segment;
		case REGISTER:
			return op1->reg == op2->reg;
		case MEM_ADDRESS:
			return m1->reg == m2->reg && m1->constDisplacement == m2->constDisplacement && m1->ptrSize == m2->ptrSize && m1->scale == m2->scale && m1->constSegment == m2->constSegment && m1->regDisplacement == m2->regDisplacement && m1->segment == m2->segment;
		case IMMEDIATE:
			return op1->immediate == op2->immediate;
		}
	}

	return 0;
}

unsigned char operandToValue(struct DisassembledInstruction* instructions, unsigned long long* addresses, int startInstructionIndex, struct Operand* operand, unsigned long long* result)
{
	if (operand->type == IMMEDIATE)
	{
		*result = operand->immediate;
		return 1;
	}
	else if (operand->type == MEM_ADDRESS)
	{
		if (compareRegisters(operand->memoryAddress.reg, IP))
		{
			*result = addresses[startInstructionIndex + 1] + operand->memoryAddress.constDisplacement;
			return 1;
		}
		else if (operand->memoryAddress.reg == NO_REG)
		{
			*result = operand->memoryAddress.constDisplacement;
			return 1;
		}
		else
		{
			struct Operand baseReg = { 0 };
			baseReg.type = REGISTER;
			baseReg.reg = operand->memoryAddress.reg;

			unsigned long long regValue = 0;
			if (!operandToValue(instructions, addresses, startInstructionIndex, &baseReg, &regValue))
			{
				return 0;
			}

			*result = regValue + operand->memoryAddress.constDisplacement;
		}

		return 1;
	}
	else if (operand->type == REGISTER)
	{
		for (int i = startInstructionIndex - 1; i >= 0; i--)
		{
			if (instructions[i].opcode == MOV && compareRegisters(instructions[i].operands[0].reg, operand->reg))
			{
				return operandToValue(instructions, addresses, i, &(instructions[i].operands[1]), result);
			}
		}

		return 0;
	}

	return 0;
}
