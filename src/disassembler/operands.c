#include "operands.h"
#include "disassemblyUtils.h"
#include "opcodes.h"
#include "mnemonics.h"
#include "modRM.h"

unsigned char handleOperands(unsigned char** bytesPtr, unsigned char* maxBytesAddr, unsigned char* startBytePtr, char hasGotModRM, unsigned char* modRMByteRef, unsigned char is64BitMode, struct Opcode* opcode, struct LegacyPrefixes* legPrefixes, struct REXPrefix* rexPrefix, struct VEXPrefix* vexPrefix, struct Operand* result)
{
	int operandIndex = 0; // see Hps ?
	for (int i = 0; i < 4; i++)
	{
		enum OperandCode currentOperandCode = opcode->operands[i];
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
		case Gd:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_REGISTER, 4, 0, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Gv:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_REGISTER, legPrefixes->group3 == OSO ? 2 : is64BitOperandSize ? 8 : 4, 0, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Gz:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_REGISTER, legPrefixes->group3 == OSO ? 2 : 4, 0, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Gw:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_REGISTER, 2, 0, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Gy:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_REGISTER, legPrefixes->group3 != OSO && is64BitMode ? 8 : 4, 0, is64BitMode, rexPrefix, currentOperand)) { return 0; }
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
		case Mps:
		case Mpd:
		case Mx:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_MEM_ADDRESS, opcode->opcodeSuperscript == f256 ? 32 : 16, legPrefixes->group4 == ASO, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case My:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_MEM_ADDRESS, legPrefixes->group3 != OSO && is64BitMode ? 8 : 4, legPrefixes->group4 == ASO, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Ib:
			if ((*bytesPtr) > maxBytesAddr) { return 0; }
			currentOperand->type = IMMEDIATE;
			currentOperand->immediate = getUIntFromBytes(bytesPtr, 1);
			break;
		case Iv:
			operandSize = legPrefixes->group3 == OSO ? 2 : is64BitOperandSize ? 8 : 4;
			if (((*bytesPtr) + operandSize - 1) > maxBytesAddr) { return 0; }
			currentOperand->type = IMMEDIATE;
			currentOperand->immediate = getUIntFromBytes(bytesPtr, operandSize);
			break;
		case Iz:
			operandSize = legPrefixes->group3 == OSO ? 2 : 4;
			if (((*bytesPtr) + operandSize - 1) > maxBytesAddr) { return 0; }
			currentOperand->type = IMMEDIATE;
			currentOperand->immediate = getUIntFromBytes(bytesPtr, operandSize);
			break;
		case Iw:
			if (((*bytesPtr) + 1) > maxBytesAddr) { return 0; }
			currentOperand->type = IMMEDIATE;
			currentOperand->immediate = getUIntFromBytes(bytesPtr, 2);
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
		case Rd:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_MEM_ADDRESS, 4, legPrefixes->group4 == ASO, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Ry:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_MEM_ADDRESS, legPrefixes->group3 != OSO && is64BitMode ? 8 : 4, legPrefixes->group4 == ASO, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Rv:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_MEM_ADDRESS, legPrefixes->group3 == OSO ? 2 : is64BitOperandSize ? 8 : 4, legPrefixes->group4 == ASO, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Cd:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_CONTROL_REG, 4, 0, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Dd:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_DEBUG_REG, 4, 0, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
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
		case Ppi:
		case Pq:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_MMX_REG, 4, 0, 0, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Qd:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_MEM_ADDRESS_MMX, 4, 0, 0, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Qpi:
		case Qq:
		case Nq:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_MEM_ADDRESS_MMX, 8, 0, 0, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Ups:
		case Upd:
		case Uq:
		case Ux:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_MEM_ADDRESS, opcode->opcodeSuperscript == f256 ? 32 : 16, legPrefixes->group4 == ASO, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Udq:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_MEM_ADDRESS, 16, legPrefixes->group4 == ASO, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Vps:
		case Vpd:
		case Vx:
		case Vy:
		case Vq:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_REGISTER, opcode->opcodeSuperscript == f256 ? 32 : 16, legPrefixes->group4 == ASO, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Vss:
		case Vsd:
		case Vdq:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_REGISTER, 16, legPrefixes->group4 == ASO, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Wps:
		case Wpd:
		case Wx:
		case Wq:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_MEM_ADDRESS, opcode->opcodeSuperscript == f256 ? 32 : 16, legPrefixes->group4 == ASO, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Wss:
		case Wsd:
		case Wdq:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, GET_MEM_ADDRESS, 16, legPrefixes->group4 == ASO, is64BitMode, rexPrefix, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Hps:
		case Hpd:
		case Hx:
		case Hq:
			if (!vexPrefix->isValidVEX) { operandIndex--; break; }
			currentOperand->type = REGISTER;
			currentOperand->reg = opcode->opcodeSuperscript == f256 ? (YMM0 + vexPrefix->vvvv) : (XMM0 + vexPrefix->vvvv);
			break;
		case Hss:
		case Hsd:
		case Hdq:
			if (!vexPrefix->isValidVEX) { operandIndex--; break; }
			currentOperand->type = REGISTER;
			currentOperand->reg = (XMM0 + vexPrefix->vvvv);
			break;
		case A_BYTE:
			(*bytesPtr)++;
			break;
		}

		operandIndex++;
	}

	return 1;
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

unsigned char getLastOperand(struct DisassembledInstruction* instruction)
{
	unsigned char result = 0;

	for (int i = 3; i >= 0; i--)
	{
		if (instruction->operands[i].type != NO_OPERAND)
		{
			result = i;
			break;
		}
	}

	return result;
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
