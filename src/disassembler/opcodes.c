#include "disassembler.h"
#include "opcodes.h"
#include "mnemonics.h"
#include "prefixes.h"

#include "oneByteOpcodeMap.h"
#include "twoByteOpcodeMap.h"
#include "threeByteOpcodeMaps.h"
#include "extendedOpcodeMap.h"
#include "escapeOpcodeMaps.h"

unsigned char handleOpcode(struct DisassemblyParameters* params, struct Opcode* result)
{
	if (params->bytes > params->maxBytesAddr) { return 0; }

	unsigned char prefixIndex = params->legPrefixes->group3 == OSO ? 1 : params->legPrefixes->group1 == REPZ ? 2 : params->legPrefixes->group1 == REPNZ ? 3 : 0;
	unsigned char opcodeByte = 0;
	unsigned char escapeToCoprocessor = 0;

	int mmm = params->vexPrefix->mmmmm != 0 ? params->vexPrefix->mmmmm : params->evexPrefix->mmm;
	if (params->bytes[0] == 0x0F || mmm != 0)
	{
		if ((params->bytes + 2) <= params->maxBytesAddr && (params->bytes[1] == 0x38 || mmm == 0b00010)) // sequence: 0x0F 0x38 opcode
		{
			opcodeByte = params->bytes[mmm != 0 ? 0 : 2];
			
			if (prefixIndex != 0)
			{
				params->legPrefixes->group1 = NO_PREFIX;
			}

			if (threeByteOpcodeMap38[opcodeByte][prefixIndex].mnemonic == NO_MNEMONIC)
			{
				*result = threeByteOpcodeMap38[opcodeByte][0];
			}
			else
			{
				*result = threeByteOpcodeMap38[opcodeByte][prefixIndex];
			}

			params->bytes += mmm != 0 ? 1 : 3;
		}
		else if ((params->bytes + 2) <= params->maxBytesAddr && (params->bytes[1] == 0x3A || mmm == 0b00011)) // sequence: 0x0F 0x3A opcode
		{
			opcodeByte = params->bytes[mmm != 0 ? 0 : 2];

			if (prefixIndex != 0)
			{
				params->legPrefixes->group1 = NO_PREFIX;
			}

			if (threeByteOpcodeMap3A[opcodeByte][prefixIndex].mnemonic == NO_MNEMONIC)
			{
				*result = threeByteOpcodeMap3A[opcodeByte][0];
			}
			else
			{
				*result = threeByteOpcodeMap3A[opcodeByte][prefixIndex];
			}

			params->bytes += mmm != 0 ? 1 : 3;
		}
		else if ((params->bytes + 1) <= params->maxBytesAddr) // sequence: 0x0F opcode
		{
			opcodeByte = params->bytes[mmm != 0 ? 0 : 1];

			if (prefixIndex != 0)
			{
				params->legPrefixes->group1 = NO_PREFIX;
			}

			if (twoByteOpcodeMap[opcodeByte][prefixIndex].mnemonic == NO_MNEMONIC)
			{
				*result = twoByteOpcodeMap[opcodeByte][0];
			}
			else
			{
				*result = twoByteOpcodeMap[opcodeByte][prefixIndex];
			}

			params->bytes += mmm != 0 ? 1 : 2;
		}
		else
		{
			return 0;
		}
	}
	else if (params->bytes <= params->maxBytesAddr) // sequence: opcode
	{
		opcodeByte = params->bytes[0];
		*result = oneByteOpcodeMap[opcodeByte];

		if (params->is64BitMode && opcodeByte == 0x63)
		{
			*result = alternateX63;
		}

		escapeToCoprocessor = opcodeByte > 0xD7 && opcodeByte < 0xE0;

		params->bytes++;
	}
	else
	{
		return 0;
	}

	// handle extended opcodes or escape opcodes
	if (result->mnemonic == EXTENDED_OPCODE)
	{
		if (params->bytes > params->maxBytesAddr) { return 0; }

		unsigned char modRMByte = params->bytes[0];
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
			if (mod == 0b11)
			{
				extendedOpcode = &extendedOpcodeMapGroup711B[reg][rm];
			}
			else 
			{
				extendedOpcode = &extendedOpcodeMapGroup7[reg];
			}
		}
		else if (result->extensionGroup == 8)
		{
			extendedOpcode = &extendedOpcodeMapGroup8[reg];
		}
		else if (result->extensionGroup == 9)
		{
			if (mod == 0b11)
			{
				if (params->legPrefixes->group1 == REPZ)
				{
					extendedOpcode = &extendedOpcodeMapGroup9F311B[reg];
				}
				else
				{
					extendedOpcode = &extendedOpcodeMapGroup911B[reg];
				}
			}
			else 
			{
				if (params->legPrefixes->group3 == OSO)
				{
					extendedOpcode = &extendedOpcodeMapGroup966[reg];
				}
				else if (params->legPrefixes->group1 == REPZ)
				{
					extendedOpcode = &extendedOpcodeMapGroup9F3[reg];
				}
				else 
				{
					extendedOpcode = &extendedOpcodeMapGroup9[reg];

					if(extendedOpcode->mnemonic == CMPXCH8B && params->rexPrefix->w)
					{
						extendedOpcode = &alternateCMPXCH;
					}
				}
			}
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
		else if (result->extensionGroup == 12)
		{
			if (mod == 0b11)
			{
				if (params->legPrefixes->group3 == OSO)
				{
					extendedOpcode = &extendedOpcodeMapGroup126611B[reg];
				}
				else
				{
					extendedOpcode = &extendedOpcodeMapGroup1211B[reg];
				}
			}
		}
		else if (result->extensionGroup == 13)
		{
			if (mod == 0b11)
			{
				if (params->legPrefixes->group3 == OSO)
				{
					extendedOpcode = &extendedOpcodeMapGroup136611B[reg];
				}
				else
				{
					extendedOpcode = &extendedOpcodeMapGroup1311B[reg];
				}
			}
		}
		else if (result->extensionGroup == 14)
		{
			if (mod == 0b11)
			{
				if (params->legPrefixes->group3 == OSO)
				{
					extendedOpcode = &extendedOpcodeMapGroup146611B[reg];
				}
				else
				{
					extendedOpcode = &extendedOpcodeMapGroup1411B[reg];
				}
			}
		}
		else if (result->extensionGroup == 15)
		{
			if (mod == 0b11)
			{
				if (params->legPrefixes->group1 == REPZ)
				{
					extendedOpcode = &extendedOpcodeMapGroup15F311B[reg];
				}
				else
				{
					extendedOpcode = &extendedOpcodeMapGroup1511B[reg];
				}
			}
			else
			{
				extendedOpcode = &extendedOpcodeMapGroup15[reg];
			}
		}
		else if (result->extensionGroup == 16)
		{
			if (mod != 0b11)
			{
				extendedOpcode = &extendedOpcodeMapGroup16[reg];
			}
		}
		else if (result->extensionGroup == 17)
		{
			extendedOpcode = &extendedOpcodeMapGroup17[reg];
		}

		if (extendedOpcode == 0)
		{
			return 0;
		}

		result->mnemonic = extendedOpcode->mnemonic;

		for (int i = 0; i < 4; i++)
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

		params->hasGotModRM = 1;
		params->modRMByte= modRMByte;
		params->bytes++;
	}
	else if (escapeToCoprocessor) // escape to coprocessor instruction set
	{
		if (params->bytes > params->maxBytesAddr) { return 0; }

		unsigned char modRMByte = params->bytes[0];
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

		params->hasGotModRM = 1;
		params->modRMByte = modRMByte;
		params->bytes++;
	}

	handleAlternateMnemonics(params, result);

	return 1;
}

static void handleAlternateMnemonics(struct DisassemblyParameters* params, struct Opcode* opcode)
{
	switch (opcode->mnemonic) 
	{
	case CWDE:
		opcode->mnemonic = params->rexPrefix->w ? CDQE : params->legPrefixes->group1 == OSO ? CBW : CWDE;
		break;
	case VINSERTI32X8:
		opcode->mnemonic = params->evexPrefix->w ? VINSERTI64X4 : VINSERTI32X8;
		break;
	case VSHUFI32X4:
		opcode->mnemonic = params->evexPrefix->w ? VSHUFI64X2 : VSHUFI32X4;
		break;
	}
}