#include "opcodes.h"
#include "modRM.h"
#include "mnemonics.h"

#include "oneByteOpcodeMap.h"
#include "twoByteOpcodeMap.h"
#include "threeByteOpcodeMaps.h"
#include "extendedOpcodeMaps.h"
#include "escapeOpcodeMaps.h"

enum JdcStatus handleOpcode(struct DisassemblyParameters* params)
{
	if (params->bytes > params->maxBytesAddr) { return ERROR_JDC; }

	uint8_t prefixIndex = params->legPrefixes.group1 == REPNZ_BND ? 3 : params->legPrefixes.group1 == REPZ ? 2 : params->legPrefixes.group3 == OSO ? 1 : 0;
	uint8_t opcodeByte = 0;
	bool escapeToCoprocessor = false;

	int32_t mmm = params->vexPrefix.isValidVEX ? params->vexPrefix.m_mmmm : params->evexPrefix.isValidEVEX ? params->evexPrefix.mmm : 0;
	bool mmmIsValid = mmm == 1 || mmm == 2 || mmm == 3; // 1: 0x0F; 2: 0x0F 0x38; 3: 0x0F 0x3A
	if (params->bytes[0] == 0x0F || mmmIsValid)
	{
		uint8_t opcodeByteIndex = mmmIsValid ? 0 : 2;
		if ((params->bytes + opcodeByteIndex) <= params->maxBytesAddr &&
			((!mmmIsValid && params->bytes[1] == 0x38) || mmm == 2)) // sequence: 0x0F 0x38 opcode
		{
			opcodeByte = params->bytes[opcodeByteIndex];
			params->bytes += opcodeByteIndex + 1;

			if (threeByteOpcodeMap38[opcodeByte][prefixIndex].mnemonic == NO_OPCODE)
			{
				params->opcode = threeByteOpcodeMap38[opcodeByte][0];
			}
			else
			{
				params->opcode = threeByteOpcodeMap38[opcodeByte][prefixIndex];

				if (prefixIndex == 3 || prefixIndex == 2)
				{
					params->legPrefixes.group1 = NO_PREFIX;
				}
				else if (prefixIndex == 1)
				{
					params->legPrefixes.group3 = NO_PREFIX;
				}
			}
		}
		else if ((params->bytes + opcodeByteIndex) <= params->maxBytesAddr &&
			((!mmmIsValid && params->bytes[1] == 0x3A) || mmm == 3)) // sequence: 0x0F 0x3A opcode
		{
			opcodeByte = params->bytes[opcodeByteIndex];
			params->bytes += opcodeByteIndex + 1;

			if (threeByteOpcodeMap3A[opcodeByte][prefixIndex].mnemonic == NO_OPCODE)
			{
				params->opcode = threeByteOpcodeMap3A[opcodeByte][0];
			}
			else
			{
				params->opcode = threeByteOpcodeMap3A[opcodeByte][prefixIndex];

				if (prefixIndex == 3 || prefixIndex == 2)
				{
					params->legPrefixes.group1 = NO_PREFIX;
				}
				else if (prefixIndex == 1)
				{
					params->legPrefixes.group3 = NO_PREFIX;
				}
			}
		}
		else if (mmmIsValid || (params->bytes + 1) <= params->maxBytesAddr) // sequence: 0x0F opcode
		{
			opcodeByteIndex = mmmIsValid ? 0 : 1;
			opcodeByte = params->bytes[opcodeByteIndex];
			params->bytes += opcodeByteIndex + 1;;

			if (twoByteOpcodeMap[opcodeByte][prefixIndex].mnemonic == NO_OPCODE)
			{
				params->opcode = twoByteOpcodeMap[opcodeByte][0];
			}
			else
			{
				params->opcode = twoByteOpcodeMap[opcodeByte][prefixIndex];

				if (prefixIndex == 3 || prefixIndex == 2)
				{
					params->legPrefixes.group1 = NO_PREFIX;
				}
				else if (prefixIndex == 1)
				{
					params->legPrefixes.group3 = NO_PREFIX;
				}
			}

			if (opcodeByte == 0x12 || opcodeByte == 0x16)
			{
				if (ERROR_JDC == getModRM(params))
				{
					return ERROR_JDC;
				}

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
			else if (opcodeByte == 0x1E)
			{
				if (params->bytes[0] == 0xFA)
				{
					params->opcode.mnemonic = ENDBR64_X86;
					params->bytes++;
				}
				else if (params->bytes[0] == 0xFB)
				{
					params->opcode.mnemonic = ENDBR32_X86;
					params->bytes++;
				}
				else
				{
					if (ERROR_JDC == getModRM(params) || params->modRM.mod != 0b11)
					{
						return ERROR_JDC;
					}

					params->opcode = alternateX1E;
					if (params->rexPrefix.W)
					{
						params->opcode.mnemonic = RDSSPQ_X86;
					}
				}
			}
		}
		else
		{
			return ERROR_JDC;
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
		return ERROR_JDC;
	}

	// handle extended opcodes or escape opcodes
	if (params->opcode.mnemonic == EXTENDED_OPCODE_X86)
	{
		if (ERROR_JDC == getModRM(params))
		{
			return ERROR_JDC;
		}

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
					params->legPrefixes.group1 = NO_PREFIX;
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
					params->legPrefixes.group3 = NO_PREFIX;
				}
				else if (params->legPrefixes.group1 == REPZ)
				{
					extendedOpcode = &extendedOpcodeMapGroup9F3[params->modRM.reg];
					params->legPrefixes.group1 = NO_PREFIX;
				}
				else
				{
					extendedOpcode = &extendedOpcodeMapGroup9[params->modRM.reg];

					if (extendedOpcode->mnemonic == CMPXCH8B_X86 && params->rexPrefix.W)
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
					params->legPrefixes.group3 = NO_PREFIX;
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
					params->legPrefixes.group3 = NO_PREFIX;
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
					params->legPrefixes.group3 = NO_PREFIX;
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
					params->legPrefixes.group1 = NO_PREFIX;
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
			return ERROR_JDC;
		}

		params->opcode.mnemonic = extendedOpcode->mnemonic;

		for (int32_t i = 0; i < 4; i++)
		{
			if (extendedOpcode->operands[i] != NONE_OC)
			{
				params->opcode.operands[i] = extendedOpcode->operands[i];
			}
		}

		if (extendedOpcode->opcodeSuperscript != NO_SUPERSCRIPT)
		{
			params->opcode.opcodeSuperscript = extendedOpcode->opcodeSuperscript;
		}

		if (params->opcode.extensionGroup == 3 && opcodeByte == 0xF7 && params->opcode.mnemonic == TEST_X86)
		{
			params->opcode.operands[1] = IMM_WD_X86_OC;
		}
	}
	else if (escapeToCoprocessor) // escape to coprocessor instruction set
	{
		uint8_t modRMByte = params->bytes[0];
		if (ERROR_JDC == getModRM(params))
		{
			return ERROR_JDC;
		}

		switch (opcodeByte)
		{
		case 0xD8:
			params->opcode = modRMByte < 0xC0 ? escapeD8OpcodeMapBits[params->modRM.reg] : escapeD8OpcodeMapByte[modRMByte - 0xC0];
			break;
		case 0xD9:
			params->opcode = modRMByte < 0xC0 ? escapeD9OpcodeMapBits[params->modRM.reg] : escapeD9OpcodeMapByte[modRMByte - 0xC0];
			break;
		case 0xDA:
			params->opcode = modRMByte < 0xC0 ? escapeDAOpcodeMapBits[params->modRM.reg] : escapeDAOpcodeMapByte[modRMByte - 0xC0];
			break;
		case 0xDB:
			params->opcode = modRMByte < 0xC0 ? escapeDBOpcodeMapBits[params->modRM.reg] : escapeDBOpcodeMapByte[modRMByte - 0xC0];
			break;
		case 0xDC:
			params->opcode = modRMByte < 0xC0 ? escapeDCOpcodeMapBits[params->modRM.reg] : escapeDCOpcodeMapByte[modRMByte - 0xC0];
			break;
		case 0xDD:
			params->opcode = modRMByte < 0xC0 ? escapeDDOpcodeMapBits[params->modRM.reg] : escapeDDOpcodeMapByte[modRMByte - 0xC0];
			break;
		case 0xDE:
			params->opcode = modRMByte < 0xC0 ? escapeDEOpcodeMapBits[params->modRM.reg] : escapeDEOpcodeMapByte[modRMByte - 0xC0];
			break;
		case 0xDF:
			params->opcode = modRMByte < 0xC0 ? escapeDFOpcodeMapBits[params->modRM.reg] : escapeDFOpcodeMapByte[modRMByte - 0xC0];
			break;
		}
	}

	if (params->opcode.mnemonic == NO_OPCODE ||
		(params->is64BitMode && params->opcode.opcodeSuperscript == i64) ||
		(!params->is64BitMode && params->opcode.opcodeSuperscript == o64))
	{
		return ERROR_JDC;
	}

	handleAlternateMnemonics(params);
	return SUCCESS_JDC;
}

static void handleAlternateMnemonics(struct DisassemblyParameters* params) // this should only be for when the alternate mnemonic represents a different instruction
{
	switch (params->opcode.mnemonic)
	{
	case CWDE_X86:
		params->opcode.mnemonic = params->rexPrefix.W ? CDQE_X86 : params->legPrefixes.group1 == OSO ? CBW_X86 : CWDE_X86;
		break;
	case CDQ_X86:
		params->opcode.mnemonic = params->rexPrefix.W ? CQO_X86 : params->legPrefixes.group1 == OSO ? CWD_X86 : CDQ_X86;
		break;
	case VMOVUPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVUPS_X86 : VMOVUPS_X86;
		break;
	case VMOVUPD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVUPD_X86 : VMOVUPD_X86;
		break;
	case VMOVSS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVSS_X86 : VMOVSS_X86;
		break;
	case VMOVSD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVSD_X86 : VMOVSD_X86;
		break;
	case VMOVLPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVLPS_X86 : VMOVLPS_X86;
		break;
	case VMOVHLPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVHLPS_X86 : VMOVHLPS_X86;
		break;
	case VMOVLPD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVLPD_X86 : VMOVLPD_X86;
		break;
	case VMOVSLDUP_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVSLDUP_X86 : VMOVSLDUP_X86;
		break;
	case VMOVDDUP_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVDDUP_X86 : VMOVDDUP_X86;
		break;
	case VUNPCKLPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? UNPCKLPS_X86 : VUNPCKLPS_X86;
		break;
	case VUNPCKLPD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? UNPCKLPD_X86 : VUNPCKLPD_X86;
		break;
	case VUNPCKHPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? UNPCKHPS_X86 : VUNPCKHPS_X86;
		break;
	case VUNPCKHPD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? UNPCKHPD_X86 : VUNPCKHPD_X86;
		break;
	case VMOVHPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVHPS_X86 : VMOVHPS_X86;
		break;
	case VMOVLHPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVLHPS_X86 : VMOVLHPS_X86;
		break;
	case VMOVHPD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVHPD_X86 : VMOVHPD_X86;
		break;
	case VMOVSHDUP_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVSHDUP_X86 : VMOVSHDUP_X86;
		break;
	case VMOVAPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVAPS_X86 : VMOVAPS_X86;
		break;
	case VMOVAPD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVAPD_X86 : VMOVAPD_X86;
		break;
	case VCVTSI2SS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTSI2SS_X86 : VCVTSI2SS_X86;
		break;
	case VCVTSI2SD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTSI2SD_X86 : VCVTSI2SD_X86;
		break;
	case VMOVNTPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVNTPS_X86 : VMOVNTPS_X86;
		break;
	case VMOVNTPD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVNTPD_X86 : VMOVNTPD_X86;
		break;
	case VCVTTSS2SI_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTTSS2SI_X86 : VCVTTSS2SI_X86;
		break;
	case VCVTTSD2SI_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTTSD2SI_X86 : VCVTTSD2SI_X86;
		break;
	case VCVTSS2SI_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTSS2SI_X86 : VCVTSS2SI_X86;
		break;
	case VCVTSD2SI_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTSD2SI_X86 : VCVTSD2SI_X86;
		break;
	case VUCOMISS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? UCOMISS_X86 : VUCOMISS_X86;
		break;
	case VUCOMISD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? UCOMISD_X86 : VUCOMISD_X86;
		break;
	case VCOMISS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? COMISS_X86 : VCOMISS_X86;
		break;
	case VCOMISD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? COMISD_X86 : VCOMISD_X86;
		break;
	case VMOVMSKPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVMSKPS_X86 : VMOVMSKPS_X86;
		break;
	case VMOVMSKPD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVMSKPD_X86 : VMOVMSKPD_X86;
		break;
	case VSQRTPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? SQRTPS_X86 : VSQRTPS_X86;
		break;
	case VSQRTPD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? SQRTPD_X86 : VSQRTPD_X86;
		break;
	case VSQRTSS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? SQRTSS_X86 : VSQRTSS_X86;
		break;
	case VSQRTSD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? SQRTSD_X86 : VSQRTSD_X86;
		break;
	case VRSQRTPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? RSQRTPS_X86 : VRSQRTPS_X86;
		break;
	case VRSQRTSS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? RSQRTSS_X86 : VRSQRTSS_X86;
		break;
	case VRCPPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? RCPPS_X86 : VRCPPS_X86;
		break;
	case VRCPSS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? RCPSS_X86 : VRCPSS_X86;
		break;
	case VANDPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? ANDPS_X86 : VANDPS_X86;
		break;
	case VANDPD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? ANDPD_X86 : VANDPD_X86;
		break;
	case VANDNPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? ANDNPS_X86 : VANDNPS_X86;
		break;
	case VANDNPD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? ANDNPD_X86 : VANDNPD_X86;
		break;
	case VORPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? ORPS_X86 : VORPS_X86;
		break;
	case VORPD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? ORPD_X86 : VORPD_X86;
		break;
	case VXORPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? XORPS_X86 : VXORPS_X86;
		break;
	case VXORPD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? XORPD_X86 : VXORPD_X86;
		break;
	case VADDPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? ADDPS_X86 : VADDPS_X86;
		break;
	case VADDPD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? ADDPD_X86 : VADDPD_X86;
		break;
	case VADDSS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? ADDSS_X86 : VADDSS_X86;
		break;
	case VADDSD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? ADDSD_X86 : VADDSD_X86;
		break;
	case VMULPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MULPS_X86 : VMULPS_X86;
		break;
	case VMULPD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MULPD_X86 : VMULPD_X86;
		break;
	case VMULSS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MULSS_X86 : VMULSS_X86;
		break;
	case VMULSD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MULSD_X86 : VMULSD_X86;
		break;
	case VCVTPS2PD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTPS2PD_X86 : VCVTPS2PD_X86;
		break;
	case VCVTPD2PS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTPD2PS_X86 : VCVTPD2PS_X86;
		break;
	case VCVTSS2SD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTSS2SD_X86 : VCVTSS2SD_X86;
		break;
	case VCVTSD2SS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTSD2SS_X86 : VCVTSD2SS_X86;
		break;
	case VCVTDQ2PS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTDQ2PS_X86 : VCVTDQ2PS_X86;
		break;
	case VCVTPS2DQ_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTPS2DQ_X86 : VCVTPS2DQ_X86;
		break;
	case VCVTTPS2DQ_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTTPS2DQ_X86 : VCVTTPS2DQ_X86;
		break;
	case VSUBPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? SUBPS_X86 : VSUBPS_X86;
		break;
	case VSUBPD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? SUBPD_X86 : VSUBPD_X86;
		break;
	case VSUBSS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? SUBSS_X86 : VSUBSS_X86;
		break;
	case VSUBSD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? SUBSD_X86 : VSUBSD_X86;
		break;
	case VMINPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MINPS_X86 : VMINPS_X86;
		break;
	case VMINPD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MINPD_X86 : VMINPD_X86;
		break;
	case VMINSS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MINSS_X86 : VMINSS_X86;
		break;
	case VMINSD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MINSD_X86 : VMINSD_X86;
		break;
	case VDIVPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? DIVPS_X86 : VDIVPS_X86;
		break;
	case VDIVPD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? DIVPD_X86 : VDIVPD_X86;
		break;
	case VDIVSS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? DIVSS_X86 : VDIVSS_X86;
		break;
	case VDIVSD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? DIVSD_X86 : VDIVSD_X86;
		break;
	case VMAXPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MAXPS_X86 : VMAXPS_X86;
		break;
	case VMAXPD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MAXPD_X86 : VMAXPD_X86;
		break;
	case VMAXSS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MAXSS_X86 : VMAXSS_X86;
		break;
	case VMAXSD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MAXSD_X86 : VMAXSD_X86;
		break;
	case VPUNPCKLBW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PUNPCKLBW_X86 : VPUNPCKLBW_X86;
		break;
	case VPUNPCKLWD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PUNPCKLWD_X86 : VPUNPCKLWD_X86;
		break;
	case VPUNPCKLDQ_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PUNPCKLDQ_X86 : VPUNPCKLDQ_X86;
		break;
	case VPACKSSWB_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PACKSSWB_X86 : VPACKSSWB_X86;
		break;
	case VPCMPGTB_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PCMPGTB_X86 : VPCMPGTB_X86;
		break;
	case VPCMPGTW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PCMPGTW_X86 : VPCMPGTW_X86;
		break;
	case VPCMPGTD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PCMPGTD_X86 : VPCMPGTD_X86;
		break;
	case VPACKUSWB_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PACKUSWB_X86 : VPACKUSWB_X86;
		break;
	case VPUNPCKHBW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PUNPCKHBW_X86 : VPUNPCKHBW_X86;
		break;
	case VPUNPCKHWD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PUNPCKHWD_X86 : VPUNPCKHWD_X86;
		break;
	case VPUNPCKHDQ_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PUNPCKHDQ_X86 : VPUNPCKHDQ_X86;
		break;
	case VPACKSSDW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PACKSSDW_X86 : VPACKSSDW_X86;
		break;
	case VPUNPCKLQDQ_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PUNPCKLQDQ_X86 : VPUNPCKLQDQ_X86;
		break;
	case VPUNPCKHQDQ_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PUNPCKHQDQ_X86 : VPUNPCKHQDQ_X86;
		break;
	case MOVD_X86:
		params->opcode.mnemonic = params->rexPrefix.W ? MOVQ_X86 : params->vexPrefix.W || params->evexPrefix.W ? VMOVQ_X86 : !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVD_X86 : VMOVD_X86;
		break;
	case VMOVD_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VMOVQ_X86 : !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVD_X86 : VMOVD_X86;
		break;
	case VMOVDQA_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVDQA_X86 : VMOVDQA_X86;
		break;
	case VMOVDQU_X86:
		params->opcode.mnemonic = params->evexPrefix.W ? VMOVDQU64_X86 : params->evexPrefix.isValidEVEX ? VMOVDQU32_X86 : params->vexPrefix.isValidVEX ? VMOVDQU_X86 : MOVDQU_X86;
		break;
	case VPSHUFD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSHUFD_X86 : VPSHUFD_X86;
		break;
	case VPSHUFHW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSHUFHW_X86 : VPSHUFHW_X86;
		break;
	case VPSHUFLW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSHUFLW_X86 : VPSHUFLW_X86;
		break;
	case VPCMPEQB_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PCMPEQB_X86 : VPCMPEQB_X86;
		break;
	case VPCMPEQW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PCMPEQW_X86 : VPCMPEQW_X86;
		break;
	case VPCMPEQD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PCMPEQD_X86 : VPCMPEQD_X86;
		break;
	case EMMS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? EMMS_X86 : params->vexPrefix.L == 0 ? VZEROUPPER_X86 : VZEROALL_X86;
		break;
	case VHADDPD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? HADDPD_X86 : VHADDPD_X86;
		break;
	case VHADDPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? HADDPS_X86 : VHADDPS_X86;
		break;
	case VHSUBPD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? HSUBPD_X86 : VHSUBPD_X86;
		break;
	case VHSUBPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? HSUBPS_X86 : VHSUBPS_X86;
		break;
	case VCMPPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CMPPS_X86 : VCMPPS_X86;
		break;
	case VCMPPD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CMPPD_X86 : VCMPPD_X86;
		break;
	case VCMPSS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CMPSS_X86 : VCMPSS_X86;
		break;
	case VCMPSD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CMPSD_X86 : VCMPSD_X86;
		break;
	case VPINSRW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PINSRW_X86 : VPINSRW_X86;
		break;
	case VPEXTRW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PEXTRW_X86 : VPEXTRW_X86;
		break;
	case VSHUFPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? SHUFPS_X86 : VSHUFPS_X86;
		break;
	case VSHUFPD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? SHUFPD_X86 : VSHUFPD_X86;
		break;
	case VADDSUBPD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? ADDSUBPD_X86 : VADDSUBPD_X86;
		break;
	case VADDSUBPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? ADDSUBPS_X86 : VADDSUBPS_X86;
		break;
	case VPSRLW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSRLW_X86 : VPSRLW_X86;
		break;
	case VPSRLD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSRLD_X86 : VPSRLD_X86;
		break;
	case VPSRLQ_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSRLQ_X86 : VPSRLQ_X86;
		break;
	case VPSRLDQ_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSRLDQ_X86 : VPSRLDQ_X86;
		break;
	case VPADDQ_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PADDQ_X86 : VPADDQ_X86;
		break;
	case VPMULLW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMULLW_X86 : VPMULLW_X86;
		break;
	case VPMOVMSKB_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMOVMSKB_X86 : VPMOVMSKB_X86;
		break;
	case VPSUBUSB_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSUBUSB_X86 : VPSUBUSB_X86;
		break;
	case VPSUBUSW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSUBUSW_X86 : VPSUBUSW_X86;
		break;
	case VPMINUB_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMINUB_X86 : VPMINUB_X86;
		break;
	case VPAND_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PAND_X86 : VPAND_X86;
		break;
	case VPADDUSB_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PADDUSB_X86 : VPADDUSB_X86;
		break;
	case VPADDUSW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PADDUSW_X86 : VPADDUSW_X86;
		break;
	case VPANDN_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PANDN_X86 : VPANDN_X86;
		break;
	case VPAVGB_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PAVGB_X86 : VPAVGB_X86;
		break;
	case VPSRAW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSRAW_X86 : VPSRAW_X86;
		break;
	case VPSRAD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSRAD_X86 : VPSRAD_X86;
		break;
	case VPAVGW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PAVGW_X86 : VPAVGW_X86;
		break;
	case VPMULHUW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMULHUW_X86 : VPMULHUW_X86;
		break;
	case VPMULHW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMULHW_X86 : VPMULHW_X86;
		break;
	case VCVTTPD2DQ_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTTPD2DQ_X86 : VCVTTPD2DQ_X86;
		break;
	case VCVTDQ2PD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTDQ2PD_X86 : VCVTDQ2PD_X86;
		break;
	case VCVTPD2DQ_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? CVTPD2DQ_X86 : VCVTPD2DQ_X86;
		break;
	case VMOVNTDQ_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVNTDQ_X86 : VMOVNTDQ_X86;
		break;
	case VPSUBSB_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSUBSB_X86 : VPSUBSB_X86;
		break;
	case VPSUBSW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSUBSW_X86 : VPSUBSW_X86;
		break;
	case VPMINSW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMINSW_X86 : VPMINSW_X86;
		break;
	case VPOR_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? POR_X86 : VPOR_X86;
		break;
	case VPADDSB_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PADDSB_X86 : VPADDSB_X86;
		break;
	case VPADDSW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PADDSW_X86 : VPADDSW_X86;
		break;
	case VPMAXSW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMAXSW_X86 : VPMAXSW_X86;
		break;
	case VPXOR_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PXOR_X86 : VPXOR_X86;
		break;
	case VLDDQU_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? LDDQU_X86 : VLDDQU_X86;
		break;
	case VPSLLW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSLLW_X86 : VPSLLW_X86;
		break;
	case VPSLLD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSLLD_X86 : VPSLLD_X86;
		break;
	case VPSLLQ_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSLLQ_X86 : VPSLLQ_X86;
		break;
	case VPMULUDQ_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMULUDQ_X86 : VPMULUDQ_X86;
		break;
	case VPMADDWD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMADDWD_X86 : VPMADDWD_X86;
		break;
	case VPSADBW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSADBW_X86 : VPSADBW_X86;
		break;
	case VMASKMOVDQU_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? MASKMOVDQU_X86 : VMASKMOVDQU_X86;
		break;
	case VPSUBB_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSUBB_X86 : VPSUBB_X86;
		break;
	case VPSUBW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSUBW_X86 : VPSUBW_X86;
		break;
	case VPSUBD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSUBD_X86 : VPSUBD_X86;
		break;
	case VPSUBQ_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSUBQ_X86 : VPSUBQ_X86;
		break;
	case VPADDB_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PADDB_X86 : VPADDB_X86;
		break;
	case VPADDW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PADDW_X86 : VPADDW_X86;
		break;
	case VPADDD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PADDD_X86 : VPADDD_X86;
		break;
	case VPSHUFB_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSHUFB_X86 : VPSHUFB_X86;
		break;
	case VPHADDW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PHADDW_X86 : VPHADDW_X86;
		break;
	case VPHADDD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PHADDD_X86 : VPHADDD_X86;
		break;
	case VPHADDSW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PHADDSW_X86 : VPHADDSW_X86;
		break;
	case VPMADDUBSW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMADDUBSW_X86 : VPMADDUBSW_X86;
		break;
	case VPHSUBW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PHSUBW_X86 : VPHSUBW_X86;
		break;
	case VPHSUBD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PHSUBD_X86 : VPHSUBD_X86;
		break;
	case VPHSUBSW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PHSUBSW_X86 : VPHSUBSW_X86;
		break;
	case VPSIGNB_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSIGNB_X86 : VPSIGNB_X86;
		break;
	case VPSIGNW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSIGNW_X86 : VPSIGNW_X86;
		break;
	case VPSIGND_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PSIGND_X86 : VPSIGND_X86;
		break;
	case VPMULHRSW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMULHRSW_X86 : VPMULHRSW_X86;
		break;
	case VPTEST_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? PTEST_X86 : VPTEST_X86;
		break;
	case VPABSB_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PABSB_X86 : VPABSB_X86;
		break;
	case VPABSW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PABSW_X86 : VPABSW_X86;
		break;
	case VPABSD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PABSD_X86 : VPABSD_X86;
		break;
	case VPMOVSXBW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMOVSXBW_X86 : VPMOVSXBW_X86;
		break;
	case VPMOVSXBD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMOVSXBD_X86 : VPMOVSXBD_X86;
		break;
	case VPMOVSXBQ_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMOVSXBQ_X86 : VPMOVSXBQ_X86;
		break;
	case VPMOVSXWD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMOVSXWD_X86 : VPMOVSXWD_X86;
		break;
	case VPMOVSXWQ_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMOVSXWQ_X86 : VPMOVSXWQ_X86;
		break;
	case VPMOVSXDQ_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMOVSXDQ_X86 : VPMOVSXDQ_X86;
		break;
	case VPMULDQ_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMULDQ_X86 : VPMULDQ_X86;
		break;
	case VPCMPEQQ_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PCMPEQQ_X86 : VPCMPEQQ_X86;
		break;
	case VMOVNTDQA_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? MOVNTDQA_X86 : VMOVNTDQA_X86;
		break;
	case VPACKUSDW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PACKUSDW_X86 : VPACKUSDW_X86;
		break;
	case VPMOVZXBW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMOVZXBW_X86 : VPMOVZXBW_X86;
		break;
	case VPMOVZXBD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMOVZXBD_X86 : VPMOVZXBD_X86;
		break;
	case VPMOVZXBQ_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMOVZXBQ_X86 : VPMOVZXBQ_X86;
		break;
	case VPMOVZXWD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMOVZXWD_X86 : VPMOVZXWD_X86;
		break;
	case VPMOVZXWQ_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMOVZXWQ_X86 : VPMOVZXWQ_X86;
		break;
	case VPMOVZXDQ_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMOVZXDQ_X86 : VPMOVZXDQ_X86;
		break;
	case VPCMPGTQ_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PCMPGTQ_X86 : VPCMPGTQ_X86;
		break;
	case VPMINSB_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMINSB_X86 : VPMINSB_X86;
		break;
	case VPMINSD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMINSD_X86 : VPMINSD_X86;
		break;
	case VPMINUW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMINUW_X86 : VPMINUW_X86;
		break;
	case VPMINUD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMINUD_X86 : VPMINUD_X86;
		break;
	case VPMAXSB_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMAXSB_X86 : VPMAXSB_X86;
		break;
	case VPMAXSD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMAXSD_X86 : VPMAXSD_X86;
		break;
	case VPMAXUW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMAXUW_X86 : VPMAXUW_X86;
		break;
	case VPMAXUD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMAXUD_X86 : VPMAXUD_X86;
		break;
	case VPMULLD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PMULLD_X86 : VPMULLD_X86;
		break;
	case VPHMINPOSUW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? PHMINPOSUW_X86 : VPHMINPOSUW_X86;
		break;
	case VPSRLVD_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VPSRLVQ_X86 : VPSRLVD_X86;
		break;
	case VPSLLVD_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VPSLLVQ_X86 : VPSLLVD_X86;
		break;
	case VBROADCASTI128_X86:
		params->opcode.mnemonic = !params->evexPrefix.isValidEVEX ? VBROADCASTI128_X86 : params->evexPrefix.W ? VBROADCASTI64X2_X86 : VBROADCASTI32X4_X86;
		break;
	case VPMASKMOVD_X86:
		params->opcode.mnemonic = params->vexPrefix.W ? VPMASKMOVQ_X86 : VPMASKMOVD_X86;
		break;
	case VGATHERDD_X86:
		params->opcode.mnemonic = params->evexPrefix.W ? VGATHERDQ_X86 : VGATHERDD_X86;
		break;
	case VGATHERQD_X86:
		params->opcode.mnemonic = params->evexPrefix.W ? VGATHERQQ_X86 : VGATHERQD_X86;
		break;
	case VGATHERDPS_X86:
		params->opcode.mnemonic = params->evexPrefix.W ? VGATHERDPD_X86 : VGATHERDPS_X86;
		break;
	case VGATHERQPS_X86:
		params->opcode.mnemonic = params->evexPrefix.W ? VGATHERQPD_X86 : VGATHERQPS_X86;
		break;
	case VFMADDSUB132PS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMADDSUB132PD_X86 : VFMADDSUB132PS_X86;
		break;
	case VFMSUBADD132PS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMSUBADD132PD_X86 : VFMSUBADD132PS_X86;
		break;
	case VFMADD132PS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMADD132PD_X86 : VFMADD132PS_X86;
		break;
	case VFMADD132SS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMADD132SD_X86 : VFMADD132SS_X86;
		break;
	case VFMSUB132PS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMSUB132PD_X86 : VFMSUB132PS_X86;
		break;
	case VFMSUB132SS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMSUB132SD_X86 : VFMSUB132SS_X86;
		break;
	case VFNMADD132PS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFNMADD132PD_X86 : VFNMADD132PS_X86;
		break;
	case VFNMADD132SS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFNMADD132SD_X86 : VFNMADD132SS_X86;
		break;
	case VFNMSUB132PS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFNMSUB132PD_X86 : VFNMSUB132PS_X86;
		break;
	case VFNMSUB132SS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFNMSUB132SD_X86 : VFNMSUB132SS_X86;
		break;
	case VFMADDSUB213PS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMADDSUB213PD_X86 : VFMADDSUB213PS_X86;
		break;
	case VFMSUBADD213PS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMSUBADD213PD_X86 : VFMSUBADD213PS_X86;
		break;
	case VFMADD213PS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMADD213PD_X86 : VFMADD213PS_X86;
		break;
	case VFMADD213SS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMADD213SD_X86 : VFMADD213SS_X86;
		break;
	case VFMSUB213PS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMSUB213PD_X86 : VFMSUB213PS_X86;
		break;
	case VFMSUB213SS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMSUB213SD_X86 : VFMSUB213SS_X86;
		break;
	case VFNMADD213PS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFNMADD213PD_X86 : VFNMADD213PS_X86;
		break;
	case VFNMADD213SS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFNMADD213SD_X86 : VFNMADD213SS_X86;
		break;
	case VFNMSUB213PS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFNMSUB213PD_X86 : VFNMSUB213PS_X86;
		break;
	case VFNMSUB213SS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFNMSUB213SD_X86 : VFNMSUB213SS_X86;
		break;
	case VFMADDSUB231PS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMADDSUB231PD_X86 : VFMADDSUB231PS_X86;
		break;
	case VFMSUBADD231PS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMSUBADD231PD_X86 : VFMSUBADD231PS_X86;
		break;
	case VFMADD231PS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMADD231PD_X86 : VFMADD231PS_X86;
		break;
	case VFMADD231SS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMADD231SD_X86 : VFMADD231SS_X86;
		break;
	case VFMSUB231PS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMSUB231PD_X86 : VFMSUB231PS_X86;
		break;
	case VFMSUB231SS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFMSUB231SD_X86 : VFMSUB231SS_X86;
		break;
	case VFNMADD231PS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFNMADD231PD_X86 : VFNMADD231PS_X86;
		break;
	case VFNMADD231SS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFNMADD231SD_X86 : VFNMADD231SS_X86;
		break;
	case VFNMSUB231PS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFNMSUB231PD_X86 : VFNMSUB231PS_X86;
		break;
	case VFNMSUB231SS_X86:
		params->opcode.mnemonic = params->vexPrefix.W || params->evexPrefix.W ? VFNMSUB231SD_X86 : VFNMSUB231SS_X86;
		break;
	case VAESIMC_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? AESIMC_X86 : VAESIMC_X86;
		break;
	case VAESENC_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? AESENC_X86 : VAESENC_X86;
		break;
	case VAESENCLAST_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? AESENCLAST_X86 : VAESENCLAST_X86;
		break;
	case VAESDEC_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? AESDEC_X86 : VAESDEC_X86;
		break;
	case VAESDECLAST_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? AESDECLAST_X86 : VAESDECLAST_X86;
		break;
	case VROUNDPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? ROUNDPS_X86 : VROUNDPS_X86;
		break;
	case VROUNDPD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? ROUNDPD_X86 : VROUNDPD_X86;
		break;
	case VROUNDSS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? ROUNDSS_X86 : VROUNDSS_X86;
		break;
	case VROUNDSD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? ROUNDSD_X86 : VROUNDSD_X86;
		break;
	case VBLENDPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? BLENDPS_X86 : VBLENDPS_X86;
		break;
	case VBLENDPD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? BLENDPD_X86 : VBLENDPD_X86;
		break;
	case VPBLENDW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PBLENDW_X86 : VPBLENDW_X86;
		break;
	case VPALIGNR_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PALIGNR_X86 : VPALIGNR_X86;
		break;
	case VPEXTRB_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PEXTRB_X86 : VPEXTRB_X86;
		break;
	case VPEXTRD_X86:
		params->opcode.mnemonic = params->rexPrefix.W ? PEXTRQ_X86 : params->vexPrefix.W || params->evexPrefix.W ? VPEXTRQ_X86 : !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PEXTRD_X86 : VPEXTRD_X86;
		break;
	case VEXTRACTPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? EXTRACTPS_X86 : VEXTRACTPS_X86;
		break;
	case VINSERTF128_X86:
		params->opcode.mnemonic = !params->evexPrefix.isValidEVEX ? VINSERTF128_X86 : params->evexPrefix.W ? VINSERTF64X2_X86 : VINSERTF32X4_X86;
		break;
	case VEXTRACTF128_X86:
		params->opcode.mnemonic = !params->evexPrefix.isValidEVEX ? VEXTRACTF128_X86 : params->evexPrefix.W ? VEXTRACTF64X2_X86 : VEXTRACTF32X4_X86;
		break;
	case VPINSRB_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PINSRB_X86 : VPINSRB_X86;
		break;
	case VINSERTPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? INSERTPS_X86 : VINSERTPS_X86;
		break;
	case VPINSRD_X86:
		params->opcode.mnemonic = params->rexPrefix.W ? PINSRQ_X86 : params->vexPrefix.W || params->evexPrefix.W ? VPINSRQ_X86 : !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PINSRD_X86 : VPINSRD_X86;
		break;
	case VINSERTI128_X86:
		params->opcode.mnemonic = !params->evexPrefix.isValidEVEX ? VINSERTI128_X86 : params->evexPrefix.W ? VINSERTI64X2_X86 : VINSERTI32X4_X86;
		break;
	case VEXTRACTI128_X86:
		params->opcode.mnemonic = !params->evexPrefix.isValidEVEX ? VEXTRACTI128_X86 : params->evexPrefix.W ? VEXTRACTI64X2_X86 : VEXTRACTI32X4_X86;
		break;
	case VDPPS_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? DPPS_X86 : VDPPS_X86;
		break;
	case VDPPD_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? DPPD_X86 : VDPPD_X86;
		break;
	case VMPSADBW_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? MPSADBW_X86 : VMPSADBW_X86;
		break;
	case VPCLMULQDQ_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX && !params->evexPrefix.isValidEVEX ? PCLMULQDQ_X86 : VPCLMULQDQ_X86;
		break;
	case VPCMPESTRM_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? PCMPESTRM_X86 : VPCMPESTRM_X86;
		break;
	case VPCMPESTRI_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? PCMPESTRI_X86 : VPCMPESTRI_X86;
		break;
	case VPCMPISTRM_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? PCMPISTRM_X86 : VPCMPISTRM_X86;
		break;
	case VPCMPISTRI_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? PCMPISTRI_X86 : VPCMPISTRI_X86;
		break;
	case VAESKEYGENASSIST_X86:
		params->opcode.mnemonic = !params->vexPrefix.isValidVEX ? AESKEYGENASSIST_X86 : VAESKEYGENASSIST_X86;
		break;
	case VMOVDQU8_X86:
		params->opcode.mnemonic = params->evexPrefix.W ? VMOVDQU16_X86 : VMOVDQU8_X86;
		break;
	case VPTERNLOGD_X86:
		params->opcode.mnemonic = params->evexPrefix.W ? VPTERNLOGQ_X86 : VPTERNLOGD_X86;
		break;
	case VINSERTI32X8_X86:
		params->opcode.mnemonic = params->evexPrefix.W ? VINSERTI64X4_X86 : VINSERTI32X8_X86;
		break;
	case VSHUFI32X4_X86:
		params->opcode.mnemonic = params->evexPrefix.W ? VSHUFI64X2_X86 : VSHUFI32X4_X86;
		break;
	}
}
