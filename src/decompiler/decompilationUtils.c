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

enum JdcStatus addDecompiledLine(struct DecompilationParameters* params, struct JdcStr* decompiledFunction, int32_t associatedInstruction, const char* format, ...)
{
	for (int32_t i = 0; i < params->numOfIndents; i++)
	{
		strcatJdc(decompiledFunction, "\t");
	}

	va_list args;
	va_start(args, format);
	enum JdcStatus result = sprintfJdcArgs(decompiledFunction, true, format, args);
	va_end(args);

	strcatJdc(decompiledFunction, "\n");

	addAssociatedInstruction(params->currentFunc, associatedInstruction);
	params->currentFunc->numOfLines++;
	return result;
}

uint64_t getJmpDst(struct DecompilationParameters* params, int32_t startInstructionIndex)
{
	struct DisassembledInstruction* instruction = &params->instructions[startInstructionIndex];
	if (!isOpcodeJmp(instruction->opcode) && !isOpcodeJcc(instruction->opcode) && !isOpcodeCall(instruction->opcode))
	{
		return 0;
	}

	uint64_t dst = 0;
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

static enum JdcStatus operandToValue(struct DecompilationParameters* params, int32_t startInstructionIndex, struct Operand* operand, uint64_t* result)
{
	if (operand->type == IMMEDIATE)
	{
		*result = operand->immediate.value;
		return SUCCESS_JDC;
	}
	else if (operand->type == MEM_ADDRESS)
	{
		uint64_t address = 0;
		if (compareRegisters(operand->memoryAddress.reg, IP)) // this needs to be checked here because of startInstructionIndex - 1 in regToValue call
		{
			address = params->instructions[startInstructionIndex].address + params->instructions[startInstructionIndex].numOfBytes;
		}
		else if(operand->memoryAddress.reg != NO_REG)
		{
			uint64_t baseRegVal = 0;
			if (ERROR_JDC == regToValue(params, startInstructionIndex - 1, operand->memoryAddress.reg, &baseRegVal))
			{
				return ERROR_JDC;
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
			uint64_t displacementRegVal = 0;
			if (ERROR_JDC == regToValue(params, startInstructionIndex - 1, operand->memoryAddress.regDisplacement, &displacementRegVal))
			{
				return ERROR_JDC;
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
			uint64_t fileOffset = rvaToFileOffset(params->sections, params->numOfSections, address - params->imageBase, &section);

			if (fileOffset == 0 || fileOffset >= params->numOfFileBytes || !section || section->type != INIT_DATA_FST || !section->isReadOnly)
			{
				return ERROR_JDC;
			}

			switch (operand->memoryAddress.ptrSize) 
			{
			case 1:
				*result = *(uint8_t*)(params->fileBytes + fileOffset);
				break;
			case 2:
				*result = *(uint16_t*)(params->fileBytes + fileOffset);
				break;
			case 4:
				*result = *(uint32_t*)(params->fileBytes + fileOffset);
				break;
			case 8:
				*result = *(uint64_t*)(params->fileBytes + fileOffset);
				break;
			}
		}

		return SUCCESS_JDC;
	}
	else if (operand->type == REGISTER)
	{
		return regToValue(params, startInstructionIndex - 1, operand->reg, result);
	}

	return ERROR_JDC;
}

static enum JdcStatus regToValue(struct DecompilationParameters* params, int32_t startInstructionIndex, enum Register reg, uint64_t* result)
{
	if (reg == NO_REG)
	{
		return ERROR_JDC;
	}

	if (compareRegisters(reg, IP))
	{
		*result = params->instructions[startInstructionIndex].address + params->instructions[startInstructionIndex].numOfBytes;
		return SUCCESS_JDC;
	}

	int32_t minInstructionIndex = params->currentFunc ? params->currentFunc->firstInstructionIndex : startInstructionIndex - 0x1000;
	for (int32_t i = startInstructionIndex; i >= minInstructionIndex && i >= 0; i--)
	{
		if ((params->instructions[i].opcode == MOV || params->instructions[i].opcode == LEA) && 
			params->instructions[i].operands[0].type == REGISTER && compareRegisters(params->instructions[i].operands[0].reg, reg))
		{
			int32_t start = i;
			if (params->instructions[i].operands[1].type == REGISTER && compareRegisters(params->instructions[i].operands[0].reg, params->instructions[i].operands[1].reg))
			{
				start--;
			}

			return operandToValue(params, start, &(params->instructions[i].operands[1]), result);
		}
	}

	return ERROR_JDC;
}

uint64_t resolveJmpChain(struct DecompilationParameters* params, int32_t startInstructionIndex)
{
	uint64_t jmpAddress = getJmpDst(params, startInstructionIndex);
	if (jmpAddress == 0)
	{
		return 0;
	}

	int32_t instructionIndex = findInstructionByAddress(params->instructions, params->numOfInstructions, jmpAddress);
	if (instructionIndex != -1)
	{
		struct DisassembledInstruction* jmpInstruction = &(params->instructions[instructionIndex]);
		if (instructionIndex != startInstructionIndex && isOpcodeJmp(jmpInstruction->opcode))
		{
			uint64_t nextJmpAddress = resolveJmpChain(params, instructionIndex);
			if (nextJmpAddress != 0) 
			{
				return nextJmpAddress;
			}
		}
	}

	return jmpAddress;
}

enum JdcStatus getJumpTable(struct DecompilationParameters* params, int32_t instructionIndex, struct JumpTable* result)
{
	struct DisassembledInstruction* jmpInstruction = &params->instructions[instructionIndex];

	if (jmpInstruction->opcode != JMP_NEAR)
	{
		return ERROR_JDC;
	}

	if (jmpInstruction->operands[0].type == REGISTER && instructionIndex > 0)
	{
		enum Register targetReg = jmpInstruction->operands[0].reg;
		for(int32_t i = instructionIndex - 1; i >= 0; i--)
		{
			struct DisassembledInstruction* instruction = &params->instructions[i];
			
			// the actual value of targetReg will be an address of some instruction to jmp to, but this code is just looking for the address of the jmp table and not the value of targetReg
			if (instruction->opcode == MOV &&
				instruction->operands[0].type == REGISTER && compareRegisters(targetReg, instruction->operands[0].reg) &&
				instruction->operands[1].type == MEM_ADDRESS && instruction->operands[1].memoryAddress.scale > 1)
			{
				uint64_t jmpTableAddress = instruction->operands[1].memoryAddress.constDisplacement;

				uint64_t regDisplacementVal = 0;
				if (ERROR_JDC == regToValue(params, i, instruction->operands[1].memoryAddress.regDisplacement, &regDisplacementVal))
				{
					return ERROR_JDC;
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
					uint64_t indirectTableAddress = prevInstruction->operands[1].memoryAddress.constDisplacement;

					regDisplacementVal = 0;
					if (ERROR_JDC == regToValue(params, i - 1, prevInstruction->operands[1].memoryAddress.regDisplacement, &regDisplacementVal))
					{
						return ERROR_JDC;
					}

					indirectTableAddress += regDisplacementVal;

					result->indirectTableAddress = indirectTableAddress;
				}
				else
				{
					result->indirectTableAddress = 0;
				}

				return SUCCESS_JDC;
			}
		}
	}
	else if (jmpInstruction->operands[0].type == MEM_ADDRESS && jmpInstruction->operands[0].memoryAddress.scale > 1)
	{
		uint64_t jmpTableAddress = jmpInstruction->operands[0].memoryAddress.constDisplacement;

		uint64_t regDisplacementVal = 0;
		if (ERROR_JDC == regToValue(params, instructionIndex, jmpInstruction->operands[0].memoryAddress.regDisplacement, &regDisplacementVal))
		{
			return ERROR_JDC;
		}

		jmpTableAddress += regDisplacementVal;

		result->addressSize = jmpInstruction->operands[0].memoryAddress.ptrSize;
		result->jmpTableAddress = jmpTableAddress;
		result->jmpInstructionAddress = jmpInstruction->address;
		result->indirectTableAddress = 0; // not sure if should check for this here too

		return SUCCESS_JDC;
	}

	return ERROR_JDC;
}

int32_t findInstructionByAddress(struct DisassembledInstruction* instructions, int32_t numOfInstructions, uint64_t address)
{
	int32_t low = 0;
	int32_t high = numOfInstructions - 1;
	while (low <= high)
	{
		int32_t mid = low + (high - low) / 2;

		if (instructions[mid].address == address) { return mid; }

		if (instructions[mid].address < address) { low = mid + 1; }
		else { high = mid - 1; }
	}

	return -1;
}

int32_t findInstructionByAddressInclusive(struct DisassembledInstruction* instructions, int32_t numOfInstructions, uint64_t address)
{
	int32_t low = 0;
	int32_t high = numOfInstructions - 1;
	while (low <= high)
	{
		int32_t mid = low + (high - low) / 2;

		if (instructions[mid].address <= address && instructions[mid].address + instructions[mid].numOfBytes > address) { return mid; }

		if (instructions[mid].address < address) { low = mid + 1; }
		else { high = mid - 1; }
	}

	return -1;
}

int32_t findInstructionInsertPoint(struct DisassembledInstruction* instructions, int32_t numOfInstructions, uint64_t address)
{
	int32_t low = 0;
	int32_t high = numOfInstructions - 1;
	while (low <= high)
	{
		int32_t mid = low + (high - low) / 2;

		if (instructions[mid].address < address && (mid == high || instructions[mid + 1].address > address)) { return mid + 1; }

		if (instructions[mid].address < address) { low = mid + 1; }
		else { high = mid - 1; }
	}

	return 0;
}

int32_t findAddressInArr(uint64_t* addresses, int32_t numOfAddresses, uint64_t address)
{
	int32_t low = 0;
	int32_t high = numOfAddresses - 1;
	while (low <= high)
	{
		int32_t mid = low + (high - low) / 2;

		if (addresses[mid] == address) { return mid; }

		if (addresses[mid] < address) { low = mid + 1; }
		else { high = mid - 1; }
	}

	return -1;
}

int32_t findJumpTableByAddress(struct JumpTable* jumpTables, int32_t numOfJumpTables, uint64_t address, bool* foundIndirectTable)
{
	int32_t low = 0;
	int32_t high = numOfJumpTables - 1;
	while (low <= high)
	{
		int32_t mid = low + (high - low) / 2;

		if (jumpTables[mid].jmpTableAddress == address) 
		{ 
			if (foundIndirectTable) { *foundIndirectTable = false; }
			return mid; 
		}
		else if (jumpTables[mid].indirectTableAddress == address)
		{
			if (foundIndirectTable) { *foundIndirectTable = true; }
			return mid;
		}

		if (jumpTables[mid].jmpTableAddress < address) { low = mid + 1; }
		else { high = mid - 1; }
	}

	return -1;
}

bool checkForAddressInArrInRange(uint64_t* addresses, int32_t numOfAddresses, uint64_t minAddress, uint64_t maxAddress)
{
	int32_t low = 0;
	int32_t high = numOfAddresses - 1;
	while (low <= high)
	{
		int32_t mid = low + (high - low) / 2;

		if (addresses[mid] >= minAddress && addresses[mid] <= maxAddress) { return true; }

		if (addresses[mid] < minAddress) { low = mid + 1; }
		else { high = mid - 1; }
	}

	return false;
}

bool doesInstructionModifyOperand(struct DecompilationParameters* params, int32_t instructionIndex, uint8_t operandNum, bool* overwrites)
{
	if (overwrites != 0)
	{
		*overwrites = false;
	}

	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];

	if (instruction->group1Prefix != NO_PREFIX) // REPZ instructions are decompiled as void intrinsics and I have not handled the other prefixes yet
	{
		return false;
	}

	enum Mnemonic opcode = instruction->opcode;

	if (operandNum == 0)
	{
		if ((isOpcodeXor(opcode) || opcode == SBB) && compareOperands(&instruction->operands[0], &instruction->operands[1]))
		{
			if (overwrites != 0) { *overwrites = true; }
			return true;
		}
		else if (isOpcodeOr(opcode) && instruction->operands[1].type == IMMEDIATE)
		{
			if (overwrites != 0) { *overwrites = isImmediateAllOnes(&instruction->operands[1].immediate); }
			return true;
		}

		if (opcode == IMUL)
		{
			if (instruction->numOfOperands == 3)
			{
				if (overwrites != 0)
				{
					*overwrites = !compareOperands(&instruction->operands[0], &instruction->operands[1]);
				}

				return true;
			}
			else if (instruction->numOfOperands == 2)
			{
				return true;
			}
			else if (instruction->numOfOperands == 1)
			{
				return false;
			}
		}

		if (doesOpcodeOverwriteFirstOperand(opcode))
		{
			if (overwrites != 0) { *overwrites = true; }
			return true;
		}
		else if (doesOpcodeModifyFirstOperand(opcode))
		{
			return true;
		}
	}
	else if (operandNum == 1)
	{
		if (opcode == XCHG)
		{
			if (overwrites != 0) { *overwrites = true; }
			return true;
		}
		else if (isOpcodeXor(opcode) || opcode == SBB)
		{
			if (compareOperands(&instruction->operands[0], &instruction->operands[1]))
			{
				if (overwrites != 0) { *overwrites = true; }
				return true;
			}

			return false;
		}
	}

	return false;
}

bool doesInstructionAccessRegister(struct DecompilationParameters* params, int32_t instructionIndex, enum Register reg, bool checkUnknownCalls, enum Register* specificReg)
{
	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];
	enum Mnemonic opcode = instruction->opcode;
	
	if (isRegisterStatusFlag(reg)) 
	{
		switch (opcode) 
		{
		case JA_SHORT:
		case JBE_SHORT:
		case CMOVA:
		case CMOVBE:
		case SETA:
		case SETBE:
			return reg == CF || reg == ZF;
		case JB_SHORT:
		case JNB_SHORT:
		case CMOVB:
		case CMOVNB:
		case SETB:
		case SETNB:
		case ADC:
		case SBB:
			return reg == CF;
		case JG_SHORT:
		case JLE_SHORT:
		case CMOVG:
		case CMOVLE:
		case SETG:
		case SETLE:
			return reg == ZF || reg == SF || reg == OF;
		case JGE_SHORT:
		case JL_SHORT: 
		case CMOVGE:
		case CMOVL:
		case SETL:
			return reg == SF || reg == OF;
		case JS_SHORT:
		case JNS_SHORT:
		case CMOVS:
		case CMOVNS:
		case SETS:
		case SETNS:
			return reg == SF;
		case JNO_SHORT:
		case JO_SHORT:
		case CMOVNO:
		case CMOVO:
		case SETNO:
		case SETO:
			return reg == OF;
		case JNP_SHORT:
		case JP_SHORT:
		case CMOVNP:
		case CMOVP:
		case SETNP:
		case SETP:
			return reg == PF;
		case JNZ_SHORT:
		case JZ_SHORT:
		case CMOVNZ:
		case CMOVZ:
		case SETNZ:
		case SETZ:
			return reg == ZF;
		}

		return false;
	}
	
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

			return true;
		}
	}
	else if (checkUnknownCalls && checkForUnknownFunctionCall(params, instructionIndex))
	{
		int32_t numOfPlatformRegArgs = getNumOfPlatformRegArgs(params->fileFormat);
		const enum Register* platformRegArgs = getPlatformRegArgs(params->fileFormat);
		for (int32_t i = 0; i < numOfPlatformRegArgs; i++)
		{
			if (compareRegisters(reg, platformRegArgs[i])) 
			{
				if (specificReg)
				{
					*specificReg = reg;
				}

				return true;
			}
		}
	}
	
	for (int32_t i = 0; i < instruction->numOfOperands; i++)
	{
		bool overwrites = false;
		struct Operand* op = &(instruction->operands[i]);
		if (op->type == MEM_ADDRESS)
		{
			if (compareRegisters(op->memoryAddress.reg, reg))
			{
				if (specificReg)
				{
					*specificReg = op->memoryAddress.reg;
				}

				return true;
			}
			else if (compareRegisters(op->memoryAddress.regDisplacement, reg))
			{
				if (specificReg)
				{
					*specificReg = op->memoryAddress.regDisplacement;
				}

				return true;
			}
		}
		else if ((!doesInstructionModifyOperand(params, instructionIndex, i, &overwrites) || !overwrites) && op->type == REGISTER && compareRegisters(op->reg, reg))
		{
			if (specificReg)
			{
				*specificReg = op->reg;
			}

			return true;
		}
	}

	return false;
}

bool doesInstructionModifyRegister(struct DecompilationParameters* params, int32_t instructionIndex, enum Register reg, enum Register* specificReg, bool* overwrites)
{
	if (specificReg) { *specificReg = NO_REG; }
	if (overwrites) { *overwrites = false; }

	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];
	enum Mnemonic opcode = instruction->opcode;

	if (isRegisterStatusFlag(reg)) 
	{
		if (overwrites) { *overwrites = true; }

		if (isOpcodeCmp(opcode))
		{
			return true;
		}

		switch (opcode)
		{
		case ADD:
		case ADC:
		case SUB:
		case SBB:
		case NEG:
		case CMPXCHG:
			return true;
		case TEST:
		case AND:
		case OR:
		case XOR:
			return reg != AF;
		case BT:
		case BTS:
		case BTR:
			return reg == CF;
		case INC:
		case DEC:
			return reg != CF;
		}

		if (overwrites) { *overwrites = false; }
		return false;
	}
	
	struct Function* callee = 0;
	if ((checkForKnownFunctionCall(params, instructionIndex, &callee) && callee && compareRegisters(callee->returnReg, reg)) ||
		(checkForUnknownFunctionCall(params, instructionIndex) && compareRegisters(reg, AX)))
	{
		if (overwrites) { *overwrites = true; }
		if (specificReg) 
		{ 
			if (callee) { *specificReg = callee->returnReg; }
			else { *specificReg = params->is64Bit ? RAX : EAX; }
		}

		return true;
	}

	if (compareRegisters(reg, AX)) // some opcodes may modify a register even if it isn't an operand
	{
		if (opcode == CMPXCHG) 
		{
			if (specificReg)
			{
				int32_t size = getSizeOfRegister(instruction->operands[1].reg);
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
			
			return true;
		}
		
		if (opcode == IDIV || opcode == DIV)
		{
			if (specificReg) 
			{
				int32_t size = getSizeOfRegister(instruction->operands[0].reg);
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
			
			return true;
		}

		if ((opcode == IMUL || opcode == MUL) && instruction->numOfOperands == 1)
		{
			if (specificReg)
			{
				int32_t size = getSizeOfRegister(instruction->operands[0].reg);
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

			return true;
		}
	}
	else if (compareRegisters(reg, DX))
	{
		if ((opcode == IMUL || opcode == MUL || opcode == IDIV || opcode == DIV) && instruction->numOfOperands == 1)
		{
			if (specificReg) 
			{ 
				int32_t size = getSizeOfRegister(instruction->operands[0].reg);
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

			if (overwrites) { *overwrites = true; }
			return true;
		}
	}
	else if (compareRegisters(reg, ST0))
	{
		switch (opcode)
		{
		case FLD:
			if (specificReg) { *specificReg = ST0; }
			if (overwrites) { *overwrites = true; }
			return true;
		}
	}

	for (int32_t i = 0; i < instruction->numOfOperands; i++)
	{
		struct Operand* op = &(instruction->operands[i]);
		if (op->type == REGISTER && compareRegisters(op->reg, reg))
		{
			if (doesInstructionModifyOperand(params, instructionIndex, i, overwrites))
			{
				if (specificReg) { *specificReg = op->reg; }
				return true;
			}
		}
	}

	return false;
}

bool doesInstructionConditionallyModifyRegister(struct DecompilationParameters* params, int32_t instructionIndex, enum Register reg) 
{
	struct DisassembledInstruction* instruction = &params->instructions[instructionIndex];
	enum Mnemonic opcode = instruction->opcode;

	// CMOVcc is not checked here because it is decompiled as a ternary operator, so its dst is always set to something

	if (compareRegisters(reg, AX))
	{
		return opcode == CMPXCHG;
	}

	return false;
}

bool doesInstructionDoNothing(struct DisassembledInstruction* instruction)
{
	if (instruction->opcode == NOP)
	{
		return true;
	}
	else if (isOpcodeMov(instruction->opcode) && compareOperands(&instruction->operands[0], &instruction->operands[1]))
	{
		return true;
	}
	else if (instruction->opcode == LEA &&
		instruction->operands[0].type == REGISTER && instruction->operands[1].type == MEM_ADDRESS &&
		compareRegisters(instruction->operands[0].reg, instruction->operands[1].memoryAddress.reg) &&
		instruction->operands[1].memoryAddress.constDisplacement == 0 && instruction->operands[1].memoryAddress.regDisplacement == NO_REG && instruction->operands[1].memoryAddress.scale == 1)
	{
		return true;
	}
	else if ((isOpcodeAdd(instruction->opcode) || isOpcodeSub(instruction->opcode)) && instruction->operands[1].type == IMMEDIATE && instruction->operands[1].immediate.value == 0)
	{
		return true;
	}
	else if ((instruction->opcode == JMP_NEAR || instruction->opcode == JMP_SHORT) && instruction->operands[0].type == IMMEDIATE && instruction->operands[0].immediate.value == 0)
	{
		return true;
	}

	return false;
}

bool doesInstructionGenerateInterruptOrException(struct DisassembledInstruction* instruction)
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
		return true;
	}

	return instruction->isInvalid;
}

bool isImmediateAllOnes(struct Immediate* immediate)
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

	return false;
}

bool compareOperands(struct Operand* op1, struct Operand* op2)
{
	if (op1->type == op2->type)
	{
		struct MemoryAddress* m1 = &op1->memoryAddress;
		struct MemoryAddress* m2 = &op2->memoryAddress;

		switch (op1->type)
		{
		case NO_OPERAND:
			return true;
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

	return false;
}

uint8_t getSizeOfOperand(struct Operand* operand)
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

bool validateName(struct DecompilationParameters* params, const char* name) 
{
	size_t nameLen = strlen(name);
	if (nameLen == 0) 
	{
		return false;
	}

	if (name[0] != '_' && (name[0] < 'a' || name[0] > 'z') && (name[0] < 'A' || name[0] > 'Z')) // first character cannot be a number
	{
		return false;
	}

	for (int32_t i = 1; i < nameLen; i++) 
	{
		if (name[i] != '_' && 
			(name[i] < 'a' || name[i] > 'z') && 
			(name[i] < 'A' || name[i] > 'Z') &&
			(name[i] < '0' || name[i] > '9'))
		{
			return false;
		}
	}
	
	for (int32_t i = 0; i < NUM_OF_KEYWORDS; i++) 
	{
		if (strcmp(keywordStrs[i], name) == 0)
		{
			return false;
		}
	}

	for (int32_t i = 0; i < NUM_OF_PRIMITIVE_TYPES; i++)
	{
		if (strcmp(primitiveTypeStrs[i], name) == 0)
		{
			return false;
		}
	}

	for (int32_t i = 0; i < NUM_OF_RETURNING_INTRINSICS; i++)
	{
		if (strcmp(returningIntrinsics[i].name, name) == 0)
		{
			return false;
		}
	}

	for (int32_t i = 0; i < NUM_OF_VOID_INTRINSICS; i++)
	{
		if (strcmp(voidIntrinsics[i].name, name) == 0)
		{
			return false;
		}
	}

	for (int32_t i = 0; i < NUM_OF_CALLING_CONVENTIONS; i++)
	{
		if (strcmp(callingConventionStrs[i], name) == 0)
		{
			return false;
		}
	}
	
	for (int32_t i = 0; i < params->numOfFunctions; i++) 
	{
		if (strcmp(params->functions[i].name.buffer, name) == 0) 
		{
			return false;
		}
	}

	for (int32_t i = 0; i < params->numOfImports; i++)
	{
		if (strcmp(params->imports[i].name.buffer, name) == 0)
		{
			return false;
		}
	}

	if (params->currentFunc) 
	{
		for (int32_t i = 0; i < params->currentFunc->numOfRegVars; i++)
		{
			if (strcmp(params->currentFunc->regVars[i].name.buffer, name) == 0)
			{
				return false;
			}
		}

		for (int32_t i = 0; i < params->currentFunc->numOfStackVars; i++)
		{
			if (strcmp(params->currentFunc->stackVars[i].name.buffer, name) == 0)
			{
				return false;
			}
		}

		for (int32_t i = 0; i < params->currentFunc->numOfReturnedVars; i++)
		{
			if (strcmp(params->currentFunc->returnedVars[i].name.buffer, name) == 0)
			{
				return false;
			}
		}
	}

	return true;
}

bool checkRegVarScope(struct DecompilationParameters* params, struct RegisterVariable* regVar, int32_t instructionIndex)
{
	// the scope startIndex is when the reg is initialized, so the reg var needs to be decompiled when it is the dst but not as a src
	// the scope endIndex is the last time the reg is accessed before it is initialized again, so as a src it still needs to be the reg var but not as the dst if the instruction also initializes the reg
	bool isRegOverwritten = 0;
	doesInstructionModifyRegister(params, instructionIndex, regVar->reg, 0, &isRegOverwritten);
	
	for (int32_t i = 0; i < regVar->numOfScopes; i++)
	{
		if (isRegOverwritten)
		{
			if (instructionIndex >= regVar->scopes[i].startIndex && instructionIndex < regVar->scopes[i].endIndex)
			{
				return true;
			}
		}
		else
		{
			if (instructionIndex > regVar->scopes[i].startIndex && instructionIndex <= regVar->scopes[i].endIndex)
			{
				return true;
			}
		}
	}

	return false;
}