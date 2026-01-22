#pragma once
#include "disassemblyStructs.h"
#include "opcodes.h"
#include "prefixes.h"
#include "registers.h"

unsigned char handleOperands(unsigned char** bytesPtr, unsigned char* maxBytesAddr, unsigned char* startBytePtr, char hasGotModRM, unsigned char* modRMByteRef, unsigned char is64BitMode, struct Opcode* opcode, struct LegacyPrefixes* legPrefixes, struct REXPrefix* rexPrefix, struct VEXPrefix* vexPrefix, struct Operand* result);

unsigned long long getUIntFromBytes(unsigned char** bytesPtr, unsigned char resultSize);

unsigned char isOperandStackArgument(struct Operand* operand);

unsigned char isOperandLocalVariable(struct Operand* operand);

unsigned char getLastOperand(struct DisassembledInstruction* instruction);

unsigned char areOperandsEqual(struct Operand* op1, struct Operand* op2);

unsigned char operandToValue(struct DisassembledInstruction* instructions, unsigned long long* addresses, int startInstructionIndex, struct Operand* operand, unsigned long long* result);
