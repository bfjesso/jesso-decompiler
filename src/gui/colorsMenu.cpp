#include "colorsMenu.h"

wxBEGIN_EVENT_TABLE(ColorsMenu, wxFrame)
EVT_CLOSE(ColorsMenu::CloseMenu)
wxEND_EVENT_TABLE()

ColorsMenu::ColorsMenu(wxStyledTextCtrl* textCtrl, wxStyledTextCtrl* textCtrl2) : wxFrame(nullptr, MainWindowID, "Colors Menu", wxPoint(50, 50), wxSize(400, 600))
{
	SetOwnBackgroundColour(backgroundColor);

	disassemblyTextCtrl = textCtrl;
	decompilationTextCtrl = textCtrl2;

	vSizer = new wxBoxSizer(wxVERTICAL);

	for (int i = 0; i < numberOfDecompColors; i++) 
	{
		wxStaticText* label = new wxStaticText(this, wxID_ANY, decompColorNames[i]);
		label->SetOwnForegroundColour(textColor);
		wxColourPickerCtrl* ctrl = new wxColourPickerCtrl(this, wxID_ANY, defaultDecompColors[i], wxPoint(0, 0), wxSize(250, 25));

		vSizer->Add(label, 0, wxCENTER | wxLEFT | wxRIGHT | wxUP, 10);
		vSizer->Add(ctrl, 0, wxCENTER | wxLEFT | wxRIGHT | wxDOWN, 10);

		colorPickerCtrls.push_back(ctrl);
	}

	applyButton = new wxButton(this, ApplyButtonID, "Apply", wxPoint(0, 0), wxSize(250, 25));
	applyButton->SetOwnBackgroundColour(foregroundColor);
	applyButton->SetOwnForegroundColour(textColor);
	applyButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& e) -> void { ApplyColors(disassemblyTextCtrl); ApplyColors(decompilationTextCtrl); });

	vSizer->Add(applyButton, 0, wxCENTER | wxALL, 10);

	SetSizerAndFit(vSizer);

	ApplyColors(disassemblyTextCtrl);
	ApplyColors(decompilationTextCtrl);
}

void ColorsMenu::ApplyColors(wxStyledTextCtrl* ctrl)
{
	for (int i = 0; i < numberOfDecompColors; i++) 
	{
		ctrl->StyleSetForeground(i, colorPickerCtrls[i]->GetColour());
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

void ColorsMenu::CloseMenu(wxCloseEvent& e) // stops this frame from being destroyed and the data being lost
{
	Hide();
}
