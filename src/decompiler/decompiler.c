#include "decompiler.h"
#include "decompilationUtils.h"
#include "functions.h"
#include "expressions.h"
#include "assignment.h"
#include "returnStatements.h"
#include "functionCalls.h"
#include "dataTypes.h"
#include "../disassembler/registers.h"

const char* indent = "    ";

unsigned char decompileFunction(struct DecompilationParameters params, struct JdcStr* result)
{
	struct Condition conditions[20] = { 0 };
	int numOfConditions = getAllConditions(params, conditions, 20);
	
	if (!params.currentFunc->hasGottenLocalVars)
	{
		if (!getAllRegVars(params, conditions, numOfConditions))
		{
			return 0;
		}

		params.axRegVarIndex = -1;
		for(int i = 0; i < params.currentFunc->numOfRegVars; i++)
		{
			if(compareRegisters(params.currentFunc->regVars[i].reg, AX))
			{
				params.axRegVarIndex = i;
				break;
			}
		}

		if (!getAllReturnedVars(params)) // still need to get these even if AX is a reg var in order to check if the returned value is used or not
		{
			return 0;
		}

		params.currentFunc->hasGottenLocalVars = 1;
	}
	
	if (!generateFunctionHeader(params.currentFunc, result))
	{
		return 0;
	}

	strcatJdc(result, "{\n");

	if (params.currentFunc->numOfLocalVars > 0 || params.currentFunc->numOfReturnedVars > 0 || params.currentFunc->numOfRegVars > 0)
	{
		if (!declareAllLocalVariables(params.currentFunc, params.axRegVarIndex == -1, result))
		{
			return 0;
		}
	}

	unsigned char numOfIndents = 1;
	unsigned char isInUnreachableState = 0; // if looking at instructions after a ret or jmp
	int originalIndex = -1;
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

					isInUnreachableState = 0;
				}
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

					isInUnreachableState = 0;
					numOfIndents++;
				}
			}
			else
			{
				return 0;
			}

			if (conditions[conditionIndex].requiresJumpInDecomp)
			{
				originalIndex = i;
				i = conditions[conditionIndex].dstIndex - 1;
				params.skipUpperBound = originalIndex;
				params.skipLowerBound = i;
				continue;
			}
		}

		if (isInUnreachableState) { continue; }

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
		}

		decompileMiscInstruction(params, numOfIndents, result);

		if (isOpcodeReturn(currentInstruction->opcode) || currentInstruction->opcode == JMP_SHORT)
		{
			if (originalIndex != -1) 
			{
				i = originalIndex;
				originalIndex = -1;
				params.skipUpperBound = -1;
				params.skipLowerBound = -1;

				numOfIndents--;
				addIndents(result, numOfIndents);
				strcatJdc(result, "}\n");

				isInUnreachableState = 0;
			}
			else 
			{
				isInUnreachableState = 1;
			}
		}
	}

	return strcatJdc(result, "}\n");
}

static unsigned char getAllReturnedVars(struct DecompilationParameters params)
{
	for (int i = 0; i < params.currentFunc->numOfInstructions; i++)
	{
		if (isOpcodeCall(params.currentFunc->instructions[i].opcode))
		{
			struct VarType returnType = { 0 }; // used if its an import call, also using this here to check if the return value is used
			for (int j = i; j < params.currentFunc->numOfInstructions; j++)
			{
				struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[j]);
				enum Mnemonic opcode = currentInstruction->opcode;
				unsigned char overwrites = 0;
				if(j != i)
				{
					if (isOpcodeCall(opcode) || opcode == JMP_SHORT || (doesInstructionModifyRegister(currentInstruction, AX, 0, &overwrites) && overwrites))
					{
						break;
					}
				}

				if (isOpcodeReturn(opcode))
				{
					returnType = params.currentFunc->returnType;
					break;
				}

				unsigned char operandNum = 0;
				if (doesInstructionAccessRegister(currentInstruction, AX, &operandNum))
				{
					returnType = getTypeOfOperand(opcode, &(currentInstruction->operands[operandNum]));
					break;
				}
			}
			if (returnType.primitiveType == VOID_TYPE)
			{
				continue;
			}

			int currentInstructionIndex = findInstructionByAddress(params.allInstructions, 0, params.totalNumOfInstructions - 1, params.currentFunc->instructions[i].address);
			unsigned long long calleeAddress = resolveJmpChain(params, currentInstructionIndex);

			int callNum = 0;
			for (int j = 0; j < params.currentFunc->numOfReturnedVars; j++)
			{
				if (params.currentFunc->returnedVars[j].callAddr == calleeAddress)
				{
					callNum++;
				}
			}

			int calleeIndex = findFunctionByAddress(params.functions, 0, params.numOfFunctions - 1, calleeAddress);
			if (calleeIndex != -1)
			{
				if (params.functions[calleeIndex].returnType.primitiveType == VOID_TYPE)
				{
					continue;
				}
				else if(!addReturnedVar(params.currentFunc, params.functions[calleeIndex].returnType, callNum, calleeAddress, params.functions[calleeIndex].name.buffer))
				{
					return 0;
				}
			}
			else
			{
				for (int j = 0; j < params.numOfImports; j++)
				{
					if (params.imports[j].address == calleeAddress)
					{
						if (!addReturnedVar(params.currentFunc, returnType, callNum, calleeAddress, params.imports[j].name.buffer)) 
						{
							return 0;
						}
				
						break;
					}
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
			
			for (int j = conditions[i].jccIndex; j < conditions[i].dstIndex; j++) 
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
					switch (callee->returnType.primitiveType)
					{
					case CHAR_TYPE:
						reg = AL;
						break;
					case SHORT_TYPE:
						reg = AX;
						break;
					case INT_TYPE:
						reg = EAX;
						break;
					case LONG_LONG_TYPE:
						reg = RAX;
						break;
					}
				}
				else if (checkForImportCall(params) != -1)
				{
					reg = params.is64Bit ? RAX : EAX;
				}
				else if (currentInstruction->operands[0].type == REGISTER && doesInstructionModifyOperand(currentInstruction, 0, 0))
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
			for (int j = conditions[i].dstIndex; j < params.currentFunc->numOfInstructions; j++) 
			{
				if (checkForCondition(j, conditions, numOfConditions) != -1)
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
					if (doesInstructionAccessRegister(currentInstruction, modifiedRegs[k].reg, 0) || (doesInstructionModifyRegister(currentInstruction, modifiedRegs[k].reg, 0, &overwrites) && !overwrites) || (isOpcodeReturn(currentInstruction->opcode) && compareRegisters(modifiedRegs[k].reg, AX)))
					{
						if (!addRegVar(params.currentFunc, modifiedRegs[k].type, modifiedRegs[k].reg)) 
						{
							return 0;
						}

						modifiedRegs[k].reg = NO_REG; // so it isnt checked again
						break;
					}
					else if (overwrites) 
					{
						modifiedRegs[k].reg = NO_REG;
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

		if (currentInstruction->operands[0].type == REGISTER && doesInstructionModifyOperand(currentInstruction, 0, 0))
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

static unsigned char declareAllLocalVariables(struct Function* function, unsigned char declareReturnedVars, struct JdcStr* result)
{
	struct JdcStr typeStr = initializeJdcStr();
	
	for (int i = 0; i < function->numOfLocalVars; i++)
	{
		varTypeToStr(function->localVars[i].type, &typeStr);
		sprintfJdc(result, 1, "%s%s %s;\n", indent, typeStr.buffer, function->localVars[i].name.buffer);
	}

	for (int i = 0; i < function->numOfRegVars; i++)
	{
		int argIndex = -1;
		for (int j = 0; j < function->numOfRegArgs; j++)
		{
			if (compareRegisters(function->regVars[i].reg, function->regArgs[j].reg))
			{
				argIndex = j;
				break;
			}
		}

		varTypeToStr(function->regVars[i].type, &typeStr);

		if (argIndex != -1)
		{
			sprintfJdc(result, 1, "%s%s %s = %s;\n", indent, typeStr.buffer, function->regVars[i].name.buffer, function->regArgs[argIndex].name.buffer);
		}
		else 
		{
			sprintfJdc(result, 1, "%s%s %s;\n", indent, typeStr.buffer, function->regVars[i].name.buffer);
		}
	}

	if(declareReturnedVars)
	{
		for (int i = 0; i < function->numOfReturnedVars; i++)
		{
			varTypeToStr(function->returnedVars[i].type, &typeStr);
			sprintfJdc(result, 1, "%s%s %s;\n", indent, typeStr.buffer, function->returnedVars[i].name.buffer);
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

		if (decompileRegister(params, CX, type, &code)) 
		{
			addIndents(result, numOfIndents);
			sprintfJdc(result, 1, "__fastfail(%s);\n", code.buffer);
		}

		freeJdcStr(&code);
		return;
	}
}
