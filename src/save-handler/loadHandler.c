#include "loadHandler.h"
#include "saveHandler.h"
#include "../file-handler/fileHandler.h"

enum JdcStatus isFileJdcSave(const wchar_t* filePath, bool* isJdcSave)
{
	if (!isJdcSave)
	{
		return ERROR_JDC;
	}

	const wchar_t* dot = wcsrchr(filePath, L'.');
	if (dot == NULL || wcscmp(dot, L".jdc") != 0)
	{
		*isJdcSave = false;
		return SUCCESS_JDC;
	}

	FILE* file = openFile(filePath, L"rb");
	if (file == NULL)
	{
		return ERROR_JDC;
	}

	uint32_t jdcSig = 0;
	seek64(file, JDC_SIG_FILE_OFFSET);
	fread(&jdcSig, sizeof(jdcSig), 1, file);
	fclose(file);

	*isJdcSave = jdcSig == JDC_SIG;
	return SUCCESS_JDC;
}

enum JdcStatus getJdcSaveVersion(const wchar_t* savePath, uint32_t* saveVersion)
{
	if (!saveVersion)
	{
		return ERROR_JDC;
	}

	FILE* file = openFile(savePath, L"rb");
	if (file == NULL)
	{
		return ERROR_JDC;
	}

	seek64(file, SAVE_VERSION_FILE_OFFSET);
	fread(saveVersion, sizeof(uint32_t), 1, file);
	fclose(file);

	return SUCCESS_JDC;
}

enum JdcStatus getJdcSaveFilePathLen(const wchar_t* savePath, size_t* filePathLen)
{
	if (!filePathLen)
	{
		return ERROR_JDC;
	}

	FILE* file = openFile(savePath, L"rb");
	if (file == NULL)
	{
		return ERROR_JDC;
	}

	seek64(file, FILE_PATH_LEN_FILE_OFFSET);
	fread(filePathLen, sizeof(size_t), 1, file);
	fclose(file);

	return SUCCESS_JDC;
}

enum JdcStatus getJdcSaveFilePath(const wchar_t* savePath, wchar_t* filePathBuffer, size_t filePathBufferLen)
{
	if (!savePath || !filePathBuffer)
	{
		return ERROR_JDC;
	}

	FILE* file = openFile(savePath, L"rb");
	if (file == NULL)
	{
		return ERROR_JDC;
	}

	size_t pathLen = 0;
	seek64(file, FILE_PATH_FILE_OFFSET);
	fread(filePathBuffer, sizeof(wchar_t), filePathBufferLen, file);
	fclose(file);

	return SUCCESS_JDC;
}

enum JdcStatus getNumOfFuncsFromJdcSaveFile(const wchar_t* savePath, int32_t* numOfFunctions)
{
	if (!savePath || !numOfFunctions)
	{
		return ERROR_JDC;
	}

	FILE* file = openFile(savePath, L"rb");
	if (file == NULL)
	{
		return ERROR_JDC;
	}

	size_t pathLen = 0;
	seek64(file, FILE_PATH_LEN_FILE_OFFSET);
	fread(&pathLen, sizeof(pathLen), 1, file);

	seek64(file, FILE_PATH_FILE_OFFSET + (pathLen * sizeof(wchar_t)));
	fread(numOfFunctions, sizeof(int32_t), 1, file);
	fclose(file);

	return SUCCESS_JDC;
}

enum JdcStatus getAllFunctionsFromJdcSaveFile(const wchar_t* savePath, struct Function* funcBuffer)
{
	if (!savePath || !funcBuffer)
	{
		return ERROR_JDC;
	}

	FILE* file = openFile(savePath, L"rb");
	if (file == NULL)
	{
		return ERROR_JDC;
	}

	size_t pathLen = 0;
	seek64(file, FILE_PATH_LEN_FILE_OFFSET);
	fread(&pathLen, sizeof(pathLen), 1, file);

	int32_t numOfFunctions = 0;
	seek64(file, FILE_PATH_FILE_OFFSET + (pathLen * sizeof(wchar_t)));
	fread(&numOfFunctions, sizeof(numOfFunctions), 1, file);

	uint64_t currentFileOffset = FILE_PATH_FILE_OFFSET + (pathLen * sizeof(wchar_t)) + sizeof(numOfFunctions);
	for (int32_t i = 0; i < numOfFunctions; i++)
	{
		struct Function* func = &funcBuffer[i];

		readAndUpdateOffset(file, &func->returnType, sizeof(func->returnType), &currentFileOffset);
		readAndUpdateOffset(file, &func->returnReg, sizeof(func->returnReg), &currentFileOffset);
		readAndUpdateOffset(file, &func->callingConvention, sizeof(func->callingConvention), &currentFileOffset);

		size_t funcNameBufferSize = 0;
		readAndUpdateOffset(file, &funcNameBufferSize, sizeof(funcNameBufferSize), &currentFileOffset);

		func->name = initializeJdcStrWithSize(funcNameBufferSize);
		readAndUpdateOffset(file, func->name.buffer, funcNameBufferSize, &currentFileOffset);

		readAndUpdateOffset(file, &func->numOfStackVars, sizeof(func->numOfStackVars), &currentFileOffset);
		func->stackVars = (struct StackVariable*)calloc(func->numOfStackVars, sizeof(struct StackVariable));
		for (int32_t j = 0; j < func->numOfStackVars; j++)
		{
			struct StackVariable* stackVar = &func->stackVars[j];

			readAndUpdateOffset(file, &stackVar->dataType, sizeof(stackVar->dataType), &currentFileOffset);
			readAndUpdateOffset(file, &stackVar->isArgument, sizeof(stackVar->isArgument), &currentFileOffset);
			readAndUpdateOffset(file, &stackVar->offsetFromInitSP, sizeof(stackVar->offsetFromInitSP), &currentFileOffset);

			size_t varNameBufferSize = 0;
			readAndUpdateOffset(file, &varNameBufferSize, sizeof(varNameBufferSize), &currentFileOffset);

			stackVar->name = initializeJdcStrWithSize(varNameBufferSize);
			readAndUpdateOffset(file, stackVar->name.buffer, varNameBufferSize, &currentFileOffset);
		}

		readAndUpdateOffset(file, &func->numOfReturnedVars, sizeof(func->numOfReturnedVars), &currentFileOffset);
		func->returnedVars = (struct ReturnedVariable*)calloc(func->numOfReturnedVars, sizeof(struct ReturnedVariable));
		for (int32_t j = 0; j < func->numOfReturnedVars; j++)
		{
			struct ReturnedVariable* returnedVar = &func->returnedVars[j];

			readAndUpdateOffset(file, &returnedVar->dataType, sizeof(returnedVar->dataType), &currentFileOffset);
			readAndUpdateOffset(file, &returnedVar->calleeAddress, sizeof(returnedVar->calleeAddress), &currentFileOffset);
			readAndUpdateOffset(file, &returnedVar->callInstructionAddress, sizeof(returnedVar->callInstructionAddress), &currentFileOffset);
			readAndUpdateOffset(file, &returnedVar->returnReg, sizeof(returnedVar->returnReg), &currentFileOffset);

			size_t varNameBufferSize = 0;
			readAndUpdateOffset(file, &varNameBufferSize, sizeof(varNameBufferSize), &currentFileOffset);

			returnedVar->name = initializeJdcStrWithSize(varNameBufferSize);
			readAndUpdateOffset(file, returnedVar->name.buffer, varNameBufferSize, &currentFileOffset);
		}

		readAndUpdateOffset(file, &func->numOfRegVars, sizeof(func->numOfRegVars), &currentFileOffset);
		func->regVars = (struct RegisterVariable*)calloc(func->numOfRegVars, sizeof(struct RegisterVariable));
		for (int32_t j = 0; j < func->numOfRegVars; j++)
		{
			struct RegisterVariable* regVar = &func->regVars[j];

			readAndUpdateOffset(file, &regVar->dataType, sizeof(regVar->dataType), &currentFileOffset);
			readAndUpdateOffset(file, &regVar->reg, sizeof(regVar->reg), &currentFileOffset);
			readAndUpdateOffset(file, &regVar->isArgument, sizeof(regVar->isArgument), &currentFileOffset);

			readAndUpdateOffset(file, &regVar->numOfScopes, sizeof(regVar->numOfScopes), &currentFileOffset);
			regVar->scopes = (struct RegVarScope*)calloc(regVar->numOfScopes, sizeof(struct RegVarScope));
			for (int32_t k = 0; k < regVar->numOfScopes; k++)
			{
				readAndUpdateOffset(file, &regVar->scopes[k].startIndex, sizeof(regVar->scopes[k].startIndex), &currentFileOffset);
				readAndUpdateOffset(file, &regVar->scopes[k].endIndex, sizeof(regVar->scopes[k].endIndex), &currentFileOffset);
			}

			size_t varNameBufferSize = 0;
			readAndUpdateOffset(file, &varNameBufferSize, sizeof(varNameBufferSize), &currentFileOffset);

			regVar->name = initializeJdcStrWithSize(varNameBufferSize);
			readAndUpdateOffset(file, regVar->name.buffer, varNameBufferSize, &currentFileOffset);
		}

		readAndUpdateOffset(file, &func->hasDoneInitialAnalysis, sizeof(func->hasDoneInitialAnalysis), &currentFileOffset);
		readAndUpdateOffset(file, &func->firstInstructionIndex, sizeof(func->firstInstructionIndex), &currentFileOffset);
		readAndUpdateOffset(file, &func->lastInstructionIndex, sizeof(func->lastInstructionIndex), &currentFileOffset);

		readAndUpdateOffset(file, &func->numOfConditions, sizeof(func->numOfConditions), &currentFileOffset);
		func->conditions = (struct Condition*)calloc(func->numOfConditions, sizeof(struct Condition));
		for (int32_t j = 0; j < func->numOfConditions; j++)
		{
			struct Condition* cond = &func->conditions[j];

			readAndUpdateOffset(file, &cond->jccIndex, sizeof(cond->jccIndex), &currentFileOffset);
			readAndUpdateOffset(file, &cond->dstIndex, sizeof(cond->dstIndex), &currentFileOffset);
			readAndUpdateOffset(file, &cond->exitIndex, sizeof(cond->exitIndex), &currentFileOffset);
			readAndUpdateOffset(file, &cond->firstBodyIndex, sizeof(cond->firstBodyIndex), &currentFileOffset);
			readAndUpdateOffset(file, &cond->lastBodyIndex, sizeof(cond->lastBodyIndex), &currentFileOffset);
			readAndUpdateOffset(file, &cond->conditionType, sizeof(cond->conditionType), &currentFileOffset);

			readAndUpdateOffset(file, &cond->numOfCombinedJccs, sizeof(cond->numOfCombinedJccs), &currentFileOffset);
			cond->combinedJccIndexes = (int32_t*)calloc(cond->numOfCombinedJccs, sizeof(int32_t));
			for (int32_t k = 0; k < cond->numOfCombinedJccs; k++)
			{
				readAndUpdateOffset(file, &cond->combinedJccIndexes[k], sizeof(cond->combinedJccIndexes[k]), &currentFileOffset);
			}

			readAndUpdateOffset(file, &cond->combinedJccsLogicType, sizeof(cond->combinedJccsLogicType), &currentFileOffset);
			readAndUpdateOffset(file, &cond->connectedUpperConditionIndex, sizeof(cond->connectedUpperConditionIndex), &currentFileOffset);
			readAndUpdateOffset(file, &cond->connectedLowerConditionIndex, sizeof(cond->connectedLowerConditionIndex), &currentFileOffset);
			readAndUpdateOffset(file, &cond->indentLevel, sizeof(cond->indentLevel), &currentFileOffset);
		}

		readAndUpdateOffset(file, &func->numOfDirectJmps, sizeof(func->numOfDirectJmps), &currentFileOffset);
		func->directJmps = (struct DirectJmp*)calloc(func->numOfDirectJmps, sizeof(struct DirectJmp));
		for (int32_t j = 0; j < func->numOfDirectJmps; j++)
		{
			struct DirectJmp* directJmp = &func->directJmps[j];

			readAndUpdateOffset(file, &directJmp->jmpIndex, sizeof(directJmp->jmpIndex), &currentFileOffset);
			readAndUpdateOffset(file, &directJmp->dstIndex, sizeof(directJmp->dstIndex), &currentFileOffset);
			readAndUpdateOffset(file, &directJmp->type, sizeof(directJmp->type), &currentFileOffset);
		}

		readAndUpdateOffset(file, &func->associatedInstructionsBufferLen, sizeof(func->associatedInstructionsBufferLen), &currentFileOffset);
		func->associatedInstructions = (struct AssociatedInstructions*)calloc(func->associatedInstructionsBufferLen, sizeof(struct AssociatedInstructions));
		for (int32_t j = 0; j < func->associatedInstructionsBufferLen; j++)
		{
			struct AssociatedInstructions* associatedInstructions = &func->associatedInstructions[j];

			readAndUpdateOffset(file, &associatedInstructions->numOfIndexes, sizeof(associatedInstructions->numOfIndexes), &currentFileOffset);
			associatedInstructions->indexes = (int32_t*)calloc(associatedInstructions->numOfIndexes, sizeof(int32_t));
			for (int32_t k = 0; k < associatedInstructions->numOfIndexes; k++)
			{
				readAndUpdateOffset(file, &associatedInstructions->indexes[k], sizeof(associatedInstructions->indexes[k]), &currentFileOffset);
			}
		}

		readAndUpdateOffset(file, &func->numOfLines, sizeof(func->numOfLines), &currentFileOffset);
	}

	fclose(file);
	return SUCCESS_JDC;
}

static enum JdcStatus readAndUpdateOffset(FILE* file, void* buffer, size_t size, uint64_t* currentFileOffset)
{
	if (seek64(file, *currentFileOffset) != 0) { return ERROR_JDC; }
	if (fread(buffer, size, 1, file) != 1) { return ERROR_JDC; }
	*currentFileOffset += size;
	return SUCCESS_JDC;
}