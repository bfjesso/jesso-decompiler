#pragma once
#include <atomic>
#include "guiUtils.h"
#include <wx/thread.h>
#include <wx/event.h>
#include "../disassembler/disassemblyStructs.h"

wxDECLARE_EVENT(EVT_WORKER_LOG, wxThreadEvent);
wxDECLARE_EVENT(EVT_WORKER_LOG_PROGRESS, wxThreadEvent);
wxDECLARE_EVENT(EVT_ANALYSIS_DONE, wxThreadEvent);
wxDECLARE_EVENT(EVT_DISASSEMBLY_DONE, wxThreadEvent);

class MainGui;

class WorkerThread : public wxThread
{
public:
	enum LogParams
	{
		LOG_PARAM_ERROR,
		LOG_PARAM_ADD_NEW_LINE,
		LOG_PARAM_NONE
	};

	enum WorkerThreadType
	{
		DISASSEMBLE_WORKER_THREAD,
		ANALYZE_WORKER_THREAD,
	};

	WorkerThread(MainGui* gui, enum WorkerThreadType threadType, bool getFuncNameSymbols);

	ExitCode Entry() override;

	void Stop();

private:
	MainGui* mainGui = nullptr;
	enum WorkerThreadType workerThreadType;
	bool getSymbols = false;

	std::atomic<bool> stopRequested{ false };

	void PostLog(const wxString& text, enum LogParams logParam);

	void PostLogProgress(uint64_t value, bool updateCurrentProgress);

	void Disassemble();

	enum JdcStatus DisassembleTakingJumps(uint64_t startVA, struct DisassembledInstruction* instructionBuffer, struct DisassemblerOptions* options, int32_t callNum);

	enum JdcStatus DisassembleBetweenBounds(uint64_t startVA, uint64_t endVA, struct DisassembledInstruction* instructionBuffer, struct DisassemblerOptions* options);

	enum JdcStatus HandleJmpTables();

	void Analyze();

	void FindAllFunctions();
};