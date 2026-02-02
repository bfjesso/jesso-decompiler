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
			params->bytes += mmm != 0 ? 1 : 3;
			
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
		}
		else if ((params->bytes + 2) <= params->maxBytesAddr && (params->bytes[1] == 0x3A || mmm == 0b00011)) // sequence: 0x0F 0x3A opcode
		{
			opcodeByte = params->bytes[mmm != 0 ? 0 : 2];
			params->bytes += mmm != 0 ? 1 : 3;

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
		}
		else if ((params->bytes + 1) <= params->maxBytesAddr) // sequence: 0x0F opcode
		{
			opcodeByte = params->bytes[mmm != 0 ? 0 : 1];
			params->bytes += mmm != 0 ? 1 : 2;

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

			if (opcodeByte == 0x12 || opcodeByte == 0x16)
			{
				params->modRM.hasGotModRM = 1;
				params->modRM.mod = (((params->bytes[0] >> 7) & 0x01) * 2) + ((params->bytes[0] >> 6) & 0x01);
				params->modRM.reg = (((params->bytes[0] >> 5) & 0x01) * 4) + (((params->bytes[0] >> 4) & 0x01) * 2) + ((params->bytes[0] >> 3) & 0x01);
				params->modRM.rm = (((params->bytes[0] >> 2) & 0x01) * 4) + (((params->bytes[0] >> 1) & 0x01) * 2) + ((params->bytes[0] >> 0) & 0x01);
				params->bytes++;

				if (params->modRM.mod == 0b11)
				{
					if (opcodeByte == 0x12) 
					{
						*result = alternateX12;
					}
					else 
					{
						*result = alternateX16;
					}
				}
			}
		}
		else
		{
			return 0;
		}
	}
	else if (params->bytes <= params->maxBytesAddr) // sequence: opcode
	{
		opcodeByte = params->bytes[0];
		params->bytes++;

		*result = oneByteOpcodeMap[opcodeByte];

		if (params->is64BitMode && opcodeByte == 0x63)
		{
			*result = alternateX63;
		}

		escapeToCoprocessor = opcodeByte > 0xD7 && opcodeByte < 0xE0;
	}
	else
	{
		return 0;
	}

	// handle extended opcodes or escape opcodes
	if (result->mnemonic == EXTENDED_OPCODE)
	{
		if (params->bytes > params->maxBytesAddr) { return 0; }

		params->modRM.hasGotModRM = 1;
		params->modRM.mod = (((params->bytes[0] >> 7) & 0x01) * 2) + ((params->bytes[0] >> 6) & 0x01);
		params->modRM.reg = (((params->bytes[0] >> 5) & 0x01) * 4) + (((params->bytes[0] >> 4) & 0x01) * 2) + ((params->bytes[0] >> 3) & 0x01);
		params->modRM.rm = (((params->bytes[0] >> 2) & 0x01) * 4) + (((params->bytes[0] >> 1) & 0x01) * 2) + ((params->bytes[0] >> 0) & 0x01);
		params->bytes++;

		const struct Opcode* extendedOpcode = 0;

		if (result->extensionGroup < 7)
		{
			extendedOpcode = &extendedOpcodeMapThroughGroupSix[result->extensionGroup][params->modRM.reg];
		}
		else if (result->extensionGroup == 7)
		{
			if (params->modRM.mod == 0b11)
			{
				extendedOpcode = &extendedOpcodeMapGroup711B[params->modRM.reg][params->modRM.rm];
			}
			else 
			{
				extendedOpcode = &extendedOpcodeMapGroup7[params->modRM.reg];
			}
		}
		else if (result->extensionGroup == 8)
		{
			extendedOpcode = &extendedOpcodeMapGroup8[params->modRM.reg];
		}
		else if (result->extensionGroup == 9)
		{
			if (params->modRM.mod == 0b11)
			{
				if (params->legPrefixes->group1 == REPZ)
				{
					extendedOpcode = &extendedOpcodeMapGroup9F311B[params->modRM.reg];
				}
				else
				{
					extendedOpcode = &extendedOpcodeMapGroup911B[params->modRM.reg];
				}
			}
			else 
			{
				if (params->legPrefixes->group3 == OSO)
				{
					extendedOpcode = &extendedOpcodeMapGroup966[params->modRM.reg];
				}
				else if (params->legPrefixes->group1 == REPZ)
				{
					extendedOpcode = &extendedOpcodeMapGroup9F3[params->modRM.reg];
				}
				else 
				{
					extendedOpcode = &extendedOpcodeMapGroup9[params->modRM.reg];

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
				extendedOpcode = &extendedOpcodeMapGroup11C6[params->modRM.reg];
			}
			else if (opcodeByte == 0xC7)
			{
				extendedOpcode = &extendedOpcodeMapGroup11C7[params->modRM.reg];
			}
		}
		else if (result->extensionGroup == 12)
		{
			if (params->modRM.mod == 0b11)
			{
				if (params->legPrefixes->group3 == OSO)
				{
					extendedOpcode = &extendedOpcodeMapGroup126611B[params->modRM.reg];
				}
				else
				{
					extendedOpcode = &extendedOpcodeMapGroup1211B[params->modRM.reg];
				}
			}
		}
		else if (result->extensionGroup == 13)
		{
			if (params->modRM.mod == 0b11)
			{
				if (params->legPrefixes->group3 == OSO)
				{
					extendedOpcode = &extendedOpcodeMapGroup136611B[params->modRM.reg];
				}
				else
				{
					extendedOpcode = &extendedOpcodeMapGroup1311B[params->modRM.reg];
				}
			}
		}
		else if (result->extensionGroup == 14)
		{
			if (params->modRM.mod == 0b11)
			{
				if (params->legPrefixes->group3 == OSO)
				{
					extendedOpcode = &extendedOpcodeMapGroup146611B[params->modRM.reg];
				}
				else
				{
					extendedOpcode = &extendedOpcodeMapGroup1411B[params->modRM.reg];
				}
			}
		}
		else if (result->extensionGroup == 15)
		{
			if (params->modRM.mod == 0b11)
			{
				if (params->legPrefixes->group1 == REPZ)
				{
					extendedOpcode = &extendedOpcodeMapGroup15F311B[params->modRM.reg];
				}
				else
				{
					extendedOpcode = &extendedOpcodeMapGroup1511B[params->modRM.reg];
				}
			}
			else
			{
				extendedOpcode = &extendedOpcodeMapGroup15[params->modRM.reg];
			}
		}
		else if (result->extensionGroup == 16)
		{
			if (params->modRM.mod != 0b11)
			{
				extendedOpcode = &extendedOpcodeMapGroup16[params->modRM.reg];
			}
		}
		else if (result->extensionGroup == 17)
		{
			extendedOpcode = &extendedOpcodeMapGroup17[params->modRM.reg];
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
	}
	else if (escapeToCoprocessor) // escape to coprocessor instruction set
	{
		if (params->bytes > params->maxBytesAddr) { return 0; }

		params->modRM.hasGotModRM = 1;
		params->modRM.mod = (((params->bytes[0] >> 7) & 0x01) * 2) + ((params->bytes[0] >> 6) & 0x01);
		params->modRM.reg = (((params->bytes[0] >> 5) & 0x01) * 4) + (((params->bytes[0] >> 4) & 0x01) * 2) + ((params->bytes[0] >> 3) & 0x01);
		params->modRM.rm = (((params->bytes[0] >> 2) & 0x01) * 4) + (((params->bytes[0] >> 1) & 0x01) * 2) + ((params->bytes[0] >> 0) & 0x01);

		switch (opcodeByte)
		{
		case 0xD8:
			*result = params->bytes[0] < 0xC0 ? escapeD8OpcodeMapBits[params->modRM.reg] : escapeD8OpcodeMapByte[params->bytes[0] - 0xC0];
			break;
		case 0xD9:
			*result = params->bytes[0] < 0xC0 ? escapeD9OpcodeMapBits[params->modRM.reg] : escapeD9OpcodeMapByte[params->bytes[0] - 0xC0];
			break;
		case 0xDA:
			*result = params->bytes[0] < 0xC0 ? escapeDAOpcodeMapBits[params->modRM.reg] : escapeDAOpcodeMapByte[params->bytes[0] - 0xC0];
			break;
		case 0xDB:
			*result = params->bytes[0] < 0xC0 ? escapeDBOpcodeMapBits[params->modRM.reg] : escapeDBOpcodeMapByte[params->bytes[0] - 0xC0];
			break;
		case 0xDC:
			*result = params->bytes[0] < 0xC0 ? escapeDCOpcodeMapBits[params->modRM.reg] : escapeDCOpcodeMapByte[params->bytes[0] - 0xC0];
			break;
		case 0xDD:
			*result = params->bytes[0] < 0xC0 ? escapeDDOpcodeMapBits[params->modRM.reg] : escapeDDOpcodeMapByte[params->bytes[0] - 0xC0];
			break;
		case 0xDE:
			*result = params->bytes[0] < 0xC0 ? escapeDEOpcodeMapBits[params->modRM.reg] : escapeDEOpcodeMapByte[params->bytes[0] - 0xC0];
			break;
		case 0xDF:
			*result = params->bytes[0] < 0xC0 ? escapeDFOpcodeMapBits[params->modRM.reg] : escapeDFOpcodeMapByte[params->bytes[0] - 0xC0];
			break;
		}

		params->bytes++;
	}

	handleAlternateMnemonics(params, result);

	return 1;
}

static void handleAlternateMnemonics(struct DisassemblyParameters* params, struct Opcode* opcode) // this should only be for when the alternate mnemonic represents a different instruction
{
	switch (opcode->mnemonic) 
	{
	case CWDE:
		opcode->mnemonic = params->rexPrefix->w ? CDQE : params->legPrefixes->group1 == OSO ? CBW : CWDE;
		break;
	case CDQ:
		opcode->mnemonic = params->rexPrefix->w ? CQO : params->legPrefixes->group1 == OSO ? CWD : CDQ;
		break;
	case VMOVUPS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MOVUPS : VMOVUPS;
		break;
	case VMOVUPD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MOVUPD : VMOVUPD;
		break;
	case VMOVSS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MOVSS : VMOVSS;
		break;
	case VMOVSD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MOVSD : VMOVSD;
		break;
	case VMOVLPS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MOVLPS : VMOVLPS;
		break;
	case VMOVHLPS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MOVHLPS : VMOVHLPS;
		break;
	case VMOVLPD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MOVLPD : VMOVLPD;
		break;
	case VMOVSLDUP:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MOVSLDUP : VMOVSLDUP;
		break;
	case VMOVDDUP:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MOVDDUP : VMOVDDUP;
		break;
	case VUNPCKLPS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? UNPCKLPS : VUNPCKLPS;
		break;
	case VUNPCKLPD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? UNPCKLPD : VUNPCKLPD;
		break;
	case VUNPCKHPS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? UNPCKHPS : VUNPCKHPS;
		break;
	case VUNPCKHPD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? UNPCKHPD : VUNPCKHPD;
		break;
	case VMOVHPS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MOVHPS : VMOVHPS;
		break;
	case VMOVLHPS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MOVLHPS : VMOVLHPS;
		break;
	case VMOVHPD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MOVHPD : VMOVHPD;
		break;
	case VMOVSHDUP:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MOVSHDUP : VMOVSHDUP;
		break;
	case VMOVAPS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MOVAPS : VMOVAPS;
		break;
	case VMOVAPD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MOVAPD : VMOVAPD;
		break;
	case VCVTSI2SS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? CVTSI2SS : VCVTSI2SS;
		break;
	case VCVTSI2SD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? CVTSI2SD : VCVTSI2SD;
		break;
	case VMOVNTPS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MOVNTPS : VMOVNTPS;
		break;
	case VMOVNTPD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MOVNTPD : VMOVNTPD;
		break;
	case VCVTTSS2SI:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? CVTTSS2SI : VCVTTSS2SI;
		break;
	case VCVTTSD2SI:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? CVTTSD2SI : VCVTTSD2SI;
		break;
	case VCVTSS2SI:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? CVTSS2SI : VCVTSS2SI;
		break;
	case VCVTSD2SI:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? CVTSD2SI : VCVTSD2SI;
		break;
	case VUCOMISS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? UCOMISS : VUCOMISS;
		break;
	case VUCOMISD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? UCOMISD : VUCOMISD;
		break;
	case VCOMISS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? COMISS : VCOMISS;
		break;
	case VCOMISD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? COMISD : VCOMISD;
		break;
	case VMOVMSKPS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MOVMSKPS : VMOVMSKPS;
		break;
	case VMOVMSKPD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MOVMSKPD : VMOVMSKPD;
		break;
	case VSQRTPS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? SQRTPS : VSQRTPS;
		break;
	case VSQRTPD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? SQRTPD : VSQRTPD;
		break;
	case VSQRTSS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? SQRTSS : VSQRTSS;
		break;
	case VSQRTSD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? SQRTSD : VSQRTSD;
		break;
	case VRSQRTPS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? RSQRTPS : VRSQRTPS;
		break;
	case VRSQRTSS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? RSQRTSS : VRSQRTSS;
		break;
	case VRCPPS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? RCPPS : VRCPPS;
		break;
	case VRCPSS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? RCPSS : VRCPSS;
		break;
	case VANDPS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? ANDPS : VANDPS;
		break;
	case VANDPD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? ANDPD : VANDPD;
		break;
	case VANDNPS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? ANDNPS : VANDNPS;
		break;
	case VANDNPD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? ANDNPD : VANDNPD;
		break;
	case VORPS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? ORPS : VORPS;
		break;
	case VORPD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? ORPD : VORPD;
		break;
	case VXORPS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? XORPS : VXORPS;
		break;
	case VXORPD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? XORPD : VXORPD;
		break;
	case VADDPS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? ADDPS : VADDPS;
		break;
	case VADDPD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? ADDPD : VADDPD;
		break;
	case VADDSS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? ADDSS : VADDSS;
		break;
	case VADDSD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? ADDSD : VADDSD;
		break;
	case VMULPS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MULPS : VMULPS;
		break;
	case VMULPD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MULPD : VMULPD;
		break;
	case VMULSS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MULSS : VMULSS;
		break;
	case VMULSD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MULSD : VMULSD;
		break;
	case VCVTPS2PD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? CVTPS2PD : VCVTPS2PD;
		break;
	case VCVTPD2PS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? CVTPD2PS : VCVTPD2PS;
		break;
	case VCVTSS2SD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? CVTSS2SD : VCVTSS2SD;
		break;
	case VCVTSD2SS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? CVTSD2SS : VCVTSD2SS;
		break;
	case VCVTDQ2PS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? CVTDQ2PS : VCVTDQ2PS;
		break;
	case VCVTPS2DQ:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? CVTPS2DQ : VCVTPS2DQ;
		break;
	case VCVTTPS2DQ:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? CVTTPS2DQ : VCVTTPS2DQ;
		break;
	case VSUBPS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? SUBPS : VSUBPS;
		break;
	case VSUBPD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? SUBPD : VSUBPD;
		break;
	case VSUBSS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? SUBSS : VSUBSS;
		break;
	case VSUBSD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? SUBSD : VSUBSD;
		break;
	case VMINPS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MINPS : VMINPS;
		break;
	case VMINPD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MINPD : VMINPD;
		break;
	case VMINSS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MINSS : VMINSS;
		break;
	case VMINSD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MINSD : VMINSD;
		break;
	case VDIVPS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? DIVPS : VDIVPS;
		break;
	case VDIVPD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? DIVPD : VDIVPD;
		break;
	case VDIVSS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? DIVSS : VDIVSS;
		break;
	case VDIVSD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? DIVSD : VDIVSD;
		break;
	case VMAXPS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MAXPS : VMAXPS;
		break;
	case VMAXPD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MAXPD : VMAXPD;
		break;
	case VMAXSS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MAXSS : VMAXSS;
		break;
	case VMAXSD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MAXSD : VMAXSD;
		break;
	case VPUNPCKLBW:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PUNPCKLBW : VPUNPCKLBW;
		break;
	case VPUNPCKLWD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PUNPCKLWD : VPUNPCKLWD;
		break;
	case VPUNPCKLDQ:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PUNPCKLDQ : VPUNPCKLDQ;
		break;
	case VPACKSSWB:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PACKSSWB : VPACKSSWB;
		break;
	case VPCMPGTB:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PCMPGTB : VPCMPGTB;
		break;
	case VPCMPGTW:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PCMPGTW : VPCMPGTW;
		break;
	case VPCMPGTD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PCMPGTD : VPCMPGTD;
		break;
	case VPACKUSWB:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PACKUSWB : VPACKUSWB;
		break;
	case VPUNPCKHBW:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PUNPCKHBW : VPUNPCKHBW;
		break;
	case VPUNPCKHWD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PUNPCKHWD : VPUNPCKHWD;
		break;
	case VPUNPCKHDQ:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PUNPCKHDQ : VPUNPCKHDQ;
		break;
	case VPACKSSDW:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PACKSSDW : VPACKSSDW;
		break;
	case VPUNPCKLQDQ:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PUNPCKLQDQ : VPUNPCKLQDQ;
		break;
	case VPUNPCKHQDQ:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PUNPCKHQDQ : VPUNPCKHQDQ;
		break;
	case MOVD:
		opcode->mnemonic = params->rexPrefix->w ? MOVQ : params->vexPrefix->w || params->evexPrefix->w ? VMOVQ : !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MOVD : VMOVD;
		break;
	case VMOVD:
		opcode->mnemonic = params->vexPrefix->w || params->evexPrefix->w ? VMOVQ : !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MOVD : VMOVD;
		break;
	case VMOVDQA:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MOVDQA : VMOVDQA;
		break;
	case VMOVDQU:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MOVDQU : VMOVDQU;
		break;
	case VPSHUFD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PSHUFD : VPSHUFD;
		break;
	case VPSHUFHW:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PSHUFHW : VPSHUFHW;
		break;
	case VPSHUFLW:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PSHUFLW : VPSHUFLW;
		break;
	case VPCMPEQB:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PCMPEQB : VPCMPEQB;
		break;
	case VPCMPEQW:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PCMPEQW : VPCMPEQW;
		break;
	case VPCMPEQD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PCMPEQD : VPCMPEQD;
		break;
	case EMMS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX ? EMMS : params->vexPrefix->l == 0 ? VZEROUPPER : VZEROALL;
		break;
	case VHADDPD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX ? HADDPD : VHADDPD;
		break;
	case VHADDPS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX ? HADDPS : VHADDPS;
		break;
	case VHSUBPD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX ? HSUBPD : VHSUBPD;
		break;
	case VHSUBPS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX ? HSUBPS : VHSUBPS;
		break;
	case VCMPPS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? CMPPS : VCMPPS;
		break;
	case VCMPPD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? CMPPD : VCMPPD;
		break;
	case VCMPSS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? CMPSS : VCMPSS;
		break;
	case VCMPSD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? CMPSD : VCMPSD;
		break;
	case VPINSRW:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PINSRW : VPINSRW;
		break;
	case VPEXTRW:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PEXTRW : VPEXTRW;
		break;
	case VSHUFPS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? SHUFPS : VSHUFPS;
		break;
	case VSHUFPD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? SHUFPD : VSHUFPD;
		break;
	case VADDSUBPD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX ? ADDSUBPD : VADDSUBPD;
		break;
	case VADDSUBPS:
		opcode->mnemonic = !params->vexPrefix->isValidVEX ? ADDSUBPS : VADDSUBPS;
		break;
	case VPSRLW:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PSRLW : VPSRLW;
		break;
	case VPSRLD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PSRLD : VPSRLD;
		break;
	case VPSRLQ:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PSRLQ : VPSRLQ;
		break;
	case VPADDQ:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PADDQ : VPADDQ;
		break;
	case VPMULLW:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PMULLW : VPMULLW;
		break;
	case VPMOVMSKB:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PMOVMSKB : VPMOVMSKB;
		break;
	case VPSUBUSB:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PSUBUSB : VPSUBUSB;
		break;
	case VPSUBUSW:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PSUBUSW : VPSUBUSW;
		break;
	case VPMINUB:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PMINUB : VPMINUB;
		break;
	case VPAND:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PAND : VPAND;
		break;
	case VPADDUSB:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PADDUSB : VPADDUSB;
		break;
	case VPADDUSW:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PADDUSW : VPADDUSW;
		break;
	case VPANDN:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PANDN : VPANDN;
		break;
	case VPAVGB:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PAVGB : VPAVGB;
		break;
	case VPSRAW:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PSRAW : VPSRAW;
		break;
	case VPSRAD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PSRAD : VPSRAD;
		break;
	case VPAVGW:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PAVGW : VPAVGW;
		break;
	case VPMULHUW:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PMULHUW : VPMULHUW;
		break;
	case VPMULHW:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PMULHW : VPMULHW;
		break;
	case VCVTTPD2DQ:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? CVTTPD2DQ : VCVTTPD2DQ;
		break;
	case VCVTDQ2PD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? CVTDQ2PD : VCVTDQ2PD;
		break;
	case VCVTPD2DQ:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? CVTPD2DQ : VCVTPD2DQ;
		break;
	case VMOVNTDQ:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? MOVNTDQ : VMOVNTDQ;
		break;
	case VPSUBSB:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PSUBSB : VPSUBSB;
		break;
	case VPSUBSW:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PSUBSW : VPSUBSW;
		break;
	case VPMINSW:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PMINSW : VPMINSW;
		break;
	case VPOR:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? POR : VPOR;
		break;
	case VPADDSB:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PADDSB : VPADDSB;
		break;
	case VPADDSW:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PADDSW : VPADDSW;
		break;
	case VPMAXSW:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PMAXSW : VPMAXSW;
		break;
	case VPXOR:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PXOR : VPXOR;
		break;
	case VLDDQU:
		opcode->mnemonic = !params->vexPrefix->isValidVEX ? LDDQU : VLDDQU;
		break;
	case VPSLLW:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PSLLW : VPSLLW;
		break;
	case VPSLLD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PSLLD : VPSLLD;
		break;
	case VPSLLQ:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PSLLQ : VPSLLQ;
		break;
	case VPMULUDQ:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PMULUDQ : VPMULUDQ;
		break;
	case VPMADDWD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PMADDWD : VPMADDWD;
		break;
	case VPSADBW:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PSADBW : VPSADBW;
		break;
	case VMASKMOVDQU:
		opcode->mnemonic = !params->vexPrefix->isValidVEX ? MASKMOVDQU : VMASKMOVDQU;
		break;
	case VPSUBB:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PSUBB : VPSUBB;
		break;
	case VPSUBW:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PSUBW : VPSUBW;
		break;
	case VPSUBD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PSUBD : VPSUBD;
		break;
	case VPSUBQ:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PSUBQ : VPSUBQ;
		break;
	case VPADDB:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PADDB : VPADDB;
		break;
	case VPADDW:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PADDW : VPADDW;
		break;
	case VPADDD:
		opcode->mnemonic = !params->vexPrefix->isValidVEX && !params->evexPrefix->isValidEVEX ? PADDD : VPADDD;
		break;

	case VINSERTI32X8:
		opcode->mnemonic = params->evexPrefix->w ? VINSERTI64X4 : VINSERTI32X8;
		break;
	case VSHUFI32X4:
		opcode->mnemonic = params->evexPrefix->w ? VSHUFI64X2 : VSHUFI32X4;
		break;
	}
}
