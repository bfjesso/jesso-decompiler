#pragma once
#include "jdcTabArt.h"

#define JDC_CONFIG_VERSION 1

class DisassemblerSettingsWindow : public wxWindow
{
public:
	DisassemblerSettingsWindow(wxWindow* parent);

	wxCheckBox* evaluateIPCheckBox = nullptr;
	bool evaluateIP = false;

	wxBoxSizer* vSizer = nullptr;
};

class DecompilerSettingsWindow : public wxWindow
{
public:
	DecompilerSettingsWindow(wxWindow* parent);

	wxCheckBox* useStdIntCheckBox = nullptr;
	bool useStdInt = false;

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
	wxButton* saveAndApplyButton = nullptr;

	wxBoxSizer* vSizer = nullptr;

	enum ids
	{
		SaveAndApplyButtonID
	};

	void CheckUnappliedChanges();

	void SaveAndApplySettings();

	void SavePreferences();

	void LoadPreferences();
};