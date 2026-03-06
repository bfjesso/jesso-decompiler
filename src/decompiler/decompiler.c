#include "decompiler.h"
#include "functions.h"
#include "assignment.h"
#include "returnStatements.h"
#include "functionCalls.h"
#include "dataTypes.h"
#include "../disassembler/registers.h"

const char* indent = "    ";

unsigned char decompileFunction(struct DecompilationParameters params, struct JdcStr* result)
{
	struct Condition conditions[20] = { 0 };
	int numOfConditions = getAllConditions(params, conditions);
	
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
		if (!declareAllLocalVariables(params.currentFunc, result))
		{
			return 0;
		}
	}

	params.stackFrameSize = 0;

	unsigned char numOfIndents = 1;
	unsigned char isInUnreachableState = 0; // if looking at instructions after a ret or jmp
	int originalIndex = -1;
	for (int i = 0; i < params.currentFunc->numOfInstructions; i++)
	{
		params.startInstructionIndex = i;

		struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[i]);

		params.stackFrameSize += getStackFrameChange(currentInstruction);

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
		else if (checkForFunctionCall(params, &callee))
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
		else if (checkForReturnStatement(params)) 
		{
			addIndents(result, numOfIndents);
			params.startInstructionIndex--;
			if (decompileReturnStatement(params, result))
			{
				strcatJdc(result, "\n");
			}
			else 
			{
				return 0;
			}
		}
		else if (checkForAssignment(params))
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
			for (int j = i + 1; j < params.currentFunc->numOfInstructions; j++)
			{
				struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[j]);
				enum Mnemonic opcode = currentInstruction->opcode;
				unsigned char overwrites = 0;
				if (isOpcodeCall(opcode) || opcode == JMP_SHORT || (doesInstructionModifyRegister(currentInstruction, AX, 0, &overwrites) && overwrites))
				{
					break;
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
				else
				{
					struct ReturnedVariable* newReturnedVars = (struct ReturnedVariable*)realloc(params.currentFunc->returnedVars, sizeof(struct ReturnedVariable) * (params.currentFunc->numOfReturnedVars + 1));
					if (newReturnedVars)
					{
						params.currentFunc->returnedVars = newReturnedVars;
					}
					else
					{
						return 0;
					}
					
					params.currentFunc->returnedVars[params.currentFunc->numOfReturnedVars].type = params.functions[calleeIndex].returnType;
					params.currentFunc->returnedVars[params.currentFunc->numOfReturnedVars].name = initializeJdcStr();
					sprintfJdc(&(params.currentFunc->returnedVars[params.currentFunc->numOfReturnedVars].name), 0, "%sRetVal%d", params.functions[calleeIndex].name.buffer, callNum);
					params.currentFunc->returnedVars[params.currentFunc->numOfReturnedVars].callAddr = calleeAddress;
					params.currentFunc->returnedVars[params.currentFunc->numOfReturnedVars].callNum = callNum;
					params.currentFunc->numOfReturnedVars++;
				}
			}
			else
			{
				for (int j = 0; j < params.numOfImports; j++)
				{
					if (params.imports[j].address == calleeAddress)
					{
						struct ReturnedVariable* newReturnedVars = (struct ReturnedVariable*)realloc(params.currentFunc->returnedVars, sizeof(struct ReturnedVariable) * (params.currentFunc->numOfReturnedVars + 1));
						if (newReturnedVars)
						{
							params.currentFunc->returnedVars = newReturnedVars;
						}
						else
						{
							return 0;
						}

						params.currentFunc->returnedVars[params.currentFunc->numOfReturnedVars].name = initializeJdcStr();
						sprintfJdc(&(params.currentFunc->returnedVars[params.currentFunc->numOfReturnedVars].name), 0, "%sRetVal%d", params.imports[j].name.buffer, callNum);
						params.currentFunc->returnedVars[params.currentFunc->numOfReturnedVars].type = returnType;
						params.currentFunc->returnedVars[params.currentFunc->numOfReturnedVars].callAddr = calleeAddress;
						params.currentFunc->returnedVars[params.currentFunc->numOfReturnedVars].callNum = callNum;
						params.currentFunc->numOfReturnedVars++;

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
	// check for registers being modified in a condition
	int isInCondition = 0;
	for (int i = 0; i < params.currentFunc->numOfInstructions; i++)
	{
		struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[i]);

		for (int j = 0; j < numOfConditions; j++)
		{
			if (!conditions[j].requiresJumpInDecomp && !conditions[j].isCombinedByOther && conditions[j].conditionType != LOOP_CT)
			{
				if ((conditions[j].conditionType != DO_WHILE_CT && i == conditions[j].dstIndex) || (conditions[j].conditionType == DO_WHILE_CT && i == conditions[j].jccIndex))
				{
					isInCondition = 0;
					break;
				}
				else if ((conditions[j].conditionType != DO_WHILE_CT && i == conditions[j].jccIndex) || (conditions[j].conditionType == DO_WHILE_CT && i == conditions[j].dstIndex))
				{
					isInCondition = 1;
					break;
				}
			}
		}

		enum Register reg = NO_REG;

		struct Function* callee;
		params.startInstructionIndex = i;
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
		else if (checkForImportCall(params))
		{
			reg = params.is64Bit ? RAX : EAX;
		}
		else if (currentInstruction->operands[0].type == REGISTER && doesInstructionModifyOperand(currentInstruction, 0, 0)) 
		{
			reg = currentInstruction->operands[0].reg;
		}

		if (reg != NO_REG)
		{
			int alreadyFound = 0;
			for (int j = 0; j < params.currentFunc->numOfRegVars; j++)
			{
				if (compareRegisters(reg, params.currentFunc->regVars[j].reg))
				{
					params.currentFunc->regVars[j].type.isSigned = doesOpcodeOnlyUseSignedInt(currentInstruction->opcode);
					alreadyFound = 1;
					break;
				}
			}
			if (alreadyFound)
			{
				continue;
			}

			if (isInCondition)
			{
				struct RegisterVariable* newRegVars = (struct RegisterVariable*)realloc(params.currentFunc->regVars, sizeof(struct RegisterVariable) * (params.currentFunc->numOfRegVars + 1));
				if (newRegVars)
				{
					params.currentFunc->regVars = newRegVars;
				}
				else
				{
					return 0;
				}

				params.currentFunc->regVars[params.currentFunc->numOfRegVars].reg = reg;
				params.currentFunc->regVars[params.currentFunc->numOfRegVars].type = getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[0]);
				params.currentFunc->regVars[params.currentFunc->numOfRegVars].name = initializeJdcStr();
				sprintfJdc(&(params.currentFunc->regVars[params.currentFunc->numOfRegVars].name), 0, "var%s", registerStrs[reg]);
				params.currentFunc->numOfRegVars++;
			}
		}
	}

	// check for registers that depend on the value of a reg var
	for (int i = 0; i < params.currentFunc->numOfInstructions; i++)
	{
		struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[i]);

		if (currentInstruction->operands[0].type == REGISTER && doesInstructionModifyOperand(currentInstruction, 0, 0))
		{
			int alreadyFound = 0;
			for (int j = 0; j < params.currentFunc->numOfRegVars; j++)
			{
				if (compareRegisters(currentInstruction->operands[0].reg, params.currentFunc->regVars[j].reg))
				{
					params.currentFunc->regVars[j].type.isSigned = doesOpcodeOnlyUseSignedInt(currentInstruction->opcode);
					alreadyFound = 1;
					break;
				}
			}
			if (alreadyFound)
			{
				continue;
			}

			unsigned char dependsOnRegVar = 0;
			if (currentInstruction->operands[1].type == REGISTER)
			{
				for (int j = 0; j < params.currentFunc->numOfRegVars; j++)
				{
					if (params.currentFunc->regVars[j].reg == currentInstruction->operands[1].reg)
					{
						dependsOnRegVar = 1;
						break;
					}
				}
			}
			else if (currentInstruction->operands[1].type == MEM_ADDRESS)
			{
				for (int j = 0; j < params.currentFunc->numOfRegVars; j++)
				{
					if (params.currentFunc->regVars[j].reg == currentInstruction->operands[1].memoryAddress.reg || params.currentFunc->regVars[j].reg == currentInstruction->operands[1].memoryAddress.regDisplacement)
					{
						dependsOnRegVar = 1;
						break;
					}
				}
			}

			if (dependsOnRegVar)
			{
				params.currentFunc->regVars[params.currentFunc->numOfRegVars].reg = currentInstruction->operands[0].reg;
				params.currentFunc->regVars[params.currentFunc->numOfRegVars].type = getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[0]);
				params.currentFunc->regVars[params.currentFunc->numOfRegVars].name = initializeJdcStr();
				sprintfJdc(&(params.currentFunc->regVars[params.currentFunc->numOfRegVars].name), 0, "var%s", registerStrs[currentInstruction->operands[0].reg]);
				params.currentFunc->numOfRegVars++;
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
	if (!sprintfJdc(result, 0, "%s %s %s(", typeStr.buffer, callingConventionStrs[function->callingConvention], function->name.buffer))
	{
		freeJdcStr(&typeStr);
		return 0;
	}

	for (int i = 0; i < function->numOfRegArgs; i++) 
	{
		varTypeToStr(function->regArgs[i].type, &typeStr);
		
		if (i == function->numOfRegArgs - 1 && function->numOfStackArgs == 0) 
		{
			if (!sprintfJdc(result, 1, "%s %s", typeStr.buffer, function->regArgs[i].name.buffer))
			{
				freeJdcStr(&typeStr);
				return 0;
			}
		}
		else 
		{
			if (!sprintfJdc(result, 1, "%s %s, ", typeStr.buffer, function->regArgs[i].name.buffer))
			{
				freeJdcStr(&typeStr);
				return 0;
			}
		}
	}

	for (int i = 0; i < function->numOfStackArgs; i++)
	{
		varTypeToStr(function->stackArgs[i].type, &typeStr);
		
		if (i == function->numOfStackArgs - 1)
		{
			if (!sprintfJdc(result, 1, "%s %s", typeStr.buffer, function->stackArgs[i].name.buffer))
			{
				freeJdcStr(&typeStr);
				return 0;
			}
		}
		else
		{
			if (!sprintfJdc(result, 1, "%s %s, ", typeStr.buffer, function->stackArgs[i].name.buffer))
			{
				freeJdcStr(&typeStr);
				return 0;
			}
		}
	}

	freeJdcStr(&typeStr);
	return strcatJdc(result, ")\n");
}

static unsigned char declareAllLocalVariables(struct Function* function, struct JdcStr* result)
{
	struct JdcStr typeStr = initializeJdcStr();
	
	for (int i = 0; i < function->numOfLocalVars; i++)
	{
		varTypeToStr(function->localVars[i].type, &typeStr);
		if (!sprintfJdc(result, 1, "%s%s %s;\n", indent, typeStr.buffer, function->localVars[i].name.buffer))
		{
			freeJdcStr(&typeStr);
			return 0;
		}
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
			if (!sprintfJdc(result, 1, "%s%s %s = %s;\n", indent, typeStr.buffer, function->regVars[i].name.buffer, function->regArgs[argIndex].name.buffer))
			{
				freeJdcStr(&typeStr);
				return 0;
			}
		}
		else 
		{
			if (!sprintfJdc(result, 1, "%s%s %s;\n", indent, typeStr.buffer, function->regVars[i].name.buffer))
			{
				freeJdcStr(&typeStr);
				return 0;
			}
		}
	}

	for (int i = 0; i < function->numOfReturnedVars; i++)
	{
		varTypeToStr(function->returnedVars[i].type, &typeStr);
		if (!sprintfJdc(result, 1, "%s%s %s;\n", indent, typeStr.buffer, function->returnedVars[i].name.buffer))
		{
			freeJdcStr(&typeStr);
			return 0;
		}
	}

	freeJdcStr(&typeStr);
	return strcatJdc(result, "\n");
}
