#pragma once
#include <wx/wx.h>

const wxColour backgroundColor = wxColour(35, 35, 35);
const wxColour foregroundColor = wxColour(60, 60, 60);
const wxColour textColor = wxColour(220, 220, 220);

class MainGui : public wxFrame
{
public:
	MainGui();

	wxTextCtrl* bytesInputTextCtrl = nullptr;
	wxButton* disassembleButton = nullptr;
	wxCheckBox* is64BitModeCheckBox = nullptr;
	wxStaticText* disassemblyStaticText = nullptr;

	wxBoxSizer* row1Sizer = nullptr;
	wxBoxSizer* row2Sizer = nullptr;
	wxBoxSizer* row3Sizer = nullptr;
	wxBoxSizer* vSizer = nullptr;
	
	enum ids 
	{
		MainWindowID,
		DisassembleButtonID
	};

	void DisassembleBytesInput(wxCommandEvent& e);

	bool ParseStringBytes(wxString str, unsigned char* bytesBuffer, unsigned char bytesBufferLen);

	void CloseApp(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};