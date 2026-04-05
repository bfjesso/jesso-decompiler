#include "decompiler.h"
#include "decompilationUtils.h"
#include "functions.h"
#include "assignment.h"
#include "returnStatements.h"
#include "functionCalls.h"
#include "intrinsics.h"
#include "directJmps.h"
#include "dataTypes.h"
#include "../disassembler/registers.h"

unsigned char decompileFunction(struct DecompilationParameters* params, struct JdcStr* result)
{
	if(!params->currentFunc->conditions && !getAllConditions(params))
	{
		strcpyJdc(result, "Error getting all conditions.");
		return 0;
	}

	if(!params->currentFunc->directJmps && !getAllDirectJmps(params))
	{
		strcpyJdc(result, "Error getting all direct jumps.");
		return 0;
	}
	
	if (!params->currentFunc->hasGottenLocalVars)
	{
		if (!getAllRegVars(params))
		{
			strcpyJdc(result, "Error getting all reg vars.");
			return 0;
		}

		if (!getAllReturnedVars(params))
		{
			strcpyJdc(result, "Error getting all returned vars.");
			return 0;
		}

		params->currentFunc->hasGottenLocalVars = 1;
	}
	
	if (!generateFunctionHeader(params->currentFunc, result))
	{
		strcpyJdc(result, "Error generating function header.");
		return 0;
	}

	strcatJdc(result, "{\n");

	if (params->currentFunc->numOfStackVars > 0 || params->currentFunc->numOfReturnedVars > 0 || params->currentFunc->numOfRegVars > 0)
	{
		if (!declareAllLocalVariables(params, result))
		{
			strcpyJdc(result, "Error declaring local variables.");
			return 0;
		}
	}

	params->numOfIndents = 1;
	unsigned char isInUnreachableState = 0;
	for (int i = params->currentFunc->firstInstructionIndex; i <= params->currentFunc->lastInstructionIndex; i++)
	{
		struct DisassembledInstruction* currentInstruction = &(params->instructions[i]);
		
		if (params->numOfIndents < 1)
		{
			sprintfJdc(result, 0, "Bad indentation. There is an error with condition handling at 0x%llX.", currentInstruction->address);
			return 0;
		}
		
		params->startInstructionIndex = i;

		if (currentInstruction->opcode > lastImplementedOpcode && currentInstruction->opcode < EXTENDED_OPCODE) // temporary check
		{
			sprintfJdc(result, 0, "%s at 0x%llX is not yet handled in the decompiler.", mnemonicStrs[currentInstruction->opcode], currentInstruction->address);
			return 0;
		}

		if (checkForDirectJmpEnd(params) || checkForConditionEnd(params)) 
		{
			isInUnreachableState = 0;
		}

		if (isInUnreachableState) 
		{
			continue;
		}

		if (!decompileDirectJmps(params, result)) 
		{
			sprintfJdc(result, 0, "Error decompiling direct jump at 0x%llX.", currentInstruction->address);
			return 0;
		}

		if (checkForDirectJmpStart(params))
		{
			isInUnreachableState = 1;
		}

		if (!decompileConditions(params, result))
		{
			sprintfJdc(result, 0, "Error decompiling condition at 0x%llX.", currentInstruction->address);
			return 0;
		}
		else if (checkForReturnStatement(params))
		{
			if (!decompileReturnStatement(params, result))
			{
				sprintfJdc(result, 0, "Error decompiling return statement at 0x%llX.", currentInstruction->address);
				return 0;
			}

			isInUnreachableState = 1;
		}

		struct Function* callee;
		struct IntrinsicFunc* intrinsicFunc;
		if (checkForKnownFunctionCall(params, &callee))
		{
			if (!decompileKnownFunctionCall(params, callee, result))
			{
				sprintfJdc(result, 0, "Error decompiling known function call at 0x%llX.", currentInstruction->address);
				return 0;
			}
		}
		else if (checkForUnknownFunctionCall(params))
		{
			if (!decompileUnknownFunctionCall(params, result))
			{
				sprintfJdc(result, 0, "Error decompiling unknown function call at 0x%llX.", currentInstruction->address);
				return 0;
			}
		}
		else if (checkForVoidIntrinsicFunc(params, &intrinsicFunc))
		{
			if (!decompileVoidIntrinsicFunc(params, intrinsicFunc, result))
			{
				sprintfJdc(result, 0, "Error decompiling intrinsic function at 0x%llX.", currentInstruction->address);
				return 0;
			}
		}
		else if (checkForAssignment(params))
		{
			if (!decompileAssignments(params, result))
			{
				sprintfJdc(result, 0, "Error decompiling assignment at 0x%llX.", currentInstruction->address);
				return 0;
			}
		}
	}

	if (params->numOfIndents != 1)
	{
		strcpyJdc(result, "Bad indentation. There is an error with condition handling and the decompilation did not finish at one indentation.");
		return 0;
	}

	return strcatJdc(result, "}\n");
}

static unsigned char getAllReturnedVars(struct DecompilationParameters* params)
{
	for (int i = params->currentFunc->firstInstructionIndex; i <= params->currentFunc->lastInstructionIndex; i++)
	{
		struct DisassembledInstruction* currentInstruction = &(params->instructions[i]);
		int callInstructionIndex = i;
		params->startInstructionIndex = i;
		struct Function* callee = 0;
		if (checkForKnownFunctionCall(params, &callee) || checkForUnknownFunctionCall(params))
		{
			enum Register returnReg = callee ? callee->returnReg : AX;

			unsigned long long calleeAddress = resolveJmpChain(params);

			struct VarType returnType = { 0 }; // used if its an import call, also using this here to check if the return value is used
			for (int j = i; j <= params->currentFunc->lastInstructionIndex; j++)
			{
				params->startInstructionIndex = j;
				currentInstruction = &(params->instructions[j]);
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
					returnType = params->currentFunc->returnType;
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
			for (int j = 0; j < params->currentFunc->numOfReturnedVars; j++)
			{
				if (params->currentFunc->returnedVars[j].callAddr == calleeAddress)
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
				else if (!addReturnedVar(params->currentFunc, callee->returnType, callNum, calleeAddress, returnReg, callee->name.buffer))
				{
					return 0;
				}
			}
			else
			{
				params->startInstructionIndex = callInstructionIndex;
				int importIndex = getImportIndexByAddress(params, calleeAddress);
				if (importIndex != -1) 
				{
					if (!addReturnedVar(params->currentFunc, returnType, callNum, calleeAddress, returnReg, params->imports[importIndex].name.buffer))
					{
						return 0;
					}
				}
				else 
				{
					if (!addReturnedVar(params->currentFunc, returnType, callNum, calleeAddress, returnReg, "funcPtr"))
					{
						return 0;
					}
				}
			}
		}
	}

	return 1;
}

static unsigned char getAllRegVars(struct DecompilationParameters* params)
{
	// checking for registers that are modified in a condition
	for (int i = 0; i < params->currentFunc->numOfConditions; i++)
	{
		struct Condition* condition = &params->currentFunc->conditions[i];
		if (!condition->isCombinedByOther && !condition->decompileAsReturn && !condition->decompileAsGoTo)
		{
			struct RegisterVariable modifiedRegs[ST0 - RAX] = { 0 };
			int numOfRegs = 0;

			int start = getConditionStart(condition);
			int end = getConditionEnd(condition);
			
			for (int j = start; j < end; j++)
			{
				params->startInstructionIndex = j;
				int conditionIndex = checkForConditionStart(params);
				if (conditionIndex != -1 && conditionIndex != i)
				{
					break;
				}

				if (checkForReturnStatement(params))
				{
					break;
				}

				struct DisassembledInstruction* currentInstruction = &(params->instructions[j]);

				enum Register reg = NO_REG;

				struct Function* callee;
				params->startInstructionIndex = j;
				if ((checkForKnownFunctionCall(params, &callee) && callee->returnType.primitiveType != VOID_TYPE))
				{
					reg = callee->returnReg;
				}
				else if (checkForUnknownFunctionCall(params))
				{
					reg = params->is64Bit ? RAX : EAX;
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
					if (alreadyFound || getRegVarByReg(params->currentFunc, reg))
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
			int checkingStart = condition->conditionType == LOOP_CT || condition->conditionType == DO_WHILE_CT ? start : end; // if it is a loop, the code can run more than once so it needs to start checking from the begining of the loop
			for (int j = checkingStart; j <= params->currentFunc->lastInstructionIndex; j++)
			{
				params->startInstructionIndex = j;
				int conditionIndex = checkForConditionStart(params);
				if (conditionIndex != -1 && conditionIndex != i)
				{
					break;
				}

				struct DisassembledInstruction* currentInstruction = &(params->instructions[j]);

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
						if (!addRegVar(params->currentFunc, modifiedRegs[k].type, modifiedRegs[k].reg)) 
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

			// if there are registers used in the comparisson of loop, they need to be a reg var
			if (condition->conditionType == DO_WHILE_CT || condition->conditionType == LOOP_CT)
			{
				for (int j = end; j >= start; j--) 
				{
					struct DisassembledInstruction* currentInstruction = &(params->instructions[j]);
					
					if (isOpcodeCmp(currentInstruction->opcode) || currentInstruction->opcode == TEST) 
					{
						for (int k = 0; k < 2; k++) 
						{
							if (currentInstruction->operands[k].type == REGISTER && !isRegisterPointer(currentInstruction->operands[k].reg) && !getRegVarByReg(params->currentFunc, currentInstruction->operands[k].reg))
							{
								if (!addRegVar(params->currentFunc, getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[k]), currentInstruction->operands[k].reg))
								{
									return 0;
								}
							}
							else if (currentInstruction->operands[k].type == MEM_ADDRESS && !isRegisterPointer(currentInstruction->operands[k].memoryAddress.reg) && !getRegVarByReg(params->currentFunc, currentInstruction->operands[k].memoryAddress.reg))
							{
								if (!addRegVar(params->currentFunc, getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[k]), currentInstruction->operands[k].memoryAddress.reg))
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
	for (int i = params->currentFunc->firstInstructionIndex; i <= params->currentFunc->lastInstructionIndex; i++)
	{
		struct DisassembledInstruction* currentInstruction = &(params->instructions[i]);

		if (currentInstruction->operands[0].type == REGISTER && !isRegisterPointer(currentInstruction->operands[0].reg) && doesInstructionModifyOperand(currentInstruction, 0, 0, 0))
		{
			enum Register reg = currentInstruction->operands[0].reg;
			
			unsigned char alreadyFound = 0;
			for (int j = 0; j < params->currentFunc->numOfRegVars; j++)
			{
				if (compareRegisters(reg, params->currentFunc->regVars[j].reg))
				{
					params->currentFunc->regVars[j].type.isUnsigned = doesOpcodeUseUnsignedInt(currentInstruction->opcode);
					alreadyFound = 1;
					break;
				}
			}
			if (alreadyFound)
			{
				continue;
			}

			unsigned char dependsOnRegVar = 0;
			for (int j = 0; j < params->currentFunc->numOfRegVars; j++)
			{
				if (currentInstruction->operands[1].type == REGISTER)
				{
					if (compareRegisters(params->currentFunc->regVars[j].reg, currentInstruction->operands[1].reg))
					{
						dependsOnRegVar = 1;
						break;
					}
				}
				else if (currentInstruction->operands[1].type == MEM_ADDRESS)
				{
					if (compareRegisters(params->currentFunc->regVars[j].reg, currentInstruction->operands[1].memoryAddress.reg) || compareRegisters(params->currentFunc->regVars[j].reg, currentInstruction->operands[1].memoryAddress.regDisplacement))
					{
						dependsOnRegVar = 1;
						break;
					}
				}
			}

			if (dependsOnRegVar)
			{
				if (!addRegVar(params->currentFunc, getTypeOfOperand(currentInstruction->opcode, &currentInstruction->operands[0]), reg))
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

static unsigned char declareAllLocalVariables(struct DecompilationParameters* params, struct JdcStr* result)
{
	struct JdcStr typeStr = initializeJdcStr();
	
	for (int i = 0; i < params->currentFunc->numOfStackVars; i++)
	{
		varTypeToStr(params->currentFunc->stackVars[i].type, &typeStr);
		addIndents(result, 1);
		sprintfJdc(result, 1, "%s %s;\n", typeStr.buffer, params->currentFunc->stackVars[i].name.buffer);
	}

	for (int i = 0; i < params->currentFunc->numOfRegVars; i++)
	{
		int argIndex = -1;
		for (int j = 0; j < params->currentFunc->numOfRegArgs; j++)
		{
			if (compareRegisters(params->currentFunc->regVars[i].reg, params->currentFunc->regArgs[j].reg))
			{
				argIndex = j;
				break;
			}
		}

		varTypeToStr(params->currentFunc->regVars[i].type, &typeStr);

		addIndents(result, 1);
		if (argIndex != -1)
		{
			sprintfJdc(result, 1, "%s %s = %s;\n", typeStr.buffer, params->currentFunc->regVars[i].name.buffer, params->currentFunc->regArgs[argIndex].name.buffer);
		}
		else 
		{
			sprintfJdc(result, 1, "%s %s;\n", typeStr.buffer, params->currentFunc->regVars[i].name.buffer);
		}
	}

	for (int i = 0; i < params->currentFunc->numOfReturnedVars; i++)
	{
		unsigned char isReturnRegVar = 0;
		for (int j = 0; j < params->currentFunc->numOfRegVars; j++) 
		{
			if (compareRegisters(params->currentFunc->regVars[j].reg, params->currentFunc->returnedVars[i].returnReg)) 
			{
				isReturnRegVar = 1;
				break;
			}
		}

		if (!isReturnRegVar) 
		{
			varTypeToStr(params->currentFunc->returnedVars[i].type, &typeStr);
			addIndents(result, 1);
			sprintfJdc(result, 1, "%s %s;\n", typeStr.buffer, params->currentFunc->returnedVars[i].name.buffer);
		}
	}

	freeJdcStr(&typeStr);
	return strcatJdc(result, "\n");
}
