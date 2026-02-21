#pragma once
#include "../disassembler/registers.h"
#include "../disassembler/disassembler.h"
#include "decompilationStructs.h"

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned char findNextFunction(struct DecompilationParameters params, unsigned long long nextSectionStartAddress, struct Function* result, int* instructionIndex);
	
	unsigned char fixAllFunctionReturnTypes(struct Function* functions, unsigned short numOfFunctions, unsigned char is64Bit);

	unsigned char fixAllFunctionArgs(struct Function* functions, unsigned short numOfFunctions);

	void freeFunction(struct Function* function);

	int findFunctionByAddress(struct Function* functions, int low, int high, unsigned long long address);

#ifdef __cplusplus
}
#endif

int findInstructionByAddress(struct DisassembledInstruction* instructions, int low, int high, unsigned long long address);

unsigned long long resolveJmpChain(struct DecompilationParameters param, int startInstructionIndex);

unsigned char isOperandStackVar(struct Operand* operand, int stackFrameSize);

unsigned char isOperandStackArg(struct Operand* operand, int stackFrameSize);

struct StackVariable* getLocalVarByOffset(struct Function* function, int stackOffset);

struct StackVariable* getStackArgByOffset(struct Function* function, int stackOffset);

struct RegisterVariable* getRegArgByReg(struct Function* function, enum Register reg);

struct FuncReturnVariable* findReturnedVar(struct Function* function, char callNum, unsigned long long callAddr);

unsigned char getSizeOfOperand(struct Operand* operand);

enum PrimitiveType getTypeOfRegister(enum Mnemonic opcode, enum Register reg);

enum PrimitiveType getTypeOfOperand(enum Mnemonic opcode, struct Operand* operand);

static unsigned char operandToValue(struct DecompilationParameters params, struct Operand* operand, unsigned long long* result);

static unsigned char getNumFromData(struct DecompilationParameters params, unsigned long long address, unsigned long long* result);

static void sortFunctionArguments(struct Function* function);
