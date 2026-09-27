#pragma once
#include "../jdc-str/jdcStr.h"
#include "../disassembler/disassemblyStructs.h"

#define NUM_OF_PRIMITIVE_TYPES 10

enum PrimitiveType
{
	VOID_TYPE,

	CHAR_TYPE,
	SHORT_TYPE,
	INT_TYPE,
	LONG_LONG_TYPE,

	FLOAT_TYPE,
	DOUBLE_TYPE,

	INT_128_TPYE,
	INT_256_TPYE,
	INT_512_TPYE
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

	extern const char* primitiveTypeStrs[];

	void dataTypeToStr(struct DataType dataType, struct JdcStr* result);

#ifdef __cplusplus
}
#endif

bool doDataTypesRequireCasting(struct DataType t1, struct DataType t2, bool is64Bit);

uint8_t getDataTypeSize(struct DataType type, bool is64Bit);

uint8_t getPrimitiveTypeSize(enum PrimitiveType primitiveType);

struct DataType getRegisterDataType(struct DisassembledInstruction* instruction, uint8_t operandNum, enum Register reg);

struct DataType getMemoryAddressDataType(enum Mnemonic opcode, struct MemoryAddress* memAddress);

struct DataType getOperandDataType(enum Mnemonic opcode, struct Operand* operand);
