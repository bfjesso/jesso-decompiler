#include "dataTypes.h"

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
	if (type.isSigned) 
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
	return t1.primitiveType == t2.primitiveType && t1.isSigned == t2.isSigned && t1.pointerLevel == t2.pointerLevel;
}