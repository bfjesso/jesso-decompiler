#pragma once
#include "opcodes.h"

// Appendix A: A.4.2 table A-6
// any NO_OPERAND_CODE means that the operand code will be what is in the regular opcode map
const struct Opcode extendedOpcodeMap[18][8] = // [group index][3 ModR/M bits: reg]
{
	{	// group 1
		{ ADD, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE },
		{ OR, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE },
		{ ADC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE },
		{ SBB, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE },
		{ AND, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE },
		{ SUB, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE },
		{ XOR, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE },
		{ CMP, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE }
	},
	{	// group 1A
		{ POP, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 }
	},
	{	// group 2
		{ ROL, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE },
		{ ROR, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE },
		{ RCL, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE },
		{ RCR, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE },
		{ SHL, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE },
		{ SHR, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE },
		{ 0 },
		{ SAR, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE }
	},
	{	// group 3
		{ TEST, -1, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE }, // first operands is Iz if opcode is 0xF7
		{ 0 },
		{ NOT, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE },
		{ NEG, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE },
		{ MUL, -1, AL_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE }, // first operands is rAX if opcode is 0xF7
		{ SUB, -1, AL_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE },	// first operands is rAX if opcode is 0xF7
		{ XOR, -1, AL_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE },	// first operands is rAX if opcode is 0xF7
		{ CMP, -1, AL_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE }	// first operands is rAX if opcode is 0xF7
	},
	{	// group 4
		{ INC, -1, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE },
		{ DEC, -1, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 }
	},
	{	// group 5
		{ INC, -1, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE },
		{ DEC, -1, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE },
		{ CALL_NEAR, -1, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE },
		{ CALL_FAR, -1, Ep, NO_OPERAND_CODE, NO_OPERAND_CODE },
		{ JMP_NEAR, -1, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE },
		{ JMP_FAR, -1, Mp, NO_OPERAND_CODE, NO_OPERAND_CODE },
		{ PUSH, -1, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE },
		{ 0 }
	},
	// ......
};