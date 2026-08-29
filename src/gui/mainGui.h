#pragma once
#include "guiUtils.h"
#include <wx/aui/aui.h>
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
	unsigned char is64Bit = 0;
	unsigned long long imageBase = 0;
	unsigned long long entryPoint = 0;

	unsigned char* fileBytes = nullptr;
	unsigned long long numOfFileBytes = 0;

	FileSection* sections = nullptr;
	int numOfSections = 0;

	ImportedFunction* imports = nullptr;
	int numOfImports = 0;
	JdcStr* libraryNames = nullptr;
	int numOfLibraries = 0;

	std::vector<DisassembledInstruction> disassembledInstructions;

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

	void OpenLog(int direction);

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

	void RefreshVarNames(int functionIndex);

	void AddMenuItem(wxMenu* menu, int id, const char* name, const std::function<void(wxCommandEvent&)>& function);

	void OpenFile();

	unsigned char LoadKnownFile(wxString filePath);

	unsigned char LoadUnknownFile(wxString filePath);

	void DisassembleFile();

	void AnalyzeFile();

	void ClearData();

	unsigned char DisassembleTakingJumps(unsigned long long startVA, struct DisassembledInstruction* instructionBuffer, struct DisassemblerOptions* options, unsigned long long* errorAddress);

	unsigned char DisassembleBetweenBounds(unsigned long long startVA, unsigned long long endVA, struct DisassembledInstruction* instructionBuffer, struct DisassemblerOptions* options);

	void FindAllFunctions(unsigned char getSymbols);

	void CloseApp(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};