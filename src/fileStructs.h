#pragma once
#include "./jdc-str/jdcStr.h"

struct ImportedFunction
{
	struct JdcStr name;
	unsigned long long address;
};

struct FileSection
{
	struct JdcStr name;
	unsigned long long virtualAddress;
	unsigned long long fileOffset;
	int size;
};
