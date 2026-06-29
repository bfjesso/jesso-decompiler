#pragma once
#include "guiUtils.h"
#include <wx/aui/aui.h>
#include "disassemblyTextCtrl.h"
#include "decompilationTextCtrl.h"
#include "functionsTextCtrl.h"
#include "dataTextCtrl.h"
#include "colorsMenu.h"
#include "../file-handler/fileHandler.h"
#include "../disassembler/disassembler.h"
#include "../decompiler/decompilationStructs.h"

class FunctionPropertiesMenu;

class MainGui : public wxFrame
{
public:
	MainGui();

	wxMenuBar* menuBar = nullptr;
	ColorsMenu* colorsMenu = nullptr;

	JdcTextCtrl* logTextCtrl = nullptr;

	std::vector<DisassemblyTextCtrl*> disassemblyTextCtrls;
	std::vector<DecompilationTextCtrl*> decompilationTextCtrls;
	std::vector<FunctionsTextCtrl*> functionsTextCtrls;
	std::vector<DataTextCtrl*> dataTextCtrls;

	wxAuiManager auiManager;
	wxAuiNotebook* auiNotebook;

	wxString currentFilePath = "";
	unsigned char is64Bit = 0;
	unsigned long long imageBase = 0;
	unsigned long long entryPoint = 0;

	unsigned char* fileBytes = nullptr;
	unsigned long long numOfFileBytes = 0;

	FileSection* sections = nullptr;
	int numOfSections = 0;

	ImportedFunction* imports = nullptr;
	int numOfImports = 0;

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

	void AddDisassemblyTextCtrl();

	void AddDecompilationTextCtrl();

	void AddFunctionsTextCtrl();

	void AddDataTextCtrl();

	void OnPaneClose(wxAuiManagerEvent& e);

	void OnPageClose(wxAuiNotebookEvent& e);

	void OnTabRightClick(wxAuiNotebookEvent& e);

	void OnMouseRightClick(wxMouseEvent& e);

	void RemoveTextCtrl(wxWindow* window);

	void AddMenuItem(wxMenu* menu, int id, const char* name, const std::function<void(wxCommandEvent&)>& function);

	void Log(wxString text);

	void OpenFile();

	void DisassembleFile();

	void AnalyzeFile();

	void ClearData();

	unsigned char DisassembleTakingJumps(unsigned long long startVA, struct DisassembledInstruction* instructionBuffer, struct DisassemblerOptions* options, unsigned long long* errorAddress);

	unsigned char DisassembleBetweenBounds(unsigned long long startVA, unsigned long long endVA, struct DisassembledInstruction* instructionBuffer, struct DisassemblerOptions* options);

	void FindAllFunctions(unsigned char getSymbols);

	void CloseApp(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};

#include <wx/grid.h>

class FunctionPropertiesMenu : public wxFrame
{
public:
	FunctionPropertiesMenu(wxWindow* parent, wxPoint position, MainGui* main, int funcIndex);

	wxStaticText* functionNameLabel = nullptr;
	wxTextCtrl* functionNameTextCtrl;

	wxGrid* varsGrid = nullptr;

	wxBoxSizer* vSizer = nullptr;

	MainGui* mainGui = nullptr;
	int functionIndex = -1;

	enum ids
	{
		MainWindowID
	};

	void CloseMenu(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};
