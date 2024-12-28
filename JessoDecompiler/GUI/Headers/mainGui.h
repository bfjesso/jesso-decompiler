#pragma once
#include "../Headers/guiUtils.h"
#include <wx/grid.h>
#include "../Headers/disassembleBytesMenu.h"
#include "../../Disassembler/Headers/disassembler.h"
#include "../../Decompiler/Headers/decompiler.h"

class MainGui : public wxFrame, public Utils
{
public:
	MainGui();

	wxMenuBar* menuBar = nullptr;
	BytesDisassembler* bytesDisassemblerMenu = nullptr;

	wxButton* openFileButton = nullptr;
	wxTextCtrl* numOfbytesInputTextCtrl = nullptr;
	wxButton* disassembleFileButton = nullptr;
	wxListBox* disassemblyListBox = nullptr;

	wxListBox* decompilationListBox = nullptr;

	wxGrid* functionsGrid = nullptr;

	wxBoxSizer* row1Sizer = nullptr;
	wxBoxSizer* row2Sizer = nullptr;
	wxBoxSizer* row3Sizer = nullptr;
	wxBoxSizer* row4Sizer = nullptr;
	wxBoxSizer* vSizer = nullptr;

	wxString currentFilePath;
	uintptr_t imageBase;

	std::vector<uintptr_t> instructionAddresses;
	std::vector<DisassembledInstruction> disassembledInstructions;

	std::vector<Function> functions;
	
	enum ids 
	{
		MainWindowID,
		OpenBytesDisassemblerID,
		OpenFileButtonID,
		DisassembleFileButtonID,
		DecompileButtonID
	};

	void GetFilePath(wxCommandEvent& e);

	void DisassembleCodeSection(wxCommandEvent& e);

	void DecompileFunction(Function* function, const char* name);

	void FindAllFunctions();

	void RightClickOptions(wxGridEvent& e);

	void CloseApp(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};