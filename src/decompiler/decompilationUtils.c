#include "decompilationUtils.h"
#include "functions.h"
#include "functionCalls.h"
#include "intrinsics.h"
#include "../file-handler/fileHandler.h"

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

unsigned long long getJmpDst(struct DecompilationParameters* params, int startInstructionIndex)
{
	struct DisassembledInstruction* instruction = &params->instructions[startInstructionIndex];
	if (!isOpcodeJmp(instruction->opcode) && !isOpcodeJcc(instruction->opcode) && !isOpcodeCall(instruction->opcode))
	{
		return 0;
	}

	unsigned long long dst = 0;
	if (!operandToValue(params, startInstructionIndex, &instruction->operands[0], &dst))
	{
		return 0;
	}

	if (instruction->opcode != JMP_FAR && instruction->opcode != CALL_FAR && instruction->operands[0].type == IMMEDIATE)
	{
		dst += instruction->address + instruction->numOfBytes;
	}

	return dst;
}

static unsigned char operandToValue(struct DecompilationParameters* params, int startInstructionIndex, struct Operand* operand, unsigned long long* result)
{
	if (operand->type == IMMEDIATE)
	{
		*result = operand->immediate.value;
		return 1;
	}
	else if (operand->type == MEM_ADDRESS)
	{
		unsigned long long address = 0;
		if (compareRegisters(operand->memoryAddress.reg, IP)) // this needs to be checked here because of startInstructionIndex - 1 in regToValue call
		{
			address = params->instructions[startInstructionIndex].address + params->instructions[startInstructionIndex].numOfBytes;
		}
		else
		{
			unsigned long long baseRegVal = 0;
			if (!regToValue(params, startInstructionIndex - 1, operand->memoryAddress.reg, &baseRegVal))
			{
				return 0;
			}

			address = baseRegVal;
		}

		address *= operand->memoryAddress.scale;

		if (compareRegisters(operand->memoryAddress.regDisplacement, IP))
		{
			address += params->instructions[startInstructionIndex].address + params->instructions[startInstructionIndex].numOfBytes;
		}
		else if(operand->memoryAddress.regDisplacement != NO_REG)
		{
			unsigned long long displacementRegVal = 0;
			if (!regToValue(params, startInstructionIndex - 1, operand->memoryAddress.regDisplacement, &displacementRegVal))
			{
				return 0;
			}

			address += displacementRegVal;
		}

		address += operand->memoryAddress.constDisplacement;

		if (params->instructions[startInstructionIndex].opcode == LEA || 
			getImportIndexByAddress(params, address) != -1) // the address that I store for imports is the table entry. when decompiling a function call, this table entry address is needed to check for the import
		{
			*result = address;
		}
		else 
		{
			struct FileSection* section = 0;
			unsigned long long fileOffset = rvaToFileOffset(params->sections, params->numOfSections, address - params->imageBase, &section);

			if (fileOffset == 0 || fileOffset >= params->numOfFileBytes || !section || section->type != INIT_DATA_FST || !section->isReadOnly)
			{
				return 0;
			}

			switch (operand->memoryAddress.ptrSize) 
			{
			case 1:
				*result = *(unsigned char*)(params->fileBytes + fileOffset);
				break;
			case 2:
				*result = *(unsigned short*)(params->fileBytes + fileOffset);
				break;
			case 4:
				*result = *(unsigned int*)(params->fileBytes + fileOffset);
				break;
			case 8:
				*result = *(unsigned long long*)(params->fileBytes + fileOffset);
				break;
			}
		}

		return 1;
	}
	else if (operand->type == REGISTER)
	{
		return regToValue(params, startInstructionIndex - 1, operand->reg, result);
	}

	return 0;
}

static unsigned char regToValue(struct DecompilationParameters* params, int startInstructionIndex, enum Register reg, unsigned long long* result)
{
	if (reg == NO_REG)
	{
		return 0;
	}

	if (compareRegisters(reg, IP))
	{
		*result = params->instructions[startInstructionIndex].address + params->instructions[startInstructionIndex].numOfBytes;
		return 1;
	}

	int minInstructionIndex = params->currentFunc ? params->currentFunc->firstInstructionIndex : startInstructionIndex - 0x1000;
	for (int i = startInstructionIndex; i >= minInstructionIndex && i >= 0; i--)
	{
		if ((params->instructions[i].opcode == MOV || params->instructions[i].opcode == LEA) && 
			params->instructions[i].operands[0].type == REGISTER && compareRegisters(params->instructions[i].operands[0].reg, reg))
		{
			int start = i;
			if (params->instructions[i].operands[1].type == REGISTER && compareRegisters(params->instructions[i].operands[0].reg, params->instructions[i].operands[1].reg))
			{
				start--;
			}

			return operandToValue(params, start, &(params->instructions[i].operands[1]), result);
		}
	}

	return 0;
}

unsigned long long resolveJmpChain(struct DecompilationParameters* params, int startInstructionIndex)
{
	unsigned long long jmpAddress = getJmpDst(params, startInstructionIndex);
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

unsigned char getJumpTable(struct DecompilationParameters* params, int instructionIndex, struct JumpTable* result)
{
	struct DisassembledInstruction* jmpInstruction = &params->instructions[instructionIndex];

	if (jmpInstruction->opcode != JMP_NEAR)
	{
		return 0;
	}

	if (jmpInstruction->operands[0].type == REGISTER && instructionIndex > 0)
	{
		enum Register targetReg = jmpInstruction->operands[0].reg;
		for(int i = instructionIndex - 1; i >= 0; i--)
		{
			struct DisassembledInstruction* instruction = &params->instructions[i];
			
			// the actual value of targetReg will be an address of some instruction to jmp to, but this code is just looking for the address of the jmp table and not the value of targetReg
			if (instruction->opcode == MOV &&
				instruction->operands[0].type == REGISTER && compareRegisters(targetReg, instruction->operands[0].reg) &&
				instruction->operands[1].type == MEM_ADDRESS && instruction->operands[1].memoryAddress.scale > 1)
			{
				unsigned long long jmpTableAddress = instruction->operands[1].memoryAddress.constDisplacement;

				unsigned long long regDisplacementVal = 0;
				if (!regToValue(params, i, instruction->operands[1].memoryAddress.regDisplacement, &regDisplacementVal))
				{
					return 0;
				}

				jmpTableAddress += regDisplacementVal;

				result->addressSize = instruction->operands[1].memoryAddress.ptrSize;
				result->jmpTableAddress = jmpTableAddress;
				result->jmpInstructionAddress = jmpInstruction->address;

				// checking for indirect table
				struct DisassembledInstruction* prevInstruction = &params->instructions[i - 1];
				if (prevInstruction->opcode == MOVZX &&
					prevInstruction->operands[0].type == REGISTER && compareRegisters(instruction->operands[1].memoryAddress.reg, prevInstruction->operands[0].reg) &&
					prevInstruction->operands[1].type == MEM_ADDRESS)
				{
					unsigned long long indirectTableAddress = prevInstruction->operands[1].memoryAddress.constDisplacement;

					regDisplacementVal = 0;
					if (!regToValue(params, i - 1, prevInstruction->operands[1].memoryAddress.regDisplacement, &regDisplacementVal))
					{
						return 0;
					}

					indirectTableAddress += regDisplacementVal;

					result->indirectTableAddress = indirectTableAddress;
				}
				else
				{
					result->indirectTableAddress = 0;
				}

				return 1;
			}
		}
	}
	else if (jmpInstruction->operands[0].type == MEM_ADDRESS && jmpInstruction->operands[0].memoryAddress.scale > 1)
	{
		unsigned long long jmpTableAddress = jmpInstruction->operands[0].memoryAddress.constDisplacement;

		unsigned long long regDisplacementVal = 0;
		if (!regToValue(params, instructionIndex, jmpInstruction->operands[0].memoryAddress.regDisplacement, &regDisplacementVal))
		{
			return 0;
		}

		jmpTableAddress += regDisplacementVal;

		result->addressSize = jmpInstruction->operands[0].memoryAddress.ptrSize;
		result->jmpTableAddress = jmpTableAddress;
		result->jmpInstructionAddress = jmpInstruction->address;
		result->indirectTableAddress = 0; // not sure if should check for this here too

		return 1;
	}

	return 0;
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

unsigned char doesInstructionModifyOperand(struct DisassembledInstruction* instruction, unsigned char operandNum, unsigned char* overwrites)
{
	if (overwrites != 0)
	{
		*overwrites = 0;
	}

	if (instruction->group1Prefix != NO_PREFIX) // REPZ instructions are decompiled as void intrinsics and I have not handled the other prefixes yet
	{
		return 0;
	}

	enum Mnemonic opcode = instruction->opcode;

	if (operandNum == 0)
	{
		if ((isOpcodeXor(opcode) || opcode == SBB) && compareOperands(&instruction->operands[0], &instruction->operands[1]))
		{
			if (overwrites != 0) { *overwrites = 1; }
			return 1;
		}
		else if (isOpcodeOr(opcode) && instruction->operands[1].type == IMMEDIATE)
		{
			if (overwrites != 0) { *overwrites = isImmediateAllOnes(&instruction->operands[1].immediate); }
			return 1;
		}

		if (opcode == IMUL)
		{
			if (instruction->numOfOperands == 3)
			{
				if (overwrites != 0)
				{
					*overwrites = !compareOperands(&instruction->operands[0], &instruction->operands[1]);
				}
				return 1;
			}
			else if (instruction->numOfOperands == 2)
			{
				return 1;
			}
			else if (instruction->numOfOperands == 1)
			{
				return 0;
			}
		}

		if (doesOpcodeOverwriteFirstOperand(opcode))
		{
			if (overwrites != 0) { *overwrites = 1; }
			return 1;
		}
		else if (doesOpcodeModifyFirstOperand(opcode))
		{
			return 1;
		}
	}
	else if (operandNum == 1)
	{
		if (opcode == XCHG)
		{
			if (overwrites != 0) { *overwrites = 1; }
			return 1;
		}
		else if (isOpcodeXor(opcode) || opcode == SBB)
		{
			if (compareOperands(&instruction->operands[0], &instruction->operands[1]))
			{
				if (overwrites != 0) { *overwrites = 1; }
				return 1;
			}
			return 0;
		}
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

unsigned char doesInstructionModifyZF(struct DisassembledInstruction* instruction)
{
	return !isOpcodeMov(instruction->opcode) && instruction->opcode != LEA && doesInstructionModifyOperand(instruction, 0, 0); // this isn't a full check
}

unsigned char doesInstructionDoNothing(struct DisassembledInstruction* instruction)
{
	if (instruction->opcode == NOP)
	{
		return 1;
	}
	else if (isOpcodeMov(instruction->opcode) && compareOperands(&instruction->operands[0], &instruction->operands[1]))
	{
		return 1;
	}
	else if (instruction->opcode == LEA &&
		instruction->operands[0].type == REGISTER && instruction->operands[1].type == MEM_ADDRESS &&
		compareRegisters(instruction->operands[0].reg, instruction->operands[1].memoryAddress.reg) &&
		instruction->operands[1].memoryAddress.constDisplacement == 0 && instruction->operands[1].memoryAddress.regDisplacement == NO_REG && instruction->operands[1].memoryAddress.scale == 1)
	{
		return 1;
	}
	else if ((isOpcodeAdd(instruction->opcode) || isOpcodeSub(instruction->opcode)) && instruction->operands[1].type == IMMEDIATE && instruction->operands[1].immediate.value == 0)
	{
		return 1;
	}
	else if ((instruction->opcode == JMP_NEAR || instruction->opcode == JMP_SHORT) && instruction->operands[0].type == IMMEDIATE && instruction->operands[0].immediate.value == 0)
	{
		return 1;
	}

	return 0;
}

unsigned char doesInstructionGenerateInterruptOrException(struct DisassembledInstruction* instruction)
{
	switch (instruction->opcode)
	{
	case INT3:
	case _INT:
	case HLT:
	case UD0:
	case UD1:
	case UD2:
	case DATA:
		return 1;
	}

	return instruction->isInvalid;
}

unsigned char isImmediateAllOnes(struct Immediate* immediate)
{
	switch (immediate->size)
	{
	case 1:
		return immediate->value == 0xFF;
	case 2:
		return immediate->value == 0xFFFF;
	case 4:
		return immediate->value == 0xFFFFFFFF;
	case 8:
		return immediate->value == 0xFFFFFFFFFFFFFFFF;
	}

	return 0;
}

unsigned char compareOperands(struct Operand* op1, struct Operand* op2)
{
	if (op1->type == op2->type)
	{
		struct MemoryAddress* m1 = &op1->memoryAddress;
		struct MemoryAddress* m2 = &op2->memoryAddress;

		switch (op1->type)
		{
		case NO_OPERAND:
			return 1;
		case SEGMENT:
			return op1->segment == op2->segment;
		case REGISTER:
			return op1->reg == op2->reg;
		case MEM_ADDRESS:
			return m1->reg == m2->reg && m1->constDisplacement == m2->constDisplacement && m1->ptrSize == m2->ptrSize && m1->scale == m2->scale && m1->constSegment == m2->constSegment && m1->regDisplacement == m2->regDisplacement && m1->segment == m2->segment;
		case IMMEDIATE:
			return op1->immediate.value == op2->immediate.value;
		}
	}

	return 0;
}

unsigned char getSizeOfOperand(struct Operand* operand)
{
	if (operand->type == MEM_ADDRESS)
	{
		return operand->memoryAddress.ptrSize;
	}
	else if (operand->type == REGISTER)
	{
		return getSizeOfRegister(operand->reg);
	}
	else if (operand->type == IMMEDIATE)
	{
		return operand->immediate.size;
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

unsigned char checkRegVarScope(struct RegisterVariable* regVar, int instructionIndex, unsigned char isRegDst)
{
	for (int i = 0; i < regVar->numOfScopes; i++)
	{
		if (isRegDst)
		{
			if (instructionIndex >= regVar->scopes[i].startIndex && instructionIndex < regVar->scopes[i].endIndex)
			{
				return 1;
			}
		}
		else
		{
			if (instructionIndex > regVar->scopes[i].startIndex && instructionIndex <= regVar->scopes[i].endIndex)
			{
				return 1;
			}
		}
	}

	return 0;
}