#pragma once
#include "jdcTypes.h"
#include "./jdc-str/jdcStr.h"

enum FileFormat
{
	UNKNOWN_FF,
	PE_FF,
	ELF_FF
};

struct ImportedFunction
{
	struct JdcStr name;
	uint64_t address; // this is the address of the import's entry in either the IAT for PE files or GOT/PLT for ELF files
	int32_t libraryNameIndex;
};

enum FileSectionType 
{
	OTHER_FST,
	CODE_FST,
	INIT_DATA_FST,
	UNINIT_DATA_FST
};

struct FileSection
{
	struct JdcStr name;
	enum FileSectionType type;
	bool isReadOnly;
	uint64_t rva;
	uint64_t fileOffset;
	uint64_t physicalSize;
};
