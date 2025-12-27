#pragma once

enum PrimitiveType
{
	VOID_TYPE,

	CHAR_TYPE,
	SHORT_TYPE,
	INT_TYPE,
	LONG_LONG_TYPE,

	FLOAT_TYPE,
	DOUBLE_TYPE
};

#ifdef __cplusplus
extern "C"
{
#endif

	extern const char* primitiveTypeStrs[];

#ifdef __cplusplus
}
#endif