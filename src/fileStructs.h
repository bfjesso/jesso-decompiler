#pragma once

struct ImportedFunction
{
	char name[50];
	unsigned long long address;
};

struct FileSection
{
	unsigned long long virtualAddress;
	unsigned long long fileOffset;
	int size;
	char name[8];
};