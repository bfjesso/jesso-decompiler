#include "disassembler.h"
#include "opcodes.h"
#include "mnemonics.h"
#include "prefixes.h"

#include "oneByteOpcodeMap.h"
#include "twoByteOpcodeMap.h"
#include "threeByteOpcodeMaps.h"
#include "extendedOpcodeMap.h"
#include "escapeOpcodeMaps.h"

unsigned char handleOpcode(unsigned char** bytesPtr, unsigned char* maxBytesAddr, char* hasGotModRMRef, unsigned char* modRMByteRef, struct DisassemblerOptions* disassemblerOptions, struct LegacyPrefixes* legPrefixes, struct REXPrefix* rexPrefix, struct Opcode* result)
{
	if ((*bytesPtr) > maxBytesAddr) { return 0; }

	unsigned char legPrefixByte = rexPrefix->isValidREX ? ((*bytesPtr) - 2)[0] : ((*bytesPtr) - 1)[0];
	unsigned char prefixIndex = legPrefixByte == 0x66 ? 1 : legPrefixByte == 0xF3 ? 2 : legPrefixByte == 0xF2 ? 3 : 0;
	unsigned char opcodeByte = 0;
	unsigned char escapeToCoprocessor = 0;

	// check the opcode map in use
	if ((*bytesPtr)[0] == 0x0F)
	{
		if (((*bytesPtr) + 2) <= maxBytesAddr && (*bytesPtr)[1] == 0x38) // sequence: 0x0F 0x38 opcode
		{
			opcodeByte = (*bytesPtr)[2];
			
			if (prefixIndex != 0)
			{
				legPrefixes->group1 = NO_PREFIX;
			}

			if (threeByteOpcodeMap38[opcodeByte][prefixIndex].mnemonic == NO_MNEMONIC)
			{
				*result = threeByteOpcodeMap38[opcodeByte][0];
			}
			else
			{
				*result = threeByteOpcodeMap38[opcodeByte][prefixIndex];
			}

			(*bytesPtr) += 3;
		}
		else if (((*bytesPtr) + 2) <= maxBytesAddr && (*bytesPtr)[1] == 0x3A) // sequence: 0x0F 0x3A opcode
		{
			opcodeByte = (*bytesPtr)[2];

			if (prefixIndex != 0)
			{
				legPrefixes->group1 = NO_PREFIX;
			}

			if (threeByteOpcodeMap3A[opcodeByte][prefixIndex].mnemonic == NO_MNEMONIC)
			{
				*result = threeByteOpcodeMap3A[opcodeByte][0];
			}
			else
			{
				*result = threeByteOpcodeMap3A[opcodeByte][prefixIndex];
			}

			(*bytesPtr) += 3;
		}
		else if (((*bytesPtr) + 1) <= maxBytesAddr) // sequence: 0x0F opcode
		{
			opcodeByte = (*bytesPtr)[1];

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

		escapeToCoprocessor = opcodeByte > 0xD7 && opcodeByte < 0xE0;

		(*bytesPtr)++;
	}
	else
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
			if (mod == 0b11)
			{
				extendedOpcode = &extendedOpcodeMapGroup7With11B[reg][rm];
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
				if (legPrefixByte == 0xF3)
				{
					extendedOpcode = &extendedOpcodeMapGroup9F311B[reg];
				}
				else
				{
					extendedOpcode = &extendedOpcodeMapGroup911B[reg];
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
		else if (result->extensionGroup == 14)
		{
			if (mod == 0b11)
			{
				if (legPrefixByte == 0x66)
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
				if (legPrefixByte == 0xF3)
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

		(*hasGotModRMRef) = 1;
		(*modRMByteRef) = modRMByte;
		(*bytesPtr)++;
	}
	else if (escapeToCoprocessor) // escape to coprocessor instruction set
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
