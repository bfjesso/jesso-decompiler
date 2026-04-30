#include "elfHandler.h"

extern char* __cxa_demangle(const char* mangled_name, char* output_buffer, size_t* length, int* status);

char* demangleSymbol(const char* mangledStr, int* status)
{
	char* result = 0;

	char* demangleResult = __cxa_demangle(mangledStr, 0, 0, status);
	if(*status == 0)
	{
		result = (char*)calloc(strlen(demangleResult), 1);

		int startIndex = 0;
		int i = 0;
		while(demangleResult[i] != 0)
		{
			if(demangleResult[i] == ' ') // this is looking for the return type
			{
				startIndex = i + 1;
				break;
			}
			else if(demangleResult[i] == '<' || demangleResult[i] == '(')
			{
				break;
			}

			i++;
		}

		int numOfBrakets = 0;
		int bufferIndex = 0;
		i = startIndex;
		while(demangleResult[i] != 0)
		{
			if(demangleResult[i] == '<' || demangleResult[i] == '(')
			{
				numOfBrakets++;
			}
			else if(demangleResult[i] == '>' || demangleResult[i] == ')')
			{
				numOfBrakets--;
			}
			else if(numOfBrakets == 0 && demangleResult[i] != ' ')
			{
				result[bufferIndex] = demangleResult[i];
				bufferIndex++;
			}

			i++;
		}
	}

	free(demangleResult);
	return result;
}

unsigned char isELFX64(const char* filePath, unsigned char* isX64)
{
	FILE* file = fopen(filePath, "r");
	if(file)
	{
		unsigned char e_ident[5];
		fread(e_ident, 1, 5, file);
		fclose(file);

		if(memcmp(e_ident, ELFMAG, SELFMAG) == 0)
		{
			*isX64 = e_ident[EI_CLASS] == ELFCLASS64;
			return 1;
		}
	}

	return 0;
}

unsigned char getELFSymbolByValue64(const char* filePath, unsigned long long value, struct JdcStr* result)
{
	Elf64_Shdr strtabSection;
	if(!getSectionHeaderByName64(filePath, ".strtab", &strtabSection))
	{
		return 0;
	}

	char* stringBytes = (char*)malloc(strtabSection.sh_size);
	if(!readSectionBytes64(filePath, &strtabSection, stringBytes, strtabSection.sh_size))
	{
		free(stringBytes);
		return 0;
	}

	Elf64_Shdr symtabSection;
	if(!getSectionHeaderByName64(filePath, ".symtab", &symtabSection))
	{
		return 0;
	}

	char* bytes = (char*)malloc(symtabSection.sh_size);
	if(!readSectionBytes64(filePath, &symtabSection, bytes, symtabSection.sh_size))
	{
		free(bytes);
		return 0;
	}

	int i = 0;
	while(i < symtabSection.sh_size)
	{
		Elf64_Sym* symbol = (Elf64_Sym*)(bytes + i);
		
		if(symbol->st_value == value && (stringBytes + symbol->st_name)[0] != 0)
		{
			int status = 0;
			char* demangledStr = demangleSymbol(stringBytes + symbol->st_name, &status);
			if(status == 0 && demangledStr != 0)
			{
				strcpyJdc(result, demangledStr);
			}
			else
			{
				strcpyJdc(result, stringBytes + symbol->st_name);
			}

			free(demangledStr);
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

unsigned char getELFSymbolByValue32(const char* filePath, unsigned long long value, struct JdcStr* result)
{
	Elf32_Shdr strtabSection;
	if(!getSectionHeaderByName32(filePath, ".strtab", &strtabSection))
	{
		return 0;
	}

	char* stringBytes = (char*)malloc(strtabSection.sh_size);
	if(!readSectionBytes32(filePath, &strtabSection, stringBytes, strtabSection.sh_size))
	{
		free(stringBytes);
		return 0;
	}

	Elf32_Shdr symtabSection;
	if(!getSectionHeaderByName32(filePath, ".symtab", &symtabSection))
	{
		return 0;
	}

	char* bytes = (char*)malloc(symtabSection.sh_size);
	if(!readSectionBytes32(filePath, &symtabSection, bytes, symtabSection.sh_size))
	{
		free(bytes);
		return 0;
	}

	int i = 0;
	while(i < symtabSection.sh_size)
	{
		Elf32_Sym* symbol = (Elf32_Sym*)(bytes + i);

		if(symbol->st_value == value && (stringBytes + symbol->st_name)[0] != 0)
		{
			int status = 0;
			char* demangledStr = demangleSymbol(stringBytes + symbol->st_name, &status);
			if(status == 0 && demangledStr != 0)
			{
				strcpyJdc(result, demangledStr);
			}
			else
			{
				strcpyJdc(result, stringBytes + symbol->st_name);
			}

			free(demangledStr);
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

int getNumOfELFSections64(const char* filePath)
{
	FILE* file = fopen(filePath, "r");
	if (file)
	{
		Elf64_Ehdr elfHeader;
		fread(&elfHeader, sizeof(elfHeader), 1, file);

		Elf64_Shdr sectionHeader;

		int result = 0;
		for (int i = 0; i < elfHeader.e_shnum; i++)
		{
			fseek(file, elfHeader.e_shoff + i * sizeof(sectionHeader), SEEK_SET);
			fread(&sectionHeader, 1, sizeof(sectionHeader), file);

			if(sectionHeader.sh_size == 0)
			{
				continue;
			}

			result++;
		}

		fclose(file);
		return result;
	}

	return 0;
}

int getNumOfELFSections32(const char* filePath)
{
	FILE* file = fopen(filePath, "r");
	if (file)
	{
		Elf32_Ehdr elfHeader;
		fread(&elfHeader, sizeof(elfHeader), 1, file);

		Elf32_Shdr sectionHeader;

		int result = 0;
		for (int i = 0; i < elfHeader.e_shnum; i++)
		{
			fseek(file, elfHeader.e_shoff + i * sizeof(sectionHeader), SEEK_SET);
			fread(&sectionHeader, 1, sizeof(sectionHeader), file);

			if(sectionHeader.sh_size == 0)
			{
				continue;
			}

			result++;
		}

		fclose(file);
		return result;
	}

	return 0;
}

unsigned char getAllELFSectionHeaders64(const char* filePath, struct FileSection* buffer, int bufferLen)
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
			int bufferIndex = 0;
			for (int i = 0; i < elfHeader.e_shnum; i++)
			{
				if (bufferIndex >= bufferLen)
				{
					fclose(file);
					return 0;
				}

				fseek(file, elfHeader.e_shoff + i * sizeof(sectionHeader), SEEK_SET);
				fread(&sectionHeader, 1, sizeof(sectionHeader), file);

				if(sectionHeader.sh_size == 0)
				{
					continue;
				}

				buffer[bufferIndex].name = initializeJdcStrWithVal(sectionNames + sectionHeader.sh_name);

				if (sectionHeader.sh_flags & SHF_EXECINSTR)
				{
					buffer[bufferIndex].type = CODE_FST;
				}
				else 
				{
					buffer[bufferIndex].type = INIT_DATA_FST;
				}

				buffer[bufferIndex].isReadOnly = !(sectionHeader.sh_flags & SHF_WRITE);
				
				buffer[bufferIndex].virtualAddress = sectionHeader.sh_addr;
				buffer[bufferIndex].fileOffset = sectionHeader.sh_offset;
				buffer[bufferIndex].size = sectionHeader.sh_size;
				bufferIndex++;
			}

			fclose(file);
			return 1;
		}

		fclose(file);
	}

	return 0;
}

unsigned char getAllELFSectionHeaders32(const char* filePath, struct FileSection* buffer, int bufferLen)
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
			int bufferIndex = 0;
			for (int i = 0; i < elfHeader.e_shnum; i++)
			{
				if (bufferIndex >= bufferLen)
				{
					fclose(file);
					return 0;
				}

				fseek(file, elfHeader.e_shoff + i * sizeof(sectionHeader), SEEK_SET);
				fread(&sectionHeader, 1, sizeof(sectionHeader), file);

				if(sectionHeader.sh_size == 0)
				{
					continue;
				}

				buffer[bufferIndex].name = initializeJdcStrWithVal(sectionNames + sectionHeader.sh_name);

				if (sectionHeader.sh_flags & SHF_EXECINSTR)
				{
					buffer[bufferIndex].type = CODE_FST;
				}
				else
				{
					buffer[bufferIndex].type = INIT_DATA_FST;
				}

				buffer[bufferIndex].isReadOnly = !(sectionHeader.sh_flags & SHF_WRITE);

				buffer[bufferIndex].virtualAddress = sectionHeader.sh_addr;
				buffer[bufferIndex].fileOffset = sectionHeader.sh_offset;
				buffer[bufferIndex].size = sectionHeader.sh_size;
				bufferIndex++;
			}

			fclose(file);
			return 1;
		}

		fclose(file);
	}

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

		fclose(file);
		free(sectionNames);
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

		fclose(file);
		free(sectionNames);
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
		fclose(file);
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
		fclose(file);
		return 1;
	}

	return 0;
}

unsigned char getSectionHeaderByType64(const char* filePath, unsigned int type, int index, Elf64_Shdr* result)
{
	Elf64_Ehdr elfHeader;
	Elf64_Shdr sectionHeader;
	FILE* file = fopen(filePath, "r");

	if (file)
	{
		fread(&elfHeader, sizeof(elfHeader), 1, file);

		if (memcmp(elfHeader.e_ident, ELFMAG, SELFMAG) == 0)
		{
			int num = 0;

			for (int i = 0; i < elfHeader.e_shnum; i++)
			{
				fseek(file, elfHeader.e_shoff + i * sizeof(sectionHeader), SEEK_SET);
				fread(&sectionHeader, 1, sizeof(sectionHeader), file);

				if (sectionHeader.sh_type == type)
				{
					if(num == index)
					{
						*result = sectionHeader;
						fclose(file);
						return 1;
					}

					num++;
				}
			}
		}

		fclose(file);
	}

	return 0;
}

unsigned char getSectionHeaderByType32(const char* filePath, unsigned int type, int index, Elf32_Shdr* result)
{
	Elf32_Ehdr elfHeader;
	Elf32_Shdr sectionHeader;
	FILE* file = fopen(filePath, "r");

	if (file)
	{
		fread(&elfHeader, sizeof(elfHeader), 1, file);

		if (memcmp(elfHeader.e_ident, ELFMAG, SELFMAG) == 0)
		{
			int num = 0;

			for (int i = 0; i < elfHeader.e_shnum; i++)
			{
				fseek(file, elfHeader.e_shoff + i * sizeof(sectionHeader), SEEK_SET);
				fread(&sectionHeader, 1, sizeof(sectionHeader), file);

				if (sectionHeader.sh_type == type)
				{
					if(num == index)
					{
						*result = sectionHeader;
						fclose(file);
						return 1;
					}

					num++;
				}
			}
		}

		fclose(file);
	}

	return 0;
}

int getNumOfELFImports64(const char* filePath)
{
	Elf64_Shdr dynstrSection;
	if (!getSectionHeaderByType64(filePath, SHT_STRTAB, 0, &dynstrSection))
	{
		return 0;
	}

	char* stringBytes = (char*)malloc(dynstrSection.sh_size);
	if (!readSectionBytes64(filePath, &dynstrSection, stringBytes, dynstrSection.sh_size))
	{
		free(stringBytes);
		return 0;
	}

	Elf64_Shdr dynsymSection;
	if (!getSectionHeaderByType64(filePath, SHT_DYNSYM, 0, &dynsymSection))
	{
		free(stringBytes);
		return 0;
	}

	char* dynsymBytes = (char*)malloc(dynsymSection.sh_size);
	if (!readSectionBytes64(filePath, &dynsymSection, dynsymBytes, dynsymSection.sh_size))
	{
		free(stringBytes);
		free(dynsymBytes);
		return 0;
	}

	int relaNum = 0;
	Elf64_Shdr relaSection;
	int result = 0;
	while (getSectionHeaderByType64(filePath, SHT_RELA, relaNum, &relaSection)) // going through all rela sections
	{
		char* relaBytes = (char*)malloc(relaSection.sh_size);
		if (!readSectionBytes64(filePath, &relaSection, relaBytes, relaSection.sh_size))
		{
			free(stringBytes);
			free(dynsymBytes);
			free(relaBytes);
			return 0;
		}

		int i = 0;
		while ((i * sizeof(Elf64_Rela)) < relaSection.sh_size)
		{
			Elf64_Rela* rela = (Elf64_Rela*)(relaBytes + (i * sizeof(Elf64_Rela)));
			int val = ELF64_R_SYM(rela->r_info);
			Elf64_Sym* symbol = (Elf64_Sym*)(dynsymBytes + (val * sizeof(Elf64_Sym)));

			if(strcmp(stringBytes + symbol->st_name, "") != 0)
			{
				result++;
			}

			i++;
		}

		free(relaBytes);

		relaNum++;
	}

	free(stringBytes);
	free(dynsymBytes);

	return result;
}

int getNumOfELFImports32(const char* filePath)
{
	Elf32_Shdr dynstrSection;
	if (!getSectionHeaderByType32(filePath, SHT_STRTAB, 0, &dynstrSection))
	{
		return 0;
	}

	char* stringBytes = (char*)malloc(dynstrSection.sh_size);
	if (!readSectionBytes32(filePath, &dynstrSection, stringBytes, dynstrSection.sh_size))
	{
		free(stringBytes);
		return 0;
	}

	Elf32_Shdr dynsymSection;
	if (!getSectionHeaderByType32(filePath, SHT_DYNSYM, 0, &dynsymSection))
	{
		free(stringBytes);
		return 0;
	}

	char* dynsymBytes = (char*)malloc(dynsymSection.sh_size);
	if (!readSectionBytes32(filePath, &dynsymSection, dynsymBytes, dynsymSection.sh_size))
	{
		free(stringBytes);
		free(dynsymBytes);
		return 0;
	}

	int relaNum = 0;
	Elf32_Shdr relaSection;
	int result = 0;
	while (getSectionHeaderByType32(filePath, SHT_RELA, relaNum, &relaSection)) // going through all rela sections
	{
		char* relaBytes = (char*)malloc(relaSection.sh_size);
		if (!readSectionBytes32(filePath, &relaSection, relaBytes, relaSection.sh_size))
		{
			free(stringBytes);
			free(dynsymBytes);
			free(relaBytes);
			return 0;
		}

		int i = 0;
		while ((i * sizeof(Elf32_Rela)) < relaSection.sh_size)
		{
			Elf32_Rela* rela = (Elf32_Rela*)(relaBytes + (i * sizeof(Elf32_Rela)));
			int val = ELF32_R_SYM(rela->r_info);
			Elf32_Sym* symbol = (Elf32_Sym*)(dynsymBytes + (val * sizeof(Elf32_Sym)));

			if(strcmp(stringBytes + symbol->st_name, "") != 0)
			{
				result++;
			}

			i++;
		}

		free(relaBytes);

		relaNum++;
	}

	free(stringBytes);
	free(dynsymBytes);

	return result;
}

int getAllELFImports64(const char* filePath, struct ImportedFunction* buffer, int bufferLen)
{
	Elf64_Shdr dynstrSection;
	if(!getSectionHeaderByType64(filePath, SHT_STRTAB, 0, &dynstrSection))
	{
		return 0;
	}

	char* stringBytes = (char*)malloc(dynstrSection.sh_size);
	if(!readSectionBytes64(filePath, &dynstrSection, stringBytes, dynstrSection.sh_size))
	{
		free(stringBytes);
		return 0;
	}

	Elf64_Shdr dynsymSection;
	if(!getSectionHeaderByType64(filePath, SHT_DYNSYM, 0, &dynsymSection))
	{
		free(stringBytes);
		return 0;
	}

	char* dynsymBytes = (char*)malloc(dynsymSection.sh_size);
	if(!readSectionBytes64(filePath, &dynsymSection, dynsymBytes, dynsymSection.sh_size))
	{
		free(stringBytes);
		free(dynsymBytes);
		return 0;
	}

	int relaNum = 0;
	Elf64_Shdr relaSection;
	int bufferIndex = 0;
	while(getSectionHeaderByType64(filePath, SHT_RELA, relaNum, &relaSection)) // going through all rela sections
	{
		char* relaBytes = (char*)malloc(relaSection.sh_size);
		if(!readSectionBytes64(filePath, &relaSection, relaBytes, relaSection.sh_size))
		{
			free(stringBytes);
			free(dynsymBytes);
			free(relaBytes);
			return 0;
		}

		int i = 0;
		while((i * sizeof(Elf64_Rela)) < relaSection.sh_size && bufferIndex < bufferLen)
		{
			Elf64_Rela* rela = (Elf64_Rela*)(relaBytes + (i * sizeof(Elf64_Rela)));
			int val = ELF64_R_SYM(rela->r_info);
			Elf64_Sym* symbol = (Elf64_Sym*)(dynsymBytes + (val * sizeof(Elf64_Sym)));

			if(strcmp(stringBytes + symbol->st_name, "") != 0)
			{
				buffer[bufferIndex].name = initializeJdcStrWithVal(stringBytes + symbol->st_name);
				buffer[bufferIndex].address = (unsigned long long)(rela->r_offset);
				bufferIndex++;
			}

			i++;
		}

		free(relaBytes);

		relaNum++;
	}

	free(stringBytes);
	free(dynsymBytes);

	return bufferIndex;
}

int getAllELFImports32(const char* filePath, struct ImportedFunction* buffer, int bufferLen)
{
	Elf32_Shdr dynstrSection;
	if(!getSectionHeaderByType32(filePath, SHT_STRTAB, 0, &dynstrSection))
	{
		return 0;
	}

	char* stringBytes = (char*)malloc(dynstrSection.sh_size);
	if(!readSectionBytes32(filePath, &dynstrSection, stringBytes, dynstrSection.sh_size))
	{
		free(stringBytes);
		return 0;
	}

	Elf32_Shdr dynsymSection;
	if(!getSectionHeaderByType32(filePath, SHT_DYNSYM, 0, &dynsymSection))
	{
		free(stringBytes);
		return 0;
	}

	char* dynsymBytes = (char*)malloc(dynsymSection.sh_size);
	if(!readSectionBytes32(filePath, &dynsymSection, dynsymBytes, dynsymSection.sh_size))
	{
		free(stringBytes);
		free(dynsymBytes);
		return 0;
	}

	int relaNum = 0;
	Elf32_Shdr relaSection;
	int bufferIndex = 0;
	while(getSectionHeaderByType32(filePath, SHT_RELA, relaNum, &relaSection)) // going through all rela sections
	{
		char* relaBytes = (char*)malloc(relaSection.sh_size);
		if(!readSectionBytes32(filePath, &relaSection, relaBytes, relaSection.sh_size))
		{
			free(stringBytes);
			free(dynsymBytes);
			free(relaBytes);
			return 0;
		}

		int i = 0;
		while((i * sizeof(Elf32_Rela)) < relaSection.sh_size && bufferIndex < bufferLen)
		{
			Elf32_Rela* rela = (Elf32_Rela*)(relaBytes + (i * sizeof(Elf32_Rela)));
			int val = ELF32_R_SYM(rela->r_info);
			Elf32_Sym* symbol = (Elf32_Sym*)(dynsymBytes + (val * sizeof(Elf32_Sym)));

			if(strcmp(stringBytes + symbol->st_name, "") != 0)
			{
				buffer[bufferIndex].name = initializeJdcStrWithVal(stringBytes + symbol->st_name);
				buffer[bufferIndex].address = (unsigned long long)(rela->r_offset);
				bufferIndex++;
			}

			i++;
		}

		free(relaBytes);

		relaNum++;
	}

	free(stringBytes);
	free(dynsymBytes);

	return bufferIndex;
}

unsigned char generateELFHeadersInfoStr(const char* filePath, struct JdcStr* result)
{
	FILE* file = fopen(filePath, "r");
	if (file)
	{
		Elf64_Ehdr elfHeader;
		fread(&elfHeader, sizeof(elfHeader), 1, file);

		generateELFHeaderInfoStr(&elfHeader, result);

		fclose(file);
		return 1;
	}

	return 0;
}

static void generateELFHeaderInfoStr(Elf64_Ehdr* ehdr, struct JdcStr* result)
{
	unsigned char x64 = ehdr->e_ident[EI_CLASS] == ELFCLASS64;
	if(x64)
	{
		strcatJdc(result, "Elf64_Ehdr\n\n");
	}
	else
	{
		strcatJdc(result, "Elf32_Ehdr\n\n");
	}
	
	sprintfJdc(result, 1, "0x0\te_ident[0-4]\t0x%llX\tMagic number\n", *(unsigned int*)(ehdr->e_ident));

	sprintfJdc(result, 1, "0x4\te_ident[EI_CLASS]\t");
	switch(ehdr->e_ident[EI_CLASS])
	{
	case ELFCLASSNONE:
		sprintfJdc(result, 1, "ELFCLASSNONE (0x%X)\tThis class is invalid\n", ehdr->e_ident[EI_CLASS]);
		break;
	case ELFCLASS32:
		sprintfJdc(result, 1, "ELFCLASS32 (0x%X)\t32-bit architecture\n", ehdr->e_ident[EI_CLASS]);
		break;
	case ELFCLASS64:
		sprintfJdc(result, 1, "ELFCLASS64 (0x%X)\t64-bit architecture\n", ehdr->e_ident[EI_CLASS]);
		break;
	}

	sprintfJdc(result, 1, "0x5\te_ident[EI_DATA]\t");
	switch(ehdr->e_ident[EI_DATA])
	{
	case ELFDATANONE:
		sprintfJdc(result, 1, "ELFDATANONE (0x%X)\tUnknown data format\n", ehdr->e_ident[EI_DATA]);
		break;
	case ELFDATA2LSB:
		sprintfJdc(result, 1, "ELFDATA2LSB (0x%X)\tTwo's complement, little-endian\n", ehdr->e_ident[EI_DATA]);
		break;
	case ELFDATA2MSB:
		sprintfJdc(result, 1, "ELFDATA2MSB (0x%X)\tTwo's complement, big-endian\n", ehdr->e_ident[EI_DATA]);
		break;
	}

	sprintfJdc(result, 1, "0x6\te_ident[EI_VERSION]\t");
	switch(ehdr->e_ident[EI_VERSION])
	{
	case EV_NONE:
		sprintfJdc(result, 1, "EV_NONE (0x%X)\tInvalid version\n", ehdr->e_ident[EI_VERSION]);
		break;
	case EV_CURRENT:
		sprintfJdc(result, 1, "EV_CURRENT (0x%X)\tCurrent version\n", ehdr->e_ident[EI_VERSION]);
		break;
	}

	sprintfJdc(result, 1, "0x7\te_ident[EI_OSABI]\t");
	switch(ehdr->e_ident[EI_OSABI])
	{
	case ELFOSABI_SYSV:
		sprintfJdc(result, 1, "ELFOSABI_SYSV (0x%X)\tUNIX System V ABI\n", ehdr->e_ident[EI_OSABI]);
		break;
	case ELFOSABI_HPUX:
		sprintfJdc(result, 1, "ELFOSABI_HPUX (0x%X)\tHP-UX ABI\n", ehdr->e_ident[EI_OSABI]);
		break;
	case ELFOSABI_NETBSD:
		sprintfJdc(result, 1, "ELFOSABI_NETBSD (0x%X)\tNetBSD ABI\n", ehdr->e_ident[EI_OSABI]);
		break;
	case ELFOSABI_LINUX:
		sprintfJdc(result, 1, "ELFOSABI_LINUX (0x%X)\tLinux ABI\n", ehdr->e_ident[EI_OSABI]);
		break;
	case ELFOSABI_SOLARIS:
		sprintfJdc(result, 1, "ELFOSABI_SOLARIS (0x%X)\tSolaris ABI\n", ehdr->e_ident[EI_OSABI]);
		break;
	case ELFOSABI_IRIX:
		sprintfJdc(result, 1, "ELFOSABI_IRIX (0x%X)\tIRIX ABI\n", ehdr->e_ident[EI_OSABI]);
		break;
	case ELFOSABI_FREEBSD:
		sprintfJdc(result, 1, "ELFOSABI_FREEBSD (0x%X)\tFreeBSD ABI\n", ehdr->e_ident[EI_OSABI]);
		break;
	case ELFOSABI_TRU64:
		sprintfJdc(result, 1, "ELFOSABI_TRU64 (0x%X)\tTRU64 UNIX ABI\n", ehdr->e_ident[EI_OSABI]);
		break;
	case ELFOSABI_ARM:
		sprintfJdc(result, 1, "ELFOSABI_ARM (0x%X)\tARM architecture ABI\n", ehdr->e_ident[EI_OSABI]);
		break;
	case ELFOSABI_STANDALONE:
		sprintfJdc(result, 1, "ELFOSABI_STANDALONE (0x%X)\tStand-alone (embedded) ABI\n", ehdr->e_ident[EI_OSABI]);
		break;
	}

	sprintfJdc(result, 1, "0x8\te_ident[EI_ABIVERSION]\t0x%llX\tVersion of the ABI to which the object is targeted\n", ehdr->e_ident[EI_ABIVERSION]);
	sprintfJdc(result, 1, "0x9\te_ident[EI_PAD]\t");
	for (int i = EI_PAD; i < EI_NIDENT; i++)
	{
		sprintfJdc(result, 1, "0x%llX", ehdr->e_ident[i]);
		if (i != EI_NIDENT - 1)
		{
			strcatJdc(result, ", ");
		}
		else
		{
			strcatJdc(result, "\tReserved padding bytes\n");
		}
	}

	sprintfJdc(result, 1, "0x10\te_type\t");
	switch(ehdr->e_type)
	{
	case ET_NONE:
		sprintfJdc(result, 1, "ET_NONE (0x%llX)\tUnknown type\n", ehdr->e_type);
		break;
	case ET_REL:
		sprintfJdc(result, 1, "ET_REL (0x%llX)\tRelocatable file\n", ehdr->e_type);
		break;
	case ET_EXEC:
		sprintfJdc(result, 1, "ET_EXEC (0x%llX)\tExecutable file\n", ehdr->e_type);
		break;
	case ET_DYN:
		sprintfJdc(result, 1, "ET_DYN (0x%llX)\tShared object\n", ehdr->e_type);
		break;
	case ET_CORE:
		sprintfJdc(result, 1, "ET_CORE (0x%llX)\tCore file\n", ehdr->e_type);
		break;
	}

	sprintfJdc(result, 1, "0x12\te_machine\t");
	switch(ehdr->e_machine)
	{
	case EM_NONE:
		sprintfJdc(result, 1, "EM_NONE (0x%llX)\tUnknown machine\n", ehdr->e_machine);
		break;
	case EM_M32:
		sprintfJdc(result, 1, "EM_M32 (0x%llX)\tAT&T WE 32100\n", ehdr->e_machine);
		break;
	case EM_SPARC:
		sprintfJdc(result, 1, "EM_SPARC (0x%llX)\tSun Microsystems SPARC\n", ehdr->e_machine);
		break;
	case EM_386:
		sprintfJdc(result, 1, "EM_386 (0x%llX)\tIntel 80386\n", ehdr->e_machine);
		break;
	case EM_68K:
		sprintfJdc(result, 1, "EM_68K (0x%llX)\tMotorola 68000\n", ehdr->e_machine);
		break;
	case EM_88K:
		sprintfJdc(result, 1, "EM_88K (0x%llX)\tMotorola 88000\n", ehdr->e_machine);
		break;
	case EM_860:
		sprintfJdc(result, 1, "EM_860 (0x%llX)\tIntel 80860\n", ehdr->e_machine);
		break;
	case EM_MIPS:
		sprintfJdc(result, 1, "EM_MIPS (0x%llX)\tMIPS RS3000 (big-endian only)\n", ehdr->e_machine);
		break;
	case EM_PARISC:
		sprintfJdc(result, 1, "EM_PARISC (0x%llX)\tHP/PA\n", ehdr->e_machine);
		break;
	case EM_SPARC32PLUS:
		sprintfJdc(result, 1, "EM_SPARC32PLUS (0x%llX)\tSPARC with enhanced instruction set\n", ehdr->e_machine);
		break;
	case EM_PPC:
		sprintfJdc(result, 1, "EM_PPC (0x%llX)\tPowerPC\n", ehdr->e_machine);
		break;
	case EM_PPC64:
		sprintfJdc(result, 1, "EM_PPC64 (0x%llX)\tPowerPC 64-bit\n", ehdr->e_machine);
		break;
	case EM_S390:
		sprintfJdc(result, 1, "EM_S390 (0x%llX)\tIBM S/390\n", ehdr->e_machine);
		break;
	case EM_ARM:
		sprintfJdc(result, 1, "EM_ARM (0x%llX)\tAdvanced RISC Machines\n", ehdr->e_machine);
		break;
	case EM_SH:
		sprintfJdc(result, 1, "EM_SH (0x%llX)\tRenesas SuperH\n", ehdr->e_machine);
		break;
	case EM_SPARCV9:
		sprintfJdc(result, 1, "EM_SPARCV9 (0x%llX)\tSPARC v9 64-bit\n", ehdr->e_machine);
		break;
	case EM_IA_64:
		sprintfJdc(result, 1, "EM_IA_64 (0x%llX)\tIntel Itanium\n", ehdr->e_machine);
		break;
	case EM_X86_64:
		sprintfJdc(result, 1, "EM_X86_64 (0x%llX)\tAMD x86-64\n", ehdr->e_machine);
		break;
	case EM_VAX:
		sprintfJdc(result, 1, "EM_VAX (0x%llX)\tDEC Vax\n", ehdr->e_machine);
		break;
	}

	sprintfJdc(result, 1, "0x14\te_version\t");
	switch(ehdr->e_version)
	{
	case EV_NONE:
		sprintfJdc(result, 1, "EV_NONE (0x%llX)\tInvalid version\n", ehdr->e_version);
		break;
	case EV_CURRENT:
		sprintfJdc(result, 1, "EV_CURRENT (0x%llX)\tCurrent version\n", ehdr->e_version);
		break;
	}

	if(x64)
	{
		sprintfJdc(result, 1, "0x18\te_entry\t0x%llX\tVirtual address to which the system first transfers control\n", ehdr->e_entry);
		sprintfJdc(result, 1, "0x20\te_phoff\t0x%llX\tProgram header table's file offset in bytes\n", ehdr->e_phoff);
		sprintfJdc(result, 1, "0x28\te_shoff\t0x%llX\tSection header table's file offset in bytes\n", ehdr->e_shoff);
		sprintfJdc(result, 1, "0x30\te_flags\t0x%llX\tProcessor-specific flags associated with the file\n", ehdr->e_flags);
		sprintfJdc(result, 1, "0x34\te_ehsize\t0x%llX\tELF header's size in bytes\n", ehdr->e_ehsize);
		sprintfJdc(result, 1, "0x36\te_phentsize\t0x%llX\tSize in bytes of one entry in the file's program header table\n", ehdr->e_phentsize);
		sprintfJdc(result, 1, "0x38\te_phnum\t%d\tNumber of entries in the program header table\n", ehdr->e_phnum);
		sprintfJdc(result, 1, "0x3A\te_shentsize\t0x%llX\tA sections header's size in bytes\n", ehdr->e_shentsize);
		sprintfJdc(result, 1, "0x3C\te_shnum\t%d\tNumber of entries in the section header table\n", ehdr->e_shnum);
		sprintfJdc(result, 1, "0x3E\te_shstrndx\t0x%llX\tSection header table index of the entry associated with the section name string table\n", ehdr->e_shstrndx);
	}
	else
	{
		sprintfJdc(result, 1, "0x18\te_entry\t0x%llX\tVirtual address to which the system first transfers control\n", ((Elf32_Ehdr*)ehdr)->e_entry);
		sprintfJdc(result, 1, "0x1C\te_phoff\t0x%llX\tProgram header table's file offset in bytes\n", ((Elf32_Ehdr*)ehdr)->e_phoff);
		sprintfJdc(result, 1, "0x20\te_shoff\t0x%llX\tSection header table's file offset in bytes\n", ((Elf32_Ehdr*)ehdr)->e_shoff);
		sprintfJdc(result, 1, "0x24\te_flags\t0x%llX\tProcessor-specific flags associated with the file\n", ((Elf32_Ehdr*)ehdr)->e_flags);
		sprintfJdc(result, 1, "0x28\te_ehsize\t0x%llX\tELF header's size in bytes\n", ((Elf32_Ehdr*)ehdr)->e_ehsize);
		sprintfJdc(result, 1, "0x2A\te_phentsize\t0x%llX\tSize in bytes of one entry in the file's program header table\n", ((Elf32_Ehdr*)ehdr)->e_phentsize);
		sprintfJdc(result, 1, "0x2C\te_phnum\t%d\tNumber of entries in the program header table\n", ((Elf32_Ehdr*)ehdr)->e_phnum);
		sprintfJdc(result, 1, "0x2E\te_shentsize\t0x%llX\tA sections header's size in bytes\n", ((Elf32_Ehdr*)ehdr)->e_shentsize);
		sprintfJdc(result, 1, "0x30\te_shnum\t%d\tNumber of entries in the section header table\n", ((Elf32_Ehdr*)ehdr)->e_shnum);
		sprintfJdc(result, 1, "0x32\te_shstrndx\t0x%llX\tSection header table index of the entry associated with the section name string table\n", ((Elf32_Ehdr*)ehdr)->e_shstrndx);
	}
}