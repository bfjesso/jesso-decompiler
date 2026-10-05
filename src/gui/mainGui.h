#pragma once
#include "guiUtils.h"
#include "jdcTabArt.h"
#include "settingsWindow.h"
#include "disassemblyTextCtrl.h"
#include "decompilationTextCtrl.h"
#include "functionsTextCtrl.h"
#include "dataTextCtrl.h"
#include "colorsMenu.h"
#include "logTextCtrl.h"
#include "codeReferencesWindow.h"
#include "functionInfoWindow.h"
#include "../file-handler/fileHandler.h"
#include "../disassembler/disassembler.h"
#include "../decompiler/decompilationStructs.h"

class MainGui : public wxFrame
{
public:
	MainGui();

	wxMenuBar* menuBar = nullptr;
	SettingsWindow* settingsWindow = nullptr;
	ColorsMenu* colorsMenu = nullptr;

	LogTextCtrl* logTextCtrl = nullptr;

	std::vector<DisassemblyTextCtrl*> disassemblyTextCtrls;
	std::vector<DecompilationTextCtrl*> decompilationTextCtrls;
	std::vector<FunctionsTextCtrl*> functionsTextCtrls;
	std::vector<DataTextCtrl*> dataTextCtrls;

	wxAuiManager auiManager;
	wxAuiNotebook* auiNotebook;

	wxString currentFilePath = "";
	enum FileFormat fileFormat = UNKNOWN_FF;
	bool is64Bit = false;
	uint64_t imageBase = 0;
	uint64_t entryPoint = 0;

	uint8_t* fileBytes = nullptr;
	uint64_t numOfFileBytes = 0;

	FileSection* sections = nullptr;
	int32_t numOfSections = 0;

	ImportedFunction* imports = nullptr;
	int32_t numOfImports = 0;
	JdcStr* libraryNames = nullptr;
	int32_t numOfLibraries = 0;

	std::vector<DisassembledInstruction> disassembledInstructions;
	std::vector<JumpTable> jumpTables;

	std::vector<Function> functions;

	DecompilationParameters decompParams = { 0 };
	
	enum ids 
	{
		NotebookID,
		OpenDisassemblyID,
		OpenDecompilationID,
		OpenFunctionsID,
		OpenDataID,
		OpenSectionsViewerID,
		OpenStringsMenuID,
		OpenImportsViewerID,
		OpenFileHeadersMenuID,
		OpenCodeReferencesWindowID,
		OpenCalculatorMenuID,
		OpenBytesDisassemblerID,
		OpenLogID,
		OpenSettingsID,
		ResetWindowLayoutID,
		OpenColorsMenuID,
		OpenFileID,
		DisassembleFileID,
		AnalyzeFileID,
		DisassembleFileButtonID,
		AnalyzeFileButtonID
	};

	void ResetWindowLayout();

	void AddFloatingPane(wxWindow* window, wxString caption);

	void OpenSettings(int32_t direction);

	void ShowWindowInAUI(int32_t direction, wxWindow* window);

	DisassemblyTextCtrl* AddDisassemblyTextCtrl();

	DecompilationTextCtrl* AddDecompilationTextCtrl();

	FunctionsTextCtrl* AddFunctionsTextCtrl();

	DataTextCtrl* AddDataTextCtrl();

	CodeReferencesWindow* AddCodeReferencesWindow();

	FunctionInfoWindow* AddFunctionInfoWindow(struct Function* function);

	void OnPaneClose(wxAuiManagerEvent& e);

	void OnPageClose(wxAuiNotebookEvent& e);

	void OnTabRightClick(wxAuiNotebookEvent& e);

	void OnMouseRightClick(wxMouseEvent& e);

	void RemoveTextCtrl(wxWindow* window);

	void RefreshVarNames(int32_t functionIndex);

	void AddMenuItem(wxMenu* menu, int32_t id, const char* name, const std::function<void(wxCommandEvent&)>& function);

	void OpenFile();

	enum JdcStatus LoadKnownFile(wxString filePath);

	enum JdcStatus LoadUnknownFile(wxString filePath);

	void DisassembleFile();

	void AnalyzeFile();

	void ClearData();

	enum JdcStatus DisassembleTakingJumps(uint64_t startVA, struct DisassembledInstruction* instructionBuffer, struct DisassemblerOptions* options, uint64_t* errorAddress);

	enum JdcStatus DisassembleBetweenBounds(uint64_t startVA, uint64_t endVA, struct DisassembledInstruction* instructionBuffer, struct DisassemblerOptions* options);

	enum JdcStatus HandleJmpTables();

	void FindAllFunctions(bool getSymbols);

	void CloseApp(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};