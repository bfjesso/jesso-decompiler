#pragma once
#include "jdcTabArt.h"

class DisassemblerSettingsWindow : public wxWindow
{
public:
	DisassemblerSettingsWindow(wxWindow* parent);

	wxCheckBox* evaluateIP = nullptr;

	wxBoxSizer* vSizer = nullptr;
};

class DecompilerSettingsWindow : public wxWindow
{
public:
	DecompilerSettingsWindow(wxWindow* parent);

	wxCheckBox* useStdInt = nullptr;

	wxBoxSizer* vSizer = nullptr;
};

class MainGui;

class SettingsWindow : public wxWindow
{
public:
	SettingsWindow(wxWindow* parent, MainGui* mainGuiRef);

	MainGui* mainGui = nullptr;

	DisassemblerSettingsWindow* disassemblerSettingsWindow = nullptr;
	DecompilerSettingsWindow* decompilerSettingsWindow = nullptr;

	wxAuiNotebook* auiNotebook = nullptr;
	wxButton* applyButton = nullptr;

	wxBoxSizer* vSizer = nullptr;

	enum ids
	{
		ApplyButtonID
	};

	void ApplySettings(wxCommandEvent& e);

	wxDECLARE_EVENT_TABLE();
};