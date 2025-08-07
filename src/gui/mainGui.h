#pragma once
#include "guiUtils.h"
#include <wx/grid.h>
#include "disassembleBytesMenu.h"
#include "dataViewerMenu.h"
#include "functionPropertiesMenu.h"
#include "../disassembler/disassembler.h"
#include "../decompiler/decompiler.h"

class MainGui : public wxFrame, public Utils
{
public:
	MainGui();

	wxMenuBar* menuBar = nullptr;
	BytesDisassembler* bytesDisassemblerMenu = nullptr;
	DataViewer* dataViewerMenu = nullptr;
	FunctionPropertiesMenu* functionPropertiesMenu = nullptr;

	wxButton* openFileButton = nullptr;
	wxTextCtrl* numOfbytesInputTextCtrl = nullptr;
	wxButton* disassembleFileButton = nullptr;
	wxButton* analyzeFileButton = nullptr;
	wxListBox* disassemblyListBox = nullptr;

	wxListBox* decompilationListBox = nullptr;

	wxGrid* functionsGrid = nullptr;

	wxBoxSizer* row1Sizer = nullptr;
	wxBoxSizer* row2Sizer = nullptr;
	wxBoxSizer* row3Sizer = nullptr;
	wxBoxSizer* row4Sizer = nullptr;
	wxBoxSizer* vSizer = nullptr;

	HANDLE currentFile = 0;
	unsigned char is64Bit = 0;
	uintptr_t imageBase = 0;

	std::vector<uintptr_t> instructionAddresses;
	std::vector<DisassembledInstruction> disassembledInstructions;

	std::vector<Function> functions;
	
	enum ids 
	{
		MainWindowID,
		OpenBytesDisassemblerID,
		OpenDataViewerID,
		OpenFileButtonID,
		DisassembleFileButtonID,
		AnalyzeFileButtonID
	};

	void OpenFileButton(wxCommandEvent& e);

	void DisassembleButton(wxCommandEvent& e);

	void AnalyzeButton(wxCommandEvent& e);

	void DisassembleCodeSection(unsigned int numOfBytesToRead);

	void DecompileFunction(unsigned short functionIndex);

	void FindAllFunctions();

	void RightClickOptions(wxGridEvent& e);

	void CloseApp(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};
