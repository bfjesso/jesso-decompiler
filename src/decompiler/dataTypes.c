#include "dataTypes.h"
#include "decompilationUtils.h"

const char* primitiveTypeToStr(enum PrimitiveType primitive, bool useStdInt)
{
	switch (primitive)
	{
	case VOID_TYPE:
		return "void";
	case FLOAT_TYPE:
		return "float";
	case DOUBLE_TYPE:
		return "double";
	case LONG_DOUBLE_TYPE:
		return "long double";
	case INT128_TYPE:
		return "__int128";
	case INT256_TYPE:
		return "__int256";
	case INT512_TYPE:
		return "__int512";
	}

	if (useStdInt)
	{
		switch (primitive)
		{
		case INT8_TYPE:
			return "int8_t";
		case UINT8_TYPE:
			return "uint8_t";
		case INT16_TYPE:
			return "int16_t";
		case UINT16_TYPE:
			return "uint16_t";
		case INT32_TYPE:
			return "int32_t";
		case UINT32_TYPE:
			return "uint32_t";
		case INT64_TYPE:
			return "int64_t";
		case UINT64_TYPE:
			return "uint64_t";
		}
	}
	else
	{
		switch (primitive)
		{
		case INT8_TYPE:
			return "char";
		case UINT8_TYPE:
			return "unsigned char";
		case INT16_TYPE:
			return "short";
		case UINT16_TYPE:
			return "unsigned short";
		case INT32_TYPE:
			return "int";
		case UINT32_TYPE:
			return "unsigned int";
		case INT64_TYPE:
			return "long long";
		case UINT64_TYPE:
			return "unsigned long long";
		}
	}

	return "";
}

void dataTypeToStr(struct DataType dataType, bool useStdInt, struct JdcStr* result)
{
	strcpyJdc(result, primitiveTypeToStr(dataType.primitiveType, useStdInt));

	for (int32_t i = 0; i < dataType.pointerLevel; i++)
	{
		strcatJdc(result, "*");
	}
}

bool isPrimitiveInteger(enum PrimitiveType primitive)
{
	return primitive >= INT8_TYPE && primitive <= INT512_TYPE;
}

bool isPrimitiveUnsigned(enum PrimitiveType primitive)
{
	return primitive == UINT8_TYPE || primitive == UINT16_TYPE || primitive == UINT32_TYPE || primitive == UINT64_TYPE;
}

enum PrimitiveType getIntType(uint8_t size, bool isSigned)
{
	switch (size)
	{
	case 1:
		return (isSigned ? INT8_TYPE : UINT8_TYPE);
	case 2:
		return (isSigned ? INT16_TYPE : UINT16_TYPE);
	case 4:
		return (isSigned ? INT32_TYPE : UINT32_TYPE);
	case 8:
		return (isSigned ? INT64_TYPE : UINT64_TYPE);
	case 16:
		return INT128_TYPE;
	case 32:
		return INT256_TYPE;
	case 64:
		return INT512_TYPE;
	}

	return VOID_TYPE;
}

enum PrimitiveType getAddressPrimitive(enum InstructionSetArchitecture isa)
{
	switch (isa)
	{
	case X86_ISA:
		return INT32_TYPE;
	case X86_64_ISA:
		return INT64_TYPE;
	case LC3_ISA:
		return INT16_TYPE;
	}

	return INT64_TYPE;
}

bool isDataTypePtr(struct DataType dataType)
{
	return (dataType.pointerLevel > 0 || dataType.arrayLen > 1);
}

bool doDataTypesRequireCasting(struct DataType t1, struct DataType t2, enum InstructionSetArchitecture isa)
{
	bool isT1Ptr = isDataTypePtr(t1);
	bool isT2Ptr = isDataTypePtr(t2);

	if (isT1Ptr && isT2Ptr)
	{
		return false;
	}

	enum PrimitiveType addressPrimitive = getAddressPrimitive(isa);
	if ((isT1Ptr && t2.primitiveType == addressPrimitive) || (isT2Ptr && t1.primitiveType == addressPrimitive))
	{
		return false;
	}

	return (t1.primitiveType != t2.primitiveType || isT1Ptr != isT2Ptr);
}

uint8_t getDataTypeSize(struct DataType dataType, enum InstructionSetArchitecture isa)
{
	if (isDataTypePtr(dataType))
	{
		return getPrimitiveTypeSize(getAddressPrimitive(isa));
	}

	return getPrimitiveTypeSize(dataType.primitiveType);
}

uint8_t getPrimitiveTypeSize(enum PrimitiveType primitiveType)
{
	switch (primitiveType)
	{
	case INT8_TYPE:
	case UINT8_TYPE:
		return 1;
	case INT16_TYPE:
	case UINT16_TYPE:
		return 2;
	case INT32_TYPE:
	case UINT32_TYPE:
	case FLOAT_TYPE:
		return 4;
	case INT64_TYPE:
	case UINT64_TYPE:
	case DOUBLE_TYPE:
		return 8;
	case LONG_DOUBLE_TYPE:
		return 10;
	case INT128_TYPE:
		return 16;
	case INT256_TYPE:
		return 32;
	case INT512_TYPE:
		return 64;
	}

	return 0;
}

enum JdcStatus getRegisterDataType(struct DisassembledInstruction* instruction, uint8_t operandNum, enum Register reg, struct DataType* result)
{
	struct Operand regOperand = { 0 };
	regOperand.type = REGISTER_OPERAND;
	regOperand.reg = reg;

	if (instruction)
	{
		if (ERROR_JDC == getOperandDataType(instruction->opcode, &regOperand, result))
		{
			return ERROR_JDC;
		}

		if (operandNum < 4 && instruction->operands[operandNum].type == MEMORY_OPERAND)
		{
			struct MemoryAddress* memAddress = &instruction->operands[operandNum].memoryAddress;
			if (memAddress->reg == reg && memAddress->constDisplacement < 0x10000) // this is to check that the reg is not being used as a displacement
			{
				result->pointerLevel = 1;
			}
		}

		return SUCCESS_JDC;
	}

	return getOperandDataType(NO_OPCODE, &regOperand, result);
}

enum JdcStatus getMemoryAddressDataType(enum OpcodeMnemonic opcode, struct MemoryAddress* memAddress, struct DataType* result)
{
	struct Operand memAddrOperand = { 0 };
	memAddrOperand.type = MEMORY_OPERAND;
	memAddrOperand.memoryAddress = *memAddress;

	return getOperandDataType(opcode, &memAddrOperand, result);
}

enum JdcStatus getOperandDataType(enum OpcodeMnemonic opcode, struct Operand* operand, struct DataType* result)
{
	result->arrayLen = 1;
	result->pointerLevel = 0;

	switch (opcode)
	{
	case MOVSS_X86:
	case MOVUPS_X86:
	case ADDSS_X86:
	case CVTPS2PD_X86:
	case CVTSS2SD_X86:
	case COMISS_X86:
		result->primitiveType = FLOAT_TYPE;
		return SUCCESS_JDC;
	case MOVSD_X86:
	case MOVUPD_X86:
	case ADDSD_X86:
	case CVTPD2PS_X86:
	case CVTSD2SS_X86:
	case COMISD_X86:
		result->primitiveType = DOUBLE_TYPE;
		return SUCCESS_JDC;
	}

	if (!operand)
	{
		return result->primitiveType == VOID_TYPE ? ERROR_JDC : SUCCESS_JDC;
	}

	uint8_t size = getSizeOfOperand(operand);
	switch (size)
	{
	case 1:
		result->primitiveType = INT8_TYPE;
		break;
	case 2:
		result->primitiveType = INT16_TYPE;
		break;
	case 4:
		result->primitiveType = INT32_TYPE;
		break;
	case 8:
		result->primitiveType = INT64_TYPE;
		break;
	case 10:
		result->primitiveType = LONG_DOUBLE_TYPE;
		break;
	case 16:
		result->primitiveType = INT128_TYPE;
		break;
	case 32:
		result->primitiveType = INT256_TYPE;
		break;
	case 64:
		result->primitiveType = INT512_TYPE;
		break;
	default:
		return ERROR_JDC;
	}

	if (doesOpcodeUseUnsignedInt(opcode))
	{
		result->primitiveType = getIntType(getPrimitiveTypeSize(result->primitiveType), false);
	}

	return SUCCESS_JDC;
}