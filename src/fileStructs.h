#pragma once
#include "./jdc-str/jdcStr.h"

struct ImportedFunction
{
	struct JdcStr name;
	unsigned long long address;
};

enum FileSectionType 
{
	CODE_FST,
	INIT_DATA_FST,
	UNINIT_DATA_FST,
	OTHER_FST
};

static const char* fileSectionTypeStrs[] =
{
	"CODE_FST",
	"INIT_DATA_FST",
	"UNINIT_DATA_FST",
	"OTHER_FST"
};

struct FileSection
{
	struct JdcStr name;
	enum FileSectionType type;
	unsigned char isReadOnly;
	unsigned long long virtualAddress;
	unsigned long long fileOffset;
	int size;
};
