#pragma once
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

struct VarType 
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

#ifdef __cplusplus
}
#endif

void varTypeToStr(struct VarType type, struct JdcStr* result);

unsigned char compareTypes(struct VarType t1, struct VarType t2);

struct VarType getTypeOfRegister(enum Mnemonic opcode, enum Register reg);

struct VarType getTypeOfOperand(enum Mnemonic opcode, struct Operand* operand);