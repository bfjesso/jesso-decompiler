#include "dataTypes.h"
#include "../disassembler/operands.h"

extern const char* primitiveTypeStrs[] =
{
	"void",

	"char",
	"short",
	"int",
	"long long",

	"float",
	"double",

	"__int128",
	"__int256",
	"__int512"
};

extern const int numOfPrimitiveTypes = 10;

void varTypeToStr(struct VarType type, struct JdcStr* result)
{
	if (!type.isUnsigned || type.primitiveType == FLOAT_TYPE || type.primitiveType == DOUBLE_TYPE || type.primitiveType == VOID_TYPE)
	{
		sprintfJdc(result, 0, "%s", primitiveTypeStrs[type.primitiveType]);
	}
	else 
	{
		sprintfJdc(result, 0, "unsigned %s", primitiveTypeStrs[type.primitiveType]);
	}

	for (int i = 0; i < type.pointerLevel; i++) 
	{
		strcatJdc(result, "*");
	}
}

unsigned char compareTypes(struct VarType t1, struct VarType t2) 
{
	return t1.primitiveType == t2.primitiveType && t1.isUnsigned == t2.isUnsigned && t1.pointerLevel == t2.pointerLevel;
}

struct VarType getTypeOfRegister(enum Mnemonic opcode, enum Register reg)
{
	struct Operand regOperand = { 0 };
	regOperand.type = REGISTER;
	regOperand.reg = reg;

	return getTypeOfOperand(opcode, &regOperand);
}

struct VarType getTypeOfOperand(enum Mnemonic opcode, struct Operand* operand)
{
	struct VarType result = { 0 };

	switch (opcode)
	{
	case MOVSS:
	case MOVUPS:
	case ADDSS:
	case CVTPS2PD:
	case CVTSS2SD:
	case COMISS:
		result.primitiveType = FLOAT_TYPE;
		return result;
	case MOVSD:
	case MOVUPD:
	case ADDSD:
	case CVTPD2PS:
	case CVTSD2SS:
	case COMISD:
		result.primitiveType = DOUBLE_TYPE;
		return result;
	}

	if (!operand)
	{
		return result;
	}

	unsigned char size = getSizeOfOperand(operand);
	switch (size)
	{
	case 1:
		result.primitiveType = CHAR_TYPE;
		break;
	case 2:
		result.primitiveType = SHORT_TYPE;
		break;
	case 4:
		result.primitiveType = INT_TYPE;
		break;
	case 8:
		result.primitiveType = LONG_LONG_TYPE;
		break;
	case 16:
		result.primitiveType = INT_128_TPYE;
		break;
	case 32:
		result.primitiveType = INT_256_TPYE;
		break;
	case 64:
		result.primitiveType = INT_512_TPYE;
		break;
	}

	result.isUnsigned = doesOpcodeUseUnsignedInt(opcode);

	return result;
}