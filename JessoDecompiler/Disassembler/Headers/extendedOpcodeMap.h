#pragma once
#include "opcodes.h"

// Appendix A: A.4.2 table A-6
// any NO_OPERAND_CODE means that the operand code will be what is in the regular opcode map
const struct Opcode extendedOpcodeMap[18][8] = // [group index][3 ModR/M bits: reg]
{
	{	// group 1
		{ ADD, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ OR, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ ADC, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ SBB, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ AND, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ SUB, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ XOR, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ CMP, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT }
	},
	{	// group 1A
		{ POP, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 }
	},
	{	// group 2
		{ ROL, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ ROR, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ RCL, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ RCR, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ SHL, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ SHR, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ 0 },
		{ SAR, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT }
	},
	{	// group 3
		{ TEST, -1, Ib, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT }, // first operands is Iz if opcode is 0xF7
		{ 0 },
		{ NOT, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ NEG, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ MUL, -1, AL_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT }, // first operands is rAX if opcode is 0xF7
		{ SUB, -1, AL_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },	// first operands is rAX if opcode is 0xF7
		{ XOR, -1, AL_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },	// first operands is rAX if opcode is 0xF7
		{ CMP, -1, AL_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT }	// first operands is rAX if opcode is 0xF7
	},
	{	// group 4
		{ INC, -1, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ DEC, -1, Eb, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 }
	},
	{	// group 5
		{ INC, -1, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ DEC, -1, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ CALL_NEAR, -1, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },
		{ CALL_FAR, -1, Ep, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ JMP_NEAR, -1, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, f64 },
		{ JMP_FAR, -1, Mp, NO_OPERAND_CODE, NO_OPERAND_CODE, d64 },
		{ PUSH, -1, Ev, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ 0 }
	},	// The groups after 5 are not implemented except for group 11, which is partially implemented for the MOV opcode
	{	// group 6
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 }
	},
	{	// group 7
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 }
	},
	{	// group 8
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 }
	},
	{	// group 9
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 }
	},
	{	// group 10
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 }
	},
	{	// group 11
		{ MOV, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ MOV, -1, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_OPERAND_CODE, NO_SUPERSCRIPT },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 }
	},
	{	// group 12
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 }
	},
	{	// group 13
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 }
	},
	{	// group 14
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 }
	},
	{	// group 15
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 }
	},
	{	// group 16
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 }
	},
	{	// group 17
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 }
	}
};