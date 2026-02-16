#pragma once

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

#ifdef __cplusplus
extern "C"
{
#endif

	extern const char* primitiveTypeStrs[];
	extern const int numOfPrimitiveTypes;

#ifdef __cplusplus
}
#endif