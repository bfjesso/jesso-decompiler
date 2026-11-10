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
	wxChoice* dataInputTypeChoice = nullptr;

	wxButton* disassembleButton = nullptr;
	wxChoice* isaChoice = nullptr;
	wxCheckBox* is64BitModeCheckBox = nullptr;
	DisassemblyTextCtrl* disassemblyTextCtrl = nullptr;

	wxBoxSizer* row1Sizer = nullptr;
	wxBoxSizer* row2Sizer = nullptr;
	wxBoxSizer* vSizer = nullptr;

	enum InstructionSetArchitecture isa = X86_ISA;

	std::vector<struct DisassembledInstruction> instructions;

	enum ids
	{
		DisassembleButtonID,
		ISAChoiceID,
	};

	enum DataInputType
	{
		HEX_DIT,
		BINARY_DIT,
	};

	const char* dataInputTypeStrs[2]
	{
		"Hexadecimal",
		"Binary",
	};

	const char* isaStrs[2]
	{
		"x86",
		"LC-3",
	};

	void DisassembleBytes(wxCommandEvent& e);

	enum JdcStatus ParseStringBytes(wxString str, std::vector<uint8_t>& bytesBuffer);

	void UpdateISA(wxCommandEvent& e);

	wxDECLARE_EVENT_TABLE();
};
