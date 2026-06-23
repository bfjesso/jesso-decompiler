#pragma once
#include "guiUtils.h"
#include "jdcTextCtrl.h"
#include "disassemblyWindow.h"
#include <wx/grid.h>
#include <wx/splitter.h>
#include "disassembleBytesMenu.h"
#include "sectionsViewerMenu.h"
#include "dataViewerMenu.h"
#include "stringsMenu.h"
#include "importsViewerMenu.h"
#include "colorsMenu.h"
#include "functionInfoMenu.h"
#include "../file-handler/fileHandler.h"
#include "../disassembler/disassembler.h"
#include "../decompiler/decompiler.h"

class FunctionPropertiesMenu;

class MainGui : public wxFrame
{
public:
	MainGui();

	wxMenuBar* menuBar = nullptr;
	DataViewer* dataViewerMenu = nullptr;
	StringsMenu* stringsMenu = nullptr;
	ColorsMenu* colorsMenu = nullptr;

	wxStaticText* statusStaticText = nullptr;

	DisassemblyWindow* disassemblyWindow = nullptr;
	JdcTextCtrl* decompilationTextCtrl = nullptr;
	JdcTextCtrl* functionsTextCtrl = nullptr;

	unsigned char showAssociatedDecompiledLines = 1;
	unsigned char showAssociatedInstructions = 1;
	unsigned char showAssociatedFunctions = 1;
	unsigned char showBytesInDataViewer = 1;

	wxSplitterWindow* mainSplitter = nullptr;
	wxSplitterWindow* topSplitter = nullptr;
	wxBoxSizer* vSizer = nullptr;

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

	int currentDecompiledFunc = -1;

	DecompilationParameters decompParams = { 0 };
	
	enum ids 
	{
		MainWindowID,
		OpenBytesDisassemblerID,
		OpenSectionsViewerID,
		OpenDataViewerID,
		OpenStringsMenuID,
		OpenImportsViewerID,
		OpenFileHeadersMenuID,
		OpenCalculatorMenuID,
		OpenColorsMenuID,
		OpenFileID,
		DisassembleFileID,
		AnalyzeFileID,
		DisassembleFileButtonID,
		AnalyzeFileButtonID
	};

	void AddMenuItem(wxMenu* menu, int id, const char* name, const std::function<void(wxCommandEvent&)>& function);

	void OpenFile();

	void DisassembleFile();

	void AnalyzeFile();

	void ClearData();

	unsigned char DisassembleTakingJumps(unsigned long long startVA, struct DisassembledInstruction* instructionBuffer, struct DisassemblerOptions* options, unsigned long long* errorAddress);

	unsigned char DisassembleBetweenBounds(unsigned long long startVA, unsigned long long endVA, struct DisassembledInstruction* instructionBuffer, struct DisassemblerOptions* options);

	void DecompileFunction(int functionIndex);

	void FindAllFunctions();

	void UpdateDisassemblyTextCtrl();

	void UpdateFunctionsTextCtrl(unsigned char getSymbols);

	void CloseApp(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};

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
