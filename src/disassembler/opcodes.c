#include "opcodes.h"
#include "mnemonics.h"

#include "oneByteOpcodeMap.h"
#include "twoByteOpcodeMap.h"
#include "threeByteOpcodeMaps.h"
#include "extendedOpcodeMap.h"
#include "escapeOpcodeMaps.h"

unsigned char handleOpcode(struct DisassemblyParameters* params)
{
	if (params->bytes > params->maxBytesAddr) { return 0; }

	unsigned char prefixIndex = params->legPrefixes.group3 == OSO ? 1 : params->legPrefixes.group1 == REPZ ? 2 : params->legPrefixes.group1 == REPNZ ? 3 : 0;
	unsigned char opcodeByte = 0;
	unsigned char escapeToCoprocessor = 0;

	int mmm = params->vexPrefix.m_mmmm != 0 ? params->vexPrefix.m_mmmm : params->evexPrefix.mmm;
	if (params->bytes[0] == 0x0F || mmm != 0)
	{
		if ((params->bytes + 2) <= params->maxBytesAddr && ((params->bytes[1] == 0x38 && mmm == 0) || mmm == 0b00010)) // sequence: 0x0F 0x38 opcode
		{
			opcodeByte = params->bytes[mmm != 0 ? 0 : 2];
			params->bytes += mmm != 0 ? 1 : 3;
			
			if (prefixIndex != 0)
			{
				params->legPrefixes.group1 = NO_PREFIX;
			}

			if (threeByteOpcodeMap38[opcodeByte][prefixIndex].mnemonic == NO_MNEMONIC)
			{
				params->opcode = threeByteOpcodeMap38[opcodeByte][0];
			}
			else
			{
				params->opcode = threeByteOpcodeMap38[opcodeByte][prefixIndex];
			}
		}
		else if ((params->bytes + 2) <= params->maxBytesAddr && ((params->bytes[1] == 0x3A && mmm == 0) || mmm == 0b00011)) // sequence: 0x0F 0x3A opcode
		{
			opcodeByte = params->bytes[mmm != 0 ? 0 : 2];
			params->bytes += mmm != 0 ? 1 : 3;

			if (prefixIndex != 0)
			{
				params->legPrefixes.group1 = NO_PREFIX;
			}

			if (threeByteOpcodeMap3A[opcodeByte][prefixIndex].mnemonic == NO_MNEMONIC)
			{
				params->opcode = threeByteOpcodeMap3A[opcodeByte][0];
			}
			else
			{
				params->opcode = threeByteOpcodeMap3A[opcodeByte][prefixIndex];
			}
		}
		else if ((params->bytes + 1) <= params->maxBytesAddr) // sequence: 0x0F opcode
		{
			opcodeByte = params->bytes[mmm != 0 ? 0 : 1];
			params->bytes += mmm != 0 ? 1 : 2;

			if (prefixIndex != 0)
			{
				params->legPrefixes.group1 = NO_PREFIX;
			}

			if (twoByteOpcodeMap[opcodeByte][prefixIndex].mnemonic == NO_MNEMONIC)
			{
				params->opcode = twoByteOpcodeMap[opcodeByte][0];
			}
			else
			{
				params->opcode = twoByteOpcodeMap[opcodeByte][prefixIndex];
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
						params->opcode = alternateX12;
					}
					else 
					{
						params->opcode = alternateX16;
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

		params->opcode = oneByteOpcodeMap[opcodeByte];

		if (params->is64BitMode && opcodeByte == 0x63)
		{
			params->opcode = alternateX63;
		}

		escapeToCoprocessor = opcodeByte > 0xD7 && opcodeByte < 0xE0;
	}
	else
	{
		return 0;
	}

	// handle extended opcodes or escape opcodes
	if (params->opcode.mnemonic == EXTENDED_OPCODE)
	{
		if (params->bytes > params->maxBytesAddr) { return 0; }

		params->modRM.hasGotModRM = 1;
		params->modRM.mod = (((params->bytes[0] >> 7) & 0x01) * 2) + ((params->bytes[0] >> 6) & 0x01);
		params->modRM.reg = (((params->bytes[0] >> 5) & 0x01) * 4) + (((params->bytes[0] >> 4) & 0x01) * 2) + ((params->bytes[0] >> 3) & 0x01);
		params->modRM.rm = (((params->bytes[0] >> 2) & 0x01) * 4) + (((params->bytes[0] >> 1) & 0x01) * 2) + ((params->bytes[0] >> 0) & 0x01);
		params->bytes++;

		const struct Opcode* extendedOpcode = 0;

		if (params->opcode.extensionGroup < 7)
		{
			extendedOpcode = &extendedOpcodeMapThroughGroupSix[params->opcode.extensionGroup][params->modRM.reg];
		}
		else if (params->opcode.extensionGroup == 7)
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
		else if (params->opcode.extensionGroup == 8)
		{
			extendedOpcode = &extendedOpcodeMapGroup8[params->modRM.reg];
		}
		else if (params->opcode.extensionGroup == 9)
		{
			if (params->modRM.mod == 0b11)
			{
				if (params->legPrefixes.group1 == REPZ)
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
				if (params->legPrefixes.group3 == OSO)
				{
					extendedOpcode = &extendedOpcodeMapGroup966[params->modRM.reg];
				}
				else if (params->legPrefixes.group1 == REPZ)
				{
					extendedOpcode = &extendedOpcodeMapGroup9F3[params->modRM.reg];
				}
				else 
				{
					extendedOpcode = &extendedOpcodeMapGroup9[params->modRM.reg];

					if(extendedOpcode->mnemonic == CMPXCH8B && params->rexPrefix.W)
					{
						extendedOpcode = &alternateCMPXCH;
					}
				}
			}
		}
		else if (params->opcode.extensionGroup == 10) 
		{
			extendedOpcode = &extendedOpcodeGroup10;
		}
		else if (params->opcode.extensionGroup == 11)
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
		else if (params->opcode.extensionGroup == 12)
		{
			if (params->modRM.mod == 0b11)
			{
				if (params->legPrefixes.group3 == OSO)
				{
					extendedOpcode = &extendedOpcodeMapGroup126611B[params->modRM.reg];
				}
				else
				{
					extendedOpcode = &extendedOpcodeMapGroup1211B[params->modRM.reg];
				}
			}
		}
		else if (params->opcode.extensionGroup == 13)
		{
			if (params->modRM.mod == 0b11)
			{
				if (params->legPrefixes.group3 == OSO)
				{
					extendedOpcode = &extendedOpcodeMapGroup136611B[params->modRM.reg];
				}
				else
				{
					extendedOpcode = &extendedOpcodeMapGroup1311B[params->modRM.reg];
				}
			}
		}
		else if (params->opcode.extensionGroup == 14)
		{
			if (params->modRM.mod == 0b11)
			{
				if (params->legPrefixes.group3 == OSO)
				{
					extendedOpcode = &extendedOpcodeMapGroup146611B[params->modRM.reg];
				}
				else
				{
					extendedOpcode = &extendedOpcodeMapGroup1411B[params->modRM.reg];
				}
			}
		}
		else if (params->opcode.extensionGroup == 15)
		{
			if (params->modRM.mod == 0b11)
			{
				if (params->legPrefixes.group1 == REPZ)
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
		else if (params->opcode.extensionGroup == 16)
		{
			if (params->modRM.mod != 0b11)
			{
				extendedOpcode = &extendedOpcodeMapGroup16[params->modRM.reg];
			}
		}
		else if (params->opcode.extensionGroup == 17)
		{
			extendedOpcode = &extendedOpcodeMapGroup17[params->modRM.reg];
		}

		if (extendedOpcode == 0)
		{
			return 0;
		}

		params->opcode.mnemonic = extendedOpcode->mnemonic;

		for (int i = 0; i < 4; i++)
		{
			if (extendedOpcode->operands[i] != NO_OPERAND_CODE)
			{
				params->opcode.operands[i] = extendedOpcode->operands[i];
			}
		}

		if (extendedOpcode->opcodeSuperscript != NO_SUPERSCRIPT)
		{
			params->opcode.opcodeSuperscript = extendedOpcode->opcodeSuperscript;
		}

		if (params->opcode.extensionGroup == 3 && opcodeByte == 0xF7 && params->opcode.mnemonic == TEST)
		{
			params->opcode.operands[1] = Iz;
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
			params->opcode = params->bytes[0] < 0xC0 ? escapeD8OpcodeMapBits[params->modRM.reg] : escapeD8OpcodeMapByte[params->bytes[0] - 0xC0];
			break;
		case 0xD9:
			params->opcode = params->bytes[0] < 0xC0 ? escapeD9OpcodeMapBits[params->modRM.reg] : escapeD9OpcodeMapByte[params->bytes[0] - 0xC0];
			break;
		case 0xDA:
			params->opcode = params->bytes[0] < 0xC0 ? escapeDAOpcodeMapBits[params->modRM.reg] : escapeDAOpcodeMapByte[params->bytes[0] - 0xC0];
			break;
		case 0xDB:
			params->opcode = params->bytes[0] < 0xC0 ? escapeDBOpcodeMapBits[params->modRM.reg] : escapeDBOpcodeMapByte[params->bytes[0] - 0xC0];
			break;
		case 0xDC:
			params->opcode = params->bytes[0] < 0xC0 ? escapeDCOpcodeMapBits[params->modRM.reg] : escapeDCOpcodeMapByte[params->bytes[0] - 0xC0];
			break;
		case 0xDD:
			params->opcode = params->bytes[0] < 0xC0 ? escapeDDOpcodeMapBits[params->modRM.reg] : escapeDDOpcodeMapByte[params->bytes[0] - 0xC0];
			break;
		case 0xDE:
			params->opcode = params->bytes[0] < 0xC0 ? escapeDEOpcodeMapBits[params->modRM.reg] : escapeDEOpcodeMapByte[params->bytes[0] - 0xC0];
			break;
		case 0xDF:
			params->opcode = params->bytes[0] < 0xC0 ? escapeDFOpcodeMapBits[params->modRM.reg] : escapeDFOpcodeMapByte[params->bytes[0] - 0xC0];
			break;
		}

		params->bytes++;
	}

	handleAlternateMnemonics(params);

	return 1;
}

static void handleAlternateMnemonics(struct DisassemblyParameters* params) // this should only be for when the alternate mnemonic represents a different instruction
{
	switch (params->opcode.mnemonic) 
	{
	case CWDE:
		params->opcode.mnemonic = params->rexPrefix.W ? CDQE : params->legPrefixes.group1 == OSO ? CBW : CWDE;
		break;
	case CDQ:
		params->opcode.mnemonic = params->rexPrefix.W ? CQO : params->legPrefixes.group1 == OSO ? CWD : CDQ;
		break;
	case VMOVUPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVUPS : VMOVUPS;
		break;
	case VMOVUPD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVUPD : VMOVUPD;
		break;
	case VMOVSS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVSS : VMOVSS;
		break;
	case VMOVSD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVSD : VMOVSD;
		break;
	case VMOVLPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVLPS : VMOVLPS;
		break;
	case VMOVHLPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVHLPS : VMOVHLPS;
		break;
	case VMOVLPD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVLPD : VMOVLPD;
		break;
	case VMOVSLDUP:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVSLDUP : VMOVSLDUP;
		break;
	case VMOVDDUP:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVDDUP : VMOVDDUP;
		break;
	case VUNPCKLPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? UNPCKLPS : VUNPCKLPS;
		break;
	case VUNPCKLPD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? UNPCKLPD : VUNPCKLPD;
		break;
	case VUNPCKHPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? UNPCKHPS : VUNPCKHPS;
		break;
	case VUNPCKHPD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? UNPCKHPD : VUNPCKHPD;
		break;
	case VMOVHPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVHPS : VMOVHPS;
		break;
	case VMOVLHPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVLHPS : VMOVLHPS;
		break;
	case VMOVHPD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVHPD : VMOVHPD;
		break;
	case VMOVSHDUP:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVSHDUP : VMOVSHDUP;
		break;
	case VMOVAPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVAPS : VMOVAPS;
		break;
	case VMOVAPD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVAPD : VMOVAPD;
		break;
	case VCVTSI2SS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTSI2SS : VCVTSI2SS;
		break;
	case VCVTSI2SD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTSI2SD : VCVTSI2SD;
		break;
	case VMOVNTPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVNTPS : VMOVNTPS;
		break;
	case VMOVNTPD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVNTPD : VMOVNTPD;
		break;
	case VCVTTSS2SI:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTTSS2SI : VCVTTSS2SI;
		break;
	case VCVTTSD2SI:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTTSD2SI : VCVTTSD2SI;
		break;
	case VCVTSS2SI:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTSS2SI : VCVTSS2SI;
		break;
	case VCVTSD2SI:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTSD2SI : VCVTSD2SI;
		break;
	case VUCOMISS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? UCOMISS : VUCOMISS;
		break;
	case VUCOMISD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? UCOMISD : VUCOMISD;
		break;
	case VCOMISS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? COMISS : VCOMISS;
		break;
	case VCOMISD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? COMISD : VCOMISD;
		break;
	case VMOVMSKPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVMSKPS : VMOVMSKPS;
		break;
	case VMOVMSKPD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVMSKPD : VMOVMSKPD;
		break;
	case VSQRTPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? SQRTPS : VSQRTPS;
		break;
	case VSQRTPD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? SQRTPD : VSQRTPD;
		break;
	case VSQRTSS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? SQRTSS : VSQRTSS;
		break;
	case VSQRTSD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? SQRTSD : VSQRTSD;
		break;
	case VRSQRTPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? RSQRTPS : VRSQRTPS;
		break;
	case VRSQRTSS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? RSQRTSS : VRSQRTSS;
		break;
	case VRCPPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? RCPPS : VRCPPS;
		break;
	case VRCPSS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? RCPSS : VRCPSS;
		break;
	case VANDPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? ANDPS : VANDPS;
		break;
	case VANDPD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? ANDPD : VANDPD;
		break;
	case VANDNPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? ANDNPS : VANDNPS;
		break;
	case VANDNPD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? ANDNPD : VANDNPD;
		break;
	case VORPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? ORPS : VORPS;
		break;
	case VORPD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? ORPD : VORPD;
		break;
	case VXORPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? XORPS : VXORPS;
		break;
	case VXORPD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? XORPD : VXORPD;
		break;
	case VADDPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? ADDPS : VADDPS;
		break;
	case VADDPD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? ADDPD : VADDPD;
		break;
	case VADDSS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? ADDSS : VADDSS;
		break;
	case VADDSD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? ADDSD : VADDSD;
		break;
	case VMULPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MULPS : VMULPS;
		break;
	case VMULPD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MULPD : VMULPD;
		break;
	case VMULSS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MULSS : VMULSS;
		break;
	case VMULSD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MULSD : VMULSD;
		break;
	case VCVTPS2PD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTPS2PD : VCVTPS2PD;
		break;
	case VCVTPD2PS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTPD2PS : VCVTPD2PS;
		break;
	case VCVTSS2SD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTSS2SD : VCVTSS2SD;
		break;
	case VCVTSD2SS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTSD2SS : VCVTSD2SS;
		break;
	case VCVTDQ2PS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTDQ2PS : VCVTDQ2PS;
		break;
	case VCVTPS2DQ:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTPS2DQ : VCVTPS2DQ;
		break;
	case VCVTTPS2DQ:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTTPS2DQ : VCVTTPS2DQ;
		break;
	case VSUBPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? SUBPS : VSUBPS;
		break;
	case VSUBPD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? SUBPD : VSUBPD;
		break;
	case VSUBSS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? SUBSS : VSUBSS;
		break;
	case VSUBSD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? SUBSD : VSUBSD;
		break;
	case VMINPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MINPS : VMINPS;
		break;
	case VMINPD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MINPD : VMINPD;
		break;
	case VMINSS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MINSS : VMINSS;
		break;
	case VMINSD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MINSD : VMINSD;
		break;
	case VDIVPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? DIVPS : VDIVPS;
		break;
	case VDIVPD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? DIVPD : VDIVPD;
		break;
	case VDIVSS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? DIVSS : VDIVSS;
		break;
	case VDIVSD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? DIVSD : VDIVSD;
		break;
	case VMAXPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MAXPS : VMAXPS;
		break;
	case VMAXPD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MAXPD : VMAXPD;
		break;
	case VMAXSS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MAXSS : VMAXSS;
		break;
	case VMAXSD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MAXSD : VMAXSD;
		break;
	case VPUNPCKLBW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PUNPCKLBW : VPUNPCKLBW;
		break;
	case VPUNPCKLWD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PUNPCKLWD : VPUNPCKLWD;
		break;
	case VPUNPCKLDQ:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PUNPCKLDQ : VPUNPCKLDQ;
		break;
	case VPACKSSWB:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PACKSSWB : VPACKSSWB;
		break;
	case VPCMPGTB:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PCMPGTB : VPCMPGTB;
		break;
	case VPCMPGTW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PCMPGTW : VPCMPGTW;
		break;
	case VPCMPGTD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PCMPGTD : VPCMPGTD;
		break;
	case VPACKUSWB:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PACKUSWB : VPACKUSWB;
		break;
	case VPUNPCKHBW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PUNPCKHBW : VPUNPCKHBW;
		break;
	case VPUNPCKHWD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PUNPCKHWD : VPUNPCKHWD;
		break;
	case VPUNPCKHDQ:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PUNPCKHDQ : VPUNPCKHDQ;
		break;
	case VPACKSSDW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PACKSSDW : VPACKSSDW;
		break;
	case VPUNPCKLQDQ:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PUNPCKLQDQ : VPUNPCKLQDQ;
		break;
	case VPUNPCKHQDQ:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PUNPCKHQDQ : VPUNPCKHQDQ;
		break;
	case MOVD:
		params->opcode.mnemonic = params->rexPrefix.W ? MOVQ : params->vexPrefix.W || params->evexPrefix.W ? VMOVQ : !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVD : VMOVD;
		break;
	case VMOVD:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VMOVQ : !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVD : VMOVD;
		break;
	case VMOVDQA:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVDQA : VMOVDQA;
		break;
	case VMOVDQU:
		params->opcode.mnemonic = params->evexPrefix.W ? VMOVDQU64 : params->evexPrefix.isValidEVEX ? VMOVDQU32 : params->vexPrefix.isValidVEX ? VMOVDQU : MOVDQU;
		break;
	case VPSHUFD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSHUFD : VPSHUFD;
		break;
	case VPSHUFHW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSHUFHW : VPSHUFHW;
		break;
	case VPSHUFLW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSHUFLW : VPSHUFLW;
		break;
	case VPCMPEQB:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PCMPEQB : VPCMPEQB;
		break;
	case VPCMPEQW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PCMPEQW : VPCMPEQW;
		break;
	case VPCMPEQD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PCMPEQD : VPCMPEQD;
		break;
	case EMMS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? EMMS : params->vexPrefix.L == 0 ? VZEROUPPER : VZEROALL;
		break;
	case VHADDPD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? HADDPD : VHADDPD;
		break;
	case VHADDPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? HADDPS : VHADDPS;
		break;
	case VHSUBPD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? HSUBPD : VHSUBPD;
		break;
	case VHSUBPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? HSUBPS : VHSUBPS;
		break;
	case VCMPPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CMPPS : VCMPPS;
		break;
	case VCMPPD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CMPPD : VCMPPD;
		break;
	case VCMPSS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CMPSS : VCMPSS;
		break;
	case VCMPSD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CMPSD : VCMPSD;
		break;
	case VPINSRW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PINSRW : VPINSRW;
		break;
	case VPEXTRW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PEXTRW : VPEXTRW;
		break;
	case VSHUFPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? SHUFPS : VSHUFPS;
		break;
	case VSHUFPD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? SHUFPD : VSHUFPD;
		break;
	case VADDSUBPD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? ADDSUBPD : VADDSUBPD;
		break;
	case VADDSUBPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? ADDSUBPS : VADDSUBPS;
		break;
	case VPSRLW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSRLW : VPSRLW;
		break;
	case VPSRLD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSRLD : VPSRLD;
		break;
	case VPSRLQ:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSRLQ : VPSRLQ;
		break;
	case VPADDQ:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PADDQ : VPADDQ;
		break;
	case VPMULLW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMULLW : VPMULLW;
		break;
	case VPMOVMSKB:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMOVMSKB : VPMOVMSKB;
		break;
	case VPSUBUSB:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSUBUSB : VPSUBUSB;
		break;
	case VPSUBUSW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSUBUSW : VPSUBUSW;
		break;
	case VPMINUB:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMINUB : VPMINUB;
		break;
	case VPAND:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PAND : VPAND;
		break;
	case VPADDUSB:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PADDUSB : VPADDUSB;
		break;
	case VPADDUSW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PADDUSW : VPADDUSW;
		break;
	case VPANDN:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PANDN : VPANDN;
		break;
	case VPAVGB:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PAVGB : VPAVGB;
		break;
	case VPSRAW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSRAW : VPSRAW;
		break;
	case VPSRAD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSRAD : VPSRAD;
		break;
	case VPAVGW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PAVGW : VPAVGW;
		break;
	case VPMULHUW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMULHUW : VPMULHUW;
		break;
	case VPMULHW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMULHW : VPMULHW;
		break;
	case VCVTTPD2DQ:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTTPD2DQ : VCVTTPD2DQ;
		break;
	case VCVTDQ2PD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTDQ2PD : VCVTDQ2PD;
		break;
	case VCVTPD2DQ:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTPD2DQ : VCVTPD2DQ;
		break;
	case VMOVNTDQ:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVNTDQ : VMOVNTDQ;
		break;
	case VPSUBSB:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSUBSB : VPSUBSB;
		break;
	case VPSUBSW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSUBSW : VPSUBSW;
		break;
	case VPMINSW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMINSW : VPMINSW;
		break;
	case VPOR:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? POR : VPOR;
		break;
	case VPADDSB:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PADDSB : VPADDSB;
		break;
	case VPADDSW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PADDSW : VPADDSW;
		break;
	case VPMAXSW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMAXSW : VPMAXSW;
		break;
	case VPXOR:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PXOR : VPXOR;
		break;
	case VLDDQU:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? LDDQU : VLDDQU;
		break;
	case VPSLLW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSLLW : VPSLLW;
		break;
	case VPSLLD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSLLD : VPSLLD;
		break;
	case VPSLLQ:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSLLQ : VPSLLQ;
		break;
	case VPMULUDQ:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMULUDQ : VPMULUDQ;
		break;
	case VPMADDWD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMADDWD : VPMADDWD;
		break;
	case VPSADBW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSADBW : VPSADBW;
		break;
	case VMASKMOVDQU:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? MASKMOVDQU : VMASKMOVDQU;
		break;
	case VPSUBB:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSUBB : VPSUBB;
		break;
	case VPSUBW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSUBW : VPSUBW;
		break;
	case VPSUBD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSUBD : VPSUBD;
		break;
	case VPSUBQ:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSUBQ : VPSUBQ;
		break;
	case VPADDB:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PADDB : VPADDB;
		break;
	case VPADDW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PADDW : VPADDW;
		break;
	case VPADDD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PADDD : VPADDD;
		break;
	case VPSHUFB:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSHUFB : VPSHUFB;
		break;
	case VPHADDW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PHADDW : VPHADDW;
		break;
	case VPHADDD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PHADDD : VPHADDD;
		break;
	case VPHADDSW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PHADDSW : VPHADDSW;
		break;
	case VPMADDUBSW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMADDUBSW : VPMADDUBSW;
		break;
	case VPHSUBW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PHSUBW : VPHSUBW;
		break;
	case VPHSUBD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PHSUBD : VPHSUBD;
		break;
	case VPHSUBSW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PHSUBSW : VPHSUBSW;
		break;
	case VPSIGNB:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSIGNB : VPSIGNB;
		break;
	case VPSIGNW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSIGNW : VPSIGNW;
		break;
	case VPSIGND:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSIGND : VPSIGND;
		break;
	case VPMULHRSW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMULHRSW : VPMULHRSW;
		break;
	case VPTEST:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? PTEST : VPTEST;
		break;
	case VPABSB:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PABSB : VPABSB;
		break;
	case VPABSW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PABSW : VPABSW;
		break;
	case VPABSD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PABSD : VPABSD;
		break;
	case VPMOVSXBW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMOVSXBW : VPMOVSXBW;
		break;
	case VPMOVSXBD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMOVSXBD : VPMOVSXBD;
		break;
	case VPMOVSXBQ:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMOVSXBQ : VPMOVSXBQ;
		break;
	case VPMOVSXWD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMOVSXWD : VPMOVSXWD;
		break;
	case VPMOVSXWQ:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMOVSXWQ : VPMOVSXWQ;
		break;
	case VPMOVSXDQ:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMOVSXDQ : VPMOVSXDQ;
		break;
	case VPMULDQ:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMULDQ : VPMULDQ;
		break;
	case VPCMPEQQ:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PCMPEQQ : VPCMPEQQ;
		break;
	case VMOVNTDQA:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVNTDQA : VMOVNTDQA;
		break;
	case VPACKUSDW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PACKUSDW : VPACKUSDW;
		break;
	case VPMOVZXBW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMOVZXBW : VPMOVZXBW;
		break;
	case VPMOVZXBD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMOVZXBD : VPMOVZXBD;
		break;
	case VPMOVZXBQ:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMOVZXBQ : VPMOVZXBQ;
		break;
	case VPMOVZXWD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMOVZXWD : VPMOVZXWD;
		break;
	case VPMOVZXWQ:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMOVZXWQ : VPMOVZXWQ;
		break;
	case VPMOVZXDQ:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMOVZXDQ : VPMOVZXDQ;
		break;
	case VPCMPGTQ:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PCMPGTQ : VPCMPGTQ;
		break;
	case VPMINSB:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMINSB : VPMINSB;
		break;
	case VPMINSD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMINSD : VPMINSD;
		break;
	case VPMINUW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMINUW : VPMINUW;
		break;
	case VPMINUD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMINUD : VPMINUD;
		break;
	case VPMAXSB:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMAXSB : VPMAXSB;
		break;
	case VPMAXSD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMAXSD : VPMAXSD;
		break;
	case VPMAXUW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMAXUW : VPMAXUW;
		break;
	case VPMAXUD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMAXUD : VPMAXUD;
		break;
	case VPMULLD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMULLD : VPMULLD;
		break;
	case VPHMINPOSUW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? PHMINPOSUW : VPHMINPOSUW;
		break;
	case VPSRLVD:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VPSRLVQ : VPSRLVD;
		break;
	case VPSLLVD:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VPSLLVQ : VPSLLVD;
		break;
	case VBROADCASTI128:
		params->opcode.mnemonic = !params->evexPrefix.isValidEVEX ? VBROADCASTI128 : params->evexPrefix.W ? VBROADCASTI64X2 : VBROADCASTI32X4;
		break;
	case VPMASKMOVD:
		params->opcode.mnemonic = params->vexPrefix.W ? VPMASKMOVQ : VPMASKMOVD;
		break;
	case VGATHERDD:
		params->opcode.mnemonic = params->evexPrefix.W ? VGATHERDQ : VGATHERDD;
		break;
	case VGATHERQD:
		params->opcode.mnemonic = params->evexPrefix.W ? VGATHERQQ : VGATHERQD;
		break;
	case VGATHERDPS:
		params->opcode.mnemonic = params->evexPrefix.W ? VGATHERDPD : VGATHERDPS;
		break;
	case VGATHERQPS:
		params->opcode.mnemonic = params->evexPrefix.W ? VGATHERQPD : VGATHERQPS;
		break;
	case VFMADDSUB132PS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMADDSUB132PD : VFMADDSUB132PS;
		break;
	case VFMSUBADD132PS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMSUBADD132PD : VFMSUBADD132PS;
		break;
	case VFMADD132PS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMADD132PD : VFMADD132PS;
		break;
	case VFMADD132SS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMADD132SD : VFMADD132SS;
		break;
	case VFMSUB132PS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMSUB132PD : VFMSUB132PS;
		break;
	case VFMSUB132SS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMSUB132SD : VFMSUB132SS;
		break;
	case VFNMADD132PS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFNMADD132PD : VFNMADD132PS;
		break;
	case VFNMADD132SS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFNMADD132SD : VFNMADD132SS;
		break;
	case VFNMSUB132PS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFNMSUB132PD : VFNMSUB132PS;
		break;
	case VFNMSUB132SS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFNMSUB132SD : VFNMSUB132SS;
		break;
	case VFMADDSUB213PS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMADDSUB213PD : VFMADDSUB213PS;
		break;
	case VFMSUBADD213PS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMSUBADD213PD : VFMSUBADD213PS;
		break;
	case VFMADD213PS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMADD213PD : VFMADD213PS;
		break;
	case VFMADD213SS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMADD213SD : VFMADD213SS;
		break;
	case VFMSUB213PS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMSUB213PD : VFMSUB213PS;
		break;
	case VFMSUB213SS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMSUB213SD : VFMSUB213SS;
		break;
	case VFNMADD213PS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFNMADD213PD : VFNMADD213PS;
		break;
	case VFNMADD213SS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFNMADD213SD : VFNMADD213SS;
		break;
	case VFNMSUB213PS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFNMSUB213PD : VFNMSUB213PS;
		break;
	case VFNMSUB213SS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFNMSUB213SD : VFNMSUB213SS;
		break;
	case VFMADDSUB231PS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMADDSUB231PD : VFMADDSUB231PS;
		break;
	case VFMSUBADD231PS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMSUBADD231PD : VFMSUBADD231PS;
		break;
	case VFMADD231PS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMADD231PD : VFMADD231PS;
		break;
	case VFMADD231SS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMADD231SD : VFMADD231SS;
		break;
	case VFMSUB231PS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMSUB231PD : VFMSUB231PS;
		break;
	case VFMSUB231SS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMSUB231SD : VFMSUB231SS;
		break;
	case VFNMADD231PS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFNMADD231PD : VFNMADD231PS;
		break;
	case VFNMADD231SS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFNMADD231SD : VFNMADD231SS;
		break;
	case VFNMSUB231PS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFNMSUB231PD : VFNMSUB231PS;
		break;
	case VFNMSUB231SS:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFNMSUB231SD : VFNMSUB231SS;
		break;
	case VAESIMC:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? AESIMC : VAESIMC;
		break;
	case VAESENC:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? AESENC : VAESENC;
		break;
	case VAESENCLAST:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? AESENCLAST : VAESENCLAST;
		break;
	case VAESDEC:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? AESDEC : VAESDEC;
		break;
	case VAESDECLAST:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? AESDECLAST : VAESDECLAST;
		break;
	case VROUNDPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? ROUNDPS : VROUNDPS;
		break;
	case VROUNDPD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? ROUNDPD : VROUNDPD;
		break;
	case VROUNDSS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? ROUNDSS : VROUNDSS;
		break;
	case VROUNDSD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? ROUNDSD : VROUNDSD;
		break;
	case VBLENDPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? BLENDPS : VBLENDPS;
		break;
	case VBLENDPD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? BLENDPD : VBLENDPD;
		break;
	case VPBLENDW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PBLENDW : VPBLENDW;
		break;
	case VPALIGNR:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PALIGNR : VPALIGNR;
		break;
	case VPEXTRB:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PEXTRB : VPEXTRB;
		break;
	case VPEXTRD:
		params->opcode.mnemonic = params->rexPrefix.W ? PEXTRQ : params->vexPrefix.W || params->evexPrefix.W ? VPEXTRQ : !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PEXTRD : VPEXTRD;
		break;
	case VEXTRACTPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? EXTRACTPS : VEXTRACTPS;
		break;
	case VINSERTF128:
		params->opcode.mnemonic = !params->evexPrefix.isValidEVEX ? VINSERTF128 : params->evexPrefix.W ? VINSERTF64X2 : VINSERTF32X4;
		break;
	case VEXTRACTF128:
		params->opcode.mnemonic = !params->evexPrefix.isValidEVEX ? VEXTRACTF128 : params->evexPrefix.W ? VEXTRACTF64X2 : VEXTRACTF32X4;
		break;
	case VPINSRB:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PINSRB : VPINSRB;
		break;
	case VINSERTPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? INSERTPS : VINSERTPS;
		break;
	case VPINSRD:
		params->opcode.mnemonic = params->rexPrefix.W ? PINSRQ : params->vexPrefix.W || params->evexPrefix.W ? VPINSRQ : !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PINSRD : VPINSRD;
		break;
	case VINSERTI128:
		params->opcode.mnemonic = !params->evexPrefix.isValidEVEX ? VINSERTI128 : params->evexPrefix.W ? VINSERTI64X2 : VINSERTI32X4;
		break;
	case VEXTRACTI128:
		params->opcode.mnemonic = !params->evexPrefix.isValidEVEX ? VEXTRACTI128 : params->evexPrefix.W ? VEXTRACTI64X2 : VEXTRACTI32X4;
		break;
	case VDPPS:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? DPPS : VDPPS;
		break;
	case VDPPD:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? DPPD : VDPPD;
		break;
	case VMPSADBW:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? MPSADBW : VMPSADBW;
		break;
	case VPCLMULQDQ:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PCLMULQDQ : VPCLMULQDQ;
		break;
	case VPCMPESTRM:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? PCMPESTRM : VPCMPESTRM;
		break;
	case VPCMPESTRI:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? PCMPESTRI : VPCMPESTRI;
		break;
	case VPCMPISTRM:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? PCMPISTRM : VPCMPISTRM;
		break;
	case VPCMPISTRI:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? PCMPISTRI : VPCMPISTRI;
		break;
	case VAESKEYGEN:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? AESKEYGEN : VAESKEYGEN;
		break;
	case VMOVDQU8:
		params->opcode.mnemonic = params->evexPrefix.W ? VMOVDQU16 : VMOVDQU8;
		break;
	case VPTERNLOGD:
		params->opcode.mnemonic = params->evexPrefix.W ? VPTERNLOGQ : VPTERNLOGD;
		break;
	case VINSERTI32X8:
		params->opcode.mnemonic = params->evexPrefix.W ? VINSERTI64X4 : VINSERTI32X8;
		break;
	case VSHUFI32X4:
		params->opcode.mnemonic = params->evexPrefix.W ? VSHUFI64X2 : VSHUFI32X4;
		break;
	}
}
