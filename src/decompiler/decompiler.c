#include "decompiler.h"
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
	int numOfConditions = getAllConditions(params, conditions);
	
	if (!params.currentFunc->hasGottenLocalVars)
	{
		if (!getAllReturnedVars(params)) 
		{
			return 0;
		}
		if (!getAllRegVars(params, conditions, numOfConditions))
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

		if (currentInstruction->opcode == SUB && currentInstruction->operands[0].type == REGISTER && compareRegisters(currentInstruction->operands[0].reg, SP))
		{
			params.stackFrameSize += currentInstruction->operands[1].immediate;
		}

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
			unsigned char isReturnVarUsed = 0;
			for (int j = i + 1; j < params.currentFunc->numOfInstructions; j++)
			{
				enum Mnemonic opcode = params.currentFunc->instructions[j].opcode;
				if (isOpcodeReturn(opcode) || doesInstructionAccessRegister(&(params.currentFunc->instructions[j]), AX, 0))
				{
					isReturnVarUsed = 1;
					break;
				}

				if (isOpcodeCall(opcode) || opcode == JMP_SHORT || doesInstructionModifyRegister(&(params.currentFunc->instructions[j]), AX, 0, 0))
				{
					break;
				}
			}
			if (!isReturnVarUsed)
			{
				continue;
			}

			int currentInstructionIndex = findInstructionByAddress(params.allInstructions, 0, params.totalNumOfInstructions - 1, params.currentFunc->instructions[i].address);
			unsigned long long calleeAddress = resolveJmpChain(params.allInstructions, params.totalNumOfInstructions, currentInstructionIndex);

			int callNum = 0;
			for (int j = 0; j < params.currentFunc->numOfReturnedVars; j++)
			{
				if (params.currentFunc->returnedVars[j].callAddr == calleeAddress)
				{
					callNum++;
				}
			}

			int calleIndex = findFunctionByAddress(params.functions, 0, params.numOfFunctions - 1, calleeAddress);
			if (calleIndex != -1)
			{
				if (params.functions[calleIndex].returnType == VOID_TYPE)
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
					
					params.currentFunc->returnedVars[params.currentFunc->numOfReturnedVars].type = params.functions[calleIndex].returnType;
					params.currentFunc->returnedVars[params.currentFunc->numOfReturnedVars].name = initializeJdcStr();
					sprintfJdc(&(params.currentFunc->returnedVars[params.currentFunc->numOfReturnedVars].name), 0, "%sRetVal%d", params.functions[calleIndex].name.buffer, callNum);
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
						// checking if AX is ever accessed without being assigned after the call and until the next function call
						unsigned char returnType = params.is64Bit ? LONG_LONG_TYPE : INT_TYPE; // assume it returns something by default
						for (int k = i + 1; i < params.currentFunc->numOfInstructions; k++)
						{
							enum Mnemonic opcode = params.currentFunc->instructions[k].opcode;
							if (isOpcodeCall(opcode) || opcode == JMP_SHORT)
							{
								break;
							}

							unsigned char operandNum = 0;
							if (doesInstructionAccessRegister(&(params.currentFunc->instructions[k]), AX, &operandNum))
							{
								returnType = getTypeOfOperand(params.currentFunc->instructions[k].opcode, &(params.currentFunc->instructions[k].operands[operandNum]), params.is64Bit);
								break;
							}
						}

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

// right now this only checks for registers that are modified in a condition
static unsigned char getAllRegVars(struct DecompilationParameters params, struct Condition* conditions, int numOfConditions)
{
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

		if (isInCondition && currentInstruction->operands[0].type == REGISTER)
		{
			int alreadyFound = 0;
			for (int j = 0; j < params.currentFunc->numOfRegVars; j++)
			{
				if (compareRegisters(currentInstruction->operands[0].reg, params.currentFunc->regVars[j].reg))
				{
					alreadyFound = 1;
					break;
				}
			}
			for (int j = 0; j < params.currentFunc->numOfRegArgs; j++)
			{
				if (alreadyFound) { break; }
				if (compareRegisters(currentInstruction->operands[0].reg, params.currentFunc->regArgs[j].reg))
				{
					alreadyFound = 1;
					break;
				}
			}
			if (alreadyFound) 
			{
				continue;
			}
			
			if (doesInstructionModifyOperand(currentInstruction, 0, 0))
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

				params.currentFunc->regVars[params.currentFunc->numOfRegVars].reg = currentInstruction->operands[0].reg;
				params.currentFunc->regVars[params.currentFunc->numOfRegVars].type = getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[0], params.is64Bit);
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
	if (!sprintfJdc(result, 0, "%s %s %s(", primitiveTypeStrs[function->returnType], callingConventionStrs[function->callingConvention], function->name.buffer)) 
	{
		return 0;
	}

	for (int i = 0; i < function->numOfRegArgs; i++) 
	{
		if (i == function->numOfRegArgs - 1 && function->numOfStackArgs == 0) 
		{
			if (!sprintfJdc(result, 1, "%s %s", primitiveTypeStrs[function->regArgs[i].type], function->regArgs[i].name.buffer))
			{
				return 0;
			}
		}
		else 
		{
			if (!sprintfJdc(result, 1, "%s %s, ", primitiveTypeStrs[function->regArgs[i].type], function->regArgs[i].name.buffer))
			{
				return 0;
			}
		}
	}

	for (int i = 0; i < function->numOfStackArgs; i++)
	{
		if (i == function->numOfStackArgs - 1)
		{
			if (!sprintfJdc(result, 1, "%s %s", primitiveTypeStrs[function->stackArgs[i].type], function->stackArgs[i].name.buffer))
			{
				return 0;
			}
		}
		else
		{
			if (!sprintfJdc(result, 1, "%s %s, ", primitiveTypeStrs[function->stackArgs[i].type], function->stackArgs[i].name.buffer))
			{
				return 0;
			}
		}
	}

	return strcatJdc(result, ")\n");
}

static unsigned char declareAllLocalVariables(struct Function* function, struct JdcStr* result)
{
	for (int i = 0; i < function->numOfLocalVars; i++)
	{
		if (!sprintfJdc(result, 1, "%s%s %s;\n", indent, primitiveTypeStrs[function->localVars[i].type], function->localVars[i].name.buffer))
		{
			return 0;
		}
	}

	for (int i = 0; i < function->numOfRegVars; i++)
	{
		if (!sprintfJdc(result, 1, "%s%s %s;\n", indent, primitiveTypeStrs[function->regVars[i].type], function->regVars[i].name.buffer))
		{
			return 0;
		}
	}

	for (int i = 0; i < function->numOfReturnedVars; i++)
	{
		if (!sprintfJdc(result, 1, "%s%s %s;\n", indent, primitiveTypeStrs[function->returnedVars[i].type], function->returnedVars[i].name.buffer))
		{
			return 0;
		}
	}

	return strcatJdc(result, "\n");
}
