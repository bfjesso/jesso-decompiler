#include "elfHandler.h"
#include "../file-handler/fileHandler.h"

unsigned char isFileELF(const wchar_t* filePath, unsigned char* isELF)
{
	FILE* file = openFile(filePath);
	if (file)
	{
		unsigned char e_ident[5];
		fread(e_ident, 1, 5, file);
		fclose(file);

		*isELF = memcmp(e_ident, ELFMAG, SELFMAG) == 0;
		return 1;
	}

	return 0;
}

unsigned char isELFX64(const wchar_t* filePath, unsigned char* isX64)
{
	FILE* file = openFile(filePath);
	if(file)
	{
		unsigned char e_ident[5];
		fread(e_ident, 1, 5, file);
		fclose(file);

		*isX64 = e_ident[EI_CLASS] == ELFCLASS64;
		return 1;
	}

	return 0;
}

void readElfEhdr(FILE* file, unsigned char is64Bit, Elf64_Ehdr* result)
{
	if(!file)
	{
		return;
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
}

void readElfShdr(FILE* file, unsigned char is64Bit, unsigned long long fileOffset, Elf64_Shdr* result)
{
	if(!file)
	{
		return;
	}

	fseek(file, fileOffset, SEEK_SET);

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
}

unsigned long long getELFEntryPoint(const wchar_t* filePath, unsigned char is64Bit)
{
	FILE* file = openFile(filePath);
	if (file)
	{
		Elf64_Ehdr elfHeader;
		readElfEhdr(file, is64Bit, &elfHeader);
		fclose(file);

		return elfHeader.e_entry;
	}

	return 0;
}

unsigned char getELFSymbolByValue(const wchar_t* filePath, unsigned char is64Bit, unsigned long long value, struct JdcStr* result)
{
	Elf64_Shdr strtabSection;
	if(!getSectionHeaderByName(filePath, is64Bit, ".strtab", &strtabSection))
	{
		return 0;
	}

	char* stringBytes = (char*)malloc(strtabSection.sh_size);
	if(!readSectionBytes(filePath, &strtabSection, stringBytes, strtabSection.sh_size))
	{
		free(stringBytes);
		return 0;
	}

	Elf64_Shdr symtabSection;
	if(!getSectionHeaderByName(filePath, is64Bit, ".symtab", &symtabSection))
	{
		return 0;
	}

	char* bytes = (char*)malloc(symtabSection.sh_size);
	if(!readSectionBytes(filePath, &symtabSection, bytes, symtabSection.sh_size))
	{
		free(bytes);
		return 0;
	}

	int i = 0;
	while(i < symtabSection.sh_size)
	{
		unsigned int st_name = 0;
		unsigned long long st_value = 0;
		if(is64Bit)
		{
			Elf64_Sym* symbol = (Elf64_Sym*)(bytes + i);
			st_name = symbol->st_name;
			st_value = symbol->st_value;
		}
		else
		{
			Elf32_Sym* symbol = (Elf32_Sym*)(bytes + i);
			st_name = symbol->st_name;
			st_value = symbol->st_value;
		}
		
		if(st_value == value && (stringBytes + st_name)[0] != 0)
		{
			if (!demangleCppSymbol(stringBytes + st_name, result->buffer, result->bufferSize))
			{
				strcpyJdc(result, stringBytes + st_name);
			}

			free(stringBytes);
			free(bytes);
			return 1;
		}

		i += is64Bit ? sizeof(Elf64_Sym) : sizeof(Elf32_Sym);
	}
	
	free(stringBytes);
	free(bytes);
	return 0;
}

int getNumOfELFSections(const wchar_t* filePath, unsigned char is64Bit)
{
	FILE* file = openFile(filePath);
	if (file)
	{
		Elf64_Ehdr elfHeader;
		readElfEhdr(file, is64Bit, &elfHeader);

		Elf64_Shdr sectionHeader;
		unsigned int shdrSize = is64Bit ? sizeof(Elf64_Shdr) : sizeof(Elf32_Shdr);

		int result = 0;
		for (int i = 0; i < elfHeader.e_shnum; i++)
		{
			readElfShdr(file, is64Bit, elfHeader.e_shoff + i * shdrSize, &sectionHeader);

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

unsigned char getAllELFSectionHeaders(const wchar_t* filePath, unsigned char is64Bit, struct FileSection* buffer, int bufferLen)
{
	Elf64_Ehdr elfHeader;
	Elf64_Shdr sectionHeader;
	FILE* file = openFile(filePath);

	if (file)
	{
		readElfEhdr(file, is64Bit, &elfHeader);

		unsigned int shdrSize = is64Bit ? sizeof(Elf64_Shdr) : sizeof(Elf32_Shdr);

		Elf64_Shdr nameStrTable;
		readElfShdr(file, is64Bit, elfHeader.e_shoff + elfHeader.e_shstrndx * shdrSize, &nameStrTable);

		char* sectionNames = (char*)malloc(nameStrTable.sh_size);
		fseek(file, nameStrTable.sh_offset, SEEK_SET);
		fread(sectionNames, 1, nameStrTable.sh_size, file);

		int bufferIndex = 0;
		for (int i = 0; i < elfHeader.e_shnum; i++)
		{
			if (bufferIndex >= bufferLen)
			{
				fclose(file);
				return 0;
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
		return 1;

		fclose(file);
	}

	return 0;
}

unsigned char getSectionHeaderByName(const wchar_t* filePath, unsigned char is64Bit, const char* name, Elf64_Shdr* result)
{
	Elf64_Ehdr elfHeader;
	Elf64_Shdr sectionHeader;
	FILE* file = openFile(filePath);

	if (file)
	{
		readElfEhdr(file, is64Bit, &elfHeader);

		unsigned int shdrSize = is64Bit ? sizeof(Elf64_Shdr) : sizeof(Elf32_Shdr);

		Elf64_Shdr nameStrTable;
		readElfShdr(file, is64Bit, elfHeader.e_shoff + elfHeader.e_shstrndx * shdrSize, &nameStrTable);

		char* sectionNames = (char*)malloc(nameStrTable.sh_size);
		fseek(file, nameStrTable.sh_offset, SEEK_SET);
		fread(sectionNames, 1, nameStrTable.sh_size, file);

		for (int i = 0; i < elfHeader.e_shnum; i++)
		{
			readElfShdr(file, is64Bit, elfHeader.e_shoff + i * shdrSize, &sectionHeader);
			if (strcmp(sectionNames + sectionHeader.sh_name, name) == 0)
			{
				*result = sectionHeader;

				fclose(file);
				free(sectionNames);
				return 1;
			}
		}

		fclose(file);
		free(sectionNames);
	}

	return 0;
}

unsigned char readSectionBytes(const wchar_t* filePath, Elf64_Shdr* section, char* buffer, unsigned int bufferSize)
{
	FILE* file = openFile(filePath);
	if (file)
	{
		fseek(file, section->sh_offset, SEEK_SET);
		fread(buffer, 1, bufferSize, file);
		fclose(file);
		return 1;
	}
	
	return 0;
}

unsigned char getSectionHeaderByType(const wchar_t* filePath, unsigned char is64Bit, unsigned int type, int index, Elf64_Shdr* result)
{
	Elf64_Ehdr elfHeader;
	Elf64_Shdr sectionHeader;
	FILE* file = openFile(filePath);

	if (file)
	{
		readElfEhdr(file, is64Bit, &elfHeader);

		unsigned int shdrSize = is64Bit ? sizeof(Elf64_Shdr) : sizeof(Elf32_Shdr);
		int num = 0;
		for (int i = 0; i < elfHeader.e_shnum; i++)
		{
			readElfShdr(file, is64Bit, elfHeader.e_shoff + i * shdrSize, &sectionHeader);
			if (sectionHeader.sh_type == type)
			{
				if (num == index)
				{
					*result = sectionHeader;
					fclose(file);
					return 1;
				}

				num++;
			}
		}

		fclose(file);
	}

	return 0;
}

int getNumOfELFImports(const wchar_t* filePath, unsigned char is64Bit, int* numOfLibrariesRef)
{
	*numOfLibrariesRef = 1; // this is just the .dynsym section
	
	Elf64_Shdr dynstrSection;
	if (!getSectionHeaderByType(filePath, is64Bit, SHT_STRTAB, 0, &dynstrSection))
	{
		return 0;
	}

	char* stringBytes = (char*)malloc(dynstrSection.sh_size);
	if (!readSectionBytes(filePath, &dynstrSection, stringBytes, dynstrSection.sh_size))
	{
		free(stringBytes);
		return 0;
	}

	Elf64_Shdr dynsymSection;
	if (!getSectionHeaderByType(filePath, is64Bit, SHT_DYNSYM, 0, &dynsymSection))
	{
		free(stringBytes);
		return 0;
	}

	char* dynsymBytes = (char*)malloc(dynsymSection.sh_size);
	if (!readSectionBytes(filePath, &dynsymSection, dynsymBytes, dynsymSection.sh_size))
	{
		free(stringBytes);
		free(dynsymBytes);
		return 0;
	}

	int relaNum = 0;
	Elf64_Shdr relaSection;
	int result = 0;
	while (getSectionHeaderByType(filePath, is64Bit, SHT_RELA, relaNum, &relaSection)) // going through all rela sections
	{
		char* relaBytes = (char*)malloc(relaSection.sh_size);
		if (!readSectionBytes(filePath, &relaSection, relaBytes, relaSection.sh_size))
		{
			free(stringBytes);
			free(dynsymBytes);
			free(relaBytes);
			return 0;
		}

		unsigned int relaSize = is64Bit ? sizeof(Elf64_Rela) : sizeof(Elf32_Rela);

		int i = 0;
		while ((i * relaSize) < relaSection.sh_size)
		{
			unsigned int st_name = 0;
			if(is64Bit)
			{
				Elf64_Rela* rela = (Elf64_Rela*)(relaBytes + (i * sizeof(Elf64_Rela)));
				int val = ELF64_R_SYM(rela->r_info);
				Elf64_Sym* symbol = (Elf64_Sym*)(dynsymBytes + (val * sizeof(Elf64_Sym)));
				st_name = symbol->st_name;
			}
			else
			{
				Elf32_Rela* rela = (Elf32_Rela*)(relaBytes + (i * sizeof(Elf32_Rela)));
				int val = ELF32_R_SYM(rela->r_info);
				Elf32_Sym* symbol = (Elf32_Sym*)(dynsymBytes + (val * sizeof(Elf32_Sym)));
				st_name = symbol->st_name;
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

	return result;
}

int getAllELFImports(const wchar_t* filePath, unsigned char is64Bit, struct ImportedFunction* importsBuffer, int importsBufferLen, struct JdcStr* libraryNamesBuffer, int libraryNamesBufferLen)
{
	if (libraryNamesBufferLen > 0) 
	{
		libraryNamesBuffer[0] = initializeJdcStrWithVal(".dynsym");
	}
	
	Elf64_Shdr dynstrSection;
	if(!getSectionHeaderByType(filePath, is64Bit, SHT_STRTAB, 0, &dynstrSection))
	{
		return 0;
	}

	char* stringBytes = (char*)malloc(dynstrSection.sh_size);
	if(!readSectionBytes(filePath, &dynstrSection, stringBytes, dynstrSection.sh_size))
	{
		free(stringBytes);
		return 0;
	}

	Elf64_Shdr dynsymSection;
	if(!getSectionHeaderByType(filePath, is64Bit, SHT_DYNSYM, 0, &dynsymSection))
	{
		free(stringBytes);
		return 0;
	}

	char* dynsymBytes = (char*)malloc(dynsymSection.sh_size);
	if(!readSectionBytes(filePath, &dynsymSection, dynsymBytes, dynsymSection.sh_size))
	{
		free(stringBytes);
		free(dynsymBytes);
		return 0;
	}

	int relaNum = 0;
	Elf64_Shdr relaSection;
	int importsIndex = 0;
	while(getSectionHeaderByType(filePath, is64Bit, SHT_RELA, relaNum, &relaSection)) // going through all rela sections
	{
		char* relaBytes = (char*)malloc(relaSection.sh_size);
		if(!readSectionBytes(filePath, &relaSection, relaBytes, relaSection.sh_size))
		{
			free(stringBytes);
			free(dynsymBytes);
			free(relaBytes);
			return 0;
		}

		unsigned int relaSize = is64Bit ? sizeof(Elf64_Rela) : sizeof(Elf32_Rela);

		int i = 0;
		while((i * relaSize) < relaSection.sh_size && importsIndex < importsBufferLen)
		{
			unsigned int st_name = 0;
			unsigned long long r_offset = 0;
			if(is64Bit)
			{
				Elf64_Rela* rela = (Elf64_Rela*)(relaBytes + (i * sizeof(Elf64_Rela)));
				int val = ELF64_R_SYM(rela->r_info);
				Elf64_Sym* symbol = (Elf64_Sym*)(dynsymBytes + (val * sizeof(Elf64_Sym)));
				st_name = symbol->st_name;
				r_offset = rela->r_offset;
			}
			else
			{
				Elf32_Rela* rela = (Elf32_Rela*)(relaBytes + (i * sizeof(Elf32_Rela)));
				int val = ELF32_R_SYM(rela->r_info);
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

	return importsIndex;
}

unsigned char generateELFHeadersInfoStr(const wchar_t* filePath, struct JdcStr* result)
{
	FILE* file = openFile(filePath);
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
	case ELFOSABI_NONE:
		sprintfJdc(result, 1, "ELFOSABI_NONE (0x%X)\tNo extensions or unspecified\n", ehdr->e_ident[EI_OSABI]);
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