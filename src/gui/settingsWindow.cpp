#include "settingsWindow.h"
#include "mainGui.h"

wxBEGIN_EVENT_TABLE(SettingsWindow, wxWindow)
EVT_BUTTON(ApplyButtonID, SettingsWindow::ApplySettings)
wxEND_EVENT_TABLE()

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

	applyButton = new wxButton(this, ApplyButtonID, "Apply", wxPoint(0, 0), wxSize(75, 35));
	applyButton->SetOwnBackgroundColour(foregroundColor);
	applyButton->SetOwnForegroundColour(textColor);

	vSizer = new wxBoxSizer(wxVERTICAL);
	vSizer->Add(auiNotebook, 1, wxEXPAND);
	vSizer->Add(applyButton, 0, wxEXPAND);

	SetSizerAndFit(vSizer);
	SetMinSize(wxSize(200, 200));

	Hide();
}

void SettingsWindow::ApplySettings(wxCommandEvent& e)
{
	mainGui->UpdateSettings();
}

DisassemblerSettingsWindow::DisassemblerSettingsWindow(wxWindow* parent) : wxWindow(parent, wxID_ANY)
{
	SetOwnBackgroundColour(backgroundColor);

	evaluateIP = new wxCheckBox(this, wxID_ANY, "Evaluate instruction pointer");
	evaluateIP->SetOwnForegroundColour(textColor);

	vSizer = new wxBoxSizer(wxVERTICAL);
	vSizer->Add(evaluateIP, 0, wxALL, 10);

	SetSizerAndFit(vSizer);
	SetMinSize(wxSize(200, 200));
}

DecompilerSettingsWindow::DecompilerSettingsWindow(wxWindow* parent) : wxWindow(parent, wxID_ANY)
{
	SetOwnBackgroundColour(backgroundColor);

	useStdInt = new wxCheckBox(this, wxID_ANY, "Use stdint.h types");
	useStdInt->SetOwnForegroundColour(textColor);
	useStdInt->SetValue(true);

	vSizer = new wxBoxSizer(wxVERTICAL);
	vSizer->Add(useStdInt, 0, wxALL, 10);

	SetSizerAndFit(vSizer);
	SetMinSize(wxSize(200, 200));
}