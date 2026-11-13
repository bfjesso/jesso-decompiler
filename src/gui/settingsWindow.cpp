#include "settingsWindow.h"
#include "mainGui.h"

#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/fileconf.h>
#include <wx/filefn.h>

SettingsWindow::SettingsWindow(wxWindow* parent, MainGui* mainGuiRef) : wxWindow(parent, wxID_ANY)
{
	SetName("Settings");
	SetOwnBackgroundColour(backgroundColor);

	mainGui = mainGuiRef;

	disassemblerSettingsWindow = new DisassemblerSettingsWindow(this);
	decompilerSettingsWindow = new DecompilerSettingsWindow(this);

	auiNotebook = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP);
	auiNotebook->SetArtProvider(new JdcTabArt());
	auiNotebook->AddPage(disassemblerSettingsWindow, "Disassembler");
	auiNotebook->AddPage(decompilerSettingsWindow, "Decompiler");

	saveAndApplyButton = new wxButton(this, SaveAndApplyButtonID, "Save and apply", wxPoint(0, 0), wxSize(75, 35));
	saveAndApplyButton->SetOwnBackgroundColour(foregroundColor);
	saveAndApplyButton->SetOwnForegroundColour(textColor);
	saveAndApplyButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& e) -> void { SaveAndApplySettings(); });

	vSizer = new wxBoxSizer(wxVERTICAL);
	vSizer->Add(auiNotebook, 1, wxEXPAND);
	vSizer->Add(saveAndApplyButton, 0, wxEXPAND);

	SetSizerAndFit(vSizer);
	SetMinSize(wxSize(200, 200));

	Hide();
}

void SettingsWindow::CheckUnappliedChanges()
{
	bool isThereUnsavedChanges = false;
	for (int32_t i = 0; i < NUM_OF_DISASSEMBLER_SETTINGS; i++)
	{
		if (disassemblerSettingsWindow->settings[i] != disassemblerSettingsWindow->checkBoxes[i]->GetValue())
		{
			isThereUnsavedChanges = true;
			break;
		}

	}
	for (int32_t i = 0; !isThereUnsavedChanges && i < NUM_OF_DECOMPILER_SETTINGS; i++)
	{
		if (decompilerSettingsWindow->settings[i] != decompilerSettingsWindow->checkBoxes[i]->GetValue())
		{
			isThereUnsavedChanges = true;
			break;
		}
	}

	if (isThereUnsavedChanges)
	{
		int32_t saveAnswer = wxMessageBox("You have unsaved changes. Do you want to save and apply them?", "Save changes?", wxYES_NO, this);
		if (saveAnswer == wxYES)
		{
			SaveAndApplySettings();
		}
		else
		{
			for (int32_t i = 0; i < NUM_OF_DISASSEMBLER_SETTINGS; i++)
			{
				disassemblerSettingsWindow->checkBoxes[i]->SetValue(disassemblerSettingsWindow->settings[i]);
			}
			for (int32_t i = 0; i < NUM_OF_DECOMPILER_SETTINGS; i++)
			{
				decompilerSettingsWindow->checkBoxes[i]->SetValue(decompilerSettingsWindow->settings[i]);
			}
		}
	}
}

void SettingsWindow::SaveAndApplySettings()
{
	for (int32_t i = 0; i < NUM_OF_DISASSEMBLER_SETTINGS; i++)
	{
		disassemblerSettingsWindow->settings[i] = disassemblerSettingsWindow->checkBoxes[i]->GetValue();
	}
	for (int32_t i = 0; i < NUM_OF_DECOMPILER_SETTINGS; i++)
	{
		decompilerSettingsWindow->settings[i] = decompilerSettingsWindow->checkBoxes[i]->GetValue();
	}

	mainGui->UpdateSettings();
	SavePreferences();
}

static wxString GetConfigPath()
{
	wxString cfgDir = wxStandardPaths::Get().GetUserConfigDir() + wxFileName::GetPathSeparator() + "jesso-decompiler";
	wxFileName::Mkdir(cfgDir, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
	return cfgDir + wxFileName::GetPathSeparator() + "jdc-config.ini";
}

void SettingsWindow::SavePreferences()
{
	wxString path = GetConfigPath();
	wxString tmp = path + ".tmp";

	wxFileConfig cfg("Jesso Decompiler", "bfjesso", tmp, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);

	cfg.Write("/gui/fullscreen", mainGui->IsMaximized());
	cfg.Write("/gui/window-width", mainGui->GetSize().GetWidth());
	cfg.Write("/gui/window-height", mainGui->GetSize().GetHeight());

	for (int32_t i = 0; i < NUM_OF_DISASSEMBLER_SETTINGS; i++)
	{
		disassemblerSettingsWindow->settings[i] = disassemblerSettingsWindow->checkBoxes[i]->GetValue();
		wxString settingName = wxString(disassemblerSettingsWindow->settingStrs[i]);
		settingName.MakeLower().Replace(" ", "-");

		cfg.Write("/disassembler/" + settingName, (bool)(disassemblerSettingsWindow->settings[i]));
	}
	for (int32_t i = 0; i < NUM_OF_DECOMPILER_SETTINGS; i++)
	{
		decompilerSettingsWindow->settings[i] = decompilerSettingsWindow->checkBoxes[i]->GetValue();
		wxString settingName = wxString(decompilerSettingsWindow->settingStrs[i]);
		settingName.MakeLower().Replace(" ", "-");

		cfg.Write("/decompiler/" + settingName, (bool)(decompilerSettingsWindow->settings[i]));
	}

	for (int32_t i = 0; i < NUM_OF_DISASSEMBLY_COLORS; i++)
	{
		wxString colorName = wxString(mainGui->colorsMenu->disassemblyColorNames[i]);
		colorName.MakeLower().Replace(" ", "-");

		cfg.Write("/colors/disassembly/" + colorName, mainGui->colorsMenu->disassemblyColors[i]);
	}
	for (int32_t i = 0; i < NUM_OF_DECOMP_COLORS; i++)
	{
		wxString colorName = wxString(mainGui->colorsMenu->decompColorNames[i]);
		colorName.MakeLower().Replace(" ", "-");

		cfg.Write("/colors/decompilation/" + colorName, mainGui->colorsMenu->decompColors[i]);
	}
	for (int32_t i = 0; i < NUM_OF_DATA_COLORS; i++)
	{
		wxString colorName = wxString(mainGui->colorsMenu->dataColorNames[i]);
		colorName.MakeLower().Replace(" ", "-");

		cfg.Write("/colors/data/" + colorName, mainGui->colorsMenu->dataColors[i]);
	}

	cfg.Write("/config-version", 1);
	cfg.Flush();

	if (!wxRenameFile(tmp, path, true))
	{
		wxRemoveFile(tmp);
		wxMessageBox("An error occured when attempting to overwrite config file", "Error saving preferences");
	}
}

void SettingsWindow::LoadPreferences()
{
	wxString path = GetConfigPath();
	wxFileConfig cfg("Jesso Decompiler", "bfjesso", path, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);

	int32_t w = 1980;
	int32_t h = 1080;
	bool fullscreen = true;
	cfg.Read("/gui/fullscreen", &fullscreen, true);
	cfg.Read("/gui/window-width", &w, 1980);
	cfg.Read("/gui/window-height", &h, 1080);

	mainGui->SetSize(w, h);
	if (fullscreen)
	{
		mainGui->Maximize();
	}

	for (int32_t i = 0; i < NUM_OF_DISASSEMBLER_SETTINGS; i++)
	{
		wxString settingName = wxString(disassemblerSettingsWindow->settingStrs[i]);
		settingName.MakeLower().Replace(" ", "-");

		bool setting = disassemblerSettingsWindow->defaultSettingValues[i];
		cfg.Read("/disassembler/" + settingName, &setting, disassemblerSettingsWindow->defaultSettingValues[i]);
		disassemblerSettingsWindow->checkBoxes[i]->SetValue(setting);
		disassemblerSettingsWindow->settings[i] = setting;
	}
	for (int32_t i = 0; i < NUM_OF_DECOMPILER_SETTINGS; i++)
	{
		wxString settingName = wxString(decompilerSettingsWindow->settingStrs[i]);
		settingName.MakeLower().Replace(" ", "-");

		bool setting = decompilerSettingsWindow->defaultSettingValues[i];
		cfg.Read("/decompiler/" + settingName, &setting, decompilerSettingsWindow->defaultSettingValues[i]);
		decompilerSettingsWindow->checkBoxes[i]->SetValue(setting);
		decompilerSettingsWindow->settings[i] = setting;
	}

	for (int32_t i = 0; i < NUM_OF_DISASSEMBLY_COLORS; i++)
	{
		wxColour color;
		wxString colorName = wxString(mainGui->colorsMenu->disassemblyColorNames[i]);
		colorName.MakeLower().Replace(" ", "-");

		cfg.Read("/colors/disassembly/" + colorName, &color, mainGui->colorsMenu->disassemblyColors[i]);
		mainGui->colorsMenu->disassemblyColorPickerCtrls[i]->SetColour(color);
	}
	for (int32_t i = 0; i < NUM_OF_DECOMP_COLORS; i++)
	{
		wxColour color;
		wxString colorName = wxString(mainGui->colorsMenu->decompColorNames[i]);
		colorName.MakeLower().Replace(" ", "-");

		cfg.Read("/colors/decompilation/" + colorName, &color, mainGui->colorsMenu->decompColors[i]);
		mainGui->colorsMenu->decompilationColorPickerCtrls[i]->SetColour(color);
	}
	for (int32_t i = 0; i < NUM_OF_DATA_COLORS; i++)
	{
		wxColour color;
		wxString colorName = wxString(mainGui->colorsMenu->dataColorNames[i]);
		colorName.MakeLower().Replace(" ", "-");

		cfg.Read("/colors/data/" + colorName, &color, mainGui->colorsMenu->dataColors[i]);
		mainGui->colorsMenu->dataColorPickerCtrls[i]->SetColour(color);
	}

	int32_t configVersion = 1;
	cfg.Read("/config-version", &configVersion, 1);

	if (configVersion != JDC_CONFIG_VERSION)
	{
		wxMessageBox("Preferences may not have been loaded correctly", "Outdated config version");
	}

	mainGui->colorsMenu->SaveAndApplyColors();
	SaveAndApplySettings();
}

DisassemblerSettingsWindow::DisassemblerSettingsWindow(wxWindow* parent) : wxWindow(parent, wxID_ANY)
{
	SetOwnBackgroundColour(backgroundColor);

	vSizer = new wxBoxSizer(wxVERTICAL);

	for (int32_t i = 0; i < NUM_OF_DISASSEMBLER_SETTINGS; i++)
	{
		checkBoxes.push_back(new wxCheckBox(this, wxID_ANY, settingStrs[i]));
		checkBoxes[i]->SetOwnForegroundColour(textColor);
		checkBoxes[i]->SetValue(defaultSettingValues[i]);
		settings.push_back(defaultSettingValues[i]);

		vSizer->Add(checkBoxes[i], 0, wxALL, 10);
	}

	SetSizerAndFit(vSizer);
	SetMinSize(wxSize(200, 200));
}

DecompilerSettingsWindow::DecompilerSettingsWindow(wxWindow* parent) : wxWindow(parent, wxID_ANY)
{
	SetOwnBackgroundColour(backgroundColor);

	vSizer = new wxBoxSizer(wxVERTICAL);

	for (int32_t i = 0; i < NUM_OF_DECOMPILER_SETTINGS; i++)
	{
		checkBoxes.push_back(new wxCheckBox(this, wxID_ANY, settingStrs[i]));
		checkBoxes[i]->SetOwnForegroundColour(textColor);
		checkBoxes[i]->SetValue(defaultSettingValues[i]);
		settings.push_back(defaultSettingValues[i]);

		vSizer->Add(checkBoxes[i], 0, wxALL, 10);
	}

	SetSizerAndFit(vSizer);
	SetMinSize(wxSize(200, 200));
}