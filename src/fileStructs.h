#pragma once
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
	unsigned long long address; // this is the address of the import's entry in either the IAT for PE files or GOT/PLT for ELF files
	int libraryNameIndex;
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
	unsigned char isReadOnly;
	unsigned long long rva;
	unsigned long long fileOffset;
	unsigned int physicalSize;
};
