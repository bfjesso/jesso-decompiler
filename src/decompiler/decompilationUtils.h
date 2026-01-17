#pragma once
#include "functions.h"
#include "../disassembler/disassembler.h"
#include "../fileStructs.h"
#include "../jdc-str/jdcStr.h"

struct DecompilationParameters
{
	struct Function* functions;
	int numOfFunctions;

	struct ImportedFunction* imports;
	int numOfImports;

	struct Function* currentFunc; // function being decompiled
	int startInstructionIndex; // index of instruction to start decompiling from relative to function
	
	// these are used when a condition has requiresJumpInDecomp
	int skipUpperBound;
	int skipLowerBound;

	struct DisassembledInstruction* allInstructions;
	unsigned long long* allAddresses;
	int totalNumOfInstructions;

	unsigned long long imageBase;
	struct FileSection* dataSections;
	int numOfDataSections;
	unsigned char* dataSectionByte;

	unsigned char is64Bit;
};