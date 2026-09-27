#include "elfHandler.h"
#include "../file-handler/fileHandler.h"

enum JdcStatus isFileELF(const wchar_t* filePath, bool* isELFRef)
{
	FILE* file = openFile(filePath);
	if (file)
	{
		uint8_t e_ident[5];
		fread(e_ident, 1, 5, file);
		fclose(file);

		*isELFRef = memcmp(e_ident, ELFMAG, SELFMAG) == 0;
		return SUCCESS_JDC;
	}

	return ERROR_JDC;
}

enum JdcStatus isELFX64(const wchar_t* filePath, bool* is64BitRef)
{
	FILE* file = openFile(filePath);
	if(file)
	{
		uint8_t e_ident[5];
		fread(e_ident, 1, 5, file);
		fclose(file);

		*is64BitRef = e_ident[EI_CLASS] == ELFCLASS64;
		return SUCCESS_JDC;
	}

	return ERROR_JDC;
}

static enum JdcStatus readElfEhdr(FILE* file, bool is64Bit, Elf64_Ehdr* result)
{
	if(!file)
	{
		return ERROR_JDC;
	}

	if(is64Bit)
	{
		fread(result, sizeof(Elf64_Ehdr), 1, file);
	}
	else
	{
		Elf32_Ehdr elf32Ehdr;
		fread(&elf32Ehdr, sizeof(elf32Ehdr), 1, file);

		memcpy(result, &elf32Ehdr, EI_NIDENT + 8);
		result->e_entry = elf32Ehdr.e_entry; // only these 3 fields are different between 32/64 bit versions
		result->e_phoff = elf32Ehdr.e_phoff;
		result->e_shoff = elf32Ehdr.e_shoff;
		memcpy(&(result->e_flags), &(elf32Ehdr.e_flags), 16);
	}

	return SUCCESS_JDC;
}

static enum JdcStatus readElfShdr(FILE* file, bool is64Bit, uint64_t fileOffset, Elf64_Shdr* result)
{
	if(!file)
	{
		return ERROR_JDC;
	}

	seek64(file, fileOffset);

	if(is64Bit)
	{
		fread(result, sizeof(Elf64_Shdr), 1, file);
	}
	else
	{
		Elf32_Shdr elf32Shdr;
		fread(&elf32Shdr, sizeof(elf32Shdr), 1, file);

		result->sh_name = elf32Shdr.sh_name;
		result->sh_type = elf32Shdr.sh_type;
		result->sh_flags = elf32Shdr.sh_flags;
		result->sh_addr = elf32Shdr.sh_addr;
		result->sh_offset = elf32Shdr.sh_offset;
		result->sh_size = elf32Shdr.sh_size;
		result->sh_link = elf32Shdr.sh_link;
		result->sh_info = elf32Shdr.sh_info;
		result->sh_addralign = elf32Shdr.sh_addralign;
		result->sh_entsize = elf32Shdr.sh_entsize;
	}

	return SUCCESS_JDC;
}

enum JdcStatus getELFEntryPoint(const wchar_t* filePath, bool is64Bit, uint64_t* entryPointRef)
{
	FILE* file = openFile(filePath);
	if (file && entryPointRef)
	{
		Elf64_Ehdr elfHeader;
		readElfEhdr(file, is64Bit, &elfHeader);
		fclose(file);

		*entryPointRef = elfHeader.e_entry;
		return SUCCESS_JDC;
	}

	return ERROR_JDC;
}

enum JdcStatus getELFSymbolByValue(const wchar_t* filePath, bool is64Bit, uint64_t value, struct JdcStr* result)
{
	Elf64_Shdr strtabSection;
	if(ERROR_JDC == getSectionHeaderByName(filePath, is64Bit, ".strtab", &strtabSection))
	{
		return ERROR_JDC;
	}

	uint8_t* stringBytes = (uint8_t*)malloc(strtabSection.sh_size);
	if(ERROR_JDC == readSectionBytes(filePath, &strtabSection, stringBytes, strtabSection.sh_size))
	{
		free(stringBytes);
		return ERROR_JDC;
	}

	Elf64_Shdr symtabSection;
	if(ERROR_JDC == getSectionHeaderByName(filePath, is64Bit, ".symtab", &symtabSection))
	{
		return ERROR_JDC;
	}

	uint8_t* bytes = (uint8_t*)malloc(symtabSection.sh_size);
	if(ERROR_JDC == readSectionBytes(filePath, &symtabSection, bytes, symtabSection.sh_size))
	{
		free(bytes);
		return ERROR_JDC;
	}

	int32_t i = 0;
	while(i < symtabSection.sh_size)
	{
		uint32_t st_name = 0;
		uint64_t st_value = 0;
		if(is64Bit)
		{
			Elf64_Sym* symbol = (Elf64_Sym*)(bytes + i);
			if(symbol)
			{
				st_name = symbol->st_name;
				st_value = symbol->st_value;
			}
		}
		else
		{
			Elf32_Sym* symbol = (Elf32_Sym*)(bytes + i);
			if (symbol) 
			{
				st_name = symbol->st_name;
				st_value = symbol->st_value;
			}
		}
		
		if(st_value == value && (stringBytes + st_name)[0] != 0)
		{
			if (ERROR_JDC == demangleCppSymbol(stringBytes + st_name, result->buffer, result->bufferSize))
			{
				strcpyJdc(result, stringBytes + st_name);
			}

			free(stringBytes);
			free(bytes);
			return SUCCESS_JDC;
		}

		i += is64Bit ? sizeof(Elf64_Sym) : sizeof(Elf32_Sym);
	}
	
	free(stringBytes);
	free(bytes);
	return ERROR_JDC;
}

enum JdcStatus getNumOfELFSections(const wchar_t* filePath, bool is64Bit, int32_t* numOfSectionsRef)
{
	FILE* file = openFile(filePath);
	if (file && numOfSectionsRef)
	{
		Elf64_Ehdr elfHeader;
		readElfEhdr(file, is64Bit, &elfHeader);

		Elf64_Shdr sectionHeader;
		uint32_t shdrSize = is64Bit ? sizeof(Elf64_Shdr) : sizeof(Elf32_Shdr);

		int32_t result = 0;
		for (int32_t i = 0; i < elfHeader.e_shnum; i++)
		{
			readElfShdr(file, is64Bit, elfHeader.e_shoff + i * shdrSize, &sectionHeader);

			if(sectionHeader.sh_size == 0)
			{
				continue;
			}

			result++;
		}

		fclose(file);

		*numOfSectionsRef = result;
		return SUCCESS_JDC;
	}

	return ERROR_JDC;
}

enum JdcStatus getAllELFSectionHeaders(const wchar_t* filePath, bool is64Bit, struct FileSection* buffer, int32_t bufferLen)
{
	Elf64_Ehdr elfHeader;
	Elf64_Shdr sectionHeader;
	FILE* file = openFile(filePath);

	if (file)
	{
		readElfEhdr(file, is64Bit, &elfHeader);

		uint32_t shdrSize = is64Bit ? sizeof(Elf64_Shdr) : sizeof(Elf32_Shdr);

		Elf64_Shdr nameStrTable;
		readElfShdr(file, is64Bit, elfHeader.e_shoff + elfHeader.e_shstrndx * shdrSize, &nameStrTable);

		uint8_t* sectionNames = (uint8_t*)malloc(nameStrTable.sh_size);
		if (!sectionNames) 
		{
			return ERROR_JDC;
		}

		seek64(file, nameStrTable.sh_offset);
		fread(sectionNames, 1, nameStrTable.sh_size, file);

		int32_t bufferIndex = 0;
		for (int32_t i = 0; i < elfHeader.e_shnum; i++)
		{
			if (bufferIndex >= bufferLen)
			{
				fclose(file);
				free(sectionNames);
				return ERROR_JDC;
			}

			readElfShdr(file, is64Bit, elfHeader.e_shoff + i * shdrSize, &sectionHeader);

			if (sectionHeader.sh_size == 0)
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
			buffer[bufferIndex].rva = sectionHeader.sh_addr;
			buffer[bufferIndex].fileOffset = sectionHeader.sh_offset;
			buffer[bufferIndex].physicalSize = sectionHeader.sh_size;
			bufferIndex++;
		}

		fclose(file);
		free(sectionNames);
		return SUCCESS_JDC;
	}

	return ERROR_JDC;
}

enum JdcStatus getSectionHeaderByName(const wchar_t* filePath, bool is64Bit, const char* name, Elf64_Shdr* result)
{
	Elf64_Ehdr elfHeader;
	Elf64_Shdr sectionHeader;
	FILE* file = openFile(filePath);

	if (file)
	{
		readElfEhdr(file, is64Bit, &elfHeader);

		uint32_t shdrSize = is64Bit ? sizeof(Elf64_Shdr) : sizeof(Elf32_Shdr);

		Elf64_Shdr nameStrTable;
		readElfShdr(file, is64Bit, elfHeader.e_shoff + elfHeader.e_shstrndx * shdrSize, &nameStrTable);

		uint8_t* sectionNames = (uint8_t*)malloc(nameStrTable.sh_size);
		if (!sectionNames) 
		{
			return ERROR_JDC;
		}

		seek64(file, nameStrTable.sh_offset);
		fread(sectionNames, 1, nameStrTable.sh_size, file);

		for (int32_t i = 0; i < elfHeader.e_shnum; i++)
		{
			readElfShdr(file, is64Bit, elfHeader.e_shoff + i * shdrSize, &sectionHeader);
			if (strcmp(sectionNames + sectionHeader.sh_name, name) == 0)
			{
				*result = sectionHeader;

				fclose(file);
				free(sectionNames);
				return SUCCESS_JDC;
			}
		}

		fclose(file);
		free(sectionNames);
	}

	return ERROR_JDC;
}

enum JdcStatus readSectionBytes(const wchar_t* filePath, Elf64_Shdr* section, uint8_t* buffer, uint64_t bufferSize)
{
	FILE* file = openFile(filePath);
	if (file)
	{
		seek64(file, section->sh_offset);
		fread(buffer, 1, bufferSize, file);
		fclose(file);
		return SUCCESS_JDC;
	}
	
	return ERROR_JDC;
}

enum JdcStatus getSectionHeaderByType(const wchar_t* filePath, bool is64Bit, uint32_t type, int32_t index, Elf64_Shdr* result)
{
	Elf64_Ehdr elfHeader;
	Elf64_Shdr sectionHeader;
	FILE* file = openFile(filePath);

	if (file)
	{
		readElfEhdr(file, is64Bit, &elfHeader);

		uint32_t shdrSize = is64Bit ? sizeof(Elf64_Shdr) : sizeof(Elf32_Shdr);
		int32_t num = 0;
		for (int32_t i = 0; i < elfHeader.e_shnum; i++)
		{
			readElfShdr(file, is64Bit, elfHeader.e_shoff + i * shdrSize, &sectionHeader);
			if (sectionHeader.sh_type == type)
			{
				if (num == index)
				{
					*result = sectionHeader;
					fclose(file);
					return SUCCESS_JDC;
				}

				num++;
			}
		}

		fclose(file);
	}

	return ERROR_JDC;
}

enum JdcStatus getNumOfELFImports(const wchar_t* filePath, bool is64Bit, int32_t* numOfImportsRef, int32_t* numOfLibrariesRef)
{
	if(!numOfImportsRef || !numOfLibrariesRef)
	{
		return ERROR_JDC;
	}
	
	*numOfLibrariesRef = 1; // this is just the .dynsym section
	
	Elf64_Shdr dynstrSection;
	if (ERROR_JDC == getSectionHeaderByType(filePath, is64Bit, SHT_STRTAB, 0, &dynstrSection))
	{
		return ERROR_JDC;
	}

	uint8_t* stringBytes = (uint8_t*)malloc(dynstrSection.sh_size);
	if (ERROR_JDC == readSectionBytes(filePath, &dynstrSection, stringBytes, dynstrSection.sh_size))
	{
		free(stringBytes);
		return ERROR_JDC;
	}

	Elf64_Shdr dynsymSection;
	if (ERROR_JDC == getSectionHeaderByType(filePath, is64Bit, SHT_DYNSYM, 0, &dynsymSection))
	{
		free(stringBytes);
		return ERROR_JDC;
	}

	uint8_t* dynsymBytes = (uint8_t*)malloc(dynsymSection.sh_size);
	if (ERROR_JDC == readSectionBytes(filePath, &dynsymSection, dynsymBytes, dynsymSection.sh_size))
	{
		free(stringBytes);
		free(dynsymBytes);
		return ERROR_JDC;
	}

	int32_t relaNum = 0;
	Elf64_Shdr relaSection;
	int32_t result = 0;
	while (getSectionHeaderByType(filePath, is64Bit, SHT_RELA, relaNum, &relaSection)) // going through all rela sections
	{
		uint8_t* relaBytes = (uint8_t*)malloc(relaSection.sh_size);
		if (ERROR_JDC == readSectionBytes(filePath, &relaSection, relaBytes, relaSection.sh_size))
		{
			free(stringBytes);
			free(dynsymBytes);
			free(relaBytes);
			return ERROR_JDC;
		}

		uint32_t relaSize = is64Bit ? sizeof(Elf64_Rela) : sizeof(Elf32_Rela);

		int32_t i = 0;
		while ((i * relaSize) < relaSection.sh_size)
		{
			uint32_t st_name = 0;
			if(is64Bit)
			{
				Elf64_Rela* rela = (Elf64_Rela*)(relaBytes + (i * sizeof(Elf64_Rela)));
				if (rela) 
				{
					int32_t val = ELF64_R_SYM(rela->r_info);
					Elf64_Sym* symbol = (Elf64_Sym*)(dynsymBytes + (val * sizeof(Elf64_Sym)));
					st_name = symbol->st_name;
				}
			}
			else
			{
				Elf32_Rela* rela = (Elf32_Rela*)(relaBytes + (i * sizeof(Elf32_Rela)));
				if (rela) 
				{
					int32_t val = ELF32_R_SYM(rela->r_info);
					Elf32_Sym* symbol = (Elf32_Sym*)(dynsymBytes + (val * sizeof(Elf32_Sym)));
					st_name = symbol->st_name;
				}
			}
			
			if(strcmp(stringBytes + st_name, "") != 0)
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

	*numOfImportsRef = result;
	return SUCCESS_JDC;
}

enum JdcStatus getAllELFImports(const wchar_t* filePath, bool is64Bit, struct ImportedFunction* importsBuffer, int32_t importsBufferLen, struct JdcStr* libraryNamesBuffer, int32_t libraryNamesBufferLen)
{
	if (libraryNamesBufferLen > 0) 
	{
		libraryNamesBuffer[0] = initializeJdcStrWithVal(".dynsym");
	}
	
	Elf64_Shdr dynstrSection;
	if(ERROR_JDC == getSectionHeaderByType(filePath, is64Bit, SHT_STRTAB, 0, &dynstrSection))
	{
		return ERROR_JDC;
	}

	uint8_t* stringBytes = (uint8_t*)malloc(dynstrSection.sh_size);
	if(ERROR_JDC == readSectionBytes(filePath, &dynstrSection, stringBytes, dynstrSection.sh_size))
	{
		free(stringBytes);
		return ERROR_JDC;
	}

	Elf64_Shdr dynsymSection;
	if(ERROR_JDC == getSectionHeaderByType(filePath, is64Bit, SHT_DYNSYM, 0, &dynsymSection))
	{
		free(stringBytes);
		return ERROR_JDC;
	}

	uint8_t* dynsymBytes = (uint8_t*)malloc(dynsymSection.sh_size);
	if(ERROR_JDC == readSectionBytes(filePath, &dynsymSection, dynsymBytes, dynsymSection.sh_size))
	{
		free(stringBytes);
		free(dynsymBytes);
		return ERROR_JDC;
	}

	int32_t relaNum = 0;
	Elf64_Shdr relaSection;
	int32_t importsIndex = 0;
	while(getSectionHeaderByType(filePath, is64Bit, SHT_RELA, relaNum, &relaSection)) // going through all rela sections
	{
		uint8_t* relaBytes = (uint8_t*)malloc(relaSection.sh_size);
		if(ERROR_JDC == readSectionBytes(filePath, &relaSection, relaBytes, relaSection.sh_size))
		{
			free(stringBytes);
			free(dynsymBytes);
			free(relaBytes);
			return ERROR_JDC;
		}

		uint32_t relaSize = is64Bit ? sizeof(Elf64_Rela) : sizeof(Elf32_Rela);

		int32_t i = 0;
		while((i * relaSize) < relaSection.sh_size && importsIndex < importsBufferLen)
		{
			uint32_t st_name = 0;
			uint64_t r_offset = 0;
			if(is64Bit)
			{
				Elf64_Rela* rela = (Elf64_Rela*)(relaBytes + (i * sizeof(Elf64_Rela)));
				int32_t val = ELF64_R_SYM(rela->r_info);
				Elf64_Sym* symbol = (Elf64_Sym*)(dynsymBytes + (val * sizeof(Elf64_Sym)));
				st_name = symbol->st_name;
				r_offset = rela->r_offset;
			}
			else
			{
				Elf32_Rela* rela = (Elf32_Rela*)(relaBytes + (i * sizeof(Elf32_Rela)));
				int32_t val = ELF32_R_SYM(rela->r_info);
				Elf32_Sym* symbol = (Elf32_Sym*)(dynsymBytes + (val * sizeof(Elf32_Sym)));
				st_name = symbol->st_name;
				r_offset = rela->r_offset;
			}

			if(strcmp(stringBytes + st_name, "") != 0)
			{
				importsBuffer[importsIndex].name = initializeJdcStrWithSize(255);
				if (!demangleCppSymbol(stringBytes + st_name, importsBuffer[importsIndex].name.buffer, 255))
				{
					strcpyJdc(&importsBuffer[importsIndex].name, stringBytes + st_name);
				}
				
				importsBuffer[importsIndex].address = r_offset;
				importsBuffer[importsIndex].libraryNameIndex = 0;
				importsIndex++;
			}

			i++;
		}

		free(relaBytes);

		relaNum++;
	}

	free(stringBytes);
	free(dynsymBytes);
	return SUCCESS_JDC;
}

enum JdcStatus generateELFHeadersInfoStr(const wchar_t* filePath, struct JdcStr* result)
{
	FILE* file = openFile(filePath);
	if (file)
	{
		Elf64_Ehdr elfHeader;
		fread(&elfHeader, sizeof(elfHeader), 1, file);

		generateELFHeaderInfoStr(&elfHeader, result);

		fclose(file);
		return SUCCESS_JDC;
	}

	return ERROR_JDC;
}

static void generateELFHeaderInfoStr(Elf64_Ehdr* ehdr, struct JdcStr* result)
{
	bool x64 = ehdr->e_ident[EI_CLASS] == ELFCLASS64;
	if(x64)
	{
		strcatJdc(result, "Elf64_Ehdr\n\n");
	}
	else
	{
		strcatJdc(result, "Elf32_Ehdr\n\n");
	}
	
	sprintfJdc(result, true, "0x0\te_ident[0-4]\t0x%llX\tMagic number\n", *(uint32_t*)(ehdr->e_ident));

	sprintfJdc(result, true, "0x4\te_ident[EI_CLASS]\t");
	switch(ehdr->e_ident[EI_CLASS])
	{
	case ELFCLASSNONE:
		sprintfJdc(result, true, "ELFCLASSNONE (0x%X)\tThis class is invalid\n", ehdr->e_ident[EI_CLASS]);
		break;
	case ELFCLASS32:
		sprintfJdc(result, true, "ELFCLASS32 (0x%X)\t32-bit architecture\n", ehdr->e_ident[EI_CLASS]);
		break;
	case ELFCLASS64:
		sprintfJdc(result, true, "ELFCLASS64 (0x%X)\t64-bit architecture\n", ehdr->e_ident[EI_CLASS]);
		break;
	}

	sprintfJdc(result, true, "0x5\te_ident[EI_DATA]\t");
	switch(ehdr->e_ident[EI_DATA])
	{
	case ELFDATANONE:
		sprintfJdc(result, true, "ELFDATANONE (0x%X)\tUnknown data format\n", ehdr->e_ident[EI_DATA]);
		break;
	case ELFDATA2LSB:
		sprintfJdc(result, true, "ELFDATA2LSB (0x%X)\tTwo's complement, little-endian\n", ehdr->e_ident[EI_DATA]);
		break;
	case ELFDATA2MSB:
		sprintfJdc(result, true, "ELFDATA2MSB (0x%X)\tTwo's complement, big-endian\n", ehdr->e_ident[EI_DATA]);
		break;
	}

	sprintfJdc(result, true, "0x6\te_ident[EI_VERSION]\t");
	switch(ehdr->e_ident[EI_VERSION])
	{
	case EV_NONE:
		sprintfJdc(result, true, "EV_NONE (0x%X)\tInvalid version\n", ehdr->e_ident[EI_VERSION]);
		break;
	case EV_CURRENT:
		sprintfJdc(result, true, "EV_CURRENT (0x%X)\tCurrent version\n", ehdr->e_ident[EI_VERSION]);
		break;
	}

	sprintfJdc(result, true, "0x7\te_ident[EI_OSABI]\t");
	switch(ehdr->e_ident[EI_OSABI])
	{
	case ELFOSABI_NONE:
		sprintfJdc(result, true, "ELFOSABI_NONE (0x%X)\tNo extensions or unspecified\n", ehdr->e_ident[EI_OSABI]);
		break;
	case ELFOSABI_HPUX:
		sprintfJdc(result, true, "ELFOSABI_HPUX (0x%X)\tHP-UX ABI\n", ehdr->e_ident[EI_OSABI]);
		break;
	case ELFOSABI_NETBSD:
		sprintfJdc(result, true, "ELFOSABI_NETBSD (0x%X)\tNetBSD ABI\n", ehdr->e_ident[EI_OSABI]);
		break;
	case ELFOSABI_LINUX:
		sprintfJdc(result, true, "ELFOSABI_LINUX (0x%X)\tLinux ABI\n", ehdr->e_ident[EI_OSABI]);
		break;
	case ELFOSABI_SOLARIS:
		sprintfJdc(result, true, "ELFOSABI_SOLARIS (0x%X)\tSolaris ABI\n", ehdr->e_ident[EI_OSABI]);
		break;
	case ELFOSABI_IRIX:
		sprintfJdc(result, true, "ELFOSABI_IRIX (0x%X)\tIRIX ABI\n", ehdr->e_ident[EI_OSABI]);
		break;
	case ELFOSABI_FREEBSD:
		sprintfJdc(result, true, "ELFOSABI_FREEBSD (0x%X)\tFreeBSD ABI\n", ehdr->e_ident[EI_OSABI]);
		break;
	case ELFOSABI_TRU64:
		sprintfJdc(result, true, "ELFOSABI_TRU64 (0x%X)\tTRU64 UNIX ABI\n", ehdr->e_ident[EI_OSABI]);
		break;
	case ELFOSABI_ARM:
		sprintfJdc(result, true, "ELFOSABI_ARM (0x%X)\tARM architecture ABI\n", ehdr->e_ident[EI_OSABI]);
		break;
	case ELFOSABI_STANDALONE:
		sprintfJdc(result, true, "ELFOSABI_STANDALONE (0x%X)\tStand-alone (embedded) ABI\n", ehdr->e_ident[EI_OSABI]);
		break;
	}

	sprintfJdc(result, true, "0x8\te_ident[EI_ABIVERSION]\t0x%llX\tVersion of the ABI to which the object is targeted\n", ehdr->e_ident[EI_ABIVERSION]);
	sprintfJdc(result, true, "0x9\te_ident[EI_PAD]\t");
	for (int32_t i = EI_PAD; i < EI_NIDENT; i++)
	{
		sprintfJdc(result, true, "0x%llX", ehdr->e_ident[i]);
		if (i != EI_NIDENT - 1)
		{
			strcatJdc(result, ", ");
		}
		else
		{
			strcatJdc(result, "\tReserved padding bytes\n");
		}
	}

	sprintfJdc(result, true, "0x10\te_type\t");
	switch(ehdr->e_type)
	{
	case ET_NONE:
		sprintfJdc(result, true, "ET_NONE (0x%llX)\tUnknown type\n", ehdr->e_type);
		break;
	case ET_REL:
		sprintfJdc(result, true, "ET_REL (0x%llX)\tRelocatable file\n", ehdr->e_type);
		break;
	case ET_EXEC:
		sprintfJdc(result, true, "ET_EXEC (0x%llX)\tExecutable file\n", ehdr->e_type);
		break;
	case ET_DYN:
		sprintfJdc(result, true, "ET_DYN (0x%llX)\tShared object\n", ehdr->e_type);
		break;
	case ET_CORE:
		sprintfJdc(result, true, "ET_CORE (0x%llX)\tCore file\n", ehdr->e_type);
		break;
	}

	sprintfJdc(result, true, "0x12\te_machine\t");
	switch(ehdr->e_machine)
	{
	case EM_NONE:
		sprintfJdc(result, true, "EM_NONE (0x%llX)\tUnknown machine\n", ehdr->e_machine);
		break;
	case EM_M32:
		sprintfJdc(result, true, "EM_M32 (0x%llX)\tAT&T WE 32100\n", ehdr->e_machine);
		break;
	case EM_SPARC:
		sprintfJdc(result, true, "EM_SPARC (0x%llX)\tSun Microsystems SPARC\n", ehdr->e_machine);
		break;
	case EM_386:
		sprintfJdc(result, true, "EM_386 (0x%llX)\tIntel 80386\n", ehdr->e_machine);
		break;
	case EM_68K:
		sprintfJdc(result, true, "EM_68K (0x%llX)\tMotorola 68000\n", ehdr->e_machine);
		break;
	case EM_88K:
		sprintfJdc(result, true, "EM_88K (0x%llX)\tMotorola 88000\n", ehdr->e_machine);
		break;
	case EM_860:
		sprintfJdc(result, true, "EM_860 (0x%llX)\tIntel 80860\n", ehdr->e_machine);
		break;
	case EM_MIPS:
		sprintfJdc(result, true, "EM_MIPS (0x%llX)\tMIPS RS3000 (big-endian only)\n", ehdr->e_machine);
		break;
	case EM_PARISC:
		sprintfJdc(result, true, "EM_PARISC (0x%llX)\tHP/PA\n", ehdr->e_machine);
		break;
	case EM_SPARC32PLUS:
		sprintfJdc(result, true, "EM_SPARC32PLUS (0x%llX)\tSPARC with enhanced instruction set\n", ehdr->e_machine);
		break;
	case EM_PPC:
		sprintfJdc(result, true, "EM_PPC (0x%llX)\tPowerPC\n", ehdr->e_machine);
		break;
	case EM_PPC64:
		sprintfJdc(result, true, "EM_PPC64 (0x%llX)\tPowerPC 64-bit\n", ehdr->e_machine);
		break;
	case EM_S390:
		sprintfJdc(result, true, "EM_S390 (0x%llX)\tIBM S/390\n", ehdr->e_machine);
		break;
	case EM_ARM:
		sprintfJdc(result, true, "EM_ARM (0x%llX)\tAdvanced RISC Machines\n", ehdr->e_machine);
		break;
	case EM_SH:
		sprintfJdc(result, true, "EM_SH (0x%llX)\tRenesas SuperH\n", ehdr->e_machine);
		break;
	case EM_SPARCV9:
		sprintfJdc(result, true, "EM_SPARCV9 (0x%llX)\tSPARC v9 64-bit\n", ehdr->e_machine);
		break;
	case EM_IA_64:
		sprintfJdc(result, true, "EM_IA_64 (0x%llX)\tIntel Itanium\n", ehdr->e_machine);
		break;
	case EM_X86_64:
		sprintfJdc(result, true, "EM_X86_64 (0x%llX)\tAMD x86-64\n", ehdr->e_machine);
		break;
	case EM_VAX:
		sprintfJdc(result, true, "EM_VAX (0x%llX)\tDEC Vax\n", ehdr->e_machine);
		break;
	}

	sprintfJdc(result, true, "0x14\te_version\t");
	switch(ehdr->e_version)
	{
	case EV_NONE:
		sprintfJdc(result, true, "EV_NONE (0x%llX)\tInvalid version\n", ehdr->e_version);
		break;
	case EV_CURRENT:
		sprintfJdc(result, true, "EV_CURRENT (0x%llX)\tCurrent version\n", ehdr->e_version);
		break;
	}

	if(x64)
	{
		sprintfJdc(result, true, "0x18\te_entry\t0x%llX\tVirtual address to which the system first transfers control\n", ehdr->e_entry);
		sprintfJdc(result, true, "0x20\te_phoff\t0x%llX\tProgram header table's file offset in bytes\n", ehdr->e_phoff);
		sprintfJdc(result, true, "0x28\te_shoff\t0x%llX\tSection header table's file offset in bytes\n", ehdr->e_shoff);
		sprintfJdc(result, true, "0x30\te_flags\t0x%llX\tProcessor-specific flags associated with the file\n", ehdr->e_flags);
		sprintfJdc(result, true, "0x34\te_ehsize\t0x%llX\tELF header's size in bytes\n", ehdr->e_ehsize);
		sprintfJdc(result, true, "0x36\te_phentsize\t0x%llX\tSize in bytes of one entry in the file's program header table\n", ehdr->e_phentsize);
		sprintfJdc(result, true, "0x38\te_phnum\t%d\tNumber of entries in the program header table\n", ehdr->e_phnum);
		sprintfJdc(result, true, "0x3A\te_shentsize\t0x%llX\tA sections header's size in bytes\n", ehdr->e_shentsize);
		sprintfJdc(result, true, "0x3C\te_shnum\t%d\tNumber of entries in the section header table\n", ehdr->e_shnum);
		sprintfJdc(result, true, "0x3E\te_shstrndx\t0x%llX\tSection header table index of the entry associated with the section name string table\n", ehdr->e_shstrndx);
	}
	else
	{
		sprintfJdc(result, true, "0x18\te_entry\t0x%llX\tVirtual address to which the system first transfers control\n", ((Elf32_Ehdr*)ehdr)->e_entry);
		sprintfJdc(result, true, "0x1C\te_phoff\t0x%llX\tProgram header table's file offset in bytes\n", ((Elf32_Ehdr*)ehdr)->e_phoff);
		sprintfJdc(result, true, "0x20\te_shoff\t0x%llX\tSection header table's file offset in bytes\n", ((Elf32_Ehdr*)ehdr)->e_shoff);
		sprintfJdc(result, true, "0x24\te_flags\t0x%llX\tProcessor-specific flags associated with the file\n", ((Elf32_Ehdr*)ehdr)->e_flags);
		sprintfJdc(result, true, "0x28\te_ehsize\t0x%llX\tELF header's size in bytes\n", ((Elf32_Ehdr*)ehdr)->e_ehsize);
		sprintfJdc(result, true, "0x2A\te_phentsize\t0x%llX\tSize in bytes of one entry in the file's program header table\n", ((Elf32_Ehdr*)ehdr)->e_phentsize);
		sprintfJdc(result, true, "0x2C\te_phnum\t%d\tNumber of entries in the program header table\n", ((Elf32_Ehdr*)ehdr)->e_phnum);
		sprintfJdc(result, true, "0x2E\te_shentsize\t0x%llX\tA sections header's size in bytes\n", ((Elf32_Ehdr*)ehdr)->e_shentsize);
		sprintfJdc(result, true, "0x30\te_shnum\t%d\tNumber of entries in the section header table\n", ((Elf32_Ehdr*)ehdr)->e_shnum);
		sprintfJdc(result, true, "0x32\te_shstrndx\t0x%llX\tSection header table index of the entry associated with the section name string table\n", ((Elf32_Ehdr*)ehdr)->e_shstrndx);
	}
}