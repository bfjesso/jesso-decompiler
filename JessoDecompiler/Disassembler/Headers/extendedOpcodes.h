#pragma once
#include "opcodes.h"

// Appendix A: A.4.2 table A-6
const enum Mnemonic extendedOpcodeMnemonics[18][8] = // [group index][3 ModR/M bits]
{
	{ ADD, OR, ADC, SBB, AND, SUB, XOR, CMP },																					// group 1
	{ POP, NO_MNEMONIC, NO_MNEMONIC, NO_MNEMONIC, NO_MNEMONIC, NO_MNEMONIC, NO_MNEMONIC, NO_MNEMONIC },							// group 1A
	{ ROL, ROR, RCL, RCR, SHL, SHR, NO_MNEMONIC, SAR },																			// group 2
	{ TEST, NO_MNEMONIC, NOT, NEG, MUL, IMUL, DIV, IDIV },																		// group 3
	// .....
};