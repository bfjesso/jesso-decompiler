#pragma once
#include "../jdc-str/jdcStr.h"
#include "../disassembler/disassemblyStructs.h"

#define NUM_OF_PRIMITIVE_TYPES 10

enum PrimitiveType
{
	VOID_TYPE,

	INT8_TYPE,
	INT16_TYPE,
	INT32_TYPE,
	INT64_TYPE,
	INT128_TYPE,
	INT256_TYPE,
	INT512_TYPE,

	FLOAT_TYPE,
	DOUBLE_TYPE,
};

struct DataType
{
	bool isUnsigned;
	bool pointerLevel;
	uint16_t arrayLen;
	enum PrimitiveType primitiveType;
};

#ifdef __cplusplus
extern "C"
{
#endif

	const char* primitiveTypeToStr(enum PrimitiveType primitive, bool useStdInt);

	void dataTypeToStr(struct DataType dataType, bool useStdInt, struct JdcStr* result);

#ifdef __cplusplus
}
#endif

bool doDataTypesRequireCasting(struct DataType t1, struct DataType t2, bool is64Bit);

uint8_t getDataTypeSize(struct DataType type, bool is64Bit);

uint8_t getPrimitiveTypeSize(enum PrimitiveType primitiveType);

struct DataType getRegisterDataType(struct DisassembledInstruction* instruction, uint8_t operandNum, enum Register reg);

struct DataType getMemoryAddressDataType(enum Mnemonic opcode, struct MemoryAddress* memAddress);

struct DataType getOperandDataType(enum Mnemonic opcode, struct Operand* operand);
