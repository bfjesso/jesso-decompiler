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

	std::vector<struct DisassembledInstruction> instructions;

	enum ids
	{
		DisassembleButtonID
	};

	void DisassembleBytes(wxCommandEvent& e);

	enum JdcStatus ParseStringBytes(wxString str, std::vector<uint8_t>& bytesBuffer);

	wxDECLARE_EVENT_TABLE();
};
