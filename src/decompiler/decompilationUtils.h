#pragma once
#include "functions.h"
#include "../disassembler/disassembler.h"
#include "../fileStructs.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

const char* indent = "    ";

struct JdcStr 
{
	char* buffer;
	int bufferSize;
};

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

void wrapStrInParentheses(char* str);

unsigned char strcpyJdc(struct JdcStr* jdcStr, const char* src);

unsigned char strcatJdc(struct JdcStr* jdcStr, const char* src);

unsigned char sprintfJdc(struct JdcStr* jdcStr, unsigned char cat, const char* format, ...);

unsigned char initializeJdcStr(struct JdcStr* jdcStr, int bufferSize);

unsigned char freeJdcStr(struct JdcStr* jdcStr);

static unsigned char resizeJdcStr(struct JdcStr* jdcStr);