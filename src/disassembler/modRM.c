#include "modRM.h"
#include "registers.h"
#include "operands.h"

unsigned char handleModRM(struct DisassemblyParameters* params, enum ModRMSelection selection, unsigned char operandSize, struct Operand* result)
{
	if (!params->modRM.hasGotModRM)
	{
		if (params->bytes > params->maxBytesAddr) { return 0; }

		params->modRM.hasGotModRM = 1;
		params->modRM.mod = (((params->bytes[0] >> 7) & 0x01) * 2) + ((params->bytes[0] >> 6) & 0x01);
		params->modRM.reg = (((params->bytes[0] >> 5) & 0x01) * 4) + (((params->bytes[0] >> 4) & 0x01) * 2) + ((params->bytes[0] >> 3) & 0x01);
		params->modRM.rm = (((params->bytes[0] >> 2) & 0x01) * 4) + (((params->bytes[0] >> 1) & 0x01) * 2) + ((params->bytes[0] >> 0) & 0x01);

		params->bytes++;
	}

	if (selection == GET_REGISTER)
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
			result->reg = (params->modRM.reg + ZMM0);
			break;
		}

		if (params->rexPrefix.R)
		{
			result->reg = extendRegister(result->reg);
		}

		return 1;
	}
	else if (selection == GET_SEGMENT)
	{
		result->type = SEGMENT;
		result->segment = params->modRM.reg;

		return 1;
	}
	else if (selection == GET_MMX_REG)
	{
		result->reg = (params->modRM.reg + MM0);
		return 1;
	}
	else if (selection == GET_CONTROL_REG)
	{
		if (params->rexPrefix.R)
		{
			result->reg = (params->modRM.reg + CR8);
		}
		else
		{
			result->reg = (params->modRM.reg + CR0);
		}
		return 1;
	}
	else if (selection == GET_DEBUG_REG)
	{
		if (params->rexPrefix.R)
		{
			result->reg = (params->modRM.reg + DR8);
		}
		else
		{
			result->reg = (params->modRM.reg + DR0);
		}
		return 1;
	}
	else if (params->modRM.mod == 3)
	{
		result->type = REGISTER;

		switch (params->modRM.rm)
		{
		case 0:
			result->reg = operandSize == 64 ? ZMM0 : operandSize == 32 ? YMM0 : operandSize == 16 ? XMM0 : operandSize == 8 ? (selection == GET_MEM_ADDRESS_MMX ? MM0 : RAX) : operandSize == 4 ? EAX : operandSize == 2 ? AX : AL;
			break;
		case 1:
			result->reg = operandSize == 64 ? ZMM1 : operandSize == 32 ? YMM1 : operandSize == 16 ? XMM1 : operandSize == 8 ? (selection == GET_MEM_ADDRESS_MMX ? MM1 : RCX) : operandSize == 4 ? ECX : operandSize == 2 ? CX : CL;
			break;
		case 2:
			result->reg = operandSize == 64 ? ZMM2 : operandSize == 32 ? YMM2 : operandSize == 16 ? XMM2 : operandSize == 8 ? (selection == GET_MEM_ADDRESS_MMX ? MM2 : RDX) : operandSize == 4 ? EDX : operandSize == 2 ? DX : DL;
			break;
		case 3:
			result->reg = operandSize == 64 ? ZMM3 : operandSize == 32 ? YMM3 : operandSize == 16 ? XMM3 : operandSize == 8 ? (selection == GET_MEM_ADDRESS_MMX ? MM3 : RBX) : operandSize == 4 ? EBX : operandSize == 2 ? BX : BL;
			break;
		case 4:
			result->reg = operandSize == 64 ? ZMM4 : operandSize == 32 ? YMM4 : operandSize == 16 ? XMM4 : operandSize == 8 ? (selection == GET_MEM_ADDRESS_MMX ? MM4 : RSP) : operandSize == 4 ? ESP : operandSize == 2 ? SP : AH;
			break;
		case 5:
			result->reg = operandSize == 64 ? ZMM5 : operandSize == 32 ? YMM5 : operandSize == 16 ? XMM5 : operandSize == 8 ? (selection == GET_MEM_ADDRESS_MMX ? MM5 : RBP) : operandSize == 4 ? EBP : operandSize == 2 ? BP : CH;
			break;
		case 6:
			result->reg = operandSize == 64 ? ZMM6 : operandSize == 32 ? YMM6 : operandSize == 16 ? XMM6 : operandSize == 8 ? (selection == GET_MEM_ADDRESS_MMX ? MM6 : RSI) : operandSize == 4 ? ESI : operandSize == 2 ? SI : DH;
			break;
		case 7:
			result->reg = operandSize == 64 ? ZMM7 : operandSize == 32 ? YMM7 : operandSize == 16 ? XMM7 : operandSize == 8 ? (selection == GET_MEM_ADDRESS_MMX ? MM7 : RDI) : operandSize == 4 ? EDI : operandSize == 2 ? DI : BH;
			break;
		}

		if (params->rexPrefix.B)
		{
			result->reg = extendRegister(result->reg);
		}

		return 1;
	}

	result->type = MEM_ADDRESS;
	result->memoryAddress.ptrSize = operandSize;

	unsigned char usedSIB = 0;

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
				if ((params->bytes + 1) > params->maxBytesAddr) { return 0; }
				result->memoryAddress.constDisplacement = (short)getUIntFromBytes(&params->bytes, 2); // disp16
				break;
			case 7:
				result->memoryAddress.reg = BX;
				break;
			}

			break;
		}
		case 1:
		{
			switch (params->modRM.rm)
			{
			case 0:
				if (params->bytes > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BX;
				result->memoryAddress.regDisplacement = SI;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(&params->bytes, 1); // disp8
				break;
			case 1:
				if (params->bytes > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BX;
				result->memoryAddress.regDisplacement = DI;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(&params->bytes, 1); // disp8
				break;
			case 2:
				if (params->bytes > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BP;
				result->memoryAddress.regDisplacement = SI;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(&params->bytes, 1); // disp8
				break;
			case 3:
				if (params->bytes > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BP;
				result->memoryAddress.regDisplacement = DI;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(&params->bytes, 1); // disp8
				break;
			case 4:
				if (params->bytes > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = SI;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(&params->bytes, 1); // disp8
				break;
			case 5:
				if (params->bytes > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = DI;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(&params->bytes, 1); // disp8
				break;
			case 6:
				if (params->bytes > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BP;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(&params->bytes, 1); // disp8
				break;
			case 7:
				if (params->bytes > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BX;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(&params->bytes, 1); // disp8
				break;
			}

			break;
		}
		case 2:
		{
			switch (params->modRM.rm)
			{
			case 0:
				if ((params->bytes + 1) > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BX;
				result->memoryAddress.regDisplacement = SI;
				result->memoryAddress.constDisplacement = (short)getUIntFromBytes(&params->bytes, 2); // disp16
				break;
			case 1:
				if ((params->bytes + 1) > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BX;
				result->memoryAddress.regDisplacement = DI;
				result->memoryAddress.constDisplacement = (short)getUIntFromBytes(&params->bytes, 2); // disp16
				break;
			case 2:
				if ((params->bytes + 1) > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BP;
				result->memoryAddress.regDisplacement = SI;
				result->memoryAddress.constDisplacement = (short)getUIntFromBytes(&params->bytes, 2); // disp16
				break;
			case 3:
				if ((params->bytes + 1) > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BP;
				result->memoryAddress.regDisplacement = DI;
				result->memoryAddress.constDisplacement = (short)getUIntFromBytes(&params->bytes, 2); // disp16
				break;
			case 4:
				if ((params->bytes + 1) > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = SI;
				result->memoryAddress.constDisplacement = (short)getUIntFromBytes(&params->bytes, 2); // disp16
				break;
			case 5:
				if ((params->bytes + 1) > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = DI;
				result->memoryAddress.constDisplacement = (short)getUIntFromBytes(&params->bytes, 2); // disp16
				break;
			case 6:
				if ((params->bytes + 1) > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BP;
				result->memoryAddress.constDisplacement = (short)getUIntFromBytes(&params->bytes, 2); // disp16
				break;
			case 7:
				if ((params->bytes + 1) > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BX;
				result->memoryAddress.constDisplacement = (short)getUIntFromBytes(&params->bytes, 2); // disp16
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
				if (params->bytes > params->maxBytesAddr) { return 0; }
				handleSIB(params, 0, result);
				usedSIB = 1;
				break;
			case 5:
				if ((params->bytes + 3) > params->maxBytesAddr) { return 0; }
				if (params->is64BitMode) { result->memoryAddress.reg = RIP; }
				else { result->memoryAddress.segment = DS; }
				result->memoryAddress.constDisplacement = (int)getUIntFromBytes(&params->bytes, 4); // disp32
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
			switch (params->modRM.rm)
			{
			case 0:
				if (params->bytes > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EAX;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(&params->bytes, 1); // disp8
				break;
			case 1:
				if (params->bytes > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = ECX;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(&params->bytes, 1); // disp8
				break;
			case 2:
				if (params->bytes > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EDX;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(&params->bytes, 1); // disp8
				break;
			case 3:
				if (params->bytes > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EBX;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(&params->bytes, 1); // disp8
				break;
			case 4:
				if ((params->bytes + 1) > params->maxBytesAddr) { return 0; }
				unsigned char gotDisp = 0;
				handleSIB(params, &gotDisp, result);
				if (!gotDisp) { result->memoryAddress.constDisplacement = (char)getUIntFromBytes(&params->bytes, 1); } // disp8
				usedSIB = 1;
				break;
			case 5:
				if (params->bytes > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EBP;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(&params->bytes, 1); // disp8
				break;
			case 6:
				if (params->bytes > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = ESI;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(&params->bytes, 1); // disp8
				break;
			case 7:
				if (params->bytes > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EDI;
				result->memoryAddress.constDisplacement = (char)getUIntFromBytes(&params->bytes, 1); // disp8
				break;
			}

			break;
		}
		case 2:
		{
			switch (params->modRM.rm)
			{
			case 0:
				if ((params->bytes + 3) > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EAX;
				result->memoryAddress.constDisplacement = (int)getUIntFromBytes(&params->bytes, 4); // disp32
				break;
			case 1:
				if ((params->bytes + 3) > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = ECX;
				result->memoryAddress.constDisplacement = (int)getUIntFromBytes(&params->bytes, 4); // disp32
				break;
			case 2:
				if ((params->bytes + 3) > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EDX;
				result->memoryAddress.constDisplacement = (int)getUIntFromBytes(&params->bytes, 4); // disp32
				break;
			case 3:
				if ((params->bytes + 3) > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EBX;
				result->memoryAddress.constDisplacement = (int)getUIntFromBytes(&params->bytes, 4); // disp32
				break;
			case 4:
				if ((params->bytes + 4) > params->maxBytesAddr) { return 0; }
				unsigned char gotDisp = 0;
				handleSIB(params, &gotDisp, result);
				if (!gotDisp) { result->memoryAddress.constDisplacement = (int)getUIntFromBytes(&params->bytes, 4); } // disp32
				usedSIB = 1;
				break;
			case 5:
				if ((params->bytes + 3) > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EBP;
				result->memoryAddress.constDisplacement = (int)getUIntFromBytes(&params->bytes, 4); // disp32
				break;
			case 6:
				if ((params->bytes + 3) > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = ESI;
				result->memoryAddress.constDisplacement = (int)getUIntFromBytes(&params->bytes, 4); // disp32
				break;
			case 7:
				if ((params->bytes + 3) > params->maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EDI;
				result->memoryAddress.constDisplacement = (int)getUIntFromBytes(&params->bytes, 4); // disp32
				break;
			}

			break;
		}
		}
	}

	if (!usedSIB && params->is64BitMode && params->legPrefixes.group4 != ASO && result->memoryAddress.reg != NO_REG && result->memoryAddress.reg != RIP)
	{
		result->memoryAddress.reg = increaseRegisterSize(result->memoryAddress.reg);

		if (params->rexPrefix.B)
		{
			result->memoryAddress.reg = extendRegister(result->memoryAddress.reg);
		}
	}

	return 1;
}

static unsigned char handleSIB(struct DisassemblyParameters* params, unsigned char* gotDisp, struct Operand* result)
{
	unsigned char sibByte = params->bytes[0];
	params->bytes++;

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

	if (params->is64BitMode)
	{
		result->memoryAddress.reg = increaseRegisterSize(result->memoryAddress.reg);
		if (result->memoryAddress.regDisplacement != NO_REG)
		{
			result->memoryAddress.regDisplacement = increaseRegisterSize(result->memoryAddress.regDisplacement);
		}

		if (params->rexPrefix.X)
		{
			result->memoryAddress.reg = extendRegister(result->memoryAddress.reg);
		}
		if (params->rexPrefix.B && result->memoryAddress.regDisplacement != NO_REG)
		{
			result->memoryAddress.regDisplacement = extendRegister(result->memoryAddress.regDisplacement);
		}
	}

	if (base == 5)
	{
		switch (params->modRM.mod)
		{
		case 0:
			if (index != 4) { result->memoryAddress.regDisplacement = NO_REG; }
			else { result->memoryAddress.reg = NO_REG; }
			result->memoryAddress.constDisplacement = (int)getUIntFromBytes(&params->bytes, 4); // disp32
			break;
		case 1:
			result->memoryAddress.constDisplacement = (char)getUIntFromBytes(&params->bytes, 1); // disp8
			break;
		case 2:
			result->memoryAddress.constDisplacement = (int)getUIntFromBytes(&params->bytes, 4); // disp32
			break;
		}

		if (gotDisp)
		{
			*gotDisp = 1;
		}
	}

	return 1;
}
