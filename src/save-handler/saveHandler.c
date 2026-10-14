#include "saveHandler.h"
#include "../file-handler/fileHandler.h"

enum JdcStatus saveToFile(const wchar_t* filePath, struct Function* functions, int32_t numOfFunctions)
{
    wchar_t tempPath[1024];
    if (swprintf(tempPath, sizeof(tempPath), L"%ls.jdctmp", filePath) <= 0)
    {
        return ERROR_JDC;
    }

    FILE* file = openFile(tempPath, L"wb");
    if (file == NULL) 
    {
        return ERROR_JDC;
    }

    size_t filePathLen = wcslen(filePath);
    if (fwrite(filePath, sizeof(wchar_t), filePathLen, file) != filePathLen)
    {
        fclose(file);
        removeFile(tempPath);
        return ERROR_JDC;
    }

    uint32_t version = CURRENT_SAVE_FILE_VERSION;
    if (fwrite(&version, sizeof(version), 1, file) != 1)
    {
        fclose(file);
        removeFile(tempPath);
        return ERROR_JDC;
    }

    if (fwrite(&numOfFunctions, sizeof(numOfFunctions), 1, file) != 1)
    {
        fclose(file);
        removeFile(tempPath);
        return ERROR_JDC;
    }

    for (int32_t i = 0; i < numOfFunctions; i++)
    {
        if (writeFunction(file, &functions[i]) != SUCCESS_JDC)
        {
            fclose(file);
            removeFile(tempPath);
            return ERROR_JDC;
        }
    }

    if (fclose(file) != 0)
    {
        return ERROR_JDC;
    }

    wchar_t savePath[1024];
    if (swprintf(savePath, sizeof(savePath), L"%ls.jdc", filePath) <= 0)
    {
        return ERROR_JDC;
    }

    removeFile(savePath);
    if (renameFile(tempPath, savePath) == ERROR_JDC)
    {
        return ERROR_JDC;
    }

    return SUCCESS_JDC;
}

static enum JdcStatus writeFunction(FILE* file, const struct Function* func)
{
    if (fwrite(&func->returnType, sizeof(func->returnType), 1, file) != 1) 
    {
        return ERROR_JDC;
    }

    if (fwrite(&func->returnReg, sizeof(func->returnReg), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(&func->callingConvention, sizeof(func->callingConvention), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(&func->name.bufferSize, sizeof(func->name.bufferSize), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(func->name.buffer, 1, func->name.bufferSize, file) != func->name.bufferSize)
    {
        return ERROR_JDC;
    }

    if (fwrite(&func->numOfStackVars, sizeof(func->numOfStackVars), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    for (int32_t i = 0; i < func->numOfStackVars; i++) 
    {
        if (writeStackVar(file, &func->stackVars[i]) == ERROR_JDC) 
        {
            return ERROR_JDC;
        }
    }

    if (fwrite(&func->numOfReturnedVars, sizeof(func->numOfReturnedVars), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    for (int32_t i = 0; i < func->numOfReturnedVars; i++)
    {
        if (writeReturnedVar(file, &func->returnedVars[i]) == ERROR_JDC)
        {
            return ERROR_JDC;
        }
    }

    if (fwrite(&func->numOfRegVars, sizeof(func->numOfRegVars), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    for (int32_t i = 0; i < func->numOfRegVars; i++)
    {
        if (writeRegVar(file, &func->regVars[i]) == ERROR_JDC)
        {
            return ERROR_JDC;
        }
    }

    if (fwrite(&func->hasDoneInitialAnalysis, sizeof(func->hasDoneInitialAnalysis), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(&func->firstInstructionIndex, sizeof(func->firstInstructionIndex), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(&func->lastInstructionIndex, sizeof(func->lastInstructionIndex), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(&func->numOfConditions, sizeof(func->numOfConditions), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    for (int32_t i = 0; i < func->numOfConditions; i++)
    {
        if (writeCondition(file, &func->conditions[i]) == ERROR_JDC)
        {
            return ERROR_JDC;
        }
    }

    if (fwrite(&func->numOfDirectJmps, sizeof(func->numOfDirectJmps), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    for (int32_t i = 0; i < func->numOfDirectJmps; i++)
    {
        if (writeDirectJmp(file, &func->directJmps[i]) == ERROR_JDC)
        {
            return ERROR_JDC;
        }
    }

    if (fwrite(&func->associatedInstructionsBufferLen, sizeof(func->associatedInstructionsBufferLen), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    for (int32_t i = 0; i < func->associatedInstructionsBufferLen; i++)
    {
        if (writeAssociatedInstructions(file, &func->associatedInstructions[i]) == ERROR_JDC)
        {
            return ERROR_JDC;
        }
    }

    if (fwrite(&func->numOfLines, sizeof(func->numOfLines), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    return SUCCESS_JDC;
}

static enum JdcStatus writeStackVar(FILE* file, const struct StackVariable* stackVar)
{
    if (fwrite(&stackVar->dataType, sizeof(stackVar->dataType), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(&stackVar->isArgument, sizeof(stackVar->isArgument), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(&stackVar->offsetFromInitSP, sizeof(stackVar->offsetFromInitSP), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(&stackVar->name.bufferSize, sizeof(stackVar->name.bufferSize), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(stackVar->name.buffer, 1, stackVar->name.bufferSize, file) != stackVar->name.bufferSize)
    {
        return ERROR_JDC;
    }
    
    return SUCCESS_JDC;
}

static enum JdcStatus writeReturnedVar(FILE* file, const struct ReturnedVariable* returnedVar)
{
    if (fwrite(&returnedVar->dataType, sizeof(returnedVar->dataType), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(&returnedVar->calleeAddress, sizeof(returnedVar->calleeAddress), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(&returnedVar->callInstructionAddress, sizeof(returnedVar->callInstructionAddress), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(&returnedVar->returnReg, sizeof(returnedVar->returnReg), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(&returnedVar->name.bufferSize, sizeof(returnedVar->name.bufferSize), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(returnedVar->name.buffer, 1, returnedVar->name.bufferSize, file) != returnedVar->name.bufferSize)
    {
        return ERROR_JDC;
    }

    return SUCCESS_JDC;
}

static enum JdcStatus writeRegVar(FILE* file, const struct RegisterVariable* regVar)
{
    if (fwrite(&regVar->dataType, sizeof(regVar->dataType), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(&regVar->reg, sizeof(regVar->reg), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(&regVar->isArgument, sizeof(regVar->isArgument), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(&regVar->numOfScopes, sizeof(regVar->numOfScopes), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    for (int32_t i = 0; i < regVar->numOfScopes; i++) 
    {
        if (fwrite(&regVar->scopes[i].startIndex, sizeof(regVar->scopes[i].startIndex), 1, file) != 1)
        {
            return ERROR_JDC;
        }

        if (fwrite(&regVar->scopes[i].endIndex, sizeof(regVar->scopes[i].endIndex), 1, file) != 1)
        {
            return ERROR_JDC;
        }
    }

    if (fwrite(&regVar->name.bufferSize, sizeof(regVar->name.bufferSize), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(regVar->name.buffer, 1, regVar->name.bufferSize, file) != regVar->name.bufferSize)
    {
        return ERROR_JDC;
    }

    return SUCCESS_JDC;
}

static enum JdcStatus writeCondition(FILE* file, const struct Condition* cond)
{
    if (fwrite(&cond->jccIndex, sizeof(cond->jccIndex), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(&cond->dstIndex, sizeof(cond->dstIndex), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(&cond->exitIndex, sizeof(cond->exitIndex), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(&cond->firstBodyIndex, sizeof(cond->firstBodyIndex), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(&cond->lastBodyIndex, sizeof(cond->lastBodyIndex), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(&cond->conditionType, sizeof(cond->conditionType), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(&cond->numOfCombinedJccs, sizeof(cond->numOfCombinedJccs), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    for (int32_t i = 0; i < cond->numOfCombinedJccs; i++) 
    {
        if (fwrite(&cond->combinedJccIndexes[i], sizeof(cond->combinedJccIndexes[i]), 1, file) != 1)
        {
            return ERROR_JDC;
        }
    }

    if (fwrite(&cond->combinedJccsLogicType, sizeof(cond->combinedJccsLogicType), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(&cond->connectedUpperConditionIndex, sizeof(cond->connectedUpperConditionIndex), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(&cond->connectedLowerConditionIndex, sizeof(cond->connectedLowerConditionIndex), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(&cond->indentLevel, sizeof(cond->indentLevel), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    return SUCCESS_JDC;
}

static enum JdcStatus writeDirectJmp(FILE* file, const struct DirectJmp* directJmp)
{
    if (fwrite(&directJmp->jmpIndex, sizeof(directJmp->jmpIndex), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(&directJmp->dstIndex, sizeof(directJmp->dstIndex), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    if (fwrite(&directJmp->type, sizeof(directJmp->type), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    return SUCCESS_JDC;
}

static enum JdcStatus writeAssociatedInstructions(FILE* file, const struct AssociatedInstructions* ai)
{
    if (fwrite(&ai->numOfIndexes, sizeof(ai->numOfIndexes), 1, file) != 1)
    {
        return ERROR_JDC;
    }

    for (int32_t i = 0; i < ai->numOfIndexes; i++) 
    {
        if (fwrite(&ai->indexes[i], sizeof(ai->indexes[i]), 1, file) != 1)
        {
            return ERROR_JDC;
        }
    }

    return SUCCESS_JDC;
}