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
		result->type = REGISTER;

		switch (operandSize)
		{
		case 1:
			result->reg = (params->modRM.reg + AL);
			break;
		case 2:
			result->reg = (params->modRM.reg + AX);
			break;
		case 4:
			result->reg = (params->modRM.reg + EAX);
			break;
		case 8:
			result->reg = (params->modRM.reg + RAX);
			break;
		case 16:
			result->reg = (params->modRM.reg + XMM0);
			break;
		case 32:
			result->reg = (params->modRM.reg + YMM0);
			break;
		case 64:
			result->reg = (params->evexPrefix.isValidEVEX && !params->evexPrefix.R_prime) ? (params->modRM.reg + ZMM16) : (params->modRM.reg + ZMM0);
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
		result->type = SEGMENT;
		result->segment = (params->modRM.reg + ES);
		return SUCCESS_JDC;
	}
	else if (selection == REG_MMX_MODRM)
	{
		result->type = REGISTER;
		result->reg = (params->modRM.reg + MM0);
		return SUCCESS_JDC;
	}
	else if (selection == REG_CONTROL_MODRM)
	{
		result->type = REGISTER;
		if (params->rexPrefix.R)
		{
			result->reg = (params->modRM.reg + CR8);
		}
		else
		{
			result->reg = (params->modRM.reg + CR0);
		}

		return SUCCESS_JDC;
	}
	else if (selection == REG_DEBUG_MODRM)
	{
		result->type = REGISTER;
		if (params->rexPrefix.R)
		{
			result->reg = (params->modRM.reg + DR8);
		}
		else
		{
			result->reg = (params->modRM.reg + DR0);
		}

		return SUCCESS_JDC;
	}

	if (selection != RM_MODRM && selection != RM_MMX_MODRM && selection != M_MODRM)
	{
		return ERROR_JDC;
	}

	if (params->modRM.mod == 3)
	{
		if (selection == M_MODRM)
		{
			return ERROR_JDC;
		}

		result->type = REGISTER;

		switch (params->modRM.rm)
		{
		case 0:
			result->reg = operandSize == 64 ? ZMM0 : operandSize == 32 ? YMM0 : operandSize == 16 ? XMM0 : operandSize == 8 ? (selection == RM_MMX_MODRM ? MM0 : RAX) : operandSize == 4 ? EAX : operandSize == 2 ? AX : AL;
			break;
		case 1:
			result->reg = operandSize == 64 ? ZMM1 : operandSize == 32 ? YMM1 : operandSize == 16 ? XMM1 : operandSize == 8 ? (selection == RM_MMX_MODRM ? MM1 : RCX) : operandSize == 4 ? ECX : operandSize == 2 ? CX : CL;
			break;
		case 2:
			result->reg = operandSize == 64 ? ZMM2 : operandSize == 32 ? YMM2 : operandSize == 16 ? XMM2 : operandSize == 8 ? (selection == RM_MMX_MODRM ? MM2 : RDX) : operandSize == 4 ? EDX : operandSize == 2 ? DX : DL;
			break;
		case 3:
			result->reg = operandSize == 64 ? ZMM3 : operandSize == 32 ? YMM3 : operandSize == 16 ? XMM3 : operandSize == 8 ? (selection == RM_MMX_MODRM ? MM3 : RBX) : operandSize == 4 ? EBX : operandSize == 2 ? BX : BL;
			break;
		case 4:
			result->reg = operandSize == 64 ? ZMM4 : operandSize == 32 ? YMM4 : operandSize == 16 ? XMM4 : operandSize == 8 ? (selection == RM_MMX_MODRM ? MM4 : RSP) : operandSize == 4 ? ESP : operandSize == 2 ? SP : AH;
			break;
		case 5:
			result->reg = operandSize == 64 ? ZMM5 : operandSize == 32 ? YMM5 : operandSize == 16 ? XMM5 : operandSize == 8 ? (selection == RM_MMX_MODRM ? MM5 : RBP) : operandSize == 4 ? EBP : operandSize == 2 ? BP : CH;
			break;
		case 6:
			result->reg = operandSize == 64 ? ZMM6 : operandSize == 32 ? YMM6 : operandSize == 16 ? XMM6 : operandSize == 8 ? (selection == RM_MMX_MODRM ? MM6 : RSI) : operandSize == 4 ? ESI : operandSize == 2 ? SI : DH;
			break;
		case 7:
			result->reg = operandSize == 64 ? ZMM7 : operandSize == 32 ? YMM7 : operandSize == 16 ? XMM7 : operandSize == 8 ? (selection == RM_MMX_MODRM ? MM7 : RDI) : operandSize == 4 ? EDI : operandSize == 2 ? DI : BH;
			break;
		}

		if (checkFlagB(params))
		{
			result->reg = extendRegister(result->reg);
		}

		return SUCCESS_JDC;
	}

	result->type = MEM_ADDRESS;
	result->memoryAddress.scale = 1;
	result->memoryAddress.ptrSize = operandSize;
	result->memoryAddress.segment = segmentOverrideToSegment(params->legPrefixes.group2);

	bool usedSIB = false;

	if (params->legPrefixes.group4 == ASO && !params->is64BitMode)
	{
		switch (params->modRM.mod)
		{
		case 0:
		{
			switch (params->modRM.rm)
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
				if (ERROR_JDC == readIntFromBytes(params, 2, &result->memoryAddress.constDisplacement)) { return ERROR_JDC; } // disp16
				break;
			case 7:
				result->memoryAddress.reg = BX;
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
				result->memoryAddress.reg = BP;
				break;
			case 7:
				result->memoryAddress.reg = BX;
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
				result->memoryAddress.reg = BP;
				break;
			case 7:
				result->memoryAddress.reg = BX;
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
				if (ERROR_JDC == handleSIB(params, 0, result)) { return ERROR_JDC; }
				usedSIB = true;
				break;
			case 5:
				if (ERROR_JDC == readIntFromBytes(params, 4, &result->memoryAddress.constDisplacement)) { return ERROR_JDC; } // disp32
				if (params->is64BitMode) { result->memoryAddress.reg = RIP; }
				else { result->memoryAddress.segment = DS; }
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
			bool gotDisp = false;
			switch (params->modRM.rm)
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
				if (ERROR_JDC == handleSIB(params, &gotDisp, result)) { return ERROR_JDC; }
				usedSIB = true;
				break;
			case 5:
				result->memoryAddress.reg = EBP;
				break;
			case 6:
				result->memoryAddress.reg = ESI;
				break;
			case 7:
				result->memoryAddress.reg = EDI;
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
				if (ERROR_JDC == handleSIB(params, &gotDisp, result)) { return ERROR_JDC; }
				usedSIB = true;
				break;
			case 5:
				result->memoryAddress.reg = EBP;
				break;
			case 6:
				result->memoryAddress.reg = ESI;
				break;
			case 7:
				result->memoryAddress.reg = EDI;
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

	if (!usedSIB && params->is64BitMode && params->legPrefixes.group4 != ASO && result->memoryAddress.reg != NO_REG && result->memoryAddress.reg != RIP)
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
		switch (params->opcode.opcodeSuperscript)
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
		result->memoryAddress.reg = (enum Register)(index + EAX);
		result->memoryAddress.regDisplacement = (enum Register)(base + EAX);
	}
	else
	{
		result->memoryAddress.reg = (enum Register)(base + EAX);
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