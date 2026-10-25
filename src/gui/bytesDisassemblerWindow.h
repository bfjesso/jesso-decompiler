#pragma once
#include "guiUtils.h"
#include "disassemblyTextCtrl.h"
#include "../disassembler/disassemblyStructs.h"

class MainGui;

class BytesDisassemblerWindow : public wxWindow
{
public:
	BytesDisassemblerWindow(wxWindow* parent, MainGui* mainGuiRef);

	MainGui* mainGui = nullptr;

	wxTextCtrl* bytesTextCtrl = nullptr;
	wxButton* disassembleButton = nullptr;
	wxCheckBox* is64BitModeCheckBox = nullptr;
	DisassemblyTextCtrl* disassemblyTextCtrl = nullptr;

	wxBoxSizer* row1Sizer = nullptr;
	wxBoxSizer* row2Sizer = nullptr;
	wxBoxSizer* vSizer = nullptr;

	struct DisassembledInstruction currentInstruction;

	enum ids
	{
		DisassembleButtonID
	};

	void DisassembleBytes(wxCommandEvent& e);

	int32_t ParseStringBytes(wxString str, uint8_t* bytesBuffer, uint8_t bytesBufferLen);

	wxDECLARE_EVENT_TABLE();
};
