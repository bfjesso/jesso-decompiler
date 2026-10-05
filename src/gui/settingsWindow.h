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

class SettingsWindow : public wxWindow
{
public:
	SettingsWindow(wxWindow* parent);

	DisassemblerSettingsWindow* disassemblerSettingsWindow = nullptr;
	DecompilerSettingsWindow* decompilerSettingsWindow = nullptr;

	wxAuiNotebook* auiNotebook = nullptr;

	wxBoxSizer* vSizer = nullptr;
};