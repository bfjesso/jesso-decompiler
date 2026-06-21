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

unsigned char decompileFunction(struct DecompilationParameters* params, struct JdcStr* result, struct JdcStr* statusMessage, int* errorInstructionIndex)
{
	if (!params->currentFunc->hasDoneInitialAnalysis)
	{
		if (!getAllDirectJmps(params))
		{
			strcpyJdc(statusMessage, "Error getting all direct jumps.");
			return 0;
		}
		
		if (!getAllRegVars(params))
		{
			strcpyJdc(statusMessage, "Error getting all reg vars.");
			return 0;
		}

		if (!getAllReturnedVars(params))
		{
			strcpyJdc(statusMessage, "Error getting all returned vars.");
			return 0;
		}

		params->currentFunc->associatedInstructionsBufferLen = params->currentFunc->lastInstructionIndex - params->currentFunc->firstInstructionIndex;
		params->currentFunc->associatedInstructions = (struct AssociatedInstructions*)calloc(params->currentFunc->associatedInstructionsBufferLen, sizeof(struct AssociatedInstructions));
		if (!params->currentFunc->associatedInstructions) 
		{
			return 0;
		}

		params->currentFunc->hasDoneInitialAnalysis = 1;
	}
	
	if (!generateFunctionHeader(params->currentFunc, result))
	{
		strcpyJdc(statusMessage, "Error generating function header.");
		if (errorInstructionIndex) { *errorInstructionIndex = params->currentFunc->firstInstructionIndex; }
		return 0;
	}

	strcatJdc(result, "\n{\n");

	if (params->currentFunc->numOfStackVars > 0 || params->currentFunc->numOfReturnedVars > 0 || params->currentFunc->numOfRegVars > 0)
	{
		if (!declareAllLocalVariables(params, result))
		{
			strcpyJdc(statusMessage, "Error declaring local variables.");
			if (errorInstructionIndex) { *errorInstructionIndex = params->currentFunc->firstInstructionIndex; }
			return 0;
		}
	}

	int len = (int)strlen(result->buffer);
	for (int i = 0; i < len; i++) 
	{
		if (result->buffer[i] == '\n')
		{
			addAssociatedInstruction(params->currentFunc, params->currentFunc->firstInstructionIndex);
			params->currentFunc->numOfLines++;
		}
	}

	params->numOfIndents = 1;
	unsigned char isInUnreachableState = 0;
	int numOfSkippedInstructions = 0;
	for (int i = params->currentFunc->firstInstructionIndex; i <= params->currentFunc->lastInstructionIndex; i++)
	{
		struct DisassembledInstruction* currentInstruction = &(params->instructions[i]);

		if (params->numOfIndents < 1)
		{
			sprintfJdc(statusMessage, 0, "Bad indentation. There is an error with condition handling at 0x%llX.", currentInstruction->address);
			if (errorInstructionIndex) { *errorInstructionIndex = i; }
			return 0;
		}
		
		if (currentInstruction->opcode > lastImplementedOpcode && currentInstruction->opcode < EXTENDED_OPCODE) // temporary check
		{
			sprintfJdc(statusMessage, 0, "%s at 0x%llX is not yet handled in the decompiler.", mnemonicStrs[currentInstruction->opcode], currentInstruction->address);
			if (errorInstructionIndex) { *errorInstructionIndex = i; }
			return 0;
		}

		if (isInUnreachableState)
		{
			if (getDirectJmpDst(params, i) != -1 || checkForConditionDst(params, i))
			{
				if(numOfSkippedInstructions > 0)
				{
					addIndents(result, params->numOfIndents);
					sprintfJdc(result, 1, "// %i instruction(s) skipped\n", numOfSkippedInstructions);

					for (int j = numOfSkippedInstructions; j > 0; j--)
					{
						addAssociatedInstruction(params->currentFunc, i - j);
					}
					params->currentFunc->numOfLines++;
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

		if (!decompileConditions(params, i, result))
		{
			sprintfJdc(statusMessage, 0, "Error decompiling condition at 0x%llX.", currentInstruction->address);
			if (errorInstructionIndex) { *errorInstructionIndex = i; }
			return 0;
		}

		if (!decompileDirectJmps(params, i, &isInUnreachableState, result))
		{
			sprintfJdc(statusMessage, 0, "Error decompiling direct jump at 0x%llX.", currentInstruction->address);
			if (errorInstructionIndex) { *errorInstructionIndex = i; }
			return 0;
		}

		struct Function* callee;
		struct IntrinsicFunc* intrinsicFunc;
		if (checkForKnownFunctionCall(params, i, &callee))
		{
			if (!decompileKnownFunctionCall(params, i, callee, result))
			{
				sprintfJdc(statusMessage, 0, "Error decompiling known function call at 0x%llX.", currentInstruction->address);
				if (errorInstructionIndex) { *errorInstructionIndex = i; }
				return 0;
			}
		}
		else if (checkForUnknownFunctionCall(params, i))
		{
			if (!decompileUnknownFunctionCall(params, i, result))
			{
				sprintfJdc(statusMessage, 0, "Error decompiling unknown function call at 0x%llX.", currentInstruction->address);
				if (errorInstructionIndex) { *errorInstructionIndex = i; }
				return 0;
			}
		}
		else if (checkForVoidIntrinsicFunc(params, i, &intrinsicFunc))
		{
			if (!decompileVoidIntrinsicFunc(params, i, intrinsicFunc, result))
			{
				sprintfJdc(statusMessage, 0, "Error decompiling intrinsic function at 0x%llX.", currentInstruction->address);
				if (errorInstructionIndex) { *errorInstructionIndex = i; }
				return 0;
			}
		}
		else if (checkForAssignment(params, i))
		{
			if (!decompileAssignments(params, i, result))
			{
				sprintfJdc(statusMessage, 0, "Error decompiling assignment at 0x%llX.", currentInstruction->address);
				if (errorInstructionIndex) { *errorInstructionIndex = i; }
				return 0;
			}
		}

		if (checkForReturnStatement(params, i))
		{
			if (!decompileReturnStatement(params, i, &isInUnreachableState, result))
			{
				sprintfJdc(statusMessage, 0, "Error decompiling return statement at 0x%llX.", currentInstruction->address);
				if (errorInstructionIndex) { *errorInstructionIndex = i; }
				return 0;
			}
		}
	}

	if (params->numOfIndents != 1)
	{
		strcpyJdc(statusMessage, "Bad indentation. There is an error with condition handling and the decompilation did not finish at one indentation.");
		if (errorInstructionIndex) { *errorInstructionIndex = params->currentFunc->lastInstructionIndex; }
		return 0;
	}

	addAssociatedInstruction(params->currentFunc, params->currentFunc->lastInstructionIndex);
	params->currentFunc->numOfLines++;
	return strcatJdc(result, "}");
}

static unsigned char isRegisterAccessedBeforeInit(struct DecompilationParameters* params, int startInstructionIndex, int lastInstructionIndex, enum Register reg, unsigned char ignoreInitialization, struct DataType* dataTypeRef, int callNum)
{
	// preventing recursive loop. this assumes it is accessed
	if(callNum > 9)
	{
		if (dataTypeRef) { *dataTypeRef = getRegisterDataType(NO_MNEMONIC, reg); }
		return 1;
	}

	// this happens if the last instruction of the function also initializes the return reg. the start instruction index is incremented before isRegisterAccessedBeforeInit is called, so the loop here wont run
	if (startInstructionIndex > params->currentFunc->lastInstructionIndex && compareRegisters(params->currentFunc->returnReg, reg))
	{
		if (dataTypeRef) { *dataTypeRef = params->currentFunc->returnType; }
		return 1;
	}
	
	for (int i = startInstructionIndex; i <= lastInstructionIndex; i++)
	{
		if (doesInstructionAccessRegister(params, i, reg, 0, dataTypeRef))
		{
			return 1;
		}

		if (!ignoreInitialization) 
		{
			unsigned char overwrites = 0;
			if (doesInstructionModifyRegister(params, i, reg, 0, &overwrites) && overwrites)
			{
				return 0;
			}
		}

		if (checkForReturnStatement(params, i))
		{
			if (compareRegisters(params->currentFunc->returnReg, reg))
			{
				if (dataTypeRef) { *dataTypeRef = params->currentFunc->returnType; }
				return 1;
			}
			return 0;
		}

		struct DisassembledInstruction* instruction = &(params->instructions[i]);
		if (isOpcodeJmp(instruction->opcode) || isOpcodeJcc(instruction->opcode))
		{
			int dstIndex = findInstructionByAddress(params->instructions, 0, params->numOfInstructions - 1, resolveJmpChain(params, i));
			if (dstIndex > i)
			{
				if (isOpcodeJcc(instruction->opcode))
				{
					if (isRegisterAccessedBeforeInit(params, i + 1, dstIndex - 1, reg, ignoreInitialization, dataTypeRef, callNum + 1))
					{
						return 1;
					}
				}

				i = dstIndex - 1;
			}
		}
	}

	return 0;
}

static unsigned char getAllReturnedVars(struct DecompilationParameters* params)
{
	for (int i = params->currentFunc->firstInstructionIndex; i <= params->currentFunc->lastInstructionIndex; i++)
	{
		struct Function* callee = 0;
		if ((checkForKnownFunctionCall(params, i, &callee) && callee && callee->returnType.primitiveType != VOID_TYPE) || checkForUnknownFunctionCall(params, i))
		{
			enum Register returnReg = callee ? callee->returnReg : AX;

			struct DataType returnType = { 0 }; // used if its an unknown function
			if (isRegisterAccessedBeforeInit(params, i + 1, params->currentFunc->lastInstructionIndex, returnReg, 0, &returnType, 0))
			{
				unsigned long long calleeAddress = resolveJmpChain(params, i);
				struct DisassembledInstruction* callInstruction = &(params->instructions[i]);
				
				if (callee)
				{
					if (!addReturnedVar(params->currentFunc, callee->returnType, calleeAddress, callInstruction->address, returnReg, callee->name.buffer))
					{
						return 0;
					}
				}
				else
				{
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
	}

	return 1;
}

static unsigned char getAllRegVars(struct DecompilationParameters* params)
{
	// checking for registers that are modified in a condition
	enum Register* modifiedRegs = (enum Register*)calloc(NUM_OF_REGISTERS, sizeof(enum Register));
	for (int i = 0; i < params->currentFunc->numOfConditions; i++)
	{
		memset(modifiedRegs, 0, NUM_OF_REGISTERS * sizeof(enum Register));
		int numOfRegs = 0;
		
		struct Condition* condition = &params->currentFunc->conditions[i];
		if (!condition->isCombinedByOther && condition->conditionType != CONDITIONAL_RETURN_CT)
		{
			for (int j = condition->startIndex; j < condition->endIndex; j++)
			{
				int conditionIndex = getConditionStart(params, j);
				if (conditionIndex != -1 && conditionIndex != i)
				{
					struct Condition* cond = &params->currentFunc->conditions[conditionIndex];
					if (isConditionRegular(cond))
					{
						if (cond->endIndex > j && cond->endIndex <= condition->endIndex)
						{
							j = cond->endIndex - 1;
							continue;
						}
					}
				}

				if (checkForReturnStatement(params, j))
				{
					break;
				}

				struct DisassembledInstruction* currentInstruction = &(params->instructions[j]);

				enum Register reg = NO_REG;

				struct Function* callee;
				if ((checkForKnownFunctionCall(params, j, &callee) && callee->returnType.primitiveType != VOID_TYPE))
				{
					reg = callee->returnReg;
				}
				else if (checkForUnknownFunctionCall(params, j))
				{
					reg = params->is64Bit ? RAX : EAX;
				}
				else if (currentInstruction->numOfOperands > 0 && currentInstruction->operands[0].type == REGISTER && doesInstructionModifyOperand(currentInstruction, 0, 0))
				{
					reg = currentInstruction->operands[0].reg;
				}

				if (reg != NO_REG && !isRegisterPointer(reg))
				{
					int alreadyFound = 0;
					for (int k = 0; k < numOfRegs; k++)
					{
						if (compareRegisters(reg, modifiedRegs[k]))
						{
							alreadyFound = 1;
							break;
						}
					}
					if (alreadyFound || getRegVarByReg(params->currentFunc, reg))
					{
						continue;
					}

					modifiedRegs[numOfRegs] = reg;
					numOfRegs++;
				}
			}

			// checking if the modified regs are accessed before being overwritten after the condition
			for (int j = 0; j < numOfRegs; j++)
			{
				struct DataType regVarType = { 0 };
				if ((condition->conditionType == LOOP_CT || condition->conditionType == DO_WHILE_CT))
				{
					// if condition is a loop, it needs to check from the start of it since the code can run more than once
					if (isRegisterAccessedBeforeInit(params, condition->startIndex, condition->endIndex - 1, modifiedRegs[j], 1, &regVarType, 0))
					{
						if (!addRegVar(params->currentFunc, regVarType, modifiedRegs[j]))
						{
							free(modifiedRegs);
							return 0;
						}
					}
				}
				
				if (isRegisterAccessedBeforeInit(params, condition->endIndex, params->currentFunc->lastInstructionIndex, modifiedRegs[j], 0, &regVarType, 0))
				{
					if (!addRegVar(params->currentFunc, regVarType, modifiedRegs[j]))
					{
						free(modifiedRegs);
						return 0;
					}
				}
			}
		}
	}

	free(modifiedRegs);

	// if a reg is set to a regVar, and then that regVar changes before the reg is accessed, the reg needs to also be a regVar
	for (int i = params->currentFunc->firstInstructionIndex; i <= params->currentFunc->lastInstructionIndex; i++)
	{
		struct DisassembledInstruction* currentInstruction = &(params->instructions[i]);

		if (currentInstruction->numOfOperands == 2 && 
			currentInstruction->operands[0].type == REGISTER && 
			!isRegisterPointer(currentInstruction->operands[0].reg) && doesInstructionModifyOperand(currentInstruction, 0, 0))
		{
			enum Register reg = currentInstruction->operands[0].reg;
			if (getRegVarByReg(params->currentFunc, reg)) 
			{
				continue;
			}

			struct RegisterVariable* regVar = 0;
			if (currentInstruction->operands[1].type == REGISTER)
			{
				regVar = getRegVarByReg(params->currentFunc, currentInstruction->operands[1].reg);
			}
			else if (currentInstruction->operands[1].type == MEM_ADDRESS)
			{
				regVar = getRegVarByReg(params->currentFunc, currentInstruction->operands[1].memoryAddress.reg);
				if (!regVar) 
				{
					regVar = getRegVarByReg(params->currentFunc, currentInstruction->operands[1].memoryAddress.regDisplacement);
				}
			}

			if (regVar) 
			{
				for (int j = i + 1; j <= params->currentFunc->lastInstructionIndex; j++)
				{
					unsigned char overwrites = 0;
					if (doesInstructionModifyRegister(params, j, reg, 0, &overwrites) && overwrites) 
					{
						break;
					}
					
					if (doesInstructionModifyRegister(params, j, regVar->reg, 0, 0) && currentInstruction->numOfOperands > 0)
					{
						if (!addRegVar(params->currentFunc, getOperandDataType(currentInstruction->opcode, &currentInstruction->operands[0]), reg))
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

unsigned char generateFunctionHeader(struct Function* function, struct JdcStr* result)
{
	struct JdcStr typeStr = initializeJdcStr();

	dataTypeToStr(function->returnType, &typeStr);
	sprintfJdc(result, 0, "%s %s %s(", typeStr.buffer, callingConventionStrs[function->callingConvention], function->name.buffer);

	for (int i = 0; i < function->numOfRegArgs; i++) 
	{
		dataTypeToStr(function->regArgs[i].dataType, &typeStr);
		
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
		dataTypeToStr(function->stackArgs[i].dataType, &typeStr);
		
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
	return strcatJdc(result, ")");
}

static unsigned char declareAllLocalVariables(struct DecompilationParameters* params, struct JdcStr* result)
{
	struct JdcStr typeStr = initializeJdcStr();
	
	for (int i = 0; i < params->currentFunc->numOfStackVars; i++)
	{
		dataTypeToStr(params->currentFunc->stackVars[i].dataType, &typeStr);
		addIndents(result, 1);
		sprintfJdc(result, 1, "%s %s;\n", typeStr.buffer, params->currentFunc->stackVars[i].name.buffer);
	}

	for (int i = 0; i < params->currentFunc->numOfRegVars; i++)
	{
		struct RegisterVariable* regVar = &params->currentFunc->regVars[i];
		dataTypeToStr(regVar->dataType, &typeStr);

		addIndents(result, 1);
		struct RegisterVariable* regArg = getRegArgByReg(params->currentFunc, regVar->reg);
		if (regArg)
		{
			if (!compareDataTypes(regVar->dataType, regArg->dataType)) 
			{
				sprintfJdc(result, 1, "%s %s = (%s)%s;\n", typeStr.buffer, regVar->name.buffer, typeStr.buffer, regArg->name.buffer);
			}
			else 
			{
				sprintfJdc(result, 1, "%s %s = %s;\n", typeStr.buffer, regVar->name.buffer, regArg->name.buffer);
			}
		}
		else 
		{
			sprintfJdc(result, 1, "%s %s;\n", typeStr.buffer, regVar->name.buffer);
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
			dataTypeToStr(params->currentFunc->returnedVars[i].dataType, &typeStr);
			addIndents(result, 1);
			sprintfJdc(result, 1, "%s %s;\n", typeStr.buffer, params->currentFunc->returnedVars[i].name.buffer);
		}
	}

	freeJdcStr(&typeStr);
	return strcatJdc(result, "\n");
}
