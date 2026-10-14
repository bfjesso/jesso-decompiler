#pragma once
#include "../decompiler/decompilationStructs.h"


#ifdef __cplusplus
extern "C"
{
#endif

	enum JdcStatus isFileJdcSave(const wchar_t* filePath, bool* isJdcSave);

	enum JdcStatus getJdcSaveVersion(const wchar_t* savePath, uint32_t* saveVersion);

	enum JdcStatus getJdcSaveFilePathLen(const wchar_t* savePath, size_t* filePathLen);

	enum JdcStatus getJdcSaveFilePath(const wchar_t* savePath, wchar_t* filePathBuffer, size_t filePathBufferLen); // this gets the path of the file analyzed, not the save file path

	enum JdcStatus getNumOfFuncsFromJdcSaveFile(const wchar_t* savePath, int32_t* numOfFunctions);

	enum JdcStatus getAllFunctionsFromJdcSaveFile(const wchar_t* savePath, struct Function* funcBuffer);

#ifdef __cplusplus
}
#endif

static enum JdcStatus readAndUpdateOffset(FILE* file, void* buffer, size_t size, uint64_t* currentFileOffset);