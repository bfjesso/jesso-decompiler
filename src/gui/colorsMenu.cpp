#include "colorsMenu.h"

wxBEGIN_EVENT_TABLE(ColorsMenu, wxFrame)
EVT_CLOSE(ColorsMenu::CloseMenu)
wxEND_EVENT_TABLE()

ColorsMenu::ColorsMenu(SettingsWindow* settingsWin) : wxFrame(nullptr, MainWindowID, "Colors Menu", wxPoint(50, 50), wxSize(400, 600))
{
	settingsWindow = settingsWin;

	SetOwnBackgroundColour(backgroundColor);

	disassemblySizer = new wxBoxSizer(wxVERTICAL);
	decompilationSizer = new wxBoxSizer(wxVERTICAL);
	dataSizer = new wxBoxSizer(wxVERTICAL);
	hSizer = new wxBoxSizer(wxHORIZONTAL);
	vSizer = new wxBoxSizer(wxVERTICAL);

	disassemblyScrollWindow = new wxScrolledWindow(this, wxID_ANY, wxPoint(0, 0), wxSize(350, 500), wxVSCROLL | wxFULL_REPAINT_ON_RESIZE);
	disassemblyScrollWindow->SetOwnBackgroundColour(gridColor);
	disassemblyScrollWindow->SetScrollRate(0, 10);

	decompilationScrollWindow = new wxScrolledWindow(this, wxID_ANY, wxPoint(0, 0), wxSize(350, 500), wxVSCROLL | wxFULL_REPAINT_ON_RESIZE);
	decompilationScrollWindow->SetOwnBackgroundColour(gridColor);
	decompilationScrollWindow->SetScrollRate(0, 10);

	dataScrollWindow = new wxScrolledWindow(this, wxID_ANY, wxPoint(0, 0), wxSize(350, 500), wxVSCROLL | wxFULL_REPAINT_ON_RESIZE);
	dataScrollWindow->SetOwnBackgroundColour(gridColor);
	dataScrollWindow->SetScrollRate(0, 10);

	disassemblyLabel = new wxStaticText(disassemblyScrollWindow, wxID_ANY, "Disassembly colors");
	disassemblyLabel->SetOwnForegroundColour(textColor);
	disassemblySizer->Add(disassemblyLabel, 0, wxCENTER | wxLEFT | wxRIGHT | wxUP, 10);

	decompilationLabel = new wxStaticText(decompilationScrollWindow, wxID_ANY, "Decompilation colors");
	decompilationLabel->SetOwnForegroundColour(textColor);
	decompilationSizer->Add(decompilationLabel, 0, wxCENTER | wxLEFT | wxRIGHT | wxUP, 10);

	dataLabel = new wxStaticText(dataScrollWindow, wxID_ANY, "Data colors");
	dataLabel->SetOwnForegroundColour(textColor);
	dataSizer->Add(dataLabel, 0, wxCENTER | wxLEFT | wxRIGHT | wxUP, 10);

	for (int32_t i = 0; i < NUM_OF_DISASSEMBLY_COLORS; i++)
	{
		disassemblyColors[i] = defaultDisassemblyColors[i];
		wxStaticText* label = new wxStaticText(disassemblyScrollWindow, wxID_ANY, disassemblyColorNames[i]);
		label->SetOwnForegroundColour(textColor);
		wxColourPickerCtrl* ctrl = new wxColourPickerCtrl(disassemblyScrollWindow, wxID_ANY, disassemblyColors[i], wxPoint(0, 0), wxSize(250, 25));

		disassemblySizer->Add(label, 0, wxCENTER | wxLEFT | wxRIGHT | wxUP, 10);
		disassemblySizer->Add(ctrl, 0, wxCENTER | wxLEFT | wxRIGHT | wxDOWN, 10);

		disassemblyColorPickerCtrls.push_back(ctrl);
	}

	for (int32_t i = 0; i < NUM_OF_DECOMP_COLORS; i++)
	{
		decompColors[i] = defaultDecompColors[i];
		wxStaticText* label = new wxStaticText(decompilationScrollWindow, wxID_ANY, decompColorNames[i]);
		label->SetOwnForegroundColour(textColor);
		wxColourPickerCtrl* ctrl = new wxColourPickerCtrl(decompilationScrollWindow, wxID_ANY, decompColors[i], wxPoint(0, 0), wxSize(250, 25));

		decompilationSizer->Add(label, 0, wxCENTER | wxLEFT | wxRIGHT | wxUP, 10);
		decompilationSizer->Add(ctrl, 0, wxCENTER | wxLEFT | wxRIGHT | wxDOWN, 10);

		decompilationColorPickerCtrls.push_back(ctrl);
	}

	for (int32_t i = 0; i < NUM_OF_DATA_COLORS; i++)
	{
		dataColors[i] = defaultDataColors[i];
		wxStaticText* label = new wxStaticText(dataScrollWindow, wxID_ANY, dataColorNames[i]);
		label->SetOwnForegroundColour(textColor);
		wxColourPickerCtrl* ctrl = new wxColourPickerCtrl(dataScrollWindow, wxID_ANY, dataColors[i], wxPoint(0, 0), wxSize(250, 25));

		dataSizer->Add(label, 0, wxCENTER | wxLEFT | wxRIGHT | wxUP, 10);
		dataSizer->Add(ctrl, 0, wxCENTER | wxLEFT | wxRIGHT | wxDOWN, 10);

		dataColorPickerCtrls.push_back(ctrl);
	}

	disassemblyScrollWindow->SetSizer(disassemblySizer);
	decompilationScrollWindow->SetSizer(decompilationSizer);
	dataScrollWindow->SetSizer(dataSizer);
	disassemblyScrollWindow->FitInside();
	decompilationScrollWindow->FitInside();
	dataScrollWindow->FitInside();

	hSizer->Add(disassemblyScrollWindow, 1, wxEXPAND | wxALL, 5);
	hSizer->Add(decompilationScrollWindow, 1, wxEXPAND | wxALL, 5);
	hSizer->Add(dataScrollWindow, 1, wxEXPAND | wxALL, 5);

	vSizer->Add(hSizer, 1, wxEXPAND);

	saveAndApplyButton = new wxButton(this, SaveAndApplyButtonID, "Save and apply", wxPoint(0, 0), wxSize(250, 25));
	saveAndApplyButton->SetOwnBackgroundColour(foregroundColor);
	saveAndApplyButton->SetOwnForegroundColour(textColor);
	saveAndApplyButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& e) -> void { SaveAndApplyColors(); });

	restoreDefaultsButton = new wxButton(this, RestoreDefaultsButtonID, "Restore default colors", wxPoint(0, 0), wxSize(250, 25));
	restoreDefaultsButton->SetOwnBackgroundColour(foregroundColor);
	restoreDefaultsButton->SetOwnForegroundColour(textColor);
	restoreDefaultsButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& e) -> void {
		for (int32_t i = 0; i < NUM_OF_DISASSEMBLY_COLORS; i++)
		{
			disassemblyColorPickerCtrls[i]->SetColour(defaultDisassemblyColors[i]);
		}
		for (int32_t i = 0; i < NUM_OF_DECOMP_COLORS; i++)
		{
			decompilationColorPickerCtrls[i]->SetColour(defaultDecompColors[i]);
		}
		for (int32_t i = 0; i < NUM_OF_DATA_COLORS; i++)
		{
			dataColorPickerCtrls[i]->SetColour(defaultDataColors[i]);
		}
		});

	vSizer->Add(saveAndApplyButton, 0, wxCENTER | wxALL, 10);
	vSizer->Add(restoreDefaultsButton, 0, wxCENTER | wxLEFT | wxRIGHT | wxDOWN, 10);

	SetSizerAndFit(vSizer);
}

void ColorsMenu::AddDisassemblyTextCtrl(JdcTextCtrl* ctrl)
{
	disassemblyTextCtrls.push_back(ctrl);
}

void ColorsMenu::AddDecompilationTextCtrl(JdcTextCtrl* ctrl)
{
	decompilationTextCtrls.push_back(ctrl);
}

void ColorsMenu::AddDataTextCtrl(JdcTextCtrl* ctrl)
{
	dataTextCtrls.push_back(ctrl);
}

void ColorsMenu::SaveAndApplyColors()
{
	for (int32_t i = 0; i < NUM_OF_DISASSEMBLY_COLORS; i++)
	{
		disassemblyColors[i] = disassemblyColorPickerCtrls[i]->GetColour();
		for (int32_t j = 0; j < disassemblyTextCtrls.size(); j++)
		{
			disassemblyTextCtrls[j]->StyleSetForeground(i, disassemblyColors[i]);
		}
	}

	for (int32_t i = 0; i < NUM_OF_DECOMP_COLORS; i++)
	{
		decompColors[i] = decompilationColorPickerCtrls[i]->GetColour();
		for (int32_t j = 0; j < decompilationTextCtrls.size(); j++)
		{
			decompilationTextCtrls[j]->StyleSetForeground(i, decompColors[i]);
		}
	}

	for (int32_t i = 0; i < NUM_OF_DATA_COLORS; i++)
	{
		dataColors[i] = dataColorPickerCtrls[i]->GetColour();
		for (int32_t j = 0; j < dataTextCtrls.size(); j++)
		{
			dataTextCtrls[j]->StyleSetForeground(i, dataColors[i]);
		}
	}

	settingsWindow->SavePreferences();
}

void ColorsMenu::ResetColorPickers()
{
	for (int32_t i = 0; i < NUM_OF_DISASSEMBLY_COLORS; i++)
	{
		disassemblyColorPickerCtrls[i]->SetColour(disassemblyColors[i]);
	}

	for (int32_t i = 0; i < NUM_OF_DECOMP_COLORS; i++)
	{
		decompilationColorPickerCtrls[i]->SetColour(decompColors[i]);
	}

	for (int32_t i = 0; i < NUM_OF_DATA_COLORS; i++)
	{
		dataColorPickerCtrls[i]->SetColour(dataColors[i]);
	}
}

void ColorsMenu::OpenMenu(wxPoint position)
{
	position.x += 10;
	position.y += 10;
	SetPosition(position);
	Show();
	Raise();
}

void ColorsMenu::CloseMenu(wxCloseEvent& e)
{
	bool isThereUnsavedChanges = false;

	for (int32_t i = 0; i < NUM_OF_DISASSEMBLY_COLORS; i++)
	{
		if (disassemblyColors[i] != disassemblyColorPickerCtrls[i]->GetColour())
		{
			isThereUnsavedChanges = true;
			break;
		}
	}

	for (int32_t i = 0; !isThereUnsavedChanges && i < NUM_OF_DECOMP_COLORS; i++)
	{
		if (decompColors[i] != decompilationColorPickerCtrls[i]->GetColour())
		{
			isThereUnsavedChanges = true;
			break;
		}
	}

	for (int32_t i = 0; !isThereUnsavedChanges && i < NUM_OF_DATA_COLORS; i++)
	{
		if (dataColors[i] != dataColorPickerCtrls[i]->GetColour())
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
			SaveAndApplyColors();
		}
		else
		{
			ResetColorPickers();
		}
	}

	Hide();
}
