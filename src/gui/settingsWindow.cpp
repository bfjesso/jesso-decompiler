#include "settingsWindow.h"

SettingsWindow::SettingsWindow(wxWindow* parent) : wxWindow(parent, wxID_ANY)
{
	SetName("Settings");
	SetOwnBackgroundColour(backgroundColor);

	disassemblerSettingsWindow = new DisassemblerSettingsWindow(this);
	decompilerSettingsWindow = new DecompilerSettingsWindow(this);

	auiNotebook = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP);
	auiNotebook->SetArtProvider(new JdcTabArt());
	auiNotebook->AddPage(disassemblerSettingsWindow, "Disassembler");
	auiNotebook->AddPage(decompilerSettingsWindow, "Decompiler");

	vSizer = new wxBoxSizer(wxVERTICAL);
	vSizer->Add(auiNotebook, 1, wxEXPAND);

	SetSizerAndFit(vSizer);
	SetMinSize(wxSize(200, 200));

	Hide();
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