#include "decompiler.h"
#include "decompilationUtils.h"
#include "functions.h"
#include "assignment.h"
#include "returnStatements.h"
#include "functionCalls.h"
#include "intrinsics.h"
#include "conditions.h"
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
	int numOfSkippedInstructions = 0;
	for (int i = params->currentFunc->firstInstructionIndex; i <= params->currentFunc->lastInstructionIndex; i++)
	{
		struct DisassembledInstruction* currentInstruction = &(params->instructions[i]);
		if (currentInstruction->address == 0x140086E82) 
		{
			int ttt = 0;
		}

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

		if (isInUnreachableState)
		{
			if (checkForDirectJmpDst(params) != -1 || checkForConditionDst(params))
			{
				if(numOfSkippedInstructions > 0)
				{
					addIndents(result, params->numOfIndents);
					sprintfJdc(result, 1, "// %i instructions skipped\n", numOfSkippedInstructions);
				}

				isInUnreachableState = 0;
				numOfSkippedInstructions = 0;
			}
			else 
			{
				numOfSkippedInstructions++;
				continue;
			}
		}

		if (!decompileConditions(params, result))
		{
			sprintfJdc(result, 0, "Error decompiling condition at 0x%llX.", currentInstruction->address);
			return 0;
		}

		if (!decompileDirectJmps(params, &isInUnreachableState, result))
		{
			sprintfJdc(result, 0, "Error decompiling direct jump at 0x%llX.", currentInstruction->address);
			return 0;
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

		if (checkForReturnStatement(params))
		{
			if (!decompileReturnStatement(params, &isInUnreachableState, result))
			{
				sprintfJdc(result, 0, "Error decompiling return statement at 0x%llX.", currentInstruction->address);
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

static unsigned char isRegisterAccessedBeforeInit(struct DecompilationParameters* params, int lastInstructionIndex, enum Register reg, struct VarType* typeRef)
{
	int ogStartInstructionIndex = params->startInstructionIndex;
	for (int i = ogStartInstructionIndex; i <= lastInstructionIndex; i++)
	{
		struct DisassembledInstruction* instruction = &(params->instructions[i]);
		params->startInstructionIndex = i;

		unsigned char overwrites = 0;
		if (doesInstructionModifyRegister(instruction, reg, 0, 0, &overwrites) && overwrites)
		{
			return 0;
		}

		struct Function* callee;
		if (checkForKnownFunctionCall(params, &callee) && callee)
		{
			struct RegisterVariable* regArg = getRegArgByReg(callee, reg);
			if (regArg)
			{
				if (typeRef) { *typeRef = regArg->type; }
				return 1;
			}
			else if (compareRegisters(callee->returnReg, reg)) 
			{
				return 0;
			}

			continue;
		}
		
		if (checkForUnknownFunctionCall(params))
		{
			if (compareRegisters(reg, AX)) 
			{
				return 0;
			}
			
			continue;
		}
		
		if (checkForReturnStatement(params))
		{
			if (compareRegisters(params->currentFunc->returnReg, reg)) 
			{
				if (typeRef) { *typeRef = params->currentFunc->returnType; }
				return 1;
			}
			
			continue;
		}

		unsigned char operandNum = 0;
		if (doesInstructionAccessRegister(instruction, reg, &operandNum))
		{
			if (typeRef) { *typeRef = getTypeOfOperand(instruction->opcode, &(instruction->operands[operandNum])); }
			return 1;
		}

		if (isOpcodeJmp(instruction->opcode))
		{
			int dstIndex = findInstructionByAddress(params->instructions, 0, params->numOfInstructions - 1, resolveJmpChain(params));
			if (dstIndex < i)
			{
				lastInstructionIndex = i - 1;
			}

			i = dstIndex - 1;
		}
		else if (isOpcodeJcc(instruction->opcode)) 
		{
			int dstIndex = findInstructionByAddress(params->instructions, 0, params->numOfInstructions - 1, resolveJmpChain(params));
			params->startInstructionIndex = dstIndex;
			if (dstIndex < i) 
			{
				if (isRegisterAccessedBeforeInit(params, i - 1, reg, typeRef))
				{
					return 1;
				}
			}
			else 
			{
				if (isRegisterAccessedBeforeInit(params, params->currentFunc->lastInstructionIndex, reg, typeRef))
				{
					return 1;
				}
			}
		}
	}

	params->startInstructionIndex = ogStartInstructionIndex;
	return 0;
}

static unsigned char getAllReturnedVars(struct DecompilationParameters* params)
{
	for (int i = params->currentFunc->firstInstructionIndex; i <= params->currentFunc->lastInstructionIndex; i++)
	{
		params->startInstructionIndex = i;
		struct Function* callee = 0;
		if ((checkForKnownFunctionCall(params, &callee) && callee && callee->returnType.primitiveType != VOID_TYPE) || checkForUnknownFunctionCall(params))
		{
			struct DisassembledInstruction* callInstruction = &(params->instructions[i]);
			int callInstructionIndex = i;
			enum Register returnReg = callee ? callee->returnReg : AX;

			unsigned long long calleeAddress = resolveJmpChain(params);

			struct VarType returnType = { 0 }; // used both if its an import call and to check if the return value is used
			params->startInstructionIndex++;
			if(!isRegisterAccessedBeforeInit(params, params->currentFunc->lastInstructionIndex, returnReg, &returnType))
			{
				continue;
			}

			if (callee)
			{
				if (!addReturnedVar(params->currentFunc, callee->returnType, calleeAddress, callInstruction->address, returnReg, callee->name.buffer))
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
					if (!addReturnedVar(params->currentFunc, returnType, calleeAddress, callInstruction->address, returnReg, params->imports[importIndex].name.buffer))
					{
						return 0;
					}
				}
				else 
				{
					if (!addReturnedVar(params->currentFunc, returnType, calleeAddress, callInstruction->address, returnReg, "funcPtr"))
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
		if (!condition->isCombinedByOther && !condition->decompileAsReturn)
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
					struct Condition* cond = &params->currentFunc->conditions[conditionIndex];
					if (!cond->isCombinedByOther && !cond->decompileAsGoTo && !cond->decompileAsReturn) 
					{
						int conditionEnd = getConditionEnd(cond);
						if (conditionEnd <= end) 
						{
							j = conditionEnd - 1;
							continue;
						}
					}
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
			params->startInstructionIndex = end;
			for (int k = 0; k < numOfRegs; k++)
			{
				if ((condition->conditionType == LOOP_CT || condition->conditionType == DO_WHILE_CT) || // any modified reg in a loop will be added as a reg var
					isRegisterAccessedBeforeInit(params, params->currentFunc->lastInstructionIndex, modifiedRegs[k].reg, 0))
				{
					if (!addRegVar(params->currentFunc, modifiedRegs[k].type, modifiedRegs[k].reg))
					{
						return 0;
					}
				}
			}

			// if there are registers used in the comparisson of loop, they need to be a reg var
			if (condition->conditionType == DO_WHILE_CT || condition->conditionType == LOOP_CT)
			{
				for (int j = condition->jccIndex - 1; j >= params->currentFunc->firstInstructionIndex; j--)
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
