#pragma once
#include "guiUtils.h"
#include <wx/grid.h>
#include <wx/splitter.h>
#include <wx/fdrepdlg.h>
#include "disassembleBytesMenu.h"
#include "sectionsViewerMenu.h"
#include "dataViewerMenu.h"
#include "importsViewerMenu.h"
#include "colorsMenu.h"
#include "functionInfoMenu.h"
#include "../file-handler/fileHandler.h"
#include "../disassembler/disassembler.h"
#include "../decompiler/decompiler.h"

class FunctionPropertiesMenu;

class MainGui : public wxFrame, public Utils
{
public:
	MainGui();

	wxMenuBar* menuBar = nullptr;
	DataViewer* dataViewerMenu = nullptr;
	ColorsMenu* colorsMenu = nullptr;

	wxStaticText* statusStaticText = nullptr;

	wxStyledTextCtrl* disassemblyTextCtrl = nullptr;
	wxStyledTextCtrl* decompilationTextCtrl = nullptr;

	wxGrid* functionsGrid = nullptr;

	wxSplitterWindow* mainSplitter = nullptr;
	wxSplitterWindow* topSplitter = nullptr;
	wxBoxSizer* vSizer = nullptr;

	wxString currentFilePath = "";
	unsigned char is64Bit = 0;
	unsigned long long imageBase = 0;

	unsigned char* fileBytes = nullptr;

	FileSection* sections = nullptr;
	int numOfSections = 0;

	ImportedFunction* imports = nullptr;
	int numOfImports = 0;

	std::vector<DisassembledInstruction> disassembledInstructions;

	std::vector<Function> functions;

	std::vector<unsigned long long> jmpTableStartAddresses;
	std::vector<unsigned long long> indirectTableStartAddresses;

	int currentDecompiledFunc = -1;

	DecompilationParameters decompParams = { 0 };

	wxFindReplaceData findData;
	wxFindReplaceDialog* findDialog = nullptr;
	wxStyledTextCtrl* findCtrl = nullptr;
	
	enum ids 
	{
		MainWindowID,
		OpenBytesDisassemblerID,
		OpenSectionsViewerID,
		OpenDataViewerID,
		OpenImportsViewerID,
		OpenColorsMenuID,
		OpenFileID,
		DisassembleFileID,
		AnalyzeFileID,
		DisassembleFileButtonID,
		AnalyzeFileButtonID
	};

	void OpenFile();

	void DisassembleFile();

	void AnalyzeFile();

	void ClearData();

	void LoadFileBytes();

	void DisassembleCodeSections();

	int HandleJmpTables(unsigned char* bytes, unsigned int* currentIndexRef, FileSection currentCodeSection);

	unsigned char CheckForJmpTableStart(unsigned long long currentAddress);

	unsigned char CheckForIndirectTableStart(unsigned long long currentAddress);

	void DecompileFunction(int functionIndex);

	void FindAllFunctions();

	void UpdateDisassemblyTextCtrl();

	void UpdateFunctionsGrid();

	void GetFunctionSymbols();

	void GridRightClickOptions(wxGridEvent& e);

	void StyledTextCtrlRightClickOptions(wxContextMenuEvent& e);

	void OnFindDialog(wxFindDialogEvent& e);

	int FindInRange(wxStyledTextCtrl* ctrl, const wxString& text, int start, int end, int flags, unsigned char forward);

	void OnFindDialogClose(wxFindDialogEvent& e);

	void CloseApp(wxCloseEvent& e);

	char IsCharDigit(char c);

	void ApplySyntaxHighlighting(Function* function);

	void ApplyAsmHighlighting();

	void ColorAllStrs(wxString text, const char* string, ColorsMenu::DecompilationColor color, unsigned char forceColor);

	wxDECLARE_EVENT_TABLE();
};

class FunctionPropertiesMenu : public wxFrame, public Utils
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
