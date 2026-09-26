#include "decompiler.h"
#include "decompilationUtils.h"
#include "functions.h"
#include "operations.h"
#include "assignment.h"
#include "returnStatements.h"
#include "functionCalls.h"
#include "intrinsics.h"
#include "conditions.h"
#include "directJmps.h"
#include "dataTypes.h"
#include "localRegVars.h"

enum JdcStatus decompileFunction(struct DecompilationParameters* params, struct JdcStr* result, struct JdcStr* statusMessage, int* errorInstructionIndex)
{
	params->currentFunc->numOfLines = 0;
	
	if (!params->currentFunc->hasDoneInitialAnalysis)
	{
		if (ERROR_JDC == getAllDirectJmps(params))
		{
			strcpyJdc(statusMessage, "Error getting all direct jumps.");
			return ERROR_JDC;
		}
		
		if (ERROR_JDC == getAllLocalRegVars(params))
		{
			strcpyJdc(statusMessage, "Error getting all local reg vars.");
			return ERROR_JDC;
		}

		if (ERROR_JDC == getAllReturnedVars(params))
		{
			strcpyJdc(statusMessage, "Error getting all returned vars.");
			return ERROR_JDC;
		}

		params->currentFunc->associatedInstructionsBufferLen = params->currentFunc->lastInstructionIndex - params->currentFunc->firstInstructionIndex;
		params->currentFunc->associatedInstructions = (struct AssociatedInstructions*)calloc(params->currentFunc->associatedInstructionsBufferLen, sizeof(struct AssociatedInstructions));
		if (!params->currentFunc->associatedInstructions) 
		{
			return ERROR_JDC;
		}

		params->currentFunc->hasDoneInitialAnalysis = 1;
	}
	
	if (ERROR_JDC == generateFunctionHeader(params->currentFunc, result))
	{
		strcpyJdc(statusMessage, "Error generating function header.");
		if (errorInstructionIndex) { *errorInstructionIndex = params->currentFunc->firstInstructionIndex; }
		return ERROR_JDC;
	}

	addAssociatedInstruction(params->currentFunc, params->currentFunc->firstInstructionIndex);
	params->currentFunc->numOfLines++;

	strcatJdc(result, "\n{\n");
	params->numOfIndents = 1;

	addAssociatedInstruction(params->currentFunc, params->currentFunc->firstInstructionIndex);
	params->currentFunc->numOfLines++;

	if (ERROR_JDC == declareAllLocalVariables(params, result))
	{
		strcpyJdc(statusMessage, "Error declaring local variables.");
		if (errorInstructionIndex) { *errorInstructionIndex = params->currentFunc->firstInstructionIndex; }
		return ERROR_JDC;
	}

	unsigned char isInUnreachableState = 0;
	int numOfSkippedInstructions = 0;
	for (int i = params->currentFunc->firstInstructionIndex; i <= params->currentFunc->lastInstructionIndex; i++)
	{
		struct DisassembledInstruction* currentInstruction = &(params->instructions[i]);

		if (params->numOfIndents < 1)
		{
			sprintfJdc(statusMessage, 0, "Bad indentation. There is an error with condition handling at 0x%llX.", currentInstruction->address);
			if (errorInstructionIndex) { *errorInstructionIndex = i; }
			return ERROR_JDC;
		}
		
		if (ERROR_JDC == isOpcodeImplementedInDecompiler(currentInstruction->opcode)) // temporary check
		{
			sprintfJdc(statusMessage, 0, "%s at 0x%llX is not yet handled in the decompiler.", mnemonicStrs[currentInstruction->opcode], currentInstruction->address);
			if (errorInstructionIndex) { *errorInstructionIndex = i; }
			return ERROR_JDC;
		}

		if (isInUnreachableState)
		{
			struct Condition* cond = getConditionFromDstInstruction(params, i);
			if (checkForDirectJmpDst(params, i) || cond)
			{
				if(numOfSkippedInstructions > 0)
				{
					for (int j = numOfSkippedInstructions; j > 0; j--)
					{
						addAssociatedInstruction(params->currentFunc, i - j);
					}

					addDecompiledLine(params, result, -1, "// %i instruction(s) skipped", numOfSkippedInstructions);
				}

				isInUnreachableState = 0;
				numOfSkippedInstructions = 0;

				if (cond && cond->conditionType != DO_WHILE_CT)
				{
					i--; // decompileConditionEnds checks for the last body index, not the jmp dst
					if (ERROR_JDC == decompileConditionEnds(params, i, &isInUnreachableState, result))
					{
						sprintfJdc(statusMessage, 0, "Error decompiling end of condition at 0x%llX.", currentInstruction->address);
						if (errorInstructionIndex) { *errorInstructionIndex = i; }
						return ERROR_JDC;
					}

					continue;
				}
			}
			else 
			{
				numOfSkippedInstructions++;
				continue;
			}
		}

		if (ERROR_JDC == decompileConditionStarts(params, i, result))
		{
			sprintfJdc(statusMessage, 0, "Error decompiling start of condition at 0x%llX.", currentInstruction->address);
			if (errorInstructionIndex) { *errorInstructionIndex = i; }
			return ERROR_JDC;
		}

		if (ERROR_JDC == decompileDirectJmps(params, i, &isInUnreachableState, result))
		{
			sprintfJdc(statusMessage, 0, "Error decompiling direct jump at 0x%llX.", currentInstruction->address);
			if (errorInstructionIndex) { *errorInstructionIndex = i; }
			return ERROR_JDC;
		}

		struct Function* callee;
		struct Intrinsic* intrinsic;
		if (checkForKnownFunctionCall(params, i, &callee))
		{
			if (ERROR_JDC == decompileKnownFunctionCall(params, i, callee, result))
			{
				sprintfJdc(statusMessage, 0, "Error decompiling known function call at 0x%llX.", currentInstruction->address);
				if (errorInstructionIndex) { *errorInstructionIndex = i; }
				return ERROR_JDC;
			}
		}
		else if (checkForUnknownFunctionCall(params, i))
		{
			if (ERROR_JDC == decompileUnknownFunctionCall(params, i, result))
			{
				sprintfJdc(statusMessage, 0, "Error decompiling unknown function call at 0x%llX.", currentInstruction->address);
				if (errorInstructionIndex) { *errorInstructionIndex = i; }
				return ERROR_JDC;
			}
		}
		else if (checkForVoidIntrinsic(params, i, &intrinsic))
		{
			if (ERROR_JDC == decompileVoidIntrinsic(params, i, intrinsic, result))
			{
				sprintfJdc(statusMessage, 0, "Error decompiling intrinsic function at 0x%llX.", currentInstruction->address);
				if (errorInstructionIndex) { *errorInstructionIndex = i; }
				return ERROR_JDC;
			}
		}
		else if (checkForAnyAssignments(params, i))
		{
			if (ERROR_JDC == decompileOperation(params, i, NO_REG, 1, 0, result, 0))
			{
				sprintfJdc(statusMessage, 0, "Error decompiling assignment at 0x%llX.", currentInstruction->address);
				if (errorInstructionIndex) { *errorInstructionIndex = i; }
				return ERROR_JDC;
			}
		}

		if (checkForReturnStatement(params, i))
		{
			if (ERROR_JDC == decompileReturnStatement(params, i, &isInUnreachableState, result))
			{
				sprintfJdc(statusMessage, 0, "Error decompiling return statement at 0x%llX.", currentInstruction->address);
				if (errorInstructionIndex) { *errorInstructionIndex = i; }
				return ERROR_JDC;
			}
		}

		if (ERROR_JDC == decompileConditionEnds(params, i, &isInUnreachableState, result))
		{
			sprintfJdc(statusMessage, 0, "Error decompiling end of condition at 0x%llX.", currentInstruction->address);
			if (errorInstructionIndex) { *errorInstructionIndex = i; }
			return ERROR_JDC;
		}
	}

	if (params->numOfIndents != 1)
	{
		strcpyJdc(statusMessage, "Bad indentation. There is an error with condition handling and the decompilation did not finish at one indentation.");
		if (errorInstructionIndex) { *errorInstructionIndex = params->currentFunc->lastInstructionIndex; }
		return ERROR_JDC;
	}

	addAssociatedInstruction(params->currentFunc, params->currentFunc->lastInstructionIndex);
	params->currentFunc->numOfLines++;
	return strcatJdc(result, "}");
}

static enum JdcStatus getAllReturnedVars(struct DecompilationParameters* params)
{
	for (int i = params->currentFunc->firstInstructionIndex; i <= params->currentFunc->lastInstructionIndex; i++)
	{
		struct Function* callee = 0;
		if ((checkForKnownFunctionCall(params, i, &callee) && callee && callee->returnType.primitiveType != VOID_TYPE) || checkForUnknownFunctionCall(params, i))
		{
			enum Register returnReg = callee ? callee->returnReg : AX;
			if (isRegisterAccessedBeforeInit(params, i + 1, params->currentFunc->lastInstructionIndex, returnReg, 0, 0) || 
				(checkForReturnStatement(params, i) && params->currentFunc->returnReg == returnReg)) // this is if it is an unknown call that also returns
			{
				unsigned long long calleeAddress = resolveJmpChain(params, i);
				struct DisassembledInstruction* callInstruction = &(params->instructions[i]);
				
				if (callee)
				{
					if (ERROR_JDC == addReturnedVar(params->currentFunc, callee->returnType, calleeAddress, callInstruction->address, returnReg, callee->name.buffer))
					{
						return ERROR_JDC;
					}
				}
				else
				{
					struct DataType guessedReturnType = { 0 };
					guessedReturnType.primitiveType = params->is64Bit ? LONG_LONG_TYPE : INT_TYPE;

					int importIndex = getImportIndexByAddress(params, calleeAddress);
					if (importIndex != -1)
					{
						if (ERROR_JDC == addReturnedVar(params->currentFunc, guessedReturnType, calleeAddress, callInstruction->address, returnReg, params->imports[importIndex].name.buffer))
						{
							return ERROR_JDC;
						}
					}
					else
					{
						if (ERROR_JDC == addReturnedVar(params->currentFunc, guessedReturnType, calleeAddress, callInstruction->address, returnReg, "funcPtr"))
						{
							return ERROR_JDC;
						}
					}
				}
			}
		}
	}

	return SUCCESS_JDC;
}

enum JdcStatus generateFunctionHeader(struct Function* function, struct JdcStr* result)
{
	struct JdcStr typeStr = initializeJdcStr();

	dataTypeToStr(function->returnType, &typeStr);
	sprintfJdc(result, 0, "%s %s %s(", typeStr.buffer, callingConventionStrs[function->callingConvention], function->name.buffer);

	for (int i = 0; i < function->numOfRegVars; i++) 
	{
		if (function->regVars[i].isArgument) 
		{
			dataTypeToStr(function->regVars[i].dataType, &typeStr);
			sprintfJdc(result, 1, "%s %s, ", typeStr.buffer, function->regVars[i].name.buffer);
		}
	}

	for (int i = 0; i < function->numOfStackVars; i++)
	{
		if (function->stackVars[i].isArgument) 
		{
			dataTypeToStr(function->stackVars[i].dataType, &typeStr);
			sprintfJdc(result, 1, "%s %s, ", typeStr.buffer, function->stackVars[i].name.buffer);
		}
	}

	int len = (int)strlen(result->buffer);
	if (result->buffer[len - 1] != '(')
	{
		result->buffer[len - 2] = ')';
		result->buffer[len - 1] = 0;
	}
	else
	{
		strcatJdc(result, ")");
	}

	freeJdcStr(&typeStr);
	return SUCCESS_JDC;
}

static enum JdcStatus declareAllLocalVariables(struct DecompilationParameters* params, struct JdcStr* result)
{
	struct JdcStr typeStr = initializeJdcStr();
	unsigned char declaredAVar = 0;
	
	for (int i = 0; i < params->currentFunc->numOfStackVars; i++)
	{
		struct StackVariable* stackVar = &params->currentFunc->stackVars[i];
		if (!stackVar->isArgument) 
		{
			dataTypeToStr(stackVar->dataType, &typeStr);

			struct JdcStr varNameTmp = copyJdcStr(&stackVar->name);
			if (stackVar->dataType.arrayLen > 1)
			{
				sprintfJdc(&varNameTmp, 1, "[%u]", stackVar->dataType.arrayLen);
			}
			strcatJdc(&varNameTmp, ";");

			addDecompiledLine(params, result, params->currentFunc->firstInstructionIndex, "%s %s", typeStr.buffer, varNameTmp.buffer);
			freeJdcStr(&varNameTmp);
			declaredAVar = 1;
		}
	}

	for (int i = 0; i < params->currentFunc->numOfRegVars; i++)
	{
		struct RegisterVariable* localRegVar = &params->currentFunc->regVars[i];
		if (!localRegVar->isArgument)
		{
			dataTypeToStr(localRegVar->dataType, &typeStr);

			struct RegisterVariable* regArg = getRegArgByReg(params->currentFunc, localRegVar->reg);
			if (regArg && checkRegVarScope(params, localRegVar, params->currentFunc->firstInstructionIndex))
			{
				if (doDataTypesRequireCasting(localRegVar->dataType, regArg->dataType, params->is64Bit))
				{
					addDecompiledLine(params, result, params->currentFunc->firstInstructionIndex, "%s %s = (%s)%s;", typeStr.buffer, localRegVar->name.buffer, typeStr.buffer, regArg->name.buffer);
				}
				else
				{
					addDecompiledLine(params, result, params->currentFunc->firstInstructionIndex, "%s %s = %s;", typeStr.buffer, localRegVar->name.buffer, regArg->name.buffer);
				}
			}
			else
			{
				addDecompiledLine(params, result, params->currentFunc->firstInstructionIndex, "%s %s;", typeStr.buffer, localRegVar->name.buffer);
			}

			declaredAVar = 1;
		}
	}

	for (int i = 0; i < params->currentFunc->numOfReturnedVars; i++)
	{
		unsigned char isReturnRegVar = 0;
		for (int j = 0; j < params->currentFunc->numOfRegVars; j++) 
		{
			if (!params->currentFunc->regVars[j].isArgument && compareRegisters(params->currentFunc->regVars[j].reg, params->currentFunc->returnedVars[i].returnReg))
			{
				isReturnRegVar = 1;
				break;
			}
		}

		if (!isReturnRegVar) 
		{
			dataTypeToStr(params->currentFunc->returnedVars[i].dataType, &typeStr);
			addDecompiledLine(params, result, params->currentFunc->firstInstructionIndex, "%s %s;", typeStr.buffer, params->currentFunc->returnedVars[i].name.buffer);
			declaredAVar = 1;
		}
	}

	if (declaredAVar) 
	{
		addDecompiledLine(params, result, -1, "");
	}

	freeJdcStr(&typeStr);
	return SUCCESS_JDC;
}