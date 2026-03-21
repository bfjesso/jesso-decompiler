#include "decompiler.h"
#include "decompilationUtils.h"
#include "functions.h"
#include "expressions.h"
#include "assignment.h"
#include "returnStatements.h"
#include "functionCalls.h"
#include "directJmps.h"
#include "dataTypes.h"
#include "../disassembler/registers.h"

const char* indent = "    ";

unsigned char decompileFunction(struct DecompilationParameters params, struct JdcStr* result)
{
	struct Condition* conditions = (struct Condition*)calloc(20, sizeof(struct Condition));
	int numOfConditions = getAllConditions(params, &conditions, 20);
	if (numOfConditions == -1) 
	{
		free(conditions);
		return 0;
	}

	struct DirectJmp* directJmps = (struct DirectJmp*)calloc(20, sizeof(struct DirectJmp));
	int numOfDirectJmps = getAllDirectJmps(params, conditions, numOfConditions, &directJmps, 20);
	if (numOfDirectJmps == -1)
	{
		free(conditions);
		free(directJmps);
		return 0;
	}
	
	if (!params.currentFunc->hasGottenLocalVars)
	{
		if (!getAllRegVars(params, conditions, numOfConditions))
		{
			free(conditions);
			free(directJmps);
			return 0;
		}

		if (!getAllReturnedVars(params))
		{
			free(conditions);
			free(directJmps);
			return 0;
		}

		params.currentFunc->hasGottenLocalVars = 1;
	}
	
	if (!generateFunctionHeader(params.currentFunc, result))
	{
		free(conditions);
		free(directJmps);
		return 0;
	}

	strcatJdc(result, "{\n");

	if (params.currentFunc->numOfStackVars > 0 || params.currentFunc->numOfReturnedVars > 0 || params.currentFunc->numOfRegVars > 0)
	{
		if (!declareAllLocalVariables(params, result))
		{
			free(conditions);
			free(directJmps);
			return 0;
		}
	}

	// used with jump in decomp
	int originalIndex = -1; 
	int originalNumOfIndents = -1; // so it returns to same level of indents

	unsigned char numOfIndents = 1;
	int indexToJumpTo = -1; // if looking at instructions after a ret or jmp
	for (int i = 0; i < params.currentFunc->numOfInstructions; i++)
	{
		params.startInstructionIndex = i;

		struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[i]);

		// checking for end of condition
		for (int j = 0; j < numOfConditions; j++) 
		{
			if (!conditions[j].requiresJumpInDecomp && !conditions[j].isCombinedByOther && i == conditions[j].dstIndex)
			{
				if (conditions[j].conditionType == DO_WHILE_CT) 
				{
					addIndents(result, numOfIndents);
					strcatJdc(result, "do\n");
					addIndents(result, numOfIndents);
					strcatJdc(result, "{\n");
					numOfIndents++;
				}
				else 
				{
					numOfIndents--;

					addIndents(result, numOfIndents);
					strcatJdc(result, "}\n");

					indexToJumpTo = -1;
				}
			}
		}

		for (int j = 0; j < numOfDirectJmps; j++) 
		{
			if (i == directJmps[j].dstIndex && directJmps[j].type == GO_TO_DJT) 
			{
				sprintfJdc(result, 1, "label_%llX:\n", params.currentFunc->instructions[directJmps[j].dstIndex].address - params.imageBase);
				break;
			}
			else if (i == directJmps[j].jmpIndex) 
			{
				addIndents(result, numOfIndents);

				switch (directJmps[j].type) 
				{
				case GO_TO_DJT:
					sprintfJdc(result, 1, "goto label_%llX;\n", params.currentFunc->instructions[directJmps[j].dstIndex].address - params.imageBase);
					break;
				case CONTINUE_DJT:
					sprintfJdc(result, 1, "continue;\n");
					break;
				case BREAK_DJT:
					sprintfJdc(result, 1, "break;\n");
					break;
				}
				
				break;
			}
		}

		int conditionIndex = checkForCondition(i, conditions, numOfConditions);
		if (conditionIndex != -1)
		{
			if (conditions[conditionIndex].conditionType == DO_WHILE_CT)
			{
				numOfIndents--;
			}

			addIndents(result, numOfIndents);
			
			if (decompileCondition(params, conditions, conditionIndex, result))
			{
				strcatJdc(result, "\n");

				if (conditions[conditionIndex].conditionType != DO_WHILE_CT)
				{
					addIndents(result, numOfIndents);
					strcatJdc(result, "{\n");

					indexToJumpTo = -1;
					numOfIndents++;
				}
			}
			else
			{
				free(conditions);
				free(directJmps);
				return 0;
			}

			if (conditions[conditionIndex].requiresJumpInDecomp)
			{
				originalIndex = i;
				originalNumOfIndents = numOfIndents;
				i = conditions[conditionIndex].dstIndex - 1;
				params.skipUpperBound = originalIndex;
				params.skipLowerBound = i;
				continue;
			}
		}

		if (i < indexToJumpTo)
		{
			continue;
		}
		else
		{
			indexToJumpTo = -1;
		}

		struct Function* callee;
		int importIndex = checkForImportCall(params);
		if (importIndex != -1)
		{
			addIndents(result, numOfIndents);
			if (decompileImportCall(params, importIndex, result))
			{
				strcatJdc(result, "\n");
			}
			else
			{
				free(conditions);
				free(directJmps);
				return 0;
			}
		}

		if (checkForFunctionCall(params, &callee))
		{
			addIndents(result, numOfIndents);
			if (decompileFunctionCall(params, callee, result))
			{
				strcatJdc(result, "\n");
			}
			else
			{
				free(conditions);
				free(directJmps);
				return 0;
			}
		}

		if (checkForAssignment(params))
		{
			addIndents(result, numOfIndents);
			if (decompileAssignment(params, result))
			{
				strcatJdc(result, ";\n");
			}
			else
			{
				free(conditions);
				free(directJmps);
				return 0;
			}
		}

		if (checkForReturnStatement(params))
		{
			addIndents(result, numOfIndents);
			if(currentInstruction->opcode != JMP_FAR && currentInstruction->opcode != JMP_NEAR)
			{
				params.startInstructionIndex--;
			}

			if (decompileReturnStatement(params, result))
			{
				strcatJdc(result, "\n");
			}
			else
			{
				return 0;
			}

			if (originalIndex != -1) // handling end of jmp in decomp
			{
				i = originalIndex;
				originalIndex = -1;
				params.skipUpperBound = -1;
				params.skipLowerBound = -1;

				int numOfCloses = 1 + numOfIndents - originalNumOfIndents;
				for (int j = 0; j < numOfCloses; j++)
				{
					numOfIndents--;
					addIndents(result, numOfIndents);
					strcatJdc(result, "}\n");
				}

				originalNumOfIndents = -1;

				indexToJumpTo = -1;
			}
			else
			{
				if (isOpcodeJmp(currentInstruction->opcode) && currentInstruction->operands[0].type == IMMEDIATE && currentInstruction->operands[0].immediate.value > 0)
				{
					unsigned long long jumpAddr = currentInstruction->address + currentInstruction->operands[0].immediate.value;
					int instructionIndex = findInstructionByAddress(params.currentFunc->instructions, 0, params.currentFunc->numOfInstructions - 1, jumpAddr);
					if (instructionIndex > indexToJumpTo)
					{
						indexToJumpTo = instructionIndex;
					}
				}
				else
				{
					indexToJumpTo = params.currentFunc->numOfInstructions + 1; // this is so the current state will be treated as unreachable. indexToJumpTo gets reset back to -1 when there is a condtion.
				}
			}
		}

		decompileMiscInstruction(params, numOfIndents, result);
	}

	free(conditions);
	free(directJmps);

	return strcatJdc(result, "}\n");
}

static unsigned char getAllReturnedVars(struct DecompilationParameters params)
{
	for (int i = 0; i < params.currentFunc->numOfInstructions; i++)
	{
		params.startInstructionIndex = i;
		struct Function* callee;
		int importIndex = checkForImportCall(params);
		if (checkForFunctionCall(params, &callee) || importIndex != -1)
		{
			enum Register returnReg = callee ? callee->returnReg : AX;
			unsigned long long calleeAddress = callee ? callee->instructions[0].address : params.imports[importIndex].address;

			struct VarType returnType = { 0 }; // used if its an import call, also using this here to check if the return value is used
			for (int j = i; j < params.currentFunc->numOfInstructions; j++)
			{
				struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[j]);
				enum Mnemonic opcode = currentInstruction->opcode;
				unsigned char overwrites = 0;
				if (j != i)
				{
					if (isOpcodeCall(opcode) || opcode == JMP_SHORT || (doesInstructionModifyRegister(currentInstruction, returnReg, 0, 0, &overwrites) && overwrites))
					{
						break;
					}
				}

				if (checkForReturnStatement(params))
				{
					returnType = params.currentFunc->returnType;
					break;
				}

				unsigned char operandNum = 0;
				if (doesInstructionAccessRegister(currentInstruction, returnReg, &operandNum))
				{
					returnType = getTypeOfOperand(opcode, &(currentInstruction->operands[operandNum]));
					break;
				}
			}
			if (returnType.primitiveType == VOID_TYPE)
			{
				continue;
			}

			int callNum = 0;
			for (int j = 0; j < params.currentFunc->numOfReturnedVars; j++)
			{
				if (params.currentFunc->returnedVars[j].callAddr == calleeAddress)
				{
					callNum++;
				}
			}

			if (callee)
			{
				if (callee->returnType.primitiveType == VOID_TYPE)
				{
					continue;
				}
				else if (!addReturnedVar(params.currentFunc, callee->returnType, callNum, calleeAddress, returnReg, callee->name.buffer))
				{
					return 0;
				}
			}
			else
			{
				if (!addReturnedVar(params.currentFunc, returnType, callNum, calleeAddress, returnReg, params.imports[importIndex].name.buffer))
				{
					return 0;
				}
			}
		}
	}

	return 1;
}

static unsigned char getAllRegVars(struct DecompilationParameters params, struct Condition* conditions, int numOfConditions)
{
	// checking for registers that are modified in a condition
	for (int i = 0; i < numOfConditions; i++) 
	{
		if (!conditions[i].requiresJumpInDecomp && !conditions[i].isCombinedByOther)
		{
			struct RegisterVariable modifiedRegs[ST0 - RAX] = { 0 };
			int numOfRegs = 0;

			int start = conditions[i].conditionType == DO_WHILE_CT ? conditions[i].dstIndex : conditions[i].jccIndex;
			int end = conditions[i].conditionType == DO_WHILE_CT ? conditions[i].jccIndex : conditions[i].dstIndex;
			
			for (int j = start; j < end; j++)
			{
				int conditionIndex = checkForCondition(j, conditions, numOfConditions);
				if (conditionIndex != -1 && conditionIndex != i)
				{
					break;
				}

				struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[j]);

				enum Register reg = NO_REG;

				struct Function* callee;
				params.startInstructionIndex = j;
				if ((checkForFunctionCall(params, &callee) && callee->returnType.primitiveType != VOID_TYPE))
				{
					reg = callee->returnReg;
				}
				else if (checkForImportCall(params) != -1)
				{
					reg = params.is64Bit ? RAX : EAX;
				}
				else if (currentInstruction->operands[0].type == REGISTER && doesInstructionModifyOperand(currentInstruction, 0, 0, 0))
				{
					reg = currentInstruction->operands[0].reg;
				}

				if (reg != NO_REG && !isRegisterPointer(reg))
				{
					int alreadyFound = 0;
					for (int k = 0; k < numOfRegs; k++)
					{
						if (compareRegisters(reg, modifiedRegs[k].reg))
						{
							alreadyFound = 1;
							break;
						}
					}
					if (alreadyFound || getRegVarByReg(params.currentFunc, reg))
					{
						continue;
					}

					modifiedRegs[numOfRegs].reg = reg;
					modifiedRegs[numOfRegs].type = getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[0]);
					numOfRegs++;
				}
			}

			// checking if the modified regs are accessed before being overwritten after the condition
			int checkingStart = conditions[i].conditionType == LOOP_CT || conditions[i].conditionType == DO_WHILE_CT ? start : end; // if it is a loop, the code can run more than once so it needs to start checking from the begining of the loop
			for (int j = checkingStart; j < params.currentFunc->numOfInstructions; j++)
			{
				int conditionIndex = checkForCondition(j, conditions, numOfConditions);
				if (conditionIndex != -1 && conditionIndex != i)
				{
					break;
				}

				struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[j]);

				for (int k = 0; k < numOfRegs; k++)
				{
					if (modifiedRegs[k].reg == NO_REG)
					{
						continue;
					}

					unsigned char overwrites = 0;
					if (doesInstructionAccessRegister(currentInstruction, modifiedRegs[k].reg, 0) || 
						(doesInstructionModifyRegister(currentInstruction, modifiedRegs[k].reg, 0, 0, &overwrites) && !overwrites) ||
						(isOpcodeReturn(currentInstruction->opcode) && compareRegisters(modifiedRegs[k].reg, AX)))
					{
						if (!addRegVar(params.currentFunc, modifiedRegs[k].type, modifiedRegs[k].reg)) 
						{
							return 0;
						}

						modifiedRegs[k].reg = NO_REG; // so it isnt checked again
						break;
					}
					else if (overwrites && !isOpcodeCMOVcc(currentInstruction->opcode) && !isOpcodeSETcc(currentInstruction->opcode))
					{
						modifiedRegs[k].reg = NO_REG;
						break;
					}
				}
			}

			// if there are registers used in the comparisson of a do while loop, they need to be a reg var
			if (conditions[i].conditionType == DO_WHILE_CT)
			{
				for (int j = end; j >= start; j--) 
				{
					struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[j]);
					
					if (isOpcodeCmp(currentInstruction->opcode) || currentInstruction->opcode == TEST) 
					{
						for (int k = 0; k < 2; k++) 
						{
							if (currentInstruction->operands[k].type == REGISTER && !getRegVarByReg(params.currentFunc, currentInstruction->operands[k].reg))
							{
								if (!addRegVar(params.currentFunc, getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[k]), currentInstruction->operands[k].reg))
								{
									return 0;
								}
							}
							else if (currentInstruction->operands[k].type == MEM_ADDRESS && !getRegVarByReg(params.currentFunc, currentInstruction->operands[k].memoryAddress.reg))
							{
								if (!addRegVar(params.currentFunc, getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[k]), currentInstruction->operands[k].memoryAddress.reg))
								{
									return 0;
								}
							}
						}

						break;
					}
				}
			}
		}
	}

	// check for registers that depend on the value of a reg var
	for (int i = 0; i < params.currentFunc->numOfInstructions; i++)
	{
		struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[i]);

		if (currentInstruction->operands[0].type == REGISTER && doesInstructionModifyOperand(currentInstruction, 0, 0, 0))
		{
			enum Register reg = currentInstruction->operands[0].reg;
			
			unsigned char alreadyFound = 0;
			for (int j = 0; j < params.currentFunc->numOfRegVars; j++)
			{
				if (compareRegisters(reg, params.currentFunc->regVars[j].reg))
				{
					params.currentFunc->regVars[j].type.isUnsigned = doesOpcodeUseUnsignedInt(currentInstruction->opcode);
					alreadyFound = 1;
					break;
				}
			}
			if (alreadyFound)
			{
				continue;
			}

			unsigned char dependsOnRegVar = 0;
			for (int j = 0; j < params.currentFunc->numOfRegVars; j++)
			{
				if (currentInstruction->operands[1].type == REGISTER)
				{
					if (compareRegisters(params.currentFunc->regVars[j].reg, currentInstruction->operands[1].reg))
					{
						dependsOnRegVar = 1;
						break;
					}
				}
				else if (currentInstruction->operands[1].type == MEM_ADDRESS)
				{
					if (compareRegisters(params.currentFunc->regVars[j].reg, currentInstruction->operands[1].memoryAddress.reg) || compareRegisters(params.currentFunc->regVars[j].reg, currentInstruction->operands[1].memoryAddress.regDisplacement))
					{
						dependsOnRegVar = 1;
						break;
					}
				}
			}

			if (dependsOnRegVar)
			{
				if (!addRegVar(params.currentFunc, getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[0]), reg))
				{
					return 0;
				}
			}
		}
	}

	return 1;
}

static void addIndents(struct JdcStr* result, int numOfIndents)
{
	for (int i = 0; i < numOfIndents; i++)
	{
		strcatJdc(result, indent);
	}
}

static unsigned char generateFunctionHeader(struct Function* function, struct JdcStr* result)
{
	struct JdcStr typeStr = initializeJdcStr();

	varTypeToStr(function->returnType, &typeStr);
	sprintfJdc(result, 0, "%s %s %s(", typeStr.buffer, callingConventionStrs[function->callingConvention], function->name.buffer);

	for (int i = 0; i < function->numOfRegArgs; i++) 
	{
		varTypeToStr(function->regArgs[i].type, &typeStr);
		
		if (i == function->numOfRegArgs - 1 && function->numOfStackArgs == 0) 
		{
			sprintfJdc(result, 1, "%s %s", typeStr.buffer, function->regArgs[i].name.buffer);
		}
		else 
		{
			sprintfJdc(result, 1, "%s %s, ", typeStr.buffer, function->regArgs[i].name.buffer);
		}
	}

	for (int i = 0; i < function->numOfStackArgs; i++)
	{
		varTypeToStr(function->stackArgs[i].type, &typeStr);
		
		if (i == function->numOfStackArgs - 1)
		{
			sprintfJdc(result, 1, "%s %s", typeStr.buffer, function->stackArgs[i].name.buffer);
		}
		else
		{
			sprintfJdc(result, 1, "%s %s, ", typeStr.buffer, function->stackArgs[i].name.buffer);
		}
	}

	freeJdcStr(&typeStr);
	return strcatJdc(result, ")\n");
}

static unsigned char declareAllLocalVariables(struct DecompilationParameters params, struct JdcStr* result)
{
	struct JdcStr typeStr = initializeJdcStr();
	
	for (int i = 0; i < params.currentFunc->numOfStackVars; i++)
	{
		varTypeToStr(params.currentFunc->stackVars[i].type, &typeStr);
		sprintfJdc(result, 1, "%s%s %s;\n", indent, typeStr.buffer, params.currentFunc->stackVars[i].name.buffer);
	}

	for (int i = 0; i < params.currentFunc->numOfRegVars; i++)
	{
		int argIndex = -1;
		for (int j = 0; j < params.currentFunc->numOfRegArgs; j++)
		{
			if (compareRegisters(params.currentFunc->regVars[i].reg, params.currentFunc->regArgs[j].reg))
			{
				argIndex = j;
				break;
			}
		}

		varTypeToStr(params.currentFunc->regVars[i].type, &typeStr);

		if (argIndex != -1)
		{
			sprintfJdc(result, 1, "%s%s %s = %s;\n", indent, typeStr.buffer, params.currentFunc->regVars[i].name.buffer, params.currentFunc->regArgs[argIndex].name.buffer);
		}
		else 
		{
			sprintfJdc(result, 1, "%s%s %s;\n", indent, typeStr.buffer, params.currentFunc->regVars[i].name.buffer);
		}
	}

	for (int i = 0; i < params.currentFunc->numOfReturnedVars; i++)
	{
		unsigned char isReturnRegVar = 0;
		for (int j = 0; j < params.currentFunc->numOfRegVars; j++) 
		{
			if (compareRegisters(params.currentFunc->regVars[j].reg, params.currentFunc->returnedVars[i].returnReg)) 
			{
				isReturnRegVar = 1;
				break;
			}
		}

		if (!isReturnRegVar) 
		{
			varTypeToStr(params.currentFunc->returnedVars[i].type, &typeStr);
			sprintfJdc(result, 1, "%s%s %s;\n", indent, typeStr.buffer, params.currentFunc->returnedVars[i].name.buffer);
		}
	}

	freeJdcStr(&typeStr);
	return strcatJdc(result, "\n");
}

static void decompileMiscInstruction(struct DecompilationParameters params, unsigned char numOfIndents, struct JdcStr* result)
{
	struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[params.startInstructionIndex]);
	
	// these are windows functions
	if (currentInstruction->opcode == INT3) 
	{
		addIndents(result, numOfIndents);
		strcatJdc(result, "__debugbreak();\n");
		return;
	}
	else if (currentInstruction->opcode == _INT && currentInstruction->operands[0].immediate.value == 0x29) 
	{
		struct JdcStr code = initializeJdcStr();
		struct VarType type = { 0 };
		type.isUnsigned = 1;
		type.primitiveType = INT_TYPE;

		if (decompileRegister(params, CX, type, &code, 0)) 
		{
			addIndents(result, numOfIndents);
			sprintfJdc(result, 1, "__fastfail(%s);\n", code.buffer);
		}

		freeJdcStr(&code);
		return;
	}
}
