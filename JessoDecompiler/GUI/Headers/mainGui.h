#pragma once
#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/clipbrd.h>
#include "../../Disassembler/Headers/disassembler.h"
#include "../../Decompiler/Headers/decompiler.h"

const wxColour backgroundColor = wxColour(35, 35, 35);
const wxColour foregroundColor = wxColour(60, 60, 60);
const wxColour textColor = wxColour(220, 220, 220);

class MainGui : public wxFrame
{
public:
	MainGui();

	wxTextCtrl* testBytesTextCtrl = nullptr;
	wxButton* disassembleTestBytesButton = nullptr;
	wxStaticText* testDisassemblyStaticText = nullptr;

	wxButton* openFileButton = nullptr;
	wxTextCtrl* numOfbytesInputTextCtrl = nullptr;
	wxButton* disassembleFileButton = nullptr;
	wxCheckBox* is64BitModeCheckBox = nullptr;
	wxListBox* disassemblyListBox = nullptr;

	wxListBox* decompilationListBox = nullptr;

	wxGrid* functionsGrid = nullptr;

	wxBoxSizer* row1Sizer = nullptr;
	wxBoxSizer* row2Sizer = nullptr;
	wxBoxSizer* row3Sizer = nullptr;
	wxBoxSizer* row4Sizer = nullptr;
	wxBoxSizer* row5Sizer = nullptr;
	wxBoxSizer* row6Sizer = nullptr;
	wxBoxSizer* row7Sizer = nullptr;
	wxBoxSizer* vSizer = nullptr;

	wxString currentFilePath;
	uintptr_t imageBase;

	std::vector<uintptr_t> instructionAddresses;
	std::vector<DisassembledInstruction> disassembledInstructions;

	std::vector<Function> functions;
	
	enum ids 
	{
		MainWindowID,
		DisassembleTestBytesButtonID,
		OpenFileButtonID,
		DisassembleFileButtonID,
		DecompileButtonID
	};

	void DisassembleTestBytes(wxCommandEvent& e);

	void GetFilePath(wxCommandEvent& e);

	void DisassembleCodeSection(wxCommandEvent& e);

	void DecompileFunction(Function* function);

	void FindAllFunctions();

	void RightClickOptions(wxGridEvent& e);

	bool ParseStringBytes(wxString str, unsigned char* bytesBuffer, unsigned char bytesBufferLen);

	void CopyToClipboard(const char* txt);

	void CloseApp(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};