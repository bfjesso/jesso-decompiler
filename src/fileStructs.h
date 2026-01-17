#pragma once
#include "./jdc-str/jdcStr.h"

struct ImportedFunction
{
	struct JdcStr name;
	unsigned long long address;
};

struct FileSection
{
	unsigned long long virtualAddress;
	unsigned long long fileOffset;
	int size;
	char name[8];
};
