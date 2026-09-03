#include "decompilationUtils.h"
#include "functions.h"
#include "functionCalls.h"
#include "../disassembler/disassemblyUtils.h"
#include "intrinsics.h"

// this is used for name validation and syntax highlighting
extern const char* keywordStrs[NUM_OF_KEYWORDS] = 
{ 
	"if", 
	"else", 
	"for", 
	"while", 
	"do", 
	"break", 
	"continue", 
	"switch", 
	"case", 
	"goto", 
	"return" 
};

void addIndents(struct JdcStr* result, int numOfIndents)
{
	for (int i = 0; i < numOfIndents; i++)
	{
		strcatJdc(result, "\t");
	}
}

unsigned long long resolveJmpChain(struct DecompilationParameters* params, int startInstructionIndex)
{
	unsigned long long jmpAddress = getJmpDst(params->instructions, startInstructionIndex, params->currentFunc ? params->currentFunc->firstInstructionIndex : startInstructionIndex - 0x1000);
	if (jmpAddress == 0)
	{
		return 0;
	}

	int instructionIndex = findInstructionByAddress(params->instructions, params->numOfInstructions, jmpAddress);
	if (instructionIndex != -1)
	{
		struct DisassembledInstruction* jmpInstruction = &(params->instructions[instructionIndex]);
		if (instructionIndex != startInstructionIndex && isOpcodeJmp(jmpInstruction->opcode))
		{
			unsigned long long nextJmpAddress = resolveJmpChain(params, instructionIndex);
			if (nextJmpAddress != 0) 
			{
				return nextJmpAddress;
			}
		}
	}

	return jmpAddress;
}

int findInstructionByAddress(struct DisassembledInstruction* instructions, int numOfInstructions, unsigned long long address)
{
	int low = 0;
	int high = numOfInstructions - 1;
	while (low <= high)
	{
		int mid = low + (high - low) / 2;

		if (instructions[mid].address == address) { return mid; }

		if (instructions[mid].address < address) { low = mid + 1; }
		else { high = mid - 1; }
	}

	return -1;
}

int findInstructionByAddressInclusive(struct DisassembledInstruction* instructions, int numOfInstructions, unsigned long long address)
{
	int low = 0;
	int high = numOfInstructions - 1;
	while (low <= high)
	{
		int mid = low + (high - low) / 2;

		if (instructions[mid].address <= address && instructions[mid].address + instructions[mid].numOfBytes > address) { return mid; }

		if (instructions[mid].address < address) { low = mid + 1; }
		else { high = mid - 1; }
	}

	return -1;
}

int findInstructionInsertPoint(struct DisassembledInstruction* instructions, int numOfInstructions, unsigned long long address)
{
	int low = 0;
	int high = numOfInstructions - 1;
	while (low <= high)
	{
		int mid = low + (high - low) / 2;

		if (instructions[mid].address < address && (mid == high || instructions[mid + 1].address > address)) { return mid + 1; }

		if (instructions[mid].address < address) { low = mid + 1; }
		else { high = mid - 1; }
	}

	return 0;
}

int findAddressInArr(unsigned long long* addresses, int numOfAddresses, unsigned long long address)
{
	int low = 0;
	int high = numOfAddresses - 1;
	while (low <= high)
	{
		int mid = low + (high - low) / 2;

		if (addresses[mid] == address) { return mid; }

		if (addresses[mid] < address) { low = mid + 1; }
		else { high = mid - 1; }
	}

	return -1;
}

int findJumpTableByAddress(struct JumpTable* jumpTables, int numOfJumpTables, unsigned long long address, unsigned char* foundIndirectTable)
{
	int low = 0;
	int high = numOfJumpTables - 1;
	while (low <= high)
	{
		int mid = low + (high - low) / 2;

		if (jumpTables[mid].jmpTableAddress == address) 
		{ 
			if (foundIndirectTable) { *foundIndirectTable = 0; }
			return mid; 
		}
		else if (jumpTables[mid].indirectTableAddress == address)
		{
			if (foundIndirectTable) { *foundIndirectTable = 1; }
			return mid;
		}

		if (jumpTables[mid].jmpTableAddress < address) { low = mid + 1; }
		else { high = mid - 1; }
	}

	return -1;
}

unsigned char checkForAddressInArrInRange(unsigned long long* addresses, int numOfAddresses, unsigned long long minAddress, unsigned long long maxAddress)
{
	int low = 0;
	int high = numOfAddresses - 1;
	while (low <= high)
	{
		int mid = low + (high - low) / 2;

		if (addresses[mid] >= minAddress && addresses[mid] <= maxAddress) { return 1; }

		if (addresses[mid] < minAddress) { low = mid + 1; }
		else { high = mid - 1; }
	}

	return 0;
}

unsigned char doesInstructionAccessRegister(struct DecompilationParameters* params, int instructionIndex, enum Register reg, unsigned char checkUnknownCalls, enum Register* specificReg)
{
	struct Function* callee;
	if (checkForKnownFunctionCall(params, instructionIndex, &callee) && callee)
	{
		struct RegisterVariable* regArg = getRegArgByReg(callee, reg);
		if (regArg)
		{
			if (specificReg)
			{
				*specificReg = regArg->reg;
			}

			return 1;
		}
	}
	else if (checkUnknownCalls && checkForUnknownFunctionCall(params, instructionIndex))
	{
		int numOfPlatformRegArgs = getNumOfPlatformRegArgs(params->fileFormat);
		const enum Register* platformRegArgs = getPlatformRegArgs(params->fileFormat);
		for (int i = 0; i < numOfPlatformRegArgs; i++)
		{
			if (compareRegisters(reg, platformRegArgs[i])) 
			{
				if (specificReg)
				{
					*specificReg = reg;
				}

				return 1;
			}
		}
	}
	
	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];
	for (int i = 0; i < instruction->numOfOperands; i++)
	{
		unsigned char overwrites = 0;
		struct Operand* op = &(instruction->operands[i]);
		if (op->type == MEM_ADDRESS)
		{
			if (compareRegisters(op->memoryAddress.reg, reg))
			{
				if (specificReg)
				{
					*specificReg = op->memoryAddress.reg;
				}

				return 1;
			}
			else if (compareRegisters(op->memoryAddress.regDisplacement, reg))
			{
				if (specificReg)
				{
					*specificReg = op->memoryAddress.regDisplacement;
				}

				return 1;
			}
		}
		else if ((!doesInstructionModifyOperand(instruction, i, &overwrites) || !overwrites) && op->type == REGISTER && compareRegisters(op->reg, reg))
		{
			if (specificReg)
			{
				*specificReg = op->reg;
			}

			return 1;
		}
	}

	return 0;
}

unsigned char doesInstructionModifyRegister(struct DecompilationParameters* params, int instructionIndex, enum Register reg, enum Register* specificReg, unsigned char* overwrites)
{
	if (specificReg) { *specificReg = NO_REG; }
	if (overwrites) { *overwrites = 0; }
	
	struct Function* callee = 0;
	if ((checkForKnownFunctionCall(params, instructionIndex, &callee) && callee && compareRegisters(callee->returnReg, reg)) ||
		(checkForUnknownFunctionCall(params, instructionIndex) && compareRegisters(reg, AX)))
	{
		if (overwrites) { *overwrites = 1; }
		if (specificReg) 
		{ 
			if (callee) { *specificReg = callee->returnReg; }
			else { *specificReg = params->is64Bit ? RAX : EAX; }
		}
		return 1;
	}

	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];
	enum Mnemonic opcode = instruction->opcode;

	if (opcode == POP && instruction->operands[0].type == REGISTER && compareRegisters(instruction->operands[0].reg, reg))
	{
		int stackOffset = 0;
		for (int i = instructionIndex; i >= params->currentFunc->firstInstructionIndex; i--) 
		{
			if (params->instructions[i].opcode == PUSH) 
			{
				stackOffset++;
				if (stackOffset == 0)
				{
					if (params->instructions[i].operands[0].type == REGISTER && instruction->operands[0].reg == params->instructions[i].operands[0].reg)
					{
						return 0;
					}

					if (specificReg) { *specificReg = instruction->operands[0].reg; }
					if (overwrites) { *overwrites = 1; }
					return 1;
				}
			}
			else if (params->instructions[i].opcode == POP)
			{
				stackOffset--;
			}
		}

		return 0;
	}

	if (compareRegisters(reg, AX)) // some opcodes may modify a register even if it isn't an operand
	{
		if (opcode == IDIV || opcode == DIV)
		{
			if (specificReg) 
			{
				int size = getSizeOfRegister(instruction->operands[0].reg);
				if (instruction->operands[0].type == MEM_ADDRESS)
				{
					size = instruction->operands[0].memoryAddress.ptrSize;
				}

				switch (size)
				{
				case 1:
					*specificReg = AL;
					break;
				case 2:
					*specificReg = AX;
					break;
				case 4:
					*specificReg = EAX;
					break;
				case 8:
					*specificReg = RAX;
					break;
				}
			}
			
			return 1;
		}

		if ((opcode == IMUL || opcode == MUL) && instruction->numOfOperands == 1)
		{
			if (specificReg)
			{
				int size = getSizeOfRegister(instruction->operands[0].reg);
				if (instruction->operands[0].type == MEM_ADDRESS)
				{
					size = instruction->operands[0].memoryAddress.ptrSize;
				}

				switch (size)
				{
				case 1:
				case 2:
					*specificReg = AX;
					break;
				case 4:
					*specificReg = EAX;
					break;
				case 8:
					*specificReg = RAX;
					break;
				}
			}

			return 1;
		}
	}
	else if (compareRegisters(reg, DX))
	{
		if ((opcode == IMUL || opcode == MUL || opcode == IDIV || opcode == DIV) && instruction->numOfOperands == 1)
		{
			if (specificReg) 
			{ 
				int size = getSizeOfRegister(instruction->operands[0].reg);
				if (instruction->operands[0].type == MEM_ADDRESS) 
				{
					size = instruction->operands[0].memoryAddress.ptrSize;
				}

				switch (size)
				{
				case 2:
					*specificReg = DX;
					break;
				case 4:
					*specificReg = EDX;
					break;
				case 8:
					*specificReg = RDX;
					break;
				}
			}

			if (overwrites) { *overwrites = 1; }
			return 1;
		}
	}
	else if (compareRegisters(reg, ST0))
	{
		switch (opcode)
		{
		case FLD:
			if (specificReg) { *specificReg = ST0; }
			if (overwrites) { *overwrites = 1; }
			return 1;
		}
	}

	for (int i = 0; i < instruction->numOfOperands; i++)
	{
		struct Operand* op = &(instruction->operands[i]);
		if (op->type == REGISTER && compareRegisters(op->reg, reg))
		{
			if (doesInstructionModifyOperand(instruction, i, overwrites))
			{
				if (specificReg) { *specificReg = op->reg; }
				return 1;
			}
		}
	}

	return 0;
}

unsigned char validateName(struct DecompilationParameters* params, const char* name) 
{
	int nameLen = (int)strlen(name);
	if (nameLen == 0) 
	{
		return 0;
	}

	if (name[0] != '_' && (name[0] < 'a' || name[0] > 'z') && (name[0] < 'A' || name[0] > 'Z')) // first character cannot be a number
	{
		return 0;
	}

	for (int i = 1; i < nameLen; i++) 
	{
		if (name[i] != '_' && 
			(name[i] < 'a' || name[i] > 'z') && 
			(name[i] < 'A' || name[i] > 'Z') &&
			(name[i] < '0' || name[i] > '9'))
		{
			return 0;
		}
	}
	
	for (int i = 0; i < NUM_OF_KEYWORDS; i++) 
	{
		if (strcmp(keywordStrs[i], name) == 0)
		{
			return 0;
		}
	}

	for (int i = 0; i < NUM_OF_PRIMITIVE_TYPES; i++)
	{
		if (strcmp(primitiveTypeStrs[i], name) == 0)
		{
			return 0;
		}
	}

	for (int i = 0; i < NUM_OF_RETURNING_INTRINSICS; i++)
	{
		if (strcmp(returningIntrinsics[i].name, name) == 0)
		{
			return 0;
		}
	}

	for (int i = 0; i < NUM_OF_VOID_INTRINSICS; i++)
	{
		if (strcmp(voidIntrinsics[i].name, name) == 0)
		{
			return 0;
		}
	}

	for (int i = 0; i < NUM_OF_CALLING_CONVENTIONS; i++)
	{
		if (strcmp(callingConventionStrs[i], name) == 0)
		{
			return 0;
		}
	}
	
	for (int i = 0; i < params->numOfFunctions; i++) 
	{
		if (strcmp(params->functions[i].name.buffer, name) == 0) 
		{
			return 0;
		}
	}

	for (int i = 0; i < params->numOfImports; i++)
	{
		if (strcmp(params->imports[i].name.buffer, name) == 0)
		{
			return 0;
		}
	}

	if (params->currentFunc) 
	{
		for (int i = 0; i < params->currentFunc->numOfRegVars; i++)
		{
			if (strcmp(params->currentFunc->regVars[i].name.buffer, name) == 0)
			{
				return 0;
			}
		}

		for (int i = 0; i < params->currentFunc->numOfStackVars; i++)
		{
			if (strcmp(params->currentFunc->stackVars[i].name.buffer, name) == 0)
			{
				return 0;
			}
		}

		for (int i = 0; i < params->currentFunc->numOfReturnedVars; i++)
		{
			if (strcmp(params->currentFunc->returnedVars[i].name.buffer, name) == 0)
			{
				return 0;
			}
		}
	}

	return 1;
}