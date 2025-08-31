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
	Elf64_Shdr strtabSection;
	if(!getSectionHeaderByName64(filePath, ".strtab", &strtabSection))
	{
		printf("Failed to find .strtab section.\n");
		return 0;
	}

	char* stringBytes = (char*)malloc(strtabSection.sh_size);
	if(!readSectionBytes64(filePath, &strtabSection, stringBytes, strtabSection.sh_size))
	{
		printf("Failed to read .strtab section bytes.\n");
		free(stringBytes);
		return 0;
	}

	Elf64_Shdr symtabSection;
	if(!getSectionHeaderByName64(filePath, ".symtab", &symtabSection))
	{
		printf("Failed to find .symtab section.\n");
		return 0;
	}

	char* bytes = (char*)malloc(symtabSection.sh_size);
	if(!readSectionBytes64(filePath, &symtabSection, bytes, symtabSection.sh_size))
	{
		printf("Failed to read .symtab section bytes.\n");
		free(bytes);
		return 0;
	}

	int i = 0;
	while(i < symtabSection.sh_size)
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
	Elf32_Shdr strtabSection;
	if(!getSectionHeaderByName32(filePath, ".strtab", &strtabSection))
	{
		printf("Failed to find .strtab section.\n");
		return 0;
	}

	char* stringBytes = (char*)malloc(strtabSection.sh_size);
	if(!readSectionBytes32(filePath, &strtabSection, stringBytes, strtabSection.sh_size))
	{
		printf("Failed to read .strtab section bytes.\n");
		free(stringBytes);
		return 0;
	}

	Elf32_Shdr symtabSection;
	if(!getSectionHeaderByName32(filePath, ".symtab", &symtabSection))
	{
		printf("Failed to find .symtab section.\n");
		return 0;
	}

	char* bytes = (char*)malloc(symtabSection.sh_size);
	if(!readSectionBytes32(filePath, &symtabSection, bytes, symtabSection.sh_size))
	{
		printf("Failed to read .symtab section bytes.\n");
		free(bytes);
		return 0;
	}

	int i = 0;
	while(i < symtabSection.sh_size)
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
		fseek(file, section->sh_offset, SEEK_SET);
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
		fseek(file, section->sh_offset, SEEK_SET);
		fread(buffer, 1, bufferSize, file);
		return 1;
	}

	return 0;
}

unsigned char getSectionHeaderByType64(const char* filePath, unsigned int type, Elf64_Shdr* result)
{
	Elf64_Ehdr elfHeader;
	Elf64_Shdr sectionHeader;
	FILE* file = fopen(filePath, "r");

	if (file)
	{
		fread(&elfHeader, sizeof(elfHeader), 1, file);

		if (memcmp(elfHeader.e_ident, ELFMAG, SELFMAG) == 0)
		{
			for (int i = 0; i < elfHeader.e_shnum; i++)
			{
				fseek(file, elfHeader.e_shoff + i * sizeof(sectionHeader), SEEK_SET);
				fread(&sectionHeader, 1, sizeof(sectionHeader), file);

				if (sectionHeader.sh_type == type)
				{
					*result = sectionHeader;
					fclose(file);
					return 1;
				}
			}
		}
		else
		{
			printf("Not a valid ELF binary.\n");
			fclose(file);
			return 0;
		}

		printf("Couldn't find section.\n");
		fclose(file);
	}
	else
	{
		printf("Failed to open file.\n");
		return 0;
	}

	return 0;
}

unsigned char getAllELFImports64(const char* filePath, struct ImportedFunction* buffer, int bufferLen)
{
	Elf64_Shdr dynstrSection;
	if(!getSectionHeaderByType64(filePath, SHT_STRTAB, &dynstrSection))
	{
		printf("Failed to find .dynstr section.\n");
		return 0;
	}

	char* stringBytes = (char*)malloc(dynstrSection.sh_size);
	if(!readSectionBytes64(filePath, &dynstrSection, stringBytes, dynstrSection.sh_size))
	{
		printf("Failed to read .dynstr section bytes.\n");
		free(stringBytes);
		return 0;
	}

	Elf64_Shdr dynsymSection;
	if(!getSectionHeaderByType64(filePath, SHT_DYNSYM, &dynsymSection))
	{
		printf("Failed to find .dynsym section.\n");
		free(stringBytes);
		return 0;
	}

	char* dynsymBytes = (char*)malloc(dynsymSection.sh_size);
	if(!readSectionBytes64(filePath, &dynsymSection, dynsymBytes, dynsymSection.sh_size))
	{
		printf("Failed to read .dynsym section bytes.\n");
		free(stringBytes);
		free(dynsymBytes);
		return 0;
	}

	Elf64_Shdr relapltSection;
	if(!getSectionHeaderByName64(filePath, ".rela.plt", &relapltSection))
	{
		printf("Failed to find .dynamic section.\n");
		free(stringBytes);
		free(dynsymBytes);
		return 0;
	}

	char* relapltBytes = (char*)malloc(relapltSection.sh_size);
	if(!readSectionBytes64(filePath, &relapltSection, relapltBytes, relapltSection.sh_size))
	{
		printf("Failed to read .dynamic section bytes.\n");
		free(stringBytes);
		free(dynsymBytes);
		free(relapltBytes);
		return 0;
	}

	int i = 0;
	while((i * sizeof(Elf64_Rela)) < relapltSection.sh_size && i < bufferLen)
	{
		Elf64_Rela* rela = (Elf64_Rela*)(relapltBytes + (i * sizeof(Elf64_Rela)));
		int val = ELF64_R_SYM(rela->r_info);
		Elf64_Sym* symbol = (Elf64_Sym*)(dynsymBytes + (val * sizeof(Elf64_Sym)));

		buffer[i].name[0] = 0;
		strcpy(buffer[i].name, stringBytes + symbol->st_name);

		buffer[i].address = (unsigned long long)(rela->r_offset);

		i++;
	}

	free(stringBytes);
	free(dynsymBytes);
	free(relapltBytes);

	return i;
}
