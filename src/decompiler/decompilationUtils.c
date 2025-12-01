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