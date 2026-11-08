#include "modRM.h"
#include "registers.h"
#include "operands.h"
#include"prefixes.h"

enum JdcStatus getModRM(struct DisassemblyParameters* params)
{
	if (!params->modRM.hasGotModRM)
	{
		if (params->bytes > params->maxBytesAddr)
		{
			return ERROR_JDC;
		}

		params->modRM.mod = (((params->bytes[0] >> 7) & 0x01) * 2) + ((params->bytes[0] >> 6) & 0x01);
		params->modRM.reg = (((params->bytes[0] >> 5) & 0x01) * 4) + (((params->bytes[0] >> 4) & 0x01) * 2) + ((params->bytes[0] >> 3) & 0x01);
		params->modRM.rm = (((params->bytes[0] >> 2) & 0x01) * 4) + (((params->bytes[0] >> 1) & 0x01) * 2) + ((params->bytes[0] >> 0) & 0x01);

		params->modRM.hasGotModRM = true;
		params->bytes++;
	}

	return SUCCESS_JDC;
}

enum JdcStatus handleModRM(struct DisassemblyParameters* params, enum ModRMSelection selection, uint8_t operandSize, struct Operand* result)
{
	if (ERROR_JDC == getModRM(params))
	{
		return ERROR_JDC;
	}

	if (selection == REG_MODRM)
	{
		result->type = REGISTER_OPERAND;

		switch (operandSize)
		{
		case 1:
			result->reg = (params->modRM.reg + AL_X86_REG);
			break;
		case 2:
			result->reg = (params->modRM.reg + AX_X86_REG);
			break;
		case 4:
			result->reg = (params->modRM.reg + EAX_X86_REG);
			break;
		case 8:
			result->reg = (params->modRM.reg + RAX_X86_REG);
			break;
		case 16:
			result->reg = (params->modRM.reg + XMM0_X86_REG);
			break;
		case 32:
			result->reg = (params->modRM.reg + YMM0_X86_REG);
			break;
		case 64:
			result->reg = (params->is64BitMode && params->evexPrefix.isValidEVEX && !params->evexPrefix.R_prime) ? (params->modRM.reg + ZMM16_X86_REG) : (params->modRM.reg + ZMM0_X86_REG);
			break;
		}

		if (checkFlagR(params))
		{
			result->reg = extendRegister(result->reg);
		}

		return SUCCESS_JDC;
	}
	else if (selection == REG_SEG_MODRM)
	{
		result->type = REGISTER_OPERAND;
		result->reg = (params->modRM.reg + ES_X86_REG);
		return SUCCESS_JDC;
	}
	else if (selection == REG_MMX_MODRM)
	{
		result->type = REGISTER_OPERAND;
		result->reg = (params->modRM.reg + MM0_X86_REG);
		return SUCCESS_JDC;
	}
	else if (selection == REG_CONTROL_MODRM)
	{
		result->type = REGISTER_OPERAND;
		switch (operandSize)
		{
		case 4:
			result->reg = (params->modRM.reg + (params->rexPrefix.R ? CR8D_X86_REG : CR0D_X86_REG));
			break;
		case 8:
			result->reg = (params->modRM.reg + (params->rexPrefix.R ? CR8_X86_REG : CR0_X86_REG));
			break;
		}

		return SUCCESS_JDC;
	}
	else if (selection == REG_DEBUG_MODRM)
	{
		result->type = REGISTER_OPERAND;
		if (params->rexPrefix.R)
		{
			result->reg = (params->modRM.reg + DR8_X86_REG);
		}
		else
		{
			result->reg = (params->modRM.reg + DR0_X86_REG);
		}

		return SUCCESS_JDC;
	}
	else if (selection == RM_REG_MODRM)
	{
		result->type = REGISTER_OPERAND;
		switch (operandSize)
		{
		case 2:
			result->reg = (params->modRM.rm + AX_X86_REG);
			break;
		case 4:
			result->reg = (params->modRM.rm + EAX_X86_REG);
			break;
		case 8:
			result->reg = (params->modRM.rm + RAX_X86_REG);
			break;
		}

		return SUCCESS_JDC;
	}

	if (params->modRM.mod == 3)
	{
		if (selection == M_MODRM)
		{
			return ERROR_JDC;
		}

		result->type = REGISTER_OPERAND;

		if (selection == RM_MMX_MODRM || selection == RM_MOD11_MMX_MODRM)
		{
			result->reg = (params->modRM.rm + MM0_X86_REG);
			return SUCCESS_JDC;
		}

		switch (operandSize)
		{
		case 1:
			result->reg = (params->modRM.rm + AL_X86_REG);
			break;
		case 2:
			result->reg = (params->modRM.rm + AX_X86_REG);
			break;
		case 4:
			result->reg = (params->modRM.rm + EAX_X86_REG);
			break;
		case 8:
			result->reg = (params->modRM.rm + RAX_X86_REG);
			break;
		case 16:
			result->reg = (params->modRM.rm + XMM0_X86_REG);
			break;
		case 32:
			result->reg = (params->modRM.rm + YMM0_X86_REG);
			break;
		case 64:
			result->reg = (params->is64BitMode && params->evexPrefix.isValidEVEX && !params->evexPrefix.X) ? (params->modRM.rm + ZMM16_X86_REG) : (params->modRM.rm + ZMM0_X86_REG);
			break;
		}

		if (checkFlagB(params))
		{
			result->reg = extendRegister(result->reg);
		}

		return SUCCESS_JDC;
	}
	else if (selection == RM_MOD11_MMX_MODRM || selection == RM_MOD11_REG_MODRM)
	{
		return ERROR_JDC;
	}

	if (selection == RM_MW_MODRM)
	{
		operandSize = 2;
	}
	else if (selection == RM_MD_MODRM)
	{
		operandSize = 4;
	}
	else if (selection == RM_MQ_MODRM)
	{
		operandSize = 8;
	}

	result->type = MEMORY_OPERAND;
	result->memoryAddress.scale = 1;
	result->memoryAddress.ptrSize = operandSize;
	result->memoryAddress.regSegment = segmentOverrideToSegment(params->legPrefixes.group2);

	bool usedSIB = false;

	if (params->legPrefixes.group4 == ASO_LP && !params->is64BitMode)
	{
		switch (params->modRM.mod)
		{
		case 0:
		{
			switch (params->modRM.rm)
			{
			case 0:
				result->memoryAddress.reg = BX_X86_REG;
				result->memoryAddress.regDisplacement = SI_X86_REG;
				break;
			case 1:
				result->memoryAddress.reg = BX_X86_REG;
				result->memoryAddress.regDisplacement = DI_X86_REG;
				break;
			case 2:
				result->memoryAddress.reg = BP_X86_REG;
				result->memoryAddress.regDisplacement = SI_X86_REG;
				break;
			case 3:
				result->memoryAddress.reg = BP_X86_REG;
				result->memoryAddress.regDisplacement = DI_X86_REG;
				break;
			case 4:
				result->memoryAddress.reg = SI_X86_REG;
				break;
			case 5:
				result->memoryAddress.reg = DI_X86_REG;
				break;
			case 6:
				if (ERROR_JDC == readIntFromBytes(params, 2, &result->memoryAddress.constDisplacement)) { return ERROR_JDC; } // disp16
				break;
			case 7:
				result->memoryAddress.reg = BX_X86_REG;
				break;
			}

			break;
		}
		case 1:
		{
			if (ERROR_JDC == readIntFromBytes(params, 1, &result->memoryAddress.constDisplacement)) // disp8
			{
				return ERROR_JDC;
			}

			switch (params->modRM.rm)
			{
			case 0:
				result->memoryAddress.reg = BX_X86_REG;
				result->memoryAddress.regDisplacement = SI_X86_REG;
				break;
			case 1:
				result->memoryAddress.reg = BX_X86_REG;
				result->memoryAddress.regDisplacement = DI_X86_REG;
				break;
			case 2:
				result->memoryAddress.reg = BP_X86_REG;
				result->memoryAddress.regDisplacement = SI_X86_REG;
				break;
			case 3:
				result->memoryAddress.reg = BP_X86_REG;
				result->memoryAddress.regDisplacement = DI_X86_REG;
				break;
			case 4:
				result->memoryAddress.reg = SI_X86_REG;
				break;
			case 5:
				result->memoryAddress.reg = DI_X86_REG;
				break;
			case 6:
				result->memoryAddress.reg = BP_X86_REG;
				break;
			case 7:
				result->memoryAddress.reg = BX_X86_REG;
				break;
			}

			break;
		}
		case 2:
		{
			if (ERROR_JDC == readIntFromBytes(params, 2, &result->memoryAddress.constDisplacement)) // disp16
			{
				return ERROR_JDC;
			}

			switch (params->modRM.rm)
			{
			case 0:
				result->memoryAddress.reg = BX_X86_REG;
				result->memoryAddress.regDisplacement = SI_X86_REG;
				break;
			case 1:
				result->memoryAddress.reg = BX_X86_REG;
				result->memoryAddress.regDisplacement = DI_X86_REG;
				break;
			case 2:
				result->memoryAddress.reg = BP_X86_REG;
				result->memoryAddress.regDisplacement = SI_X86_REG;
				break;
			case 3:
				result->memoryAddress.reg = BP_X86_REG;
				result->memoryAddress.regDisplacement = DI_X86_REG;
				break;
			case 4:
				result->memoryAddress.reg = SI_X86_REG;
				break;
			case 5:
				result->memoryAddress.reg = DI_X86_REG;
				break;
			case 6:
				result->memoryAddress.reg = BP_X86_REG;
				break;
			case 7:
				result->memoryAddress.reg = BX_X86_REG;
				break;
			}

			break;
		}
		}
	}
	else
	{
		switch (params->modRM.mod)
		{
		case 0:
		{
			switch (params->modRM.rm)
			{
			case 0:
				result->memoryAddress.reg = EAX_X86_REG;
				break;
			case 1:
				result->memoryAddress.reg = ECX_X86_REG;
				break;
			case 2:
				result->memoryAddress.reg = EDX_X86_REG;
				break;
			case 3:
				result->memoryAddress.reg = EBX_X86_REG;
				break;
			case 4:
				if (ERROR_JDC == handleSIB(params, 0, result)) { return ERROR_JDC; }
				usedSIB = true;
				break;
			case 5:
				if (ERROR_JDC == readIntFromBytes(params, 4, &result->memoryAddress.constDisplacement)) { return ERROR_JDC; } // disp32
				if (params->is64BitMode) { result->memoryAddress.reg = RIP_X86_REG; }
				else { result->memoryAddress.regSegment = DS_X86_REG; }
				break;
			case 6:
				result->memoryAddress.reg = ESI_X86_REG;
				break;
			case 7:
				result->memoryAddress.reg = EDI_X86_REG;
				break;
			}

			break;
		}
		case 1:
		{
			bool gotDisp = false;
			switch (params->modRM.rm)
			{
			case 0:
				result->memoryAddress.reg = EAX_X86_REG;
				break;
			case 1:
				result->memoryAddress.reg = ECX_X86_REG;
				break;
			case 2:
				result->memoryAddress.reg = EDX_X86_REG;
				break;
			case 3:
				result->memoryAddress.reg = EBX_X86_REG;
				break;
			case 4:
				if (ERROR_JDC == handleSIB(params, &gotDisp, result)) { return ERROR_JDC; }
				usedSIB = true;
				break;
			case 5:
				result->memoryAddress.reg = EBP_X86_REG;
				break;
			case 6:
				result->memoryAddress.reg = ESI_X86_REG;
				break;
			case 7:
				result->memoryAddress.reg = EDI_X86_REG;
				break;
			}

			if (!gotDisp && ERROR_JDC == readIntFromBytes(params, 1, &result->memoryAddress.constDisplacement)) // disp8
			{
				return ERROR_JDC;
			}

			break;
		}
		case 2:
		{
			bool gotDisp = false;
			switch (params->modRM.rm)
			{
			case 0:
				result->memoryAddress.reg = EAX_X86_REG;
				break;
			case 1:
				result->memoryAddress.reg = ECX_X86_REG;
				break;
			case 2:
				result->memoryAddress.reg = EDX_X86_REG;
				break;
			case 3:
				result->memoryAddress.reg = EBX_X86_REG;
				break;
			case 4:
				if (ERROR_JDC == handleSIB(params, &gotDisp, result)) { return ERROR_JDC; }
				usedSIB = true;
				break;
			case 5:
				result->memoryAddress.reg = EBP_X86_REG;
				break;
			case 6:
				result->memoryAddress.reg = ESI_X86_REG;
				break;
			case 7:
				result->memoryAddress.reg = EDI_X86_REG;
				break;
			}

			if (!gotDisp && ERROR_JDC == readIntFromBytes(params, 4, &result->memoryAddress.constDisplacement)) // disp32
			{
				return ERROR_JDC;
			}

			break;
		}
		}
	}

	if (!usedSIB && params->is64BitMode && params->legPrefixes.group4 != ASO_LP && result->memoryAddress.reg != NO_REG && result->memoryAddress.reg != RIP_X86_REG)
	{
		result->memoryAddress.reg = increaseRegisterSize(result->memoryAddress.reg);

		if (checkFlagB(params))
		{
			result->memoryAddress.reg = extendRegister(result->memoryAddress.reg);
		}
	}

	// EVEX disp8*N check
	int64_t disp = result->memoryAddress.constDisplacement;
	if (params->evexPrefix.isValidEVEX && disp >= -128 && disp <= 127 && disp != 0)
	{
		uint8_t vectorLength = getVectorLength(params);

		uint8_t N = 1;
		switch (params->opcode.opcodeProperty)
		{
		case FULL_32_TT:
			N = params->evexPrefix.b == 0 ? vectorLength : 4;
			break;
		case FULL_64_TT:
			N = params->evexPrefix.b == 0 ? vectorLength : 8;
			break;
		case HALF_32_TT:
			N = params->evexPrefix.b == 0 ? (vectorLength / 2) : 4;
			break;
		case FULL_MEM_TT:
			N = vectorLength;
			break;
		case TUPLE1_SCALAR_8_TT:
			N = 1;
			break;
		case TUPLE1_SCALAR_16_TT:
			N = 2;
			break;
		case TUPLE1_SCALAR_32_TT:
		case TUPLE1_FIXED_32_TT:
			N = 4;
			break;
		case TUPLE1_SCALAR_64_TT:
		case TUPLE1_FIXED_64_TT:
		case TUPLE2_32_TT:
			N = 8;
			break;
		case TUPLE2_64_TT:
		case TUPLE4_32_TT:
		case MEM128_TT:
			N = 16;
			break;
		case TUPLE4_64_TT:
		case TUPLE8_32_TT:
			N = 32;
			break;
		case HALF_MEM_TT:
			N = vectorLength / 2;
			break;
		case QUARTER_MEM_TT:
			N = vectorLength / 4;
			break;
		case EIGHTH_MEM_TT:
			N = vectorLength / 8;
			break;
		case MOVDDUP_TT:
			N = vectorLength == 16 ? 8 : vectorLength;
			break;
		}

		result->memoryAddress.constDisplacement *= N;
	}

	return SUCCESS_JDC;
}

static enum JdcStatus handleSIB(struct DisassemblyParameters* params, bool* gotDisp, struct Operand* result)
{
	if (params->bytes > params->maxBytesAddr) { return ERROR_JDC; }

	uint8_t sibByte = params->bytes[0];
	params->bytes++;

	uint8_t scale = (((sibByte >> 7) & 0x01) * 2) + ((sibByte >> 6) & 0x01);
	uint8_t index = (((sibByte >> 5) & 0x01) * 4) + (((sibByte >> 4) & 0x01) * 2) + ((sibByte >> 3) & 0x01);
	uint8_t base = (((sibByte >> 2) & 0x01) * 4) + (((sibByte >> 1) & 0x01) * 2) + ((sibByte >> 0) & 0x01);

	for (int32_t i = 0; i < scale; i++)
	{
		result->memoryAddress.scale *= 2;
	}

	if (index != 4 || checkFlagX(params)) // ESP cant be the index, but if X is used it can encode R12
	{
		result->memoryAddress.reg = (enum Register)(index + EAX_X86_REG);
		result->memoryAddress.regDisplacement = (enum Register)(base + EAX_X86_REG);
	}
	else
	{
		result->memoryAddress.reg = (enum Register)(base + EAX_X86_REG);
	}

	if (params->is64BitMode)
	{
		result->memoryAddress.reg = increaseRegisterSize(result->memoryAddress.reg);
		if (result->memoryAddress.regDisplacement != NO_REG)
		{
			result->memoryAddress.regDisplacement = increaseRegisterSize(result->memoryAddress.regDisplacement);
		}

		if (checkFlagX(params))
		{
			result->memoryAddress.reg = extendRegister(result->memoryAddress.reg);
		}
		if (checkFlagB(params) && result->memoryAddress.regDisplacement != NO_REG)
		{
			result->memoryAddress.regDisplacement = extendRegister(result->memoryAddress.regDisplacement);
		}
	}

	if (base == 5)
	{
		switch (params->modRM.mod)
		{
		case 0:
			if (ERROR_JDC == readIntFromBytes(params, 4, &result->memoryAddress.constDisplacement)) { return ERROR_JDC; } // disp32
			if (index != 4 || checkFlagX(params)) { result->memoryAddress.regDisplacement = NO_REG; }
			else { result->memoryAddress.reg = NO_REG; }
			break;
		case 1:
			if (ERROR_JDC == readIntFromBytes(params, 1, &result->memoryAddress.constDisplacement)) { return ERROR_JDC; } // disp8
			break;
		case 2:
			if (ERROR_JDC == readIntFromBytes(params, 4, &result->memoryAddress.constDisplacement)) { return ERROR_JDC; } // disp32
			break;
		}

		if (gotDisp)
		{
			*gotDisp = true;
		}
	}

	return SUCCESS_JDC;
}