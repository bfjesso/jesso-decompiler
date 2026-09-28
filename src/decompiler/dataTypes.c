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

bool isPrimitiveUnsigned(enum PrimitiveType primitive)
{
	return primitive == UINT8_TYPE || primitive == UINT16_TYPE || primitive == UINT32_TYPE || primitive == UINT64_TYPE;
}

enum PrimitiveType getIntType(uint8_t size, bool isSigned)
{
	if (isSigned) 
	{
		switch (size) 
		{
		case 1:
			return INT8_TYPE;
		case 2:
			return INT16_TYPE;
		case 4:
			return INT32_TYPE;
		case 8:
			return INT64_TYPE;
		}
	}
	else 
	{
		switch (size)
		{
		case 1:
			return UINT8_TYPE;
		case 2:
			return UINT16_TYPE;
		case 4:
			return UINT32_TYPE;
		case 8:
			return UINT64_TYPE;
		}
	}

	return VOID_TYPE;
}

bool doDataTypesRequireCasting(struct DataType t1, struct DataType t2, bool is64Bit)
{
	if ((t1.pointerLevel > 0 || t1.arrayLen > 1) && (t2.pointerLevel > 0 || t2.arrayLen > 1)) 
	{
		return false;
	}
	
	if (((t1.pointerLevel > 0 || t1.arrayLen > 1) && t2.primitiveType == is64Bit ? INT64_TYPE : INT32_TYPE) ||
		((t2.pointerLevel > 0 || t2.arrayLen > 1) && t1.primitiveType == is64Bit ? INT64_TYPE : INT32_TYPE))
	{
		return false;
	}

	return t1.primitiveType != t2.primitiveType;
}

uint8_t getDataTypeSize(struct DataType type, bool is64Bit) 
{
	if (type.pointerLevel > 0 || type.arrayLen > 1)
	{
		return is64Bit ? 8 : 4;
	}

	return getPrimitiveTypeSize(type.primitiveType);
}

uint8_t getPrimitiveTypeSize(enum PrimitiveType primitiveType)
{
	switch (primitiveType)
	{
	case INT8_TYPE:
		return 1;
	case INT16_TYPE:
		return 2;
	case INT32_TYPE:
	case FLOAT_TYPE:
		return 4;
	case INT64_TYPE:
	case DOUBLE_TYPE:
		return 8;
	case INT128_TYPE:
		return 16;
	case INT256_TYPE:
		return 32;
	case INT512_TYPE:
		return 64;
	}

	return 0;
}

struct DataType getRegisterDataType(struct DisassembledInstruction* instruction, uint8_t operandNum, enum Register reg)
{	
	struct Operand regOperand = { 0 };
	regOperand.type = REGISTER;
	regOperand.reg = reg;

	if (instruction)
	{
		struct DataType result = getOperandDataType(instruction->opcode, &regOperand);
		if (operandNum < 4 && instruction->operands[operandNum].type == MEM_ADDRESS)
		{
			struct MemoryAddress* memAddress = &instruction->operands[operandNum].memoryAddress;
			if (memAddress->reg == reg && memAddress->constDisplacement < 0x10000) // this is to check that the reg is not being used as a displacement
			{
				result.pointerLevel = 1;
			}
		}

		return result;
	}
	
	return getOperandDataType(NO_MNEMONIC, &regOperand);
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

	uint8_t size = getSizeOfOperand(operand);
	switch (size)
	{
	case 1:
		result.primitiveType = INT8_TYPE;
		break;
	case 2:
		result.primitiveType = INT16_TYPE;
		break;
	case 4:
		result.primitiveType = INT32_TYPE;
		break;
	case 8:
		result.primitiveType = INT64_TYPE;
		break;
	case 16:
		result.primitiveType = INT128_TYPE;
		break;
	case 32:
		result.primitiveType = INT256_TYPE;
		break;
	case 64:
		result.primitiveType = INT512_TYPE;
		break;
	}

	if (doesOpcodeUseUnsignedInt(opcode)) 
	{
		result.primitiveType = getIntType(getPrimitiveTypeSize(result.primitiveType), false);
		
	}

	return result;
}