#pragma once
#include "guiUtils.h"
#include "settingsWindow.h"

class BytesDisassemblerWindow : public wxWindow
{
public:
	BytesDisassemblerWindow(wxWindow* parent, SettingsWindow* settings);

	SettingsWindow* settingsWindow = nullptr;

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

	int32_t ParseStringBytes(wxString str, uint8_t* bytesBuffer, uint8_t bytesBufferLen);

	wxDECLARE_EVENT_TABLE();
};
