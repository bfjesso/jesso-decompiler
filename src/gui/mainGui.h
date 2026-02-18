#pragma once
#include "guiUtils.h"
#include <wx/grid.h>
#include "disassembleBytesMenu.h"
#include "dataViewerMenu.h"
#include "colorsMenu.h"
#include "../file-handler/fileHandler.h"
#include "../disassembler/disassembler.h"
#include "../decompiler/decompiler.h"

class FunctionPropertiesMenu;

class MainGui : public wxFrame, public Utils
{
public:
	MainGui();

	wxMenuBar* menuBar = nullptr;
	BytesDisassembler* bytesDisassemblerMenu = nullptr;
	DataViewer* dataViewerMenu = nullptr;
	ColorsMenu* colorsMenu = nullptr;
	FunctionPropertiesMenu* functionPropertiesMenu = nullptr;

	wxStaticText* statusStaticText = nullptr;

	wxStyledTextCtrl* disassemblyTextCtrl = nullptr;
	wxStyledTextCtrl* decompilationTextCtrl = nullptr;

	wxGrid* functionsGrid = nullptr;

	wxBoxSizer* row1Sizer = nullptr;
	wxBoxSizer* row2Sizer = nullptr;
	wxBoxSizer* vSizer = nullptr;

	wxString currentFilePath = "";
	unsigned char is64Bit = 0;
	unsigned long long imageBase = 0;

	static const int importsBufferMaxSize = 50;
	ImportedFunction imports[importsBufferMaxSize] = { 0 };
	int numOfImports = 0;

	std::vector<DisassembledInstruction> disassembledInstructions;

	std::vector<Function> functions;

	unsigned char* dataSectionBytes = nullptr;
	static const int dataSectionsBufferSize = 10;
	FileSection dataSections[dataSectionsBufferSize] = { 0 };
	int numOfDataSections = 0;

	static const int codeSectionsBufferSize = 10;
	FileSection codeSections[codeSectionsBufferSize] = { 0 };
	int numOfCodeSections = 0;

	int currentDecompiledFunc = -1;

	DecompilationParameters decompParams = { 0 };
	
	enum ids 
	{
		MainWindowID,
		OpenBytesDisassemblerID,
		OpenDataViewerID,
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

	void LoadDataSectionBytes();

	void DisassembleCodeSections();

	void DecompileFunction(int functionIndex);

	void FindAllFunctions();

	void UpdateDisassemblyTextCtrl();

	void UpdateFunctionsGrid();

	void GetFunctionSymbols();

	void GridRightClickOptions(wxGridEvent& e);

	void StyledTextCtrlRightClickOptions(wxContextMenuEvent& e);

	void CloseApp(wxCloseEvent& e);

	char IsCharDigit(char c);

	void ApplySyntaxHighlighting(Function* function);

	void ApplyAsmHighlighting();

	void ColorAllStrs(wxString text, wxString str, ColorsMenu::DecompilationColor color, unsigned char forceColor);

	wxDECLARE_EVENT_TABLE();
};

class FunctionPropertiesMenu : public wxFrame, public Utils
{
public:
	FunctionPropertiesMenu(wxPoint position, MainGui* main, int funcIndex);

	wxStaticText* functionNameLabel = nullptr;
	wxTextCtrl* functionNameTextCtrl;

	std::vector<wxTextCtrl*> regArgNameTextCtrls;
	std::vector<wxTextCtrl*> stackArgNameTextCtrls;
	std::vector<wxTextCtrl*> localVarNameTextCtrls;
	std::vector<wxTextCtrl*> retVarNameTextCtrls;

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
