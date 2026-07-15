#pragma once
#include "guiUtils.h"
#include "disassemblyTextCtrl.h"

class MainGui;

class CodeReferencesWindow : public wxWindow
{
public:
	CodeReferencesWindow(MainGui* parent);

	MainGui* mainGui = nullptr;

	wxCheckBox* hexCheckBox = nullptr;
	wxTextCtrl* valueTextCtrl = nullptr;
	wxButton* findReferencesButton = nullptr;
	DisassemblyTextCtrl* disassemblyTextCtrl = nullptr;

	wxBoxSizer* row1Sizer = nullptr;
	wxBoxSizer* vSizer = nullptr;

	std::vector<struct DisassembledInstruction> foundInstructions;

	enum ids
	{
		FindReferencesID
	};

	void OnFindCodeReferencesButton(wxCommandEvent& e);

	void FindCodeReferences(long long value, unsigned char isHex);

	wxDECLARE_EVENT_TABLE();
};