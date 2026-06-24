#pragma once
#include "guiUtils.h"

class BytesDisassemblerWindow : public wxWindow
{
public:
	BytesDisassemblerWindow(wxWindow* parent);

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
		DisassembleButtonID
	};

	void DisassembleBytes(wxCommandEvent& e);

	int ParseStringBytes(wxString str, unsigned char* bytesBuffer, unsigned char bytesBufferLen);

	wxDECLARE_EVENT_TABLE();
};
