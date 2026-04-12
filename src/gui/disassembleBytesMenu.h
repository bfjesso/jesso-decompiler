#pragma once
#include "guiUtils.h"

class BytesDisassembler : public wxFrame, public Utils
{
public:
	BytesDisassembler(wxWindow* parent, wxPoint position);

	wxTextCtrl* bytesTextCtrl = nullptr;
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

	void DisassembleBytes(wxCommandEvent& e);

	int ParseStringBytes(wxString str, unsigned char* bytesBuffer, unsigned char bytesBufferLen);

	wxDECLARE_EVENT_TABLE();
};
