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

	applyButton = new wxButton(this, ApplyButtonID, "Apply and save", wxPoint(0, 0), wxSize(75, 35));
	applyButton->SetOwnBackgroundColour(foregroundColor);
	applyButton->SetOwnForegroundColour(textColor);
	applyButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& e) -> void { ApplySettings(); });

	vSizer = new wxBoxSizer(wxVERTICAL);
	vSizer->Add(auiNotebook, 1, wxEXPAND);
	vSizer->Add(applyButton, 0, wxEXPAND);

	SetSizerAndFit(vSizer);
	SetMinSize(wxSize(200, 200));

	Hide();
}

void SettingsWindow::CheckUnappliedChanges()
{
	if (disassemblerSettingsWindow->evaluateIP != disassemblerSettingsWindow->evaluateIPCheckBox->GetValue() || 
		decompilerSettingsWindow->useStdInt != decompilerSettingsWindow->useStdIntCheckBox->GetValue())
	{
		int32_t saveAnswer = wxMessageBox("You have unsaved changes. Do you want to apply them?", "Save changes?", wxYES_NO, this);
		if (saveAnswer == wxYES)
		{
			ApplySettings();
		}
		else 
		{
			disassemblerSettingsWindow->evaluateIPCheckBox->SetValue(disassemblerSettingsWindow->evaluateIP);
			decompilerSettingsWindow->useStdIntCheckBox->SetValue(decompilerSettingsWindow->useStdInt);
		}
	}
}

void SettingsWindow::ApplySettings()
{
	disassemblerSettingsWindow->evaluateIP = disassemblerSettingsWindow->evaluateIPCheckBox->GetValue();
	decompilerSettingsWindow->useStdInt = decompilerSettingsWindow->useStdIntCheckBox->GetValue();
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

	cfg.Write("/disassembler/evaluate-instruction-pointer", disassemblerSettingsWindow->evaluateIP);

	cfg.Write("/decompiler/use-stdint", decompilerSettingsWindow->useStdInt);

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

	bool evaluateIP = false;
	cfg.Read("/disassembler/evaluate-instruction-pointer", &evaluateIP, true);
	disassemblerSettingsWindow->evaluateIPCheckBox->SetValue(evaluateIP);

	bool useStdInt = false;
	cfg.Read("/decompiler/use-stdint", &useStdInt, true);
	decompilerSettingsWindow->useStdIntCheckBox->SetValue(useStdInt);

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

	mainGui->colorsMenu->ApplyColors();

	int32_t configVersion = 1;
	cfg.Read("/config-version", &configVersion, 1);

	if (configVersion != JDC_CONFIG_VERSION)
	{
		wxMessageBox("Preferences may not have been loaded correctly", "Outdated config version");
	}

	ApplySettings();
}

DisassemblerSettingsWindow::DisassemblerSettingsWindow(wxWindow* parent) : wxWindow(parent, wxID_ANY)
{
	SetOwnBackgroundColour(backgroundColor);

	evaluateIPCheckBox = new wxCheckBox(this, wxID_ANY, "Evaluate instruction pointer");
	evaluateIPCheckBox->SetOwnForegroundColour(textColor);

	vSizer = new wxBoxSizer(wxVERTICAL);
	vSizer->Add(evaluateIPCheckBox, 0, wxALL, 10);

	SetSizerAndFit(vSizer);
	SetMinSize(wxSize(200, 200));
}

DecompilerSettingsWindow::DecompilerSettingsWindow(wxWindow* parent) : wxWindow(parent, wxID_ANY)
{
	SetOwnBackgroundColour(backgroundColor);

	useStdIntCheckBox = new wxCheckBox(this, wxID_ANY, "Use stdint.h types");
	useStdIntCheckBox->SetOwnForegroundColour(textColor);
	useStdIntCheckBox->SetValue(true);

	vSizer = new wxBoxSizer(wxVERTICAL);
	vSizer->Add(useStdIntCheckBox, 0, wxALL, 10);

	SetSizerAndFit(vSizer);
	SetMinSize(wxSize(200, 200));
}