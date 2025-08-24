#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "elfHandler.h"

unsigned char isELFX64(const char* filePath, unsigned char* isX64)
{
	FILE* file = fopen(filePath, "r");
	if(file)
	{
		unsigned char e_ident[5];
		fread(e_ident, 1, 5, file);
		if(memcmp(e_ident, ELFMAG, SELFMAG) == 0)
		{
			*isX64 = e_ident[EI_CLASS] == ELFCLASS64;
			fclose(file);
			return 1;
		}
		else
		{
			printf("Not a valid ELF binary.\n");
			fclose(file);
			return 0;
		}

	}
	else
	{
		printf("Failed to open file.\n");
		return 0;
	}

	return 0;
}

unsigned char getELFSymbolByValue64(const char* filePath, unsigned long long value, char* nameBuffer)
{
	char* stringBytes = 0;
	unsigned long long startAddress = 0;
	if(!getSectionBytesByName64(filePath, ".strtab", &stringBytes, &startAddress))
	{
		printf("Failed to find .strtab section.\n");
		return 0;
	}

	char* bytes = 0;
	unsigned int symtabSize = getSectionBytesByName64(filePath, ".symtab", &bytes, &startAddress);
	if(!symtabSize)
	{
		printf("Failed to find .symtab section.\n");
		free(stringBytes);
		return 0;
	}

	int i = 0;
	while(i < symtabSize)
	{
		Elf64_Sym* symbol = (Elf64_Sym*)(bytes + i);
		
		if(symbol->st_value == value && (stringBytes + symbol->st_name)[0] != 0)
		{
			strcpy(nameBuffer, stringBytes + symbol->st_name);
			free(stringBytes);
			free(bytes);
			return 1;
		}

		i += sizeof(Elf64_Sym);
	}
	
	free(stringBytes);
	free(bytes);

	return 0;
}

unsigned char getELFSymbolByValue32(const char* filePath, unsigned long long value, char* nameBuffer)
{
	char* stringBytes = 0;
	unsigned long long startAddress = 0;
	if(!getSectionBytesByName32(filePath, ".strtab", &stringBytes, &startAddress))
	{
		printf("Failed to find .strtab section.\n");
		return 0;
	}

	char* bytes = 0;
	unsigned int symtabSize = getSectionBytesByName32(filePath, ".symtab", &bytes, &startAddress);
	if(!symtabSize)
	{
		printf("Failed to find .symtab section.\n");
		free(stringBytes);
		return 0;
	}

	int i = 0;
	while(i < symtabSize)
	{
		Elf32_Sym* symbol = (Elf32_Sym*)(bytes + i);
		
		if(symbol->st_value == value && (stringBytes + symbol->st_name)[0] != 0)
		{
			strcpy(nameBuffer, stringBytes + symbol->st_name);
			free(stringBytes);
			free(bytes);
			return 1;
		}

		i += sizeof(Elf32_Sym);
	}
	
	free(stringBytes);
	free(bytes);

	return 0;
}

unsigned char getSectionHeaderByName64(const char* filePath, char* name, Elf64_Shdr* result)
{
	Elf64_Ehdr elfHeader;
	Elf64_Shdr sectionHeader;
	FILE* file = fopen(filePath, "r");

	if (file)
	{
		fread(&elfHeader, sizeof(elfHeader), 1, file);

		Elf64_Shdr nameStrTable;
		fseek(file, elfHeader.e_shoff + elfHeader.e_shstrndx * sizeof(nameStrTable), SEEK_SET);
		fread(&nameStrTable, 1, sizeof(nameStrTable), file);

		char* sectionNames = (char*)malloc(nameStrTable.sh_size);
		fseek(file, nameStrTable.sh_offset, SEEK_SET);
		fread(sectionNames, 1, nameStrTable.sh_size, file);

		if (memcmp(elfHeader.e_ident, ELFMAG, SELFMAG) == 0)
		{
			for (int i = 0; i < elfHeader.e_shnum; i++)
			{
				fseek(file, elfHeader.e_shoff + i * sizeof(sectionHeader), SEEK_SET);
				fread(&sectionHeader, 1, sizeof(sectionHeader), file);

				if (strcmp(sectionNames + sectionHeader.sh_name, name) == 0)
				{
					*result = sectionHeader;

					fclose(file);
					free(sectionNames);

					return 1;
				}
			}
		}
		else
		{
			printf("Not a valid ELF binary.\n");
			fclose(file);
			free(sectionNames);
			return 0;
		}

		printf("Couldn't find section '%s'\n", name);

		fclose(file);
		free(sectionNames);
	}
	else
	{
		printf("Failed to open file.\n");
		return 0;
	}

	return 0;
}

unsigned char getSectionHeaderByName32(const char* filePath, char* name, Elf32_Shdr* result)
{
	Elf32_Ehdr elfHeader;
	Elf32_Shdr sectionHeader;
	FILE* file = fopen(filePath, "r");

	if (file)
	{
		fread(&elfHeader, sizeof(elfHeader), 1, file);

		Elf32_Shdr nameStrTable;
		fseek(file, elfHeader.e_shoff + elfHeader.e_shstrndx * sizeof(nameStrTable), SEEK_SET);
		fread(&nameStrTable, 1, sizeof(nameStrTable), file);

		char* sectionNames = (char*)malloc(nameStrTable.sh_size);
		fseek(file, nameStrTable.sh_offset, SEEK_SET);
		fread(sectionNames, 1, nameStrTable.sh_size, file);

		if (memcmp(elfHeader.e_ident, ELFMAG, SELFMAG) == 0)
		{
			for (int i = 0; i < elfHeader.e_shnum; i++)
			{
				fseek(file, elfHeader.e_shoff + i * sizeof(sectionHeader), SEEK_SET);
				fread(&sectionHeader, 1, sizeof(sectionHeader), file);

				if (strcmp(sectionNames + sectionHeader.sh_name, name) == 0)
				{
					*result = sectionHeader;

					fclose(file);
					free(sectionNames);
					return 1;
				}
			}
		}
		else
		{
			printf("Not a valid ELF binary.\n");
			fclose(file);
			free(sectionNames);
			return 0;
		}

		printf("Couldn't find section '%s'\n", name);

		fclose(file);
		free(sectionNames);
	}
	else
	{
		printf("Failed to open file.\n");
		return 0;
	}

	return 0;
}

unsigned char readSectionBytes64(const char* filePath, Elf64_Shdr* section, unsigned char* buffer, unsigned int bufferSize)
{
	FILE* file = fopen(filePath, "r");
	if (file) 
	{
		fseek(file, section.sh_offset, SEEK_SET);
		fread(buffer, 1, bufferSize, file);
		return 1;
	}
	
	return 0;
}

unsigned char readSectionBytes32(const char* filePath, Elf32_Shdr* section, unsigned char* buffer, unsigned int bufferSize)
{
	FILE* file = fopen(filePath, "r");
	if (file)
	{
		fseek(file, section.sh_offset, SEEK_SET);
		fread(buffer, 1, bufferSize, file);
		return 1;
	}

	return 0;
}