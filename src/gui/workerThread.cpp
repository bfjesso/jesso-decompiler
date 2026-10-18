#include "workerThread.h"
#include "mainGui.h"
#include "../disassembler/disassembler.h"
#include "../decompiler/decompilationUtils.h"
#include "../decompiler/functions.h"

WorkerThread::WorkerThread(MainGui* gui, enum WorkerThreadType threadType, bool getFuncNameSymbols) : wxThread(wxTHREAD_JOINABLE)
{
	mainGui = gui;
	workerThreadType = threadType;
	getSymbols = getFuncNameSymbols;
}

wxThread::ExitCode WorkerThread::Entry()
{
	if (workerThreadType == DISASSEMBLE_WORKER_THREAD)
	{
		Disassemble();

		wxThreadEvent* done = new wxThreadEvent(EVT_DISASSEMBLY_DONE);
		wxQueueEvent(mainGui, done);
	}
	else if (workerThreadType == ANALYZE_WORKER_THREAD)
	{
		Analyze();

		wxThreadEvent* done = new wxThreadEvent(EVT_ANALYSIS_DONE);
		wxQueueEvent(mainGui, done);
	}

	return (wxThread::ExitCode)0;
}

void WorkerThread::Stop()
{
	stopRequested.store(true);
	if (IsRunning())
	{
		Wait();
	}
}

void WorkerThread::PostLog(const wxString& text, enum LogParams logParam)
{
	wxThreadEvent* e = new wxThreadEvent(EVT_WORKER_LOG);
	e->SetString(text);
	e->SetInt(logParam);
	wxQueueEvent(mainGui, e);
}

void WorkerThread::PostLogProgress(uint64_t value, bool updateCurrentProgress)
{
	wxThreadEvent* e = new wxThreadEvent(EVT_WORKER_LOG_PROGRESS);
	e->SetExtraLong(value);
	e->SetInt(updateCurrentProgress ? 1 : 0);
	wxQueueEvent(mainGui, e);
}

bool CompareInstructions(const DisassembledInstruction& a, const DisassembledInstruction& b)
{
	return a.address < b.address;
}

bool CompareJumpTables(const JumpTable& a, const JumpTable& b)
{
	return a.jmpTableAddress < b.jmpTableAddress;
}

void WorkerThread::Disassemble()
{
	// first the instructions that are definitely executed are disassembled, then the other code sections bytes or bytes inbetween instructions are disassembled
	PostLog("disassembling jumped to instructions...", LOG_PARAM_ADD_NEW_LINE);
	PostLog("\tCurrent instruction: ", LOG_PARAM_NONE);
	PostLogProgress(mainGui->entryPoint + mainGui->imageBase, false);

	struct DisassemblerOptions options = { 0 };
	options.is64BitMode = mainGui->is64Bit;
	struct DisassembledInstruction instructionBuffer;
	bool didErrorOccur = false;
	if (ERROR_JDC == DisassembleTakingJumps(mainGui->entryPoint + mainGui->imageBase, &instructionBuffer, &options, 0))
	{
		didErrorOccur = true;
	}

	if (mainGui->disassembledInstructions.size() == 0)
	{
		PostLog("An error occured while disassembling", LOG_PARAM_ERROR);
		return;
	}

	PostLog("disassembling all other instructions...", LOG_PARAM_ADD_NEW_LINE);
	PostLog("\tCurrent instruction: ", LOG_PARAM_NONE);
	PostLogProgress(mainGui->disassembledInstructions[0].address, false);

	uint64_t firstAddress = mainGui->disassembledInstructions[0].address;
	uint64_t lastAddress = mainGui->disassembledInstructions[mainGui->disassembledInstructions.size() - 1].address + mainGui->disassembledInstructions[mainGui->disassembledInstructions.size() - 1].numOfBytes;
	for (int32_t i = 0; i < mainGui->numOfSections; i++)
	{
		if (mainGui->sections[i].type == CODE_FST)
		{
			uint64_t sectionStart = mainGui->sections[i].rva + mainGui->imageBase;
			uint64_t sectionEnd = mainGui->sections[i].rva + mainGui->sections[i].physicalSize + mainGui->imageBase;
			if (sectionEnd < firstAddress || sectionStart > lastAddress)
			{
				if (ERROR_JDC == DisassembleBetweenBounds(sectionStart, sectionEnd, &instructionBuffer, &options))
				{
					didErrorOccur = true;
				}
			}
			else
			{
				if (sectionStart < firstAddress && sectionEnd > firstAddress)
				{
					if (ERROR_JDC == DisassembleBetweenBounds(sectionStart, firstAddress, &instructionBuffer, &options))
					{
						didErrorOccur = true;
					}
				}

				if (sectionStart < lastAddress && sectionEnd > lastAddress)
				{
					if (ERROR_JDC == DisassembleBetweenBounds(lastAddress, sectionEnd, &instructionBuffer, &options))
					{
						didErrorOccur = true;
					}
				}
			}
		}
	}

	std::sort(mainGui->disassembledInstructions.begin(), mainGui->disassembledInstructions.end(), CompareInstructions);

	int32_t numOfInstructions = mainGui->disassembledInstructions.size();
	for (int32_t i = 0; i < numOfInstructions - 1; i++)
	{
		uint64_t startVA = mainGui->disassembledInstructions[i].address + mainGui->disassembledInstructions[i].numOfBytes;
		uint64_t endVA = mainGui->disassembledInstructions[i + 1].address;
		if (startVA != endVA)
		{
			if (ERROR_JDC == DisassembleBetweenBounds(startVA, endVA, &instructionBuffer, &options))
			{
				didErrorOccur = true;
			}
		}
	}

	std::sort(mainGui->disassembledInstructions.begin(), mainGui->disassembledInstructions.end(), CompareInstructions);

	if (didErrorOccur)
	{
		PostLog("An error occured while disassembling", LOG_PARAM_ERROR);
	}

	mainGui->UpdateDecompParams();

	PostLog("handling jump tables...", LOG_PARAM_ADD_NEW_LINE);
	PostLog("\tCurrent instruction: ", LOG_PARAM_NONE);
	PostLogProgress(mainGui->disassembledInstructions[0].address, false);

	if (!HandleJmpTables())
	{
		PostLog("An error occured while handling jump tables", LOG_PARAM_ERROR);
	}

	PostLog("finished disassembling file", LOG_PARAM_ADD_NEW_LINE);
}

enum JdcStatus WorkerThread::DisassembleTakingJumps(uint64_t startVA, struct DisassembledInstruction* instructionBuffer, struct DisassemblerOptions* options, int32_t callNum)
{
	struct FileSection* currentSection = 0;
	uint64_t currentFileOffset = rvaToFileOffset(mainGui->sections, mainGui->numOfSections, startVA - mainGui->imageBase, &currentSection);
	if (!currentSection || currentSection->type != CODE_FST)
	{
		return SUCCESS_JDC;
	}
	else if (currentFileOffset >= mainGui->numOfFileBytes || currentSection->fileOffset + currentSection->physicalSize > mainGui->numOfFileBytes)
	{
		PostLog(wxString::Format("instruction jumps outside file: 0x%llX", instructionBuffer->address), LOG_PARAM_ERROR);
		return ERROR_JDC;
	}

	bool storeInstruction = true;

	uint64_t currentVirtualAddress = startVA;
	while (currentFileOffset < currentSection->fileOffset + currentSection->physicalSize)
	{
		if (stopRequested.load())
		{
			return SUCCESS_JDC;
		}

		if (findInstructionByAddress(mainGui->disassembledInstructions.data(), mainGui->disassembledInstructions.size(), currentVirtualAddress) != -1)
		{
			return SUCCESS_JDC;
		}

		int32_t instructionIndex = findInstructionInsertPoint(mainGui->disassembledInstructions.data(), mainGui->disassembledInstructions.size(), currentVirtualAddress);
		if (instructionIndex > 0 && instructionIndex < mainGui->disassembledInstructions.size() &&
			currentVirtualAddress > mainGui->disassembledInstructions[instructionIndex - 1].address &&
			currentVirtualAddress < mainGui->disassembledInstructions[instructionIndex - 1].address + mainGui->disassembledInstructions[instructionIndex - 1].numOfBytes)
		{
			PostLog(wxString::Format("instruction overlaps with existing instruction: 0x%llX", currentVirtualAddress), LOG_PARAM_ERROR);
			return ERROR_JDC;
		}

		if (!disassembleInstruction(&mainGui->fileBytes[currentFileOffset], mainGui->fileBytes + currentSection->fileOffset + currentSection->physicalSize - 1, options, instructionBuffer))
		{
			mainGui->errorAddress = currentVirtualAddress;
			PostLog(wxString::Format("bad instruction at 0x%llX", currentVirtualAddress), LOG_PARAM_ERROR);
			return ERROR_JDC;
		}

		instructionBuffer->address = currentVirtualAddress;

		currentFileOffset += instructionBuffer->numOfBytes;
		currentVirtualAddress += instructionBuffer->numOfBytes;

		if (storeInstruction) // only the instructions that are jumped to are stored because inserting into the vector takes too much time
		{
			// this needs to be sorted here because the find instruction functions use a binary search
			mainGui->disassembledInstructions.insert(mainGui->disassembledInstructions.begin() + instructionIndex, *instructionBuffer);
			storeInstruction = false;
		}

		PostLogProgress(currentVirtualAddress, true);

		mainGui->decompParams.instructions = instructionBuffer;
		if (isOpcodeReturn(instructionBuffer->opcode))
		{
			return SUCCESS_JDC;
		}
		else if (isOpcodeJmp(instructionBuffer->opcode))
		{
			uint64_t jmpDst = getJmpDst(&mainGui->decompParams, 0);
			if (jmpDst != 0)
			{
				struct FileSection* section = 0;
				currentFileOffset = rvaToFileOffset(mainGui->sections, mainGui->numOfSections, jmpDst - mainGui->imageBase, &section);
				if (!section || section->type != CODE_FST)
				{
					return SUCCESS_JDC;
				}

				currentVirtualAddress = jmpDst;
				storeInstruction = true;
			}
			else
			{
				return SUCCESS_JDC;
			}
		}
		else if (isOpcodeJcc(instructionBuffer->opcode) || isOpcodeCall(instructionBuffer->opcode))
		{
			uint64_t jmpDst = getJmpDst(&mainGui->decompParams, 0);
			if (jmpDst != 0)
			{
				if (ERROR_JDC == DisassembleTakingJumps(jmpDst, instructionBuffer, options, callNum + 1))
				{
					return ERROR_JDC;
				}
			}
		}
		else if (callNum > 2000)
		{
			return SUCCESS_JDC;
		}
	}

	return SUCCESS_JDC;
}

enum JdcStatus WorkerThread::DisassembleBetweenBounds(uint64_t startVA, uint64_t endVA, struct DisassembledInstruction* instructionBuffer, struct DisassemblerOptions* options)
{
	struct FileSection* currentSection = 0;
	uint64_t currentFileOffset = rvaToFileOffset(mainGui->sections, mainGui->numOfSections, startVA - mainGui->imageBase, &currentSection);
	if (!currentSection || currentSection->type != CODE_FST)
	{
		return SUCCESS_JDC;
	}
	else if (currentFileOffset >= mainGui->numOfFileBytes)
	{
		return ERROR_JDC;
	}

	uint64_t endFileOffset = rvaToFileOffset(mainGui->sections, mainGui->numOfSections, endVA - mainGui->imageBase, 0);
	if (endFileOffset > mainGui->numOfFileBytes || endFileOffset == 0)
	{
		return ERROR_JDC;
	}

	uint64_t currentVirtualAddress = startVA;
	while (currentFileOffset < endFileOffset)
	{
		if (stopRequested.load())
		{
			return SUCCESS_JDC;
		}

		if (!disassembleInstruction(&mainGui->fileBytes[currentFileOffset], mainGui->fileBytes + endFileOffset - 1, options, instructionBuffer) ||
			currentFileOffset + instructionBuffer->numOfBytes > endFileOffset)
		{
			int32_t numOfBytes = instructionBuffer->numOfBytes;
			if (numOfBytes == 0)
			{
				return ERROR_JDC;
			}

			memset(instructionBuffer, 0, sizeof(struct DisassembledInstruction));
			instructionBuffer->opcode = DATA;
			instructionBuffer->numOfBytes = 1;

			PostLogProgress(currentVirtualAddress, true);

			for (int32_t i = 0; i < numOfBytes; i++)
			{
				if (currentFileOffset >= endFileOffset)
				{
					return SUCCESS_JDC;
				}

				instructionBuffer->address = currentVirtualAddress;
				instructionBuffer->operands = (struct Operand*)calloc(1, sizeof(struct Operand));
				if (!instructionBuffer->operands)
				{
					return ERROR_JDC;
				}

				instructionBuffer->operands[0].immediate.value = mainGui->fileBytes[currentFileOffset];
				instructionBuffer->operands[0].type = IMMEDIATE;
				instructionBuffer->operands[0].immediate.size = 1;
				instructionBuffer->numOfOperands = 1;

				currentFileOffset++;
				currentVirtualAddress++;
				mainGui->disassembledInstructions.push_back(*instructionBuffer);
			}
		}
		else
		{
			instructionBuffer->address = currentVirtualAddress;

			currentFileOffset += instructionBuffer->numOfBytes;
			currentVirtualAddress += instructionBuffer->numOfBytes;
			mainGui->disassembledInstructions.push_back(*instructionBuffer);
		}
	}

	return SUCCESS_JDC;
}

enum JdcStatus WorkerThread::HandleJmpTables()
{
	// this is setting called instructions from the RUNTIME_FUNCTION structs in .pdata
	if (mainGui->fileFormat == PE_FF && mainGui->is64Bit) // https://learn.microsoft.com/en-us/cpp/build/exception-handling-x64?view=msvc-170
	{
		for (int32_t i = 0; i < mainGui->numOfSections; i++)
		{
			if (strcmp(mainGui->sections[i].name.buffer, ".pdata") == 0)
			{
				int32_t j = 0;
				while (j < mainGui->sections[i].physicalSize)
				{
					uint64_t beginAddress = mainGui->imageBase + *(uint32_t*)(mainGui->fileBytes + mainGui->sections[i].fileOffset + j);
					if (beginAddress == 0)
					{
						break;
					}

					int32_t calledInstructionIndex = findInstructionByAddress(mainGui->disassembledInstructions.data(), mainGui->disassembledInstructions.size(), beginAddress);
					if (calledInstructionIndex != -1)
					{
						mainGui->disassembledInstructions[calledInstructionIndex].isCalled = 1;
					}

					j += 0xC;
				}

				break;
			}
		}
	}

	int32_t numOfInstructions = mainGui->disassembledInstructions.size();
	for (int32_t i = 0; i < numOfInstructions; i++)
	{
		if (stopRequested.load())
		{
			return SUCCESS_JDC;
		}

		PostLogProgress(mainGui->disassembledInstructions[i].address, true);

		if (mainGui->disassembledInstructions[i].address == mainGui->imageBase + mainGui->entryPoint)
		{
			mainGui->disassembledInstructions[i].isCalled = 1;
		}

		struct JumpTable jumpTable = { 0 };
		if (getJumpTable(&mainGui->decompParams, i, &jumpTable))
		{
			mainGui->jumpTables.push_back(jumpTable);
		}
		else if (mainGui->disassembledInstructions[i].opcode == CALL_NEAR)
		{
			int32_t calledInstructionIndex = findInstructionByAddress(mainGui->disassembledInstructions.data(), mainGui->disassembledInstructions.size(), getJmpDst(&mainGui->decompParams, i));
			if (calledInstructionIndex != -1)
			{
				mainGui->disassembledInstructions[calledInstructionIndex].isCalled = 1;
			}
		}
		else if (mainGui->disassembledInstructions[i].opcode == JMP_NEAR)
		{
			int32_t dstIndex = findInstructionByAddress(mainGui->disassembledInstructions.data(), mainGui->disassembledInstructions.size(), getJmpDst(&mainGui->decompParams, i));
			if (dstIndex != -1)
			{
				mainGui->disassembledInstructions[dstIndex].isJmpDst = 1;
			}
		}
	}

	std::sort(mainGui->jumpTables.begin(), mainGui->jumpTables.end(), CompareJumpTables);

	std::vector<struct DisassembledInstruction> dataInstructions; // this buffer is used so that only one insert call is needed
	int32_t numOfJmpTables = mainGui->jumpTables.size();
	for (int32_t i = 0; i < numOfJmpTables; i++)
	{
		if (stopRequested.load())
		{
			return SUCCESS_JDC;
		}

		uint64_t fileOffest = rvaToFileOffset(mainGui->sections, mainGui->numOfSections, mainGui->jumpTables[i].jmpTableAddress - mainGui->imageBase, 0);
		int32_t instructionIndex = findInstructionByAddress(mainGui->disassembledInstructions.data(), mainGui->disassembledInstructions.size(), mainGui->jumpTables[i].jmpTableAddress);
		if (instructionIndex != -1)
		{
			int32_t lastInstructionIndex = instructionIndex;
			while (!mainGui->disassembledInstructions[lastInstructionIndex].isCalled &&
				!mainGui->disassembledInstructions[lastInstructionIndex].isJmpDst)
			{
				free(mainGui->disassembledInstructions[lastInstructionIndex].operands);
				lastInstructionIndex++;
			}

			uint64_t lastAddress = mainGui->disassembledInstructions[lastInstructionIndex].address;
			mainGui->disassembledInstructions.erase(mainGui->disassembledInstructions.begin() + instructionIndex, mainGui->disassembledInstructions.begin() + lastInstructionIndex);

			bool isInIndirectTable = false;
			uint64_t currentAddress = mainGui->jumpTables[i].jmpTableAddress;
			uint64_t initialAddress = currentAddress;
			while (currentAddress < lastAddress)
			{
				if (currentAddress == mainGui->jumpTables[i].indirectTableAddress)
				{
					isInIndirectTable = true;
				}
				else if (i + 1 < numOfJmpTables && currentAddress == mainGui->jumpTables[i + 1].jmpTableAddress)
				{
					i++;
					isInIndirectTable = false;
				}

				struct DisassembledInstruction instruction;
				memset(&instruction, 0, sizeof(struct DisassembledInstruction));
				instruction.opcode = DATA;
				instruction.numOfOperands = 1;
				instruction.operands = (struct Operand*)calloc(1, sizeof(struct Operand));
				if (!instruction.operands)
				{
					return ERROR_JDC;
				}
				instruction.operands[0].type = IMMEDIATE;
				instruction.operands[0].immediate.size = isInIndirectTable ? 1 : mainGui->jumpTables[i].addressSize;
				instruction.numOfBytes = isInIndirectTable ? 1 : mainGui->jumpTables[i].addressSize;
				instruction.address = currentAddress;

				if (isInIndirectTable)
				{
					instruction.operands[0].immediate.value = *(uint8_t*)(mainGui->fileBytes + fileOffest + (currentAddress - initialAddress));
				}
				else if (mainGui->jumpTables[i].addressSize == 4)
				{
					instruction.operands[0].immediate.value = *(uint32_t*)(mainGui->fileBytes + fileOffest + (currentAddress - initialAddress));
				}
				else
				{
					instruction.operands[0].immediate.value = *(uint64_t*)(mainGui->fileBytes + fileOffest + (currentAddress - initialAddress));
				}

				dataInstructions.push_back(instruction);
				currentAddress += isInIndirectTable ? 1 : mainGui->jumpTables[i].addressSize;
			}

			mainGui->disassembledInstructions.insert(mainGui->disassembledInstructions.begin() + instructionIndex, dataInstructions.begin(), dataInstructions.end());
			mainGui->decompParams.numOfInstructions = mainGui->disassembledInstructions.size();
			dataInstructions.clear();
		}
	}

	return SUCCESS_JDC;
}

void WorkerThread::Analyze()
{
	PostLog("finding all functions...", LOG_PARAM_ADD_NEW_LINE);

	FindAllFunctions();

	int32_t numOfFunctions = mainGui->functions.size();

	PostLog("analyzing function return types and conditions...", LOG_PARAM_ADD_NEW_LINE);
	PostLog("\tCurrent function: ", LOG_PARAM_NONE);
	PostLogProgress(mainGui->disassembledInstructions[mainGui->functions[0].firstInstructionIndex].address, false);

	bool getConditions = true;
	bool setAnyReturnType = false;
	do
	{
		setAnyReturnType = false;
		for (int32_t i = 0; i < numOfFunctions; i++)
		{
			if (stopRequested.load())
			{
				return;
			}

			bool setReturnType = false;
			if (ERROR_JDC == getFunctionReturnTypeAndConditions(&mainGui->decompParams, &mainGui->functions[i], getConditions, &setReturnType))
			{
				PostLog(wxString::Format("error getting return type or conditions of %s (0x%llX)", mainGui->functions[i].name.buffer, mainGui->disassembledInstructions[mainGui->functions[i].firstInstructionIndex].address), LOG_PARAM_ERROR);
				return;
			}

			if (setReturnType)
			{
				setAnyReturnType = true;
			}

			PostLogProgress(mainGui->disassembledInstructions[mainGui->functions[i].firstInstructionIndex].address, true);
		}

		getConditions = false;

	} while (setAnyReturnType);

	PostLog("analyzing function arguments and stack...", LOG_PARAM_ADD_NEW_LINE);
	PostLog("\tCurrent function: ", LOG_PARAM_NONE);
	PostLogProgress(mainGui->disassembledInstructions[mainGui->functions[0].firstInstructionIndex].address, false);

	for (int32_t i = 0; i < numOfFunctions; i++)
	{
		if (stopRequested.load())
		{
			return;
		}

		if (ERROR_JDC == getFunctionRegArgsAndStackVars(&mainGui->decompParams, &mainGui->functions[i]))
		{
			PostLog(wxString::Format("error getting arguments and stack vars of %s (0x%llX)", mainGui->functions[i].name.buffer, mainGui->disassembledInstructions[mainGui->functions[i].firstInstructionIndex].address), LOG_PARAM_ERROR);
			return;
		}

		PostLogProgress(mainGui->disassembledInstructions[mainGui->functions[i].firstInstructionIndex].address, true);
	}

	PostLog("fixing function arguments and stack...", LOG_PARAM_ADD_NEW_LINE);
	PostLog("\tCurrent function: ", LOG_PARAM_NONE);
	PostLogProgress(mainGui->disassembledInstructions[mainGui->functions[0].firstInstructionIndex].address, false);

	for (int32_t i = 0; i < numOfFunctions; i++)
	{
		if (stopRequested.load())
		{
			return;
		}

		if (ERROR_JDC == fixFunctionArgs(&mainGui->decompParams, &mainGui->functions[i]))
		{
			PostLog(wxString::Format("error fixing arguments and stack vars of %s (0x%llX)", mainGui->functions[i].name.buffer, mainGui->disassembledInstructions[mainGui->functions[i].firstInstructionIndex].address), LOG_PARAM_ERROR);
			return;
		}

		PostLogProgress(mainGui->disassembledInstructions[mainGui->functions[i].firstInstructionIndex].address, true);
	}

	PostLog("setting stack var data types...", LOG_PARAM_ADD_NEW_LINE);
	PostLog("\tCurrent function: ", LOG_PARAM_NONE);
	PostLogProgress(mainGui->disassembledInstructions[mainGui->functions[0].firstInstructionIndex].address, false);

	for (int32_t i = 0; i < numOfFunctions; i++)
	{
		if (stopRequested.load())
		{
			return;
		}

		if (ERROR_JDC == setFunctionStackVarTypes(&mainGui->decompParams, &mainGui->functions[i]))
		{
			PostLog(wxString::Format("error setting stack var data types of %s (0x%llX)", mainGui->functions[i].name.buffer, mainGui->disassembledInstructions[mainGui->functions[i].firstInstructionIndex].address), LOG_PARAM_ERROR);
			return;
		}

		PostLogProgress(mainGui->disassembledInstructions[mainGui->functions[i].firstInstructionIndex].address, true);
	}

	PostLog("finished analyzing file", LOG_PARAM_ADD_NEW_LINE);

	mainGui->decompParams.currentFunc = 0;
}

void WorkerThread::FindAllFunctions()
{
	PostLog("\tCurrent function: ", LOG_PARAM_NONE);
	PostLogProgress(mainGui->disassembledInstructions[0].address, false);

	int32_t numOfInstructions = mainGui->disassembledInstructions.size();
	int32_t instructionIndex = 0;

	struct Function currentFunction;
	memset(&currentFunction, 0, sizeof(struct Function));
	int32_t numOfFunctions = 0;
	while (instructionIndex < numOfInstructions && findNextFunction(&mainGui->decompParams, &currentFunction, &instructionIndex))
	{
		if (stopRequested.load())
		{
			return;
		}

		currentFunction.name = initializeJdcStr();
		if (!getSymbols || !getSymbolByValue(mainGui->currentFilePath.c_str().AsWChar(), mainGui->fileFormat, mainGui->is64Bit, mainGui->disassembledInstructions[currentFunction.firstInstructionIndex].address, &currentFunction.name))
		{
			sprintfJdc(&currentFunction.name, false, "func%llX", mainGui->disassembledInstructions[currentFunction.firstInstructionIndex].address - mainGui->imageBase);
		}

		PostLogProgress(mainGui->disassembledInstructions[currentFunction.firstInstructionIndex].address, true);

		mainGui->functions.push_back(currentFunction);
		memset(&currentFunction, 0, sizeof(struct Function));
		numOfFunctions++;
	}

	mainGui->UpdateDecompParams();
}