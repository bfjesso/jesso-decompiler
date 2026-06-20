#include "disassembler.h"
#include "disassemblyUtils.h"
#include "prefixes.h"
#include "opcodes.h"
#include "operands.h"

unsigned char disassembleInstruction(unsigned char* bytes, unsigned char* maxBytesAddr, struct DisassemblerOptions* disassemblerOptions, struct DisassembledInstruction* result)
{
	struct DisassemblyParameters params = { 0 };
	params.bytes = bytes;
	params.maxBytesAddr = maxBytesAddr;
	params.startBytePtr = bytes;
	params.is64BitMode = disassemblerOptions->is64BitMode;

	memset(result, 0, sizeof(struct DisassembledInstruction));
	
	if (!handleLegacyPrefixes(&params))
	{
		result->numOfBytes = (unsigned char)(params.bytes - params.startBytePtr);
		return 0;
	}

	if (params.is64BitMode) 
	{
		if (!handleREXPrefix(&params))
		{
			result->numOfBytes = (unsigned char)(params.bytes - params.startBytePtr);
			return 0;
		}

		if (!params.rexPrefix.isValidREX && !handleVEXPrefix(&params))
		{
			result->numOfBytes = (unsigned char)(params.bytes - params.startBytePtr);
			return 0;
		}

		if (!params.rexPrefix.isValidREX && !params.vexPrefix.isValidVEX && !handleEVEXPrefix(&params))
		{
			result->numOfBytes = (unsigned char)(params.bytes - params.startBytePtr);
			return 0;
		}
	}

	// if the opcode is an extended one then modRM will be retrieved
	if (!handleOpcode(&params))
	{
		result->numOfBytes = (unsigned char)(params.bytes - params.startBytePtr);
		return 0;
	}

	if (!handleOperands(&params, result))
	{
		result->numOfBytes = (unsigned char)(params.bytes - params.startBytePtr);
		return 0;
	}

	result->opcode = params.opcode.mnemonic;
	result->group1Prefix = params.legPrefixes.group1;
	result->numOfBytes = (unsigned char)(params.bytes - params.startBytePtr);
	result->isInvalid = (disassemblerOptions->is64BitMode && params.opcode.opcodeSuperscript == i64) || (!disassemblerOptions->is64BitMode && params.opcode.opcodeSuperscript == o64);

	return result->numOfBytes != 0;
}

unsigned char instructionToStr(struct DisassembledInstruction* instruction, struct JdcStr* result) // this will be in intel syntax
{
	strcpyJdc(result, "");

	if (instruction->group1Prefix != NO_PREFIX) 
	{
		strcatJdc(result, getGroup1PrefixStr(instruction));
		strcatJdc(result, " ");
	}

	strcatJdc(result, mnemonicStrs[instruction->opcode]);

	for (int i = 0; i < instruction->numOfOperands; i++)
	{
		if (i != 0) 
		{
			strcatJdc(result, ", ");
		}
		else 
		{
			strcatJdc(result, " ");
		}

		struct Operand* currentOperand = &instruction->operands[i];
		switch (currentOperand->type)
		{
		case SEGMENT:
			strcatJdc(result, segmentStrs[currentOperand->segment]);
			break;
		case REGISTER:
			strcatJdc(result, registerStrs[currentOperand->reg]);
			break;
		case MEM_ADDRESS:
			if (!memAddressToStr(&currentOperand->memoryAddress, result)) { return 0; }
			break;
		case IMMEDIATE:
			sprintfJdc(result, 1, "0x%llX", currentOperand->immediate.value);
			break;
		}
	}

	return 1;
}

static unsigned char memAddressToStr(struct MemoryAddress* memAddr, struct JdcStr* result)
{
	if (memAddr->ptrSize != 0)
	{
		strcatJdc(result, getPtrSizeStr(memAddr->ptrSize));
		strcatJdc(result, " ");
	}
	
	if (memAddr->segment != NO_SEGMENT) 
	{
		strcatJdc(result, segmentStrs[memAddr->segment]);
		strcatJdc(result, ":");
	}
	else if (memAddr->constSegment != 0) 
	{
		sprintfJdc(result, 1, "0x%X", memAddr->constSegment);
		strcatJdc(result, ":");
	}

	strcatJdc(result, "[");

	if (memAddr->reg != NO_REG) 
	{
		strcatJdc(result, registerStrs[memAddr->reg]);
	}

	if (memAddr->scale > 1) 
	{
		sprintfJdc(result, 1, "*0x%X", memAddr->scale);
	}

	if (memAddr->regDisplacement != NO_REG)
	{
		strcatJdc(result, "+");
		strcatJdc(result, registerStrs[memAddr->regDisplacement]);
	}

	if (memAddr->reg != NO_REG)
	{
		if (memAddr->constDisplacement > 0)
		{
			sprintfJdc(result, 1, "+0x%llX", memAddr->constDisplacement);
		}
		else if (memAddr->constDisplacement < 0)
		{
			sprintfJdc(result, 1, "-0x%llX", -memAddr->constDisplacement);
		}
	}
	else
	{
		sprintfJdc(result, 1, "0x%llX", memAddr->constDisplacement);
	}

	strcatJdc(result, "]");

	return 1;
}

const char* getPtrSizeStr(int ptrSize)
{
	switch (ptrSize) 
	{
	case 1:
		return "BYTE PTR";
	case 2:
		return "WORD PTR";
	case 4:
		return "DWORD PTR";
	case 6:
		return "FWORD PTR";
	case 8:
		return "QWORD PTR";
	case 10:
		return "TBYTE PTR";
	case 16:
		return "XMMWORD PTR";
	case 32:
		return "YMMWORD PTR";
	case 64:
		return "ZMMWORD PTR";
	}
	
	return "";
}

const char* getGroup1PrefixStr(struct DisassembledInstruction* instruction)
{
	if (instruction->group1Prefix == REPNZ_BND &&
		(instruction->opcode == CALL_NEAR || instruction->opcode == RET_NEAR || instruction->opcode == JMP_NEAR || isOpcodeJcc(instruction->opcode))) 
	{
		return "BND";
	}

	switch (instruction->group1Prefix) 
	{
	case LOCK:
		return "LOCK";
	case REPNZ_BND:
		return "REPNZ";
	case REPZ:
		return "REP";
	}
	
	return "";
}

unsigned char checkForControlFlowJump(struct DisassembledInstruction* instructions, int instructionIndex, unsigned long long* jmpDst, unsigned char* stop)
{
	if (!instructions || !jmpDst || !stop)
	{
		return 0;
	}
	
	if (isOpcodeReturn(instructions[instructionIndex].opcode))
	{
		*jmpDst = instructions[instructionIndex].address;
		*stop = 1;
		return 1;
	}
	else if (isOpcodeJmp(instructions[instructionIndex].opcode))
	{
		*jmpDst = getJmpDst(instructions, instructionIndex, instructionIndex - 0x1000);
		*stop = 1;
		return 1;
	}
	else if (isOpcodeJcc(instructions[instructionIndex].opcode) || isOpcodeCall(instructions[instructionIndex].opcode))
	{
		*jmpDst = getJmpDst(instructions, instructionIndex, instructionIndex - 0x1000);
		*stop = 0;
		return 1;
	}

	return 0;
}

unsigned long long getJumpTableAddress(struct DisassembledInstruction* instructions, int numOfInstructions, unsigned char* size)
{
	if (numOfInstructions < 2) 
	{
		return 0;
	}
	
	struct DisassembledInstruction* jmpInstruction = &instructions[numOfInstructions - 1];

	if (jmpInstruction->opcode != JMP_NEAR)
	{
		return 0;
	}

	if (jmpInstruction->operands[0].type == REGISTER)
	{
		unsigned long long result = 0;
		
		enum Register targetReg = jmpInstruction->operands[0].reg;
		int i = numOfInstructions - 2;
		struct DisassembledInstruction* instruction = &instructions[i];
		while (!isOpcodeJcc(instruction->opcode) && !isOpcodeReturn(instruction->opcode) && i >= 0)
		{
			if (instruction->operands[0].type == REGISTER && compareRegisters(targetReg, instruction->operands[0].reg)) 
			{
				if (instruction->opcode == ADD) 
				{
					unsigned long long val = 0;
					if (operandToValue(instructions, i, i - 0x1000, &instruction->operands[1], &val)) 
					{
						result += val;
					}
				}
				else if ((instruction->opcode == MOV || instruction->opcode == LEA) && instruction->operands[1].type == MEM_ADDRESS && instruction->operands[1].memoryAddress.scale > 1)
				{
					unsigned long long regDisplacementVal = 0;
					if (regToValue(instructions, numOfInstructions - 2, numOfInstructions - 0x1000, jmpInstruction->operands[1].memoryAddress.regDisplacement, &regDisplacementVal))
					{
						result += regDisplacementVal;
					}

					*size = instruction->operands[1].memoryAddress.ptrSize;

					return result + instruction->operands[1].memoryAddress.constDisplacement;
				}
			}

			i--;
			instruction = &instructions[i];
		}
	}
	else if (jmpInstruction->operands[0].type == MEM_ADDRESS && jmpInstruction->operands[0].memoryAddress.scale > 1)
	{
		unsigned long long result = jmpInstruction->operands[0].memoryAddress.constDisplacement;

		unsigned long long regDisplacementVal = 0;
		if (regToValue(instructions, numOfInstructions - 2, numOfInstructions - 0x1000, jmpInstruction->operands[0].memoryAddress.regDisplacement, &regDisplacementVal))
		{
			result += regDisplacementVal;
		}

		*size = jmpInstruction->operands[0].memoryAddress.ptrSize;

		return result;
	}

	return 0;
}

unsigned long long getIndirectTableAddress(struct DisassembledInstruction* instructions, int numOfInstructions)
{
	struct DisassembledInstruction* instruction = &instructions[numOfInstructions - 1];

	if (instruction->opcode == MOVZX && instruction->operands[1].type == MEM_ADDRESS)
	{
		unsigned long long result = instruction->operands[1].memoryAddress.constDisplacement;
		
		unsigned long long regDisplacementVal = 0;
		if (regToValue(instructions, numOfInstructions - 2, numOfInstructions - 0x1000, instruction->operands[1].memoryAddress.regDisplacement, &regDisplacementVal))
		{
			result += regDisplacementVal;
		}

		return result;
	}

	return 0;
}