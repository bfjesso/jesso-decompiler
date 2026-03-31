#include "decompiler.h"
#include "decompilationUtils.h"
#include "expressions.h"
#include "functions.h"
#include "assignment.h"
#include "returnStatements.h"
#include "functionCalls.h"
#include "intrinsics.h"
#include "directJmps.h"
#include "dataTypes.h"
#include "../disassembler/registers.h"

unsigned char decompileFunction(struct DecompilationParameters params, struct JdcStr* result)
{
	if(!params.currentFunc->conditions && !getAllConditions(params))
	{
		return 0;
	}

	if(!params.currentFunc->directJmps && !getAllDirectJmps(params))
	{
		return 0;
	}
	
	if (!params.currentFunc->hasGottenLocalVars)
	{
		if (!getAllRegVars(params))
		{
			return 0;
		}

		if (!getAllReturnedVars(params))
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

	if (params.currentFunc->numOfStackVars > 0 || params.currentFunc->numOfReturnedVars > 0 || params.currentFunc->numOfRegVars > 0)
	{
		if (!declareAllLocalVariables(params, result))
		{
			return 0;
		}
	}

	unsigned char numOfIndents = 1;
	unsigned char isInSwitch = 0;
	for (int i = 0; i < params.currentFunc->numOfInstructions; i++)
	{
		if (numOfIndents < 1)
		{
			return 0;
		}
		
		params.startInstructionIndex = i;

		struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[i]);

		// handling condition dsts
		for (int j = 0; j < params.currentFunc->numOfConditions; j++)
		{
			struct Condition* condition = &params.currentFunc->conditions[j];
			if (!condition->decompileAsReturn && !condition->decompileAsGoTo && !condition->isCombinedByOther && i == condition->dstIndex)
			{
				if (condition->conditionType == DO_WHILE_CT)
				{
					addIndents(result, numOfIndents);
					strcatJdc(result, "do\n");
					addIndents(result, numOfIndents);
					strcatJdc(result, "{\n");
					numOfIndents++;
					condition->hasEnteredCondition = 1;
				}
				else if (condition->conditionType == SWITCH_CASE_CT) 
				{
					if (!condition->isFirstSwitchCase) 
					{
						addIndents(result, numOfIndents);
						strcatJdc(result, "break;\n");
					}
					
					struct JdcStr switchCase = initializeJdcStr();
					if (!decompileOperand(params, &condition->cmpInstruction->operands[1], getTypeOfOperand(condition->cmpInstruction->opcode, &condition->cmpInstruction->operands[1]), &switchCase))
					{
						freeJdcStr(&switchCase);
						return 0;
					}
					
					addIndents(result, numOfIndents - 1);
					sprintfJdc(result, 1, "case %s:\n", switchCase.buffer);
					freeJdcStr(&switchCase);
				}
				else if (condition->hasEnteredCondition)
				{
					numOfIndents--;
					addIndents(result, numOfIndents);
					strcatJdc(result, "}\n");
				}
			}
			else if (condition->isFirstSwitchCase && i == condition->exitIndex) 
			{
				addIndents(result, numOfIndents);
				strcatJdc(result, "break;\n");

				isInSwitch = 0;

				numOfIndents--;
				addIndents(result, numOfIndents);
				strcatJdc(result, "}\n");
			}
		}

		// handling condition Jccs
		for (int j = 0; j < params.currentFunc->numOfConditions; j++)
		{
			struct Condition* condition = &params.currentFunc->conditions[j];
			if(condition->isCombinedByOther)
			{
				continue;
			}

			if (condition->jccIndex == i && (condition->conditionType != SWITCH_CASE_CT || condition->isFirstSwitchCase))
			{
				if (condition->conditionType == DO_WHILE_CT)
				{
					if (condition->hasEnteredCondition)
					{
						numOfIndents--;
					}
					else
					{
						continue;
					}
				}

				if (condition->isFirstSwitchCase) 
				{
					isInSwitch = condition->isFirstSwitchCase;
				}

				addIndents(result, numOfIndents);

				if (decompileCondition(params, j, result))
				{
					strcatJdc(result, "\n");

					if (condition->conditionType != DO_WHILE_CT)
					{
						addIndents(result, numOfIndents);
						strcatJdc(result, "{\n");

						numOfIndents++;
						condition->hasEnteredCondition = 1;
					}
				}
				else
				{
					return 0;
				}

				if (condition->decompileAsReturn)
				{
					addIndents(result, numOfIndents);
					if (decompileReturnStatement(params, result))
					{
						strcatJdc(result, "\n");
						numOfIndents--;
						addIndents(result, numOfIndents);
						strcatJdc(result, "}\n");
					}
					else
					{
						return 0;
					}
				}
				else if (condition->decompileAsGoTo)
				{
					addIndents(result, numOfIndents);
					sprintfJdc(result, 1, "goto label_%llX;\n", params.currentFunc->instructions[condition->dstIndex].address - params.imageBase);

					numOfIndents--;
					addIndents(result, numOfIndents);
					strcatJdc(result, "}\n");
				}

				break;
			}
		}

		for (int j = 0; j < params.currentFunc->numOfConditions; j++)
		{
			struct Condition* condition = &params.currentFunc->conditions[j];
			if (condition->decompileAsGoTo && i == condition->dstIndex)
			{
				addIndents(result, numOfIndents - 1);
				sprintfJdc(result, 1, "label_%llX:\n", params.currentFunc->instructions[condition->dstIndex].address - params.imageBase);
				break;
			}
		}

		for (int j = 0; j < params.currentFunc->numOfDirectJmps; j++)
		{
			if (i == params.currentFunc->directJmps[j].dstIndex && params.currentFunc->directJmps[j].type == GO_TO_DJT)
			{
				addIndents(result, numOfIndents - 1);
				sprintfJdc(result, 1, "label_%llX:\n", params.currentFunc->instructions[params.currentFunc->directJmps[j].dstIndex].address - params.imageBase);
				break;
			}
			else if (i == params.currentFunc->directJmps[j].jmpIndex)
			{
				addIndents(result, numOfIndents);

				switch (params.currentFunc->directJmps[j].type)
				{
				case GO_TO_DJT:
					sprintfJdc(result, 1, "goto label_%llX;\n", params.currentFunc->instructions[params.currentFunc->directJmps[j].dstIndex].address - params.imageBase);
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

		struct Function* callee;
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

		struct IntrinsicFunc* intrinsicFunc;
		if (checkForVoidIntrinsicFunc(currentInstruction->opcode, &intrinsicFunc))
		{
			addIndents(result, numOfIndents);
			if (decompileVoidIntrinsicFunc(params, intrinsicFunc, result))
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
			if (!decompileAssignments(params, result, numOfIndents))
			{
				return 0;
			}
		}

		if (checkForReturnStatement(i, params.currentFunc->instructions, params.currentFunc->numOfInstructions) && !isInSwitch)
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
	}

	return strcatJdc(result, "}\n");
}

static unsigned char getAllReturnedVars(struct DecompilationParameters params)
{
	for (int i = 0; i < params.currentFunc->numOfInstructions; i++)
	{
		params.startInstructionIndex = i;
		struct Function* callee = 0;
		int importIndex = checkForImportCall(params);
		if (checkForFunctionCall(params, &callee) || importIndex != -1)
		{
			enum Register returnReg = callee ? callee->returnReg : AX;
			unsigned long long calleeAddress = callee ? callee->instructions[0].address : params.imports[importIndex].address;

			struct VarType returnType = { 0 }; // used if its an import call, also using this here to check if the return value is used
			for (int j = i; j < params.currentFunc->numOfInstructions; j++)
			{
				params.startInstructionIndex = j;
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

				if (checkForReturnStatement(j, params.currentFunc->instructions, params.currentFunc->numOfInstructions))
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

static unsigned char getAllRegVars(struct DecompilationParameters params)
{
	// checking for registers that are modified in a condition
	for (int i = 0; i < params.currentFunc->numOfConditions; i++)
	{
		if (!params.currentFunc->conditions[i].isCombinedByOther && !params.currentFunc->conditions[i].decompileAsReturn && !params.currentFunc->conditions[i].decompileAsGoTo)
		{
			struct RegisterVariable modifiedRegs[ST0 - RAX] = { 0 };
			int numOfRegs = 0;

			int start = params.currentFunc->conditions[i].jccIndex;
			int end = params.currentFunc->conditions[i].dstIndex;

			if (params.currentFunc->conditions[i].conditionType == DO_WHILE_CT)
			{
				start = params.currentFunc->conditions[i].dstIndex;
				end = params.currentFunc->conditions[i].jccIndex;
			}
			
			for (int j = start; j < end; j++)
			{
				params.startInstructionIndex = j;
				int conditionIndex = checkForConditionStart(params);
				if (conditionIndex != -1 && conditionIndex != i)
				{
					break;
				}

				if (checkForReturnStatement(j, params.currentFunc->instructions, params.currentFunc->numOfInstructions))
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

					struct VarType type = getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[0]);
					modifiedRegs[numOfRegs].reg = reg;
					modifiedRegs[numOfRegs].type = type;
					numOfRegs++;
				}
			}

			// checking if the modified regs are accessed before being overwritten after the condition
			int checkingStart = params.currentFunc->conditions[i].conditionType == LOOP_CT || params.currentFunc->conditions[i].conditionType == DO_WHILE_CT ? start : end; // if it is a loop, the code can run more than once so it needs to start checking from the begining of the loop
			for (int j = checkingStart; j < params.currentFunc->numOfInstructions; j++)
			{
				params.startInstructionIndex = j;
				int conditionIndex = checkForConditionStart(params);
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
			if (params.currentFunc->conditions[i].conditionType == DO_WHILE_CT)
			{
				for (int j = end; j >= start; j--) 
				{
					struct DisassembledInstruction* currentInstruction = &(params.currentFunc->instructions[j]);
					
					if (isOpcodeCmp(currentInstruction->opcode) || currentInstruction->opcode == TEST) 
					{
						for (int k = 0; k < 2; k++) 
						{
							if (currentInstruction->operands[k].type == REGISTER && !isRegisterPointer(currentInstruction->operands[k].reg) && !getRegVarByReg(params.currentFunc, currentInstruction->operands[k].reg))
							{
								if (!addRegVar(params.currentFunc, getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[k]), currentInstruction->operands[k].reg))
								{
									return 0;
								}
							}
							else if (currentInstruction->operands[k].type == MEM_ADDRESS && !isRegisterPointer(currentInstruction->operands[k].memoryAddress.reg) && !getRegVarByReg(params.currentFunc, currentInstruction->operands[k].memoryAddress.reg))
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

		if (currentInstruction->operands[0].type == REGISTER && !isRegisterPointer(currentInstruction->operands[0].reg) && doesInstructionModifyOperand(currentInstruction, 0, 0, 0))
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
		addIndents(result, 1);
		sprintfJdc(result, 1, "%s %s;\n", typeStr.buffer, params.currentFunc->stackVars[i].name.buffer);
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

		addIndents(result, 1);
		if (argIndex != -1)
		{
			sprintfJdc(result, 1, "%s %s = %s;\n", typeStr.buffer, params.currentFunc->regVars[i].name.buffer, params.currentFunc->regArgs[argIndex].name.buffer);
		}
		else 
		{
			sprintfJdc(result, 1, "%s %s;\n", typeStr.buffer, params.currentFunc->regVars[i].name.buffer);
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
			addIndents(result, 1);
			sprintfJdc(result, 1, "%s %s;\n", typeStr.buffer, params.currentFunc->returnedVars[i].name.buffer);
		}
	}

	freeJdcStr(&typeStr);
	return strcatJdc(result, "\n");
}
