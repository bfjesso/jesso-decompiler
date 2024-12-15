#include "../Headers/disassembler.h"
#include "../Headers/registers.h"
#include "../Headers/oneByteOpcodeMap.h"
#include "../Headers/twoByteOpcodeMap.h"
#include "../Headers/extendedOpcodeMap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char disassembleInstruction(unsigned char* bytes, unsigned char* maxBytesAddr, struct DisassemblerOptions* disassemblerOptions, struct DisassembledInstruction* result)
{
	unsigned char* startPoint = bytes;

	struct LegacyPrefixes legacyPrefixes = { NO_PREFIX, NO_PREFIX, NO_PREFIX, NO_PREFIX };
	if (!handleLegacyPrefixes(&bytes, maxBytesAddr, &legacyPrefixes))
	{
		return 0;
	}

	struct REXPrefix rexPrefix = { 0, 0, 0, 0, 0 };
	if (disassemblerOptions->is64BitMode && !handleREXPrefix(&bytes, maxBytesAddr, &rexPrefix))
	{
		return 0;
	}

	// if the opcode is an extended one then modRM will be retrieved
	unsigned char modRMByte = 0;
	char hasGotModRM = 0;
	struct Opcode opcode = { NO_MNEMONIC, -1, 0, 0, 0 };
	if (!handleOpcode(&bytes, maxBytesAddr, &hasGotModRM, &modRMByte, disassemblerOptions, &opcode))
	{
		return 0;
	}

	if (!handleOperands(&bytes, maxBytesAddr, hasGotModRM, &modRMByte, disassemblerOptions->is64BitMode, &opcode, &legacyPrefixes, &rexPrefix, &result->operands))
	{
		return 0;
	}

	result->opcode = opcode.mnemonic;
	result->numOfBytes = (bytes - startPoint);

	return 1;
}

void instructionToStr(struct DisassembledInstruction* instruction, char* buffer, unsigned char bufferSize)
{
	if (instruction->opcode == NO_MNEMONIC) // the opcode is either not implemented in the disassembler or there is a bad instruction in the file
	{
		if (5 > bufferSize) { return; }
		strcpy(buffer, "error");
		return;
	}
	
	int bufferIndex = 0;

	const char* mnemonicStr = mnemonicStrs[instruction->opcode];
	unsigned char mnemonicStrLen = strlen(mnemonicStr);

	if (bufferIndex + mnemonicStrLen - 1 > bufferSize) { return; }
	strcpy(buffer + bufferIndex, mnemonicStr);
	bufferIndex += mnemonicStrLen;

	if (bufferIndex > bufferSize) { return; }
	buffer[bufferIndex] = ' ';
	bufferIndex++;

	for (int i = 0; i < 3; i++) 
	{
		if (instruction->operands[i].type == NO_OPERAND) { break; }

		if (i != 0) 
		{
			if (bufferIndex + 1 > bufferSize) { return; }
			buffer[bufferIndex] = ',';
			buffer[bufferIndex + 1] = ' ';
			bufferIndex += 2;
		}

		struct Operand* currentOperand = &instruction->operands[i];

		const char* segmentStr;
		unsigned char segmentStrLen;

		const char* registerStr;
		unsigned char registerStrLen;

		static char immediateBuffer[20];
		switch (currentOperand->type)
		{
		case SEGMENT:
			segmentStr = segmentStrs[currentOperand->segment];
			segmentStrLen = strlen(segmentStr);
			if (bufferIndex + segmentStrLen - 1 > bufferSize) { return; }
			strcpy(buffer + bufferIndex, segmentStr);
			bufferIndex += segmentStrLen;
			break;
		case REGISTER:
			registerStr = registerStrs[currentOperand->reg];
			registerStrLen = strlen(registerStrs[currentOperand->reg]);
			if (bufferIndex + registerStrLen - 1 > bufferSize) { return; }
			strcpy(buffer + bufferIndex, registerStr);
			bufferIndex += registerStrLen;
			break;
		case MEM_ADDRESS:
			memAddressToStr(&currentOperand->memoryAddress, buffer + bufferIndex, bufferSize - bufferIndex, &bufferIndex);
			break;
		case IMMEDIATE:
			sprintf(immediateBuffer, "0x%X", currentOperand->immediate);
			if (bufferIndex + strlen(immediateBuffer) - 1 > bufferSize) { return; }
			strcpy(buffer + bufferIndex, immediateBuffer);
			bufferIndex += strlen(immediateBuffer);
			break;
		}
	}

	if (bufferIndex > bufferSize) { return; }
	buffer[bufferIndex] = 0;
	bufferIndex++;
}

static void memAddressToStr(struct MemoryAddress* memAddr, char* buffer, unsigned char bufferSize, unsigned char* resultSize) 
{
	int bufferIndex = 0;

	if (memAddr->ptrSize != 0) 
	{
		const char* ptrSizeStr = ptrSizeStrs[memAddr->ptrSize / 2];
		unsigned char ptrSizeStrLen = strlen(ptrSizeStr);

		if (bufferIndex + ptrSizeStrLen - 1 > bufferSize) { return; }
		strcpy(buffer + bufferIndex, ptrSizeStr);
		bufferIndex += ptrSizeStrLen;

		if (bufferIndex > bufferSize) { return; }
		buffer[bufferIndex] = ' ';
		bufferIndex++;
	}
	
	if (memAddr->segment != NO_SEGMENT) 
	{
		const char* segmentStr = segmentStrs[memAddr->segment];
		unsigned char segmentStrLen = strlen(segmentStr);
		
		if (bufferIndex + segmentStrLen - 1 > bufferSize) { return; }
		strcpy(buffer + bufferIndex, segmentStr);
		bufferIndex += segmentStrLen;

		if (bufferIndex > bufferSize) { return; }
		buffer[bufferIndex] = ':';
		bufferIndex++;
	}
	else if (memAddr->constSegment != 0) 
	{
		static char hexSeg[20];
		sprintf(hexSeg, "0x%X", memAddr->constSegment);

		if (bufferIndex + strlen(hexSeg) - 1 > bufferSize) { return; }
		strcpy(buffer + bufferIndex, hexSeg);
		bufferIndex += strlen(hexSeg);

		if (bufferIndex > bufferSize) { return; }
		buffer[bufferIndex] = ':';
		bufferIndex++;
	}

	if (bufferIndex > bufferSize) { return; }
	buffer[bufferIndex] = '[';
	bufferIndex++;

	if (memAddr->reg != NO_REG) 
	{
		const char* registerStr = registerStrs[memAddr->reg];
		unsigned char registerStrLen = strlen(registerStrs[memAddr->reg]);
		
		if (bufferIndex + registerStrLen - 1 > bufferSize) { return; }
		strcpy(buffer + bufferIndex, registerStr);
		bufferIndex += registerStrLen;
	}

	if (memAddr->scale > 1) 
	{
		if (bufferIndex > bufferSize) { return; }
		buffer[bufferIndex] = '*';
		bufferIndex++;

		if (bufferIndex > bufferSize) { return; }
		buffer[bufferIndex] = memAddr->scale + '0';
		bufferIndex++;
	}

	if (memAddr->regDisplacement != NO_REG)
	{
		if (bufferIndex > bufferSize) { return; }
		buffer[bufferIndex] = '+';
		bufferIndex++;
		
		const char* registerStr = registerStrs[memAddr->regDisplacement];
		unsigned char registerStrLen = strlen(registerStrs[memAddr->regDisplacement]);

		if (bufferIndex + registerStrLen - 1 > bufferSize) { return; }
		strcpy(buffer + bufferIndex, registerStr);
		bufferIndex += registerStrLen;
	}

	if (memAddr->constDisplacement != 0) 
	{
		char constDisp[20];
		if (memAddr->constDisplacement < 0) 
		{
			sprintf(constDisp, "-0x%X", -memAddr->constDisplacement);
		}
		else 
		{
			sprintf(constDisp, "0x%X", memAddr->constDisplacement);
		}

		if (buffer[bufferIndex - 1] != '[' && memAddr->constDisplacement >= 0)
		{
			if (bufferIndex > bufferSize) { return; }
			buffer[bufferIndex] = '+';
			bufferIndex++;
		}

		if (bufferIndex + strlen(constDisp) - 1 > bufferSize) { return; }
		strcpy(buffer + bufferIndex, constDisp);
		bufferIndex += strlen(constDisp);
	}

	if (bufferIndex > bufferSize) { return; }
	buffer[bufferIndex] = ']';
	bufferIndex++;

	(*resultSize) += bufferIndex;
}

static unsigned char handleLegacyPrefixes(unsigned char** bytesPtr, unsigned char* maxBytesAddr, struct LegacyPrefixes* result)
{
	result->group1 = NO_PREFIX;
	result->group2 = NO_PREFIX;
	result->group3 = NO_PREFIX;
	result->group4 = NO_PREFIX;
	
	for (int i = 0; i < 4; i++) // up to four prefixes
	{
		if ((*bytesPtr) > maxBytesAddr) { return 0; }

		unsigned char byte = (*bytesPtr)[0];
		(*bytesPtr)++;

		switch (byte)
		{
		case 0xF0:
			result->group1 = LOCK;
			break;
		case 0xF2:
			result->group1 = REPNZ;
			break;
		case 0xF3:
			result->group1 = REPZ;
			break;
		case 0x2E:
			result->group2 = CSO_BNT;
			break;
		case 0x36:
			result->group2 = SSO;
			break;
		case 0x3E:
			result->group2 = DSO_BT;
			break;
		case 0x26:
			result->group2 = ESO;
			break;
		case 0x64:
			result->group2 = FSO;
			break;
		case 0x65:
			result->group2 = GSO;
			break;
		case 0x66:
			result->group3 = OSO;
			break;
		case 0x67:
			result->group4 = ASO;
			break;
		default:
			(*bytesPtr)--;
			return 1;
		}
	}

	return 1;
}

static unsigned char handleREXPrefix(unsigned char** bytesPtr, unsigned char* maxBytesAddr, struct REXPrefix* result)
{
	if ((*bytesPtr) > maxBytesAddr) { return 0; }

	unsigned char rexByte = (*bytesPtr)[0];

	if (rexByte < 0x40 || rexByte > 0x4F) { return 1; }

	result->isValidREX = 1;
	result->w = (rexByte >> 3) & 0x01;
	result->r = (rexByte >> 2) & 0x01;
	result->x = (rexByte >> 1) & 0x01;
	result->b = (rexByte >> 0) & 0x01;

	(*bytesPtr)++;

	return 1;
}

static unsigned char handleOpcode(unsigned char** bytesPtr, unsigned char* maxBytesAddr, char* hasGotModRMRef, unsigned char* modRMByteRef, struct DisassemblerOptions* disassemblerOptions, struct Opcode* result)
{
	if ((*bytesPtr) > maxBytesAddr) { return 0; }
	
	// check the opcode map in use
	if ((*bytesPtr)[0] == 0x0F)
	{
		if (((*bytesPtr) + 2) <= maxBytesAddr && (*bytesPtr)[1] == 0x3A) // sequence: 0x0F 0x3A opcode
		{
			//result.opcode = &threeByteMap2[(*bytesPtr)[2]];
			(*bytesPtr) += 3;
		}
		else if (((*bytesPtr) + 2) <= maxBytesAddr && (*bytesPtr)[1] == 0x38) // sequence: 0x0F 0x38 opcode
		{
			//result.opcode = &threeByteMap[(*bytesPtr)[2]];
			(*bytesPtr) += 3;
		}
		else if(((*bytesPtr) + 1) <= maxBytesAddr) // sequence: 0x0F opcode
		{
			*result = twoByteOpcodeMap[(*bytesPtr)[1]];
			(*bytesPtr) += 2;
		}
		else 
		{
			return 0;
		}
	}
	else if ((*bytesPtr) <= maxBytesAddr) // sequence: opcode
	{
		*result = oneByteOpcodeMap[(*bytesPtr)[0]];

		if (disassemblerOptions->is64BitMode && (*bytesPtr)[0] == 0x63)
		{
			*result = alternateX63;
		}

		(*bytesPtr)++;
	}
	else
	{
		return 0;
	}

	if ((disassemblerOptions->is64BitMode && result->opcodeSuperscript == i64) || (!disassemblerOptions->is64BitMode && result->opcodeSuperscript == o64))
	{
		return 0;
	}

	if (result->mnemonic == EXTENDED_OPCODE)
	{
		if ((*bytesPtr) > maxBytesAddr) { return 0; }
		
		unsigned char modRMByte = (*bytesPtr)[0];
		unsigned char mod = (((modRMByte >> 7) & 0x01) * 2) + ((modRMByte >> 6) & 0x01);
		unsigned char reg = (((modRMByte >> 5) & 0x01) * 4) + (((modRMByte >> 4) & 0x01) * 2) + ((modRMByte >> 3) & 0x01);
		unsigned char rm = (((modRMByte >> 2) & 0x01) * 4) + (((modRMByte >> 1) & 0x01) * 2) + ((modRMByte >> 0) & 0x01);

		struct Opcode* extendedOpcode = &extendedOpcodeMap[result->extensionGroup][reg];

		result->mnemonic = extendedOpcode->mnemonic;

		if (extendedOpcode->operands[0] != NO_OPERAND_CODE)
		{
			for (int i = 0; i < 3; i++) 
			{
				result->operands[i] = extendedOpcode->operands[i];
			}
		}

		if (extendedOpcode->opcodeSuperscript != NO_SUPERSCRIPT)
		{
			result->opcodeSuperscript = extendedOpcode->opcodeSuperscript;
		}

		if (result->extensionGroup == 3 && ((*bytesPtr) - 1)[0] == 0xF7)
		{
			switch (result->mnemonic) 
			{
			case TEST:
				result->operands[0] = Iz;
				break;
			case MUL:
			case IMUL:
			case DIV:
			case IDIV:
				result->operands[0] = rAX;
				break;
			}
		}

		(*hasGotModRMRef) = 1;
		(*modRMByteRef) = modRMByte;
		(*bytesPtr)++;
	}

	return 1;
}

static unsigned char handleOperands(unsigned char** bytesPtr, unsigned char* maxBytesAddr, char hasGotModRM, unsigned char* modRMByteRef, unsigned char is64BitMode, struct Opcode* opcode, struct LegacyPrefixes* legPrefixes, struct REXPrefix* rexPrefix, struct Operand* result)
{
	for (int i = 0; i < 3; i++)
	{
		enum OperandCode currentOperandCode = opcode->operands[i];
		struct Operand* currentOperand = &(result[i]);

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

		char is64BitOperandSize = 0;
		if (is64BitMode && opcode->opcodeSuperscript == d64 && legPrefixes->group3 != OSO) { is64BitOperandSize = 1; }
		else if (is64BitMode && opcode->opcodeSuperscript == f64) { is64BitOperandSize = 1; }
		else if (rexPrefix->w) { is64BitOperandSize = 1; }

		unsigned char operandSize;
		switch (currentOperandCode)
		{
		case NO_OPERAND_CODE:
			return 1; // no more operands; done handling them all
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
			currentOperand->reg = rexPrefix->b ? R8 : (legPrefixes->group3 == OSO ? AX : (is64BitOperandSize ? RAX : EAX));
			break;
		case rCX_r9:
			currentOperand->type = REGISTER;
			currentOperand->reg = rexPrefix->b ? R9 : (legPrefixes->group3 == OSO ? CX : (is64BitOperandSize ? RCX : ECX));
			break;
		case rDX_r10:
			currentOperand->type = REGISTER;
			currentOperand->reg = rexPrefix->b ? R10 : (legPrefixes->group3 == OSO ? DX : (is64BitOperandSize ? RDX : EDX));
			break;
		case rBX_r11:
			currentOperand->type = REGISTER;
			currentOperand->reg = rexPrefix->b ? R11 : (legPrefixes->group3 == OSO ? BX : (is64BitOperandSize ? RBX : EBX));
			break;
		case rSP_r12:
			currentOperand->type = REGISTER;
			currentOperand->reg = rexPrefix->b ? R12 : (legPrefixes->group3 == OSO ? SP : (is64BitOperandSize ? RSP : ESP));
			break;
		case rBP_r13:
			currentOperand->type = REGISTER;
			currentOperand->reg = rexPrefix->b ? R13 : (legPrefixes->group3 == OSO ? BP : (is64BitOperandSize ? RBP : EBP));
			break;
		case rSI_r14:
			currentOperand->type = REGISTER;
			currentOperand->reg = rexPrefix->b ? R14 : (legPrefixes->group3 == OSO ? SI : (is64BitOperandSize ? RSI : ESI));
			break;
		case rDI_r15:
			currentOperand->type = REGISTER;
			currentOperand->reg = rexPrefix->b ? R15 : (legPrefixes->group3 == OSO ? DI : (is64BitOperandSize ? RDI : EDI));
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
		case Eb:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, 0, 1, legPrefixes->group4 == ASO, is64BitMode, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Ev:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, 0, legPrefixes->group3 == OSO ? 2 : is64BitOperandSize ? 8 : 4, legPrefixes->group4 == ASO, is64BitMode, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Ew:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, 0, 2, legPrefixes->group4 == ASO, is64BitMode, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Ep:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, 0, legPrefixes->group3 == OSO ? 4 : 6, legPrefixes->group4 == ASO, is64BitMode, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Gb:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, 1, 1, 0, is64BitMode, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Gv:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, 1, legPrefixes->group3 == OSO ? 2 : is64BitOperandSize ? 8 : 4, 0, is64BitMode, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Gz:
			if(!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, 1, legPrefixes->group3 == OSO ? 2 : 4, 0, is64BitMode, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Gw:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, 1, 2, 0, is64BitMode, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case M:
		case Mp:
		case Ma:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, 0, 0, legPrefixes->group4 == ASO, is64BitMode, currentOperand)) { return 0; }
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
			currentOperand->immediate = (unsigned char)(getUIntFromBytes(bytesPtr, 1) + 2); // add 2 because that is this instruction's size
			break;
		case Jz:
			operandSize = legPrefixes->group3 == OSO ? 2 : 4;
			if (((*bytesPtr) + operandSize - 1) > maxBytesAddr) { return 0; }
			currentOperand->type = IMMEDIATE;
			currentOperand->immediate = getUIntFromBytes(bytesPtr, operandSize) + (legPrefixes->group3 == OSO ? 3 : 5); // add 3 or 5 because that is this instruction's size
			break;
		case Ob:
			if ((*bytesPtr) > maxBytesAddr) { return 0; }
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.segment = legPrefixes->group2 == NO_PREFIX ? DS : (enum Segment)legPrefixes->group2;
			currentOperand->memoryAddress.constDisplacement = getUIntFromBytes(bytesPtr, 1);
			break;
		case Ov:
			operandSize = legPrefixes->group3 == OSO ? 2 : is64BitOperandSize ? 8 : 4;
			if (((*bytesPtr) + operandSize - 1) > maxBytesAddr) { return 0; }
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.segment = legPrefixes->group2 == NO_PREFIX ? DS : (enum Segment)legPrefixes->group2;
			currentOperand->memoryAddress.constDisplacement = getUIntFromBytes(bytesPtr, operandSize);
			break;
		case Sw:
			if (!handleModRM(bytesPtr, maxBytesAddr, hasGotModRM, modRMByteRef, 2, 2, 0, is64BitMode, currentOperand)) { return 0; }
			hasGotModRM = 1;
			break;
		case Ap:
			if (((*bytesPtr) + 5) > maxBytesAddr) { return 0; }
			currentOperand->type = MEM_ADDRESS;
			currentOperand->memoryAddress.constDisplacement = getUIntFromBytes(bytesPtr, 4);
			currentOperand->memoryAddress.constSegment = getUIntFromBytes(bytesPtr, 2);
			break;
		}
	}

	return 1;
}

static unsigned char handleModRM(unsigned char** bytesPtr, unsigned char* maxBytesAddr, char hasGotModRM, unsigned char* modRMByteRef, char getRegOrSeg, unsigned char operandSize, char addressSizeOverride, unsigned char is64bitMode, struct Operand* result)
{
	unsigned char modRMByte = *modRMByteRef;

	if (!hasGotModRM) 
	{
		if ((*bytesPtr) > maxBytesAddr) { return 0; }
		modRMByte = (*bytesPtr)[0];
		(*bytesPtr)++;
		(*modRMByteRef) = modRMByte;
	}
	
	unsigned char mod = (((modRMByte >> 7) & 0x01) * 2) + ((modRMByte >> 6) & 0x01);
	unsigned char reg = (((modRMByte >> 5) & 0x01) * 4) + (((modRMByte >> 4) & 0x01) * 2) + ((modRMByte >> 3) & 0x01);
	unsigned char rm = (((modRMByte >> 2) & 0x01) * 4) + (((modRMByte >> 1) & 0x01) * 2) + ((modRMByte >> 0) & 0x01);

	if (getRegOrSeg == 1)
	{
		result->type = REGISTER;
		
		switch (operandSize)
		{
		case 1:
			result->reg = (enum Register)(reg + AL);
			break;
		case 2:
			result->reg = (enum Register)(reg + AX);
			break;
		case 4:
			result->reg = (enum Register)(reg + EAX);
			break;
		case 8:
			result->reg = (enum Register)(reg + RAX);
			break;
		}

		return 1;
	}
	else if (getRegOrSeg == 2)
	{
		result->type = SEGMENT;
		result->segment = (enum Segment)reg;

		return 1;
	}
	else if (mod == 3)
	{
		result->type = REGISTER;

		switch (rm)
		{
		case 0:
			result->reg = operandSize == 8 ? RAX : operandSize == 4 ? EAX : operandSize == 2 ? AX : AL;
			break;
		case 1:
			result->reg = operandSize == 8 ? RCX : operandSize == 4 ? ECX : operandSize == 2 ? CX : CL;
			break;
		case 2:
			result->reg = operandSize == 8 ? RDX : operandSize == 4 ? EDX : operandSize == 2 ? DX : DL;
			break;
		case 3:
			result->reg = operandSize == 8 ? RBX : operandSize == 4 ? EBX : operandSize == 2 ? BX : BL;
			break;
		case 4:
			result->reg = operandSize == 8 ? RSP : operandSize == 4 ? ESP : operandSize == 2 ? SP : AH;
			break;
		case 5:
			result->reg = operandSize == 8 ? RBP : operandSize == 4 ? EBP : operandSize == 2 ? BP : CH;
			break;
		case 6:
			result->reg = operandSize == 8 ? RSI : operandSize == 4 ? ESI : operandSize == 2 ? SI : DH;
			break;
		case 7:
			result->reg = operandSize == 8 ? RDI : operandSize == 4 ? EDI : operandSize == 2 ? DI : BH;
			break;
		}

		return 1;
	}

	result->type = MEM_ADDRESS;
	result->memoryAddress.ptrSize = operandSize;

	if (addressSizeOverride && !is64bitMode)
	{
		switch (mod)
		{
		case 0:
		{
			switch (rm)
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
				if (((*bytesPtr) + 1) > maxBytesAddr) { return 0; }
				result->memoryAddress.constDisplacement = getUIntFromBytes(bytesPtr, 2); // disp16
				break;
			case 7:
				result->memoryAddress.reg = BX;
				break;
			}

			break;
		}
		case 1:
		{
			switch (rm)
			{
			case 0:
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BX;
				result->memoryAddress.regDisplacement = SI;
				result->memoryAddress.constDisplacement = (signed char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			case 1:
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BX;
				result->memoryAddress.regDisplacement = DI;
				result->memoryAddress.constDisplacement = (signed char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			case 2:
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BP;
				result->memoryAddress.regDisplacement = SI;
				result->memoryAddress.constDisplacement = (signed char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			case 3:
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BP;
				result->memoryAddress.regDisplacement = DI;
				result->memoryAddress.constDisplacement = (signed char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			case 4:
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = SI;
				result->memoryAddress.constDisplacement = (signed char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			case 5:
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = DI;
				result->memoryAddress.constDisplacement = (signed char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			case 6:
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BP;
				result->memoryAddress.constDisplacement = (signed char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			case 7:
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BX;
				result->memoryAddress.constDisplacement = (signed char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			}

			break;
		}
		case 2:
		{
			switch (rm)
			{
			case 0:
				if (((*bytesPtr) + 1) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BX;
				result->memoryAddress.regDisplacement = SI;
				result->memoryAddress.constDisplacement = getUIntFromBytes(bytesPtr, 2); // disp16
				break;
			case 1:
				if (((*bytesPtr) + 1) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BX;
				result->memoryAddress.regDisplacement = DI;
				result->memoryAddress.constDisplacement = getUIntFromBytes(bytesPtr, 2); // disp16
				break;
			case 2:
				if (((*bytesPtr) + 1) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BP;
				result->memoryAddress.regDisplacement = SI;
				result->memoryAddress.constDisplacement = getUIntFromBytes(bytesPtr, 2); // disp16
				break;
			case 3:
				if (((*bytesPtr) + 1) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BP;
				result->memoryAddress.regDisplacement = DI;
				result->memoryAddress.constDisplacement = getUIntFromBytes(bytesPtr, 2); // disp16
				break;
			case 4:
				if (((*bytesPtr) + 1) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = SI;
				result->memoryAddress.constDisplacement = getUIntFromBytes(bytesPtr, 2); // disp16
				break;
			case 5:
				if (((*bytesPtr) + 1) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = DI;
				result->memoryAddress.constDisplacement = getUIntFromBytes(bytesPtr, 2); // disp16
				break;
			case 6:
				if (((*bytesPtr) + 1) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BP;
				result->memoryAddress.constDisplacement = getUIntFromBytes(bytesPtr, 2); // disp16
				break;
			case 7:
				if (((*bytesPtr) + 1) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = BX;
				result->memoryAddress.constDisplacement = getUIntFromBytes(bytesPtr, 2); // disp16
				break;
			}

			break;
		}
		}
	}
	else
	{
		switch (mod)
		{
		case 0:
		{
			switch (rm)
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
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				handleSIB(bytesPtr, result);
				break;
			case 5:
				if (((*bytesPtr) + 3) > maxBytesAddr) { return 0; }
				if (is64bitMode) { result->memoryAddress.reg = RIP; }
				else { result->memoryAddress.segment = DS; }
				result->memoryAddress.constDisplacement = getUIntFromBytes(bytesPtr, 4); // disp32
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
			switch (rm)
			{
			case 0:
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EAX;
				result->memoryAddress.constDisplacement = (signed char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			case 1:
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = ECX;
				result->memoryAddress.constDisplacement = (signed char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			case 2:
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EDX;
				result->memoryAddress.constDisplacement = (signed char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			case 3:
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EBX;
				result->memoryAddress.constDisplacement = (signed char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			case 4:
				if (((*bytesPtr) + 1) > maxBytesAddr) { return 0; }
				handleSIB(bytesPtr, result);
				result->memoryAddress.constDisplacement = (signed char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			case 5:
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EBP;
				result->memoryAddress.constDisplacement = (signed char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			case 6:
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = ESI;
				result->memoryAddress.constDisplacement = (signed char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			case 7:
				if ((*bytesPtr) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EDI;
				result->memoryAddress.constDisplacement = (signed char)getUIntFromBytes(bytesPtr, 1); // disp8
				break;
			}

			break;
		}
		case 2:
		{
			switch (rm)
			{
			case 0:
				if (((*bytesPtr) + 3) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EAX;
				result->memoryAddress.constDisplacement = getUIntFromBytes(bytesPtr, 4); // disp32
				break;
			case 1:
				if (((*bytesPtr) + 3) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = ECX;
				result->memoryAddress.constDisplacement = getUIntFromBytes(bytesPtr, 4); // disp32
				break;
			case 2:
				if (((*bytesPtr) + 3) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EDX;
				result->memoryAddress.constDisplacement = getUIntFromBytes(bytesPtr, 4); // disp32
				break;
			case 3:
				if (((*bytesPtr) + 3) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EBX;
				result->memoryAddress.constDisplacement = getUIntFromBytes(bytesPtr, 4); // disp32
				break;
			case 4:
				if (((*bytesPtr) + 4) > maxBytesAddr) { return 0; }
				handleSIB(bytesPtr, result);
				result->memoryAddress.constDisplacement = getUIntFromBytes(bytesPtr, 4); // disp32
				break;
			case 5:
				if (((*bytesPtr) + 3) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EBP;
				result->memoryAddress.constDisplacement = getUIntFromBytes(bytesPtr, 4); // disp32
				break;
			case 6:
				if (((*bytesPtr) + 3) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = ESI;
				result->memoryAddress.constDisplacement = getUIntFromBytes(bytesPtr, 4); // disp32
				break;
			case 7:
				if (((*bytesPtr) + 3) > maxBytesAddr) { return 0; }
				result->memoryAddress.reg = EDI;
				result->memoryAddress.constDisplacement = getUIntFromBytes(bytesPtr, 4); // disp32
				break;
			}

			break;
		}
		}
	}

	if (is64bitMode && !addressSizeOverride && result->memoryAddress.reg != NO_REG && result->memoryAddress.reg != RIP)
	{
		result->memoryAddress.reg += (RAX - EAX);
	}

	return 1;
}

static unsigned char* handleSIB(unsigned char** bytesPtr, struct Operand* result)
{
	unsigned char sibByte = (*bytesPtr)[0];
	(*bytesPtr)++;

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
	

	return result;
}

static unsigned long long getUIntFromBytes(unsigned char** bytesPtr, unsigned char resultSize)
{
	unsigned long long result = 0;

	for (int i = 0; i < resultSize; i++)
	{
		result += ((*bytesPtr)[0] << (8 * i));
		(*bytesPtr)++;
	}

	return result;
}