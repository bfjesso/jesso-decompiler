#pragma once
#include <wx/wx.h>
#include "../../Disassembler/Headers/disassembler.h"

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

	wxTextCtrl* startDecompAddressTextCtrl = nullptr;
	wxTextCtrl* numOfInstructionsDecompTextCtrl = nullptr;
	wxButton* decompileButton = nullptr;
	wxListBox* decompilationListBox = nullptr;

	wxBoxSizer* row1Sizer = nullptr;
	wxBoxSizer* row2Sizer = nullptr;
	wxBoxSizer* row3Sizer = nullptr;
	wxBoxSizer* row4Sizer = nullptr;
	wxBoxSizer* row5Sizer = nullptr;
	wxBoxSizer* row6Sizer = nullptr;
	wxBoxSizer* vSizer = nullptr;

	HANDLE currentFile = INVALID_HANDLE_VALUE;

	std::vector<uintptr_t> instructionAddresses;
	std::vector<DisassembledInstruction> disassembledInstructions;
	
	enum ids 
	{
		MainWindowID,
		DisassembleTestBytesButtonID,
		OpenFileButtonID,
		DisassembleFileButtonID,
		DecompileButtonID
	};

	void DisassembleTestBytes(wxCommandEvent& e);

	void GetFileHandle(wxCommandEvent& e);

	void DisassembleCodeSection(wxCommandEvent& e);

	void DecompileInstructions(wxCommandEvent& e);

	DisassembledInstruction* GetInstructionAtAddress(uintptr_t address, int low, int high);

	bool ParseStringBytes(wxString str, unsigned char* bytesBuffer, unsigned char bytesBufferLen);

	void CloseApp(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};