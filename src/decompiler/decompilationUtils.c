#include "decompilationUtils.h"

unsigned char getOperationStr(enum Mnemonic opcode, unsigned char getAssignment, char* resultBuffer)
{
	switch (opcode)
	{
	case MOV:
	case MOVUPS:
	case MOVUPD:
	case MOVSS:
	case MOVSD:
	case MOVSX:
	case MOVZX:
		if (getAssignment) { strcpy(resultBuffer, " = "); }
		else { strcpy(resultBuffer, ""); }
		return 1;
	case LEA:
		if (getAssignment) { strcpy(resultBuffer, " = &"); }
		else { strcpy(resultBuffer, "&"); }
		return 1;
	case ADD:
	case ADDPS:
	case ADDPD:
	case ADDSS:
	case ADDSD:
		if (getAssignment) { strcpy(resultBuffer, " += "); }
		else { strcpy(resultBuffer, " + "); }
		return 1;
	case SUB:
		if (getAssignment) { strcpy(resultBuffer, " -= "); }
		else { strcpy(resultBuffer, " - "); }
		return 1;
	case AND:
		if (getAssignment) { strcpy(resultBuffer, " &= "); }
		else { strcpy(resultBuffer, " & "); }
		return 1;
	case OR:
		if (getAssignment) { strcpy(resultBuffer, " |= "); }
		else { strcpy(resultBuffer, " | "); }
		return 1;
	case XOR:
		if (getAssignment) { strcpy(resultBuffer, " ^= "); }
		else { strcpy(resultBuffer, " ^ "); }
		return 1;
	case SHL:
		if (getAssignment) { strcpy(resultBuffer, " <<= "); }
		else { strcpy(resultBuffer, " << "); }
		return 1;
	case SHR:
		if (getAssignment) { strcpy(resultBuffer, " >>= "); }
		else { strcpy(resultBuffer, " >> "); }
		return 1;
	case IMUL:
		if (getAssignment) { strcpy(resultBuffer, " *= "); }
		else { strcpy(resultBuffer, " * "); }
		return 1;
	case IDIV:
		if (getAssignment) { strcpy(resultBuffer, " /= "); }
		else { strcpy(resultBuffer, " / "); }
		return 1;
	case CVTPS2PD:
	case CVTSS2SD:
		if (getAssignment) { strcpy(resultBuffer, " = (double)"); }
		else { strcpy(resultBuffer, "(double)"); }
		return 1;
	case CVTPD2PS:
	case CVTSD2SS:
		if (getAssignment) { strcpy(resultBuffer, " = (float)"); }
		else { strcpy(resultBuffer, "(float)"); }
		return 1;
	}

	return 0;
}

void wrapStrInParentheses(char* str)
{
	int len = (int)strlen(str);

	for (int i = len; i > 0; i--)
	{
		str[i] = str[i - 1];
	}

	str[0] = '(';
	strcat(str, ")");
}

// returns address of final instruction jumped to that isnt a jmp
unsigned long long resolveJmpChain(struct DecompilationParameters params, struct DisassembledInstruction* instruction, unsigned long long address)
{
	unsigned long long jmpAddress = address + instruction->operands[0].immediate;
	if (instruction->operands[0].type == MEM_ADDRESS)
	{
		jmpAddress = instruction->operands[0].memoryAddress.constDisplacement;
		if(compareRegisters(instruction->operands[0].memoryAddress.reg, IP))
		{
			jmpAddress += params.allAddresses[params.startInstructionIndex + 1];
		}
	}

	int instructionIndex = findInstructionByAddress(params.allAddresses, 0, params.totalNumOfInstructions - 1, jmpAddress);
	if (instructionIndex != -1)
	{
		struct DisassembledInstruction* jmpInstruction = &(params.allInstructions[instructionIndex]);
		if (instructionIndex != params.startInstructionIndex && (jmpInstruction->opcode == CALL_NEAR || jmpInstruction->opcode == JMP_NEAR))
		{
			params.startInstructionIndex = instructionIndex;
			return resolveJmpChain(params, jmpInstruction, params.allAddresses[instructionIndex]);
		}
	}

	return jmpAddress;
}
