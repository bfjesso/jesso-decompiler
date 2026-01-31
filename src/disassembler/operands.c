#include "operands.h"
#include "opcodes.h"
#include "mnemonics.h"
#include "modRM.h"
#include "opcodes.h"
#include "prefixes.h"
#include "registers.h"

unsigned char handleOperands(struct DisassemblyParameters* params, struct Operand* result)
{
	int operandIndex = 0; // see Hps ?
	for (int i = 0; i < 4; i++)
	{
		enum OperandCode currentOperandCode = params->opcode->operands[i];
		struct Operand* currentOperand = &(result[operandIndex]);

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

		if (params->opcode->mnemonic == NO_MNEMONIC) { break; }

		unsigned char is64BitOperandSize = 0;
		if (params->is64BitMode && params->opcode->opcodeSuperscript == d64 && params->legPrefixes->group3 != OSO) { is64BitOperandSize = 1; }
		else if (params->is64BitMode && params->opcode->opcodeSuperscript == f64) { is64BitOperandSize = 1; }
		else if (params->rexPrefix->w) { is64BitOperandSize = 1; }

		unsigned char vectorLength = 16;
		if (params->evexPrefix->ll == 0b10) { vectorLength = 64; }
		else if (params->opcode->opcodeSuperscript == f256 || params->vexPrefix->l || params->evexPrefix->ll == 0b01) { vectorLength = 32; }

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
			currentOperand->reg = is64BitOperandSize ? RAX : (params->legPrefixes->group3 == OSO ? AX : EAX);
			break;
		case rCX:
			currentOperand->type = REGISTER;
			currentOperand->reg = is64BitOperandSize ? RCX : (params->legPrefixes->group3 == OSO ? CX : ECX);
			break;
		case rDX:
			currentOperand->type = REGISTER;
			currentOperand->reg = is64BitOperandSize ? RDX : (params->legPrefixes->group3 == OSO ? DX : EDX);
			break;
		case rBX:
			currentOperand->type = REGISTER;
			currentOperand->reg = is64BitOperandSize ? RBX : (params->legPrefixes->group3 == OSO ? BX : EBX);
			break;
		case rSP:
			currentOperand->type = REGISTER;
			currentOperand->reg = is64BitOperandSize ? RSP : (params->legPrefixes->group3 == OSO ? SP : ESP);
			break;
		case rBP:
			currentOperand->type = REGISTER;
			currentOperand->reg = is64BitOperandSize ? RBP : (params->legPrefixes->group3 == OSO ? BP : EBP);
			break;
		case rSI:
			currentOperand->type = REGISTER;
			currentOperand->reg = is64BitOperandSize ? RSI : (params->legPrefixes->group3 == OSO ? SI : ESI);
			break;
		case rDI:
			currentOperand->type = REGISTER;
			currentOperand->reg = is64BitOperandSize ? RDI : (params->legPrefixes->group3 == OSO ? DI : EDI);
			break;
		case rAX_r8:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix->b ? (is64BitOperandSize ? R8 : R8D) : (params->legPrefixes->group3 == OSO ? AX : (is64BitOperandSize ? RAX : EAX));
			break;
		case rCX_r9:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix->b ? (is64BitOperandSize ? R9 : R9D) : (params->legPrefixes->group3 == OSO ? CX : (is64BitOperandSize ? RCX : ECX));
			break;
		case rDX_r10:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix->b ? (is64BitOperandSize ? R10 : R10D) : (params->legPrefixes->group3 == OSO ? DX : (is64BitOperandSize ? RDX : EDX));
			break;
		case rBX_r11:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix->b ? (is64BitOperandSize ? R11 : R11D) : (params->legPrefixes->group3 == OSO ? BX : (is64BitOperandSize ? RBX : EBX));
			break;
		case rSP_r12:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix->b ? (is64BitOperandSize ? R12 : R12D) : (params->legPrefixes->group3 == OSO ? SP : (is64BitOperandSize ? RSP : ESP));
			break;
		case rBP_r13:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix->b ? (is64BitOperandSize ? R13 : R13D) : (params->legPrefixes->group3 == OSO ? BP : (is64BitOperandSize ? RBP : EBP));
			break;
		case rSI_r14:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix->b ? (is64BitOperandSize ? R14 : R14D) : (params->legPrefixes->group3 == OSO ? SI : (is64BitOperandSize ? RSI : ESI));
			break;
		case rDI_r15:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix->b ? (is64BitOperandSize ? R15 : R15D) : (params->legPrefixes->group3 == OSO ? DI : (is64BitOperandSize ? RDI : EDI));
			break;
		case AL_R8B:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix->b ? R8B : AL;
			break;
		case CL_R9B:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix->b ? R9B : CL;
			break;
		case DL_R10B:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix->b ? R10B : DL;
			break;
		case BL_R11B:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix->b ? R11B : BL;
			break;
		case AH_R12B:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix->b ? R12B : AH;
			break;
		case CH_R13B:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix->b ? R13B : CH;
			break;
		case DH_R14B:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix->b ? R14B : DH;
			break;
		case BH_R15B:
			currentOperand->type = REGISTER;
			currentOperand->reg = params->rexPrefix->b ? R15B : BH;
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
			if (!handleModRM(params, GET_MEM_ADDRESS, 1, currentOperand)) { return 0; }
			break;
		case Ed:
			if (!handleModRM(params, GET_MEM_ADDRESS, 4, currentOperand)) { return 0; }
			break;
		case Ev:
			if (!handleModRM(params, GET_MEM_ADDRESS, params->legPrefixes->group3 == OSO ? 2 : is64BitOperandSize ? 8 : 4, currentOperand)) { return 0; }
			break;
		case Ew:
			if (!handleModRM(params, GET_MEM_ADDRESS, 2, currentOperand)) { return 0; }
			break;
		case Ep:
			if (!handleModRM(params, GET_MEM_ADDRESS, params->legPrefixes->group3 == OSO ? 4 : 6, currentOperand)) { return 0; }
			break;
		case Ey:
			if (!handleModRM(params, GET_MEM_ADDRESS, params->legPrefixes->group3 != OSO && params->is64BitMode ? 8 : 4, currentOperand)) { return 0; }
			break;
		case Gb:
			if (!handleModRM(params, GET_REGISTER, 1, currentOperand)) { return 0; }
			break;
		case Gd:
			if (!handleModRM(params, GET_REGISTER, 4, currentOperand)) { return 0; }
			break;
		case Gv:
			if (!handleModRM(params, GET_REGISTER, params->legPrefixes->group3 == OSO ? 2 : is64BitOperandSize ? 8 : 4, currentOperand)) { return 0; }
			break;
		case Gz:
			if (!handleModRM(params, GET_REGISTER, params->legPrefixes->group3 == OSO ? 2 : 4, currentOperand)) { return 0; }
			break;
		case Gw:
			if (!handleModRM(params, GET_REGISTER, 2, currentOperand)) { return 0; }
			break;
		case Gy:
			if (!handleModRM(params, GET_REGISTER, params->legPrefixes->group3 != OSO && params->is64BitMode ? 8 : 4, currentOperand)) { return 0; }
			break;
		case By:
			if (!params->vexPrefix->isValidVEX) { operandIndex--; break; }
			currentOperand->type = REGISTER;
			currentOperand->reg = ((params->legPrefixes->group3 != OSO && params->is64BitMode ? RAX : EAX) + params->vexPrefix->vvvv);
			break;
		case M:
			if (!handleModRM(params, GET_MEM_ADDRESS, params->is64BitMode ? 8 : 4, currentOperand)) { return 0; }
			break;
		case Mb:
			if (!handleModRM(params, GET_MEM_ADDRESS, 1, currentOperand)) { return 0; }
			break;
		case Mw:
			if (!handleModRM(params, GET_MEM_ADDRESS, 2, currentOperand)) { return 0; }
			break;
		case Md:
			if (!handleModRM(params, GET_MEM_ADDRESS, 4, currentOperand)) { return 0; }
			break;
		case Mv:
			if (!handleModRM(params, GET_MEM_ADDRESS, params->legPrefixes->group3 == OSO ? 2 : is64BitOperandSize ? 8 : 4, currentOperand)) { return 0; }
			break;
		case Ma:
			if (!handleModRM(params, GET_MEM_ADDRESS, params->legPrefixes->group3 == OSO ? 4 : 8, currentOperand)) { return 0; }
			break;
		case Mp:
			if (!handleModRM(params, GET_MEM_ADDRESS, params->legPrefixes->group3 == OSO ? 4 : 6, currentOperand)) { return 0; }
			break;
		case Mq:
			if (!handleModRM(params, GET_MEM_ADDRESS, 8, currentOperand)) { return 0; }
			break;
		case Mt:
			if (!handleModRM(params, GET_MEM_ADDRESS, 10, currentOperand)) { return 0; }
			break;
		case Mps:
		case Mpd:
		case Mx:
			if (!handleModRM(params, GET_MEM_ADDRESS, vectorLength, currentOperand)) { return 0; }
			break;
		case Mdq:
			if (!handleModRM(params, GET_MEM_ADDRESS, 16, currentOperand)) { return 0; }
			break;
		case My:
			if (!handleModRM(params, GET_MEM_ADDRESS, params->legPrefixes->group3 != OSO && params->is64BitMode ? 8 : 4, currentOperand)) { return 0; }
			break;
		case Ib:
			if (params->bytes > params->maxBytesAddr) { return 0; }
			currentOperand->type = IMMEDIATE;
			currentOperand->immediate = getUIntFromBytes(&params->bytes, 1);
			break;
		case Iv:
			operandSize = params->legPrefixes->group3 == OSO ? 2 : is64BitOperandSize ? 8 : 4;
			if ((params->bytes + operandSize - 1) > params->maxBytesAddr) { return 0; }
			currentOperand->type = IMMEDIATE;
			currentOperand->immediate = getUIntFromBytes(&params->bytes, operandSize);
			break;
		case Iz:
			operandSize = params->legPrefixes->group3 == OSO ? 2 : 4;
			if ((params->bytes + operandSize - 1) > params->maxBytesAddr) { return 0; }
			currentOperand->type = IMMEDIATE;
			currentOperand->immediate = getUIntFromBytes(&params->bytes, operandSize);
			break;
		case Iw:
			if ((params->bytes + 1) > params->maxBytesAddr) { return 0; }
			currentOperand->type = IMMEDIATE;
			currentOperand->immediate = getUIntFromBytes(&params->bytes, 2);
			break;
		case Yb:
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.segment = ES;
			currentOperand->memoryAddress.reg = DI;
			break;
		case Yv:
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.segment = ES;
			currentOperand->memoryAddress.reg = params->legPrefixes->group3 == OSO ? DI : is64BitOperandSize ? RDI : EDI;
			break;
		case Yz:
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.segment = ES;
			currentOperand->memoryAddress.reg = params->legPrefixes->group3 == OSO ? DI : EDI;
			break;
		case Xb:
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.segment = DS;
			currentOperand->memoryAddress.reg = SI;
			break;
		case Xv:
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.segment = DS;
			currentOperand->memoryAddress.reg = params->legPrefixes->group3 == OSO ? SI : is64BitOperandSize ? RSI : ESI;
			break;
		case Xz:
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.segment = DS;
			currentOperand->memoryAddress.reg = params->legPrefixes->group3 == OSO ? SI : ESI;
			break;
		case Jb:
			if (params->bytes > params->maxBytesAddr) { return 0; }
			currentOperand->type = IMMEDIATE;
			currentOperand->immediate = (char)(getUIntFromBytes(&params->bytes, 1) + (params->bytes - params->startBytePtr)); // add instruction size
			break;
		case Jz:
			operandSize = params->legPrefixes->group3 == OSO ? 2 : 4;
			if ((params->bytes + operandSize - 1) > params->maxBytesAddr) { return 0; }
			currentOperand->type = IMMEDIATE;
			currentOperand->immediate = (int)getUIntFromBytes(&params->bytes, operandSize) + (params->bytes - params->startBytePtr); // add instruction size
			break;
		case Ob:
			if (params->bytes > params->maxBytesAddr) { return 0; }
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.segment = params->legPrefixes->group2 == NO_PREFIX ? DS : (enum Segment)params->legPrefixes->group2;
			currentOperand->memoryAddress.constDisplacement = (char)getUIntFromBytes(&params->bytes, 1);
			break;
		case Ov:
			operandSize = params->legPrefixes->group3 == OSO ? 2 : is64BitOperandSize ? 8 : 4;
			if ((params->bytes + operandSize - 1) > params->maxBytesAddr) { return 0; }
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.segment = params->legPrefixes->group2 == NO_PREFIX ? DS : (enum Segment)params->legPrefixes->group2;
			currentOperand->memoryAddress.constDisplacement = (long long)getUIntFromBytes(&params->bytes, operandSize);
			break;
		case Rd:
			if (!handleModRM(params, GET_MEM_ADDRESS, 4, currentOperand)) { return 0; }
			break;
		case Ry:
			if (!handleModRM(params, GET_MEM_ADDRESS, params->legPrefixes->group3 != OSO && params->is64BitMode ? 8 : 4, currentOperand)) { return 0; }
			break;
		case Rv:
			if (!handleModRM(params, GET_MEM_ADDRESS, params->legPrefixes->group3 == OSO ? 2 : is64BitOperandSize ? 8 : 4, currentOperand)) { return 0; }
			break;
		case Cd:
			if (!handleModRM(params, GET_CONTROL_REG, 4, currentOperand)) { return 0; }
			break;
		case Dd:
			if (!handleModRM(params, GET_DEBUG_REG, 4, currentOperand)) { return 0; }
			break;
		case Sw:
			if (!handleModRM(params, GET_SEGMENT, 2, currentOperand)) { return 0; }
			break;
		case Ap:
			if ((params->bytes + 5) > params->maxBytesAddr) { return 0; }
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.constDisplacement = (int)getUIntFromBytes(&params->bytes, 4);
			currentOperand->memoryAddress.constSegment = (unsigned short)getUIntFromBytes(&params->bytes, 2);
			break;
		case Pd:
		case Ppi:
		case Pq:
			if (!handleModRM(params, GET_MMX_REG, 4, currentOperand)) { return 0; }
			break;
		case Qd:
			if (!handleModRM(params, GET_MEM_ADDRESS_MMX, 4, currentOperand)) { return 0; }
			break;
		case Qpi:
		case Qq:
		case Nq:
			if (!handleModRM(params, GET_MEM_ADDRESS_MMX, 8, currentOperand)) { return 0; }
			break;
		case Ups:
		case Upd:
		case Uq:
		case Ux:
			if (!handleModRM(params, GET_MEM_ADDRESS, vectorLength, currentOperand)) { return 0; }
			break;
		case Udq:
			if (!handleModRM(params, GET_MEM_ADDRESS, 16, currentOperand)) { return 0; }
			break;
		case Vps:
		case Vpd:
		case Vx:
		case Vy:
		case Vq:
			if (!handleModRM(params, GET_REGISTER, vectorLength, currentOperand)) { return 0; }
			break;
		case Vss:
		case Vsd:
		case Vdq:
		case Vqq:
			if (!handleModRM(params, GET_REGISTER, vectorLength, currentOperand)) { return 0; }
			break;
		case Wps:
		case Wpd:
		case Wx:
		case Wq:
			if (!handleModRM(params, GET_MEM_ADDRESS, vectorLength, currentOperand)) { return 0; }
			break;
		case Wd:
			if (!handleModRM(params, GET_MEM_ADDRESS, 4, currentOperand)) { return 0; }
			break;
		case Wss:
		case Wsd:
		case Wdq:
		case Wqq:
			if (!handleModRM(params, GET_MEM_ADDRESS, vectorLength, currentOperand)) { return 0; }
			break;
		case Hps:
		case Hpd:
		case Hx:
		case Hq:
		case Hss:
		case Hsd:
		case Hdq:
			if (!params->vexPrefix->isValidVEX) { operandIndex--; break; }
			currentOperand->type = REGISTER;
			currentOperand->reg = vectorLength == 32 ? (YMM15 - params->vexPrefix->vvvv) : (XMM15 - params->vexPrefix->vvvv);
			break;
		case Hqq:
			if (!params->vexPrefix->isValidVEX) { operandIndex--; break; }
			currentOperand->type = REGISTER;
			currentOperand->reg = (YMM15 - params->vexPrefix->vvvv);
			break;
		case Lx:
			currentOperand->type = REGISTER;
			char immediate = (char)getUIntFromBytes(&params->bytes, 1) & 0b11110000; // upper 4 bits
			currentOperand->reg = vectorLength == 32 ? (YMM0 + immediate) : (XMM0 + immediate);
			break;
		case EVEXvvvv:
			if (!params->evexPrefix->isValidEVEX) { operandIndex--; break; }
			currentOperand->type = REGISTER;
			currentOperand->reg = vectorLength == 16 ? (XMM15 - params->evexPrefix->vvvv) : vectorLength == 32 ? (YMM15 - params->evexPrefix->vvvv) : (ZMM15 - params->evexPrefix->vvvv);
			break;
		case A_BYTE:
			params->bytes++;
			break;
		}

		operandIndex++;
	}

	return 1;
}

unsigned long long getUIntFromBytes(unsigned char** bytesPtr, unsigned char resultSize)
{
	unsigned long long result = 0;

	for (int i = 0; i < resultSize; i++)
	{
		result += ((unsigned long long)(*bytesPtr)[0] << (8 * i));
		(*bytesPtr)++;
	}

	return result;
}