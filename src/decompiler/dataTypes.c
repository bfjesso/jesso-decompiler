#include "dataTypes.h"
#include "../disassembler/operands.h"

extern const char* primitiveTypeStrs[NUM_OF_PRIMITIVE_TYPES] =
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

void dataTypeToStr(struct DataType dataType, struct JdcStr* result)
{
	if (!dataType.isUnsigned || dataType.primitiveType == FLOAT_TYPE || dataType.primitiveType == DOUBLE_TYPE || dataType.primitiveType == VOID_TYPE)
	{
		sprintfJdc(result, 0, "%s", primitiveTypeStrs[dataType.primitiveType]);
	}
	else 
	{
		sprintfJdc(result, 0, "unsigned %s", primitiveTypeStrs[dataType.primitiveType]);
	}

	for (int i = 0; i < dataType.pointerLevel; i++)
	{
		strcatJdc(result, "*");
	}
}

unsigned char compareDataTypes(struct DataType t1, struct DataType t2)
{
	return t1.primitiveType == t2.primitiveType && t1.isUnsigned == t2.isUnsigned && t1.pointerLevel == t2.pointerLevel;
}

struct DataType getRegisterDataType(enum Mnemonic opcode, enum Register reg)
{
	struct Operand regOperand = { 0 };
	regOperand.type = REGISTER;
	regOperand.reg = reg;

	return getOperandDataType(opcode, &regOperand);
}

struct DataType getMemoryAddressDataType(enum Mnemonic opcode, struct MemoryAddress* memAddress)
{
	struct Operand memAddrOperand = { 0 };
	memAddrOperand.type = MEM_ADDRESS;
	memAddrOperand.memoryAddress = *memAddress;

	return getOperandDataType(opcode, &memAddrOperand);
}

struct DataType getOperandDataType(enum Mnemonic opcode, struct Operand* operand)
{
	struct DataType result = { 0 };

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