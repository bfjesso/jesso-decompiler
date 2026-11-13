#pragma once
#include "jdcTabArt.h"

#define JDC_CONFIG_VERSION 1

#define NUM_OF_DISASSEMBLER_SETTINGS 2
#define NUM_OF_DECOMPILER_SETTINGS 1

enum DisassemblerSetting
{
	EVALUATE_IP_SETTING,
	SHOW_INSTRUCTION_BYTES_SETTING
};

enum DecompilerSetting
{
	USE_STD_INT_SETTING
};

class DisassemblerSettingsWindow : public wxWindow
{
public:
	DisassemblerSettingsWindow(wxWindow* parent);

	std::vector<wxCheckBox*> checkBoxes;
	std::vector<bool> settings;

	wxBoxSizer* vSizer = nullptr;

	const char* settingStrs[NUM_OF_DISASSEMBLER_SETTINGS] =
	{
		"Evaluate instruction pointer",
		"Show instruction bytes"
	};

	const bool defaultSettingValues[NUM_OF_DISASSEMBLER_SETTINGS] =
	{
		false,
		false
	};
};

class DecompilerSettingsWindow : public wxWindow
{
public:
	DecompilerSettingsWindow(wxWindow* parent);

	std::vector<wxCheckBox*> checkBoxes;
	std::vector<bool> settings;

	wxBoxSizer* vSizer = nullptr;

	const char* settingStrs[NUM_OF_DECOMPILER_SETTINGS] =
	{
		"Use stdint data types"
	};

	const bool defaultSettingValues[NUM_OF_DECOMPILER_SETTINGS] =
	{
		true
	};
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