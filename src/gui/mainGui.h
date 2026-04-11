#pragma once
#include "guiUtils.h"
#include <wx/grid.h>
#include <wx/splitter.h>
#include "disassembleBytesMenu.h"
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
	BytesDisassembler* bytesDisassemblerMenu = nullptr;
	DataViewer* dataViewerMenu = nullptr;
	ImportsViewer* importsViewerMenu = nullptr;
	ColorsMenu* colorsMenu = nullptr;;

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

	ImportedFunction* imports = nullptr;
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

	std::vector<unsigned long long> jmpTableStartAddresses;
	std::vector<unsigned long long> indirectTableStartAddresses;

	int currentDecompiledFunc = -1;

	DecompilationParameters decompParams = { 0 };
	
	enum ids 
	{
		MainWindowID,
		OpenBytesDisassemblerID,
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

	void LoadDataSectionBytes();

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
