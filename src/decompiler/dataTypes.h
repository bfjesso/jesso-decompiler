#pragma once
#include "../jdc-str/jdcStr.h"
#include "../disassembler/disassemblyStructs.h"

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
	unsigned char isUnsigned;
	unsigned char pointerLevel;
	enum PrimitiveType primitiveType;
};

#ifdef __cplusplus
extern "C"
{
#endif

	extern const char* primitiveTypeStrs[];
	extern const int numOfPrimitiveTypes;

	void dataTypeToStr(struct DataType dataType, struct JdcStr* result);

#ifdef __cplusplus
}
#endif

unsigned char compareDataTypes(struct DataType t1, struct DataType t2);

struct DataType getRegisterDataType(enum Mnemonic opcode, enum Register reg);

struct DataType getMemoryAddressDataType(enum Mnemonic opcode, struct MemoryAddress* memAddress);

struct DataType getOperandDataType(enum Mnemonic opcode, struct Operand* operand);
