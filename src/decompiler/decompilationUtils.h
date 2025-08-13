#pragma once
#include "functions.h"
#include "../disassembler/disassembler.h"
#include "../importedFunctions.h"

#include <stdio.h>
#include <string.h>

struct LineOfC
{
	char line[150];
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
};

unsigned char getOperationStr(unsigned char opcode, unsigned char getAssignment, char* resultBuffer);

void wrapStrInParentheses(char* str);

unsigned long long resolveJmpChain(struct DecompilationParameters params, struct DisassembledInstruction* instruction, unsigned long long address);