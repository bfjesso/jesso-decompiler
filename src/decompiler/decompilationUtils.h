#pragma once
#include "functions.h"
#include "../disassembler/disassembler.h"
#include "../fileStructs.h"

#include <stdio.h>
#include <string.h>

struct LineOfC
{
	char line[255];
	unsigned char indents;
};

struct DecompilationParameters
{
	struct Function* functions;
	int numOfFunctions;

	struct ImportedFunction* imports;
	int numOfImports;

	struct Function* currentFunc; // function being decompiled
	int startInstructionIndex; // index of instruction to start decompiling from

	struct DisassembledInstruction* allInstructions;
	unsigned long long* allAddresses;
	int totalNumOfInstructions;

	unsigned long long imageBase;
	struct FileSection* dataSections;
	int numOfDataSections;
	unsigned char* dataSectionByte;
};

unsigned char getOperationStr(enum Mnemonic opcode, unsigned char getAssignment, char* resultBuffer);

void wrapStrInParentheses(char* str);
