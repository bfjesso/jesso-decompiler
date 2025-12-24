#pragma once
#include "guiUtils.h"
#include <wx/grid.h>
#include "disassembleBytesMenu.h"
#include "dataViewerMenu.h"
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
	FunctionPropertiesMenu* functionPropertiesMenu = nullptr;

	wxButton* disassembleFileButton = nullptr;
	wxButton* analyzeFileButton = nullptr;
	wxGrid* disassemblyGrid = nullptr;

	wxTextCtrl* decompilationTextCtrl = nullptr;

	wxGrid* functionsGrid = nullptr;

	wxBoxSizer* row1Sizer = nullptr;
	wxBoxSizer* row2Sizer = nullptr;
	wxBoxSizer* row3Sizer = nullptr;
	wxBoxSizer* vSizer = nullptr;

	wxString currentFilePath = "";
	unsigned char is64Bit = 0;
	unsigned long long imageBase = 0;

	static const int importsBufferMaxSize = 50;
	ImportedFunction imports[importsBufferMaxSize] = { 0 };
	int numOfImports = 0;

	std::vector<unsigned long long> instructionAddresses;
	std::vector<DisassembledInstruction> disassembledInstructions;

	std::vector<Function> functions;

	unsigned char* dataSectionBytes = nullptr;
	static const int dataSectionsBufferSize = 10;
	FileSection dataSections[dataSectionsBufferSize] = { 0 };
	int numOfDataSections = 0;

	int currentDecompiledFunc = -1;
	
	enum ids 
	{
		MainWindowID,
		OpenBytesDisassemblerID,
		OpenDataViewerID,
		OpenFileID,
		DisassembleFileButtonID,
		AnalyzeFileButtonID
	};

	void OpenFile();

	void DisassembleButton(wxCommandEvent& e);

	void AnalyzeButton(wxCommandEvent& e);

	void LoadDataSectionBytes();

	void DisassembleCodeSections();

	void DecompileFunction(unsigned short functionIndex);

	void FindAllFunctions();

	void RightClickOptions(wxGridEvent& e);

	void CloseApp(wxCloseEvent& e);

	void ReplaceEscapeChars(wxString* str);

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
