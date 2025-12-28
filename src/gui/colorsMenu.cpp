#include "colorsMenu.h"

wxBEGIN_EVENT_TABLE(ColorsMenu, wxFrame)
EVT_CLOSE(ColorsMenu::CloseMenu)
wxEND_EVENT_TABLE()

ColorsMenu::ColorsMenu(wxStyledTextCtrl* textCtrl) : wxFrame(nullptr, MainWindowID, "Colors Menu", wxPoint(50, 50), wxSize(400, 600))
{
	SetOwnBackgroundColour(backgroundColor);

	decompilationTextCtrl = textCtrl;

	operatorColorLabel = new wxStaticText(this, wxID_ANY, "Operators");
	operatorColorLabel->SetOwnForegroundColour(textColor);
	operatorColorPickerCtrl = new wxColourPickerCtrl(this, wxID_ANY, wxColour(180, 180, 180), wxPoint(0, 0), wxSize(250, 25));

	localVarColorLabel = new wxStaticText(this, wxID_ANY, "Local variables");
	localVarColorLabel->SetOwnForegroundColour(textColor);
	localVarColorPickerCtrl = new wxColourPickerCtrl(this, wxID_ANY, wxColour(156, 220, 254), wxPoint(0, 0), wxSize(250, 25));

	argumentColorLabel = new wxStaticText(this, wxID_ANY, "Arguments");
	argumentColorLabel->SetOwnForegroundColour(textColor);
	argumentColorPickerCtrl = new wxColourPickerCtrl(this, wxID_ANY, wxColour(154, 154, 154), wxPoint(0, 0), wxSize(250, 25));

	functionColorLabel = new wxStaticText(this, wxID_ANY, "Functions");
	functionColorLabel->SetOwnForegroundColour(textColor);
	functionColorPickerCtrl = new wxColourPickerCtrl(this, wxID_ANY, wxColour(220, 220, 170), wxPoint(0, 0), wxSize(250, 25));

	importColorLabel = new wxStaticText(this, wxID_ANY, "Imports");
	importColorLabel->SetOwnForegroundColour(textColor);
	importColorPickerCtrl = new wxColourPickerCtrl(this, wxID_ANY, wxColour(190, 183, 255), wxPoint(0, 0), wxSize(250, 25));

	primitiveTypeColorLabel = new wxStaticText(this, wxID_ANY, "Primitive types");
	primitiveTypeColorLabel->SetOwnForegroundColour(textColor);
	primitiveTypeColorPickerCtrl = new wxColourPickerCtrl(this, wxID_ANY, wxColour(86, 156, 214), wxPoint(0, 0), wxSize(250, 25));

	keywordColorLabel = new wxStaticText(this, wxID_ANY, "Keywords");
	keywordColorLabel->SetOwnForegroundColour(textColor);
	keywordColorPickerCtrl = new wxColourPickerCtrl(this, wxID_ANY, wxColour(216, 160, 223), wxPoint(0, 0), wxSize(250, 25));

	stringColorLabel = new wxStaticText(this, wxID_ANY, "Strings");
	stringColorLabel->SetOwnForegroundColour(textColor);
	stringColorPickerCtrl = new wxColourPickerCtrl(this, wxID_ANY, wxColour(232, 201, 187), wxPoint(0, 0), wxSize(250, 25));

	numberColorLabel = new wxStaticText(this, wxID_ANY, "Numbers");
	numberColorLabel->SetOwnForegroundColour(textColor);
	numberColorPickerCtrl = new wxColourPickerCtrl(this, wxID_ANY, wxColour(181, 206, 168), wxPoint(0, 0), wxSize(250, 25));

	applyButton = new wxButton(this, ApplyButtonID, "Apply", wxPoint(0, 0), wxSize(250, 25));
	applyButton->SetOwnBackgroundColour(foregroundColor);
	applyButton->SetOwnForegroundColour(textColor);
	applyButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& e) -> void { ApplyColors(); });

	vSizer = new wxBoxSizer(wxVERTICAL);

	vSizer->Add(operatorColorLabel, 0, wxCENTER | wxLEFT | wxRIGHT | wxUP, 10);
	vSizer->Add(operatorColorPickerCtrl, 0, wxCENTER | wxLEFT | wxRIGHT | wxDOWN, 10);

	vSizer->Add(localVarColorLabel, 0, wxCENTER | wxLEFT | wxRIGHT | wxUP, 10);
	vSizer->Add(localVarColorPickerCtrl, 0, wxCENTER | wxLEFT | wxRIGHT | wxDOWN, 10);

	vSizer->Add(argumentColorLabel, 0, wxCENTER | wxLEFT | wxRIGHT | wxUP, 10);
	vSizer->Add(argumentColorPickerCtrl, 0, wxCENTER | wxLEFT | wxRIGHT | wxDOWN, 10);

	vSizer->Add(functionColorLabel, 0, wxCENTER | wxLEFT | wxRIGHT | wxUP, 10);
	vSizer->Add(functionColorPickerCtrl, 0, wxCENTER | wxLEFT | wxRIGHT | wxDOWN, 10);

	vSizer->Add(importColorLabel, 0, wxCENTER | wxLEFT | wxRIGHT | wxUP, 10);
	vSizer->Add(importColorPickerCtrl, 0, wxCENTER | wxLEFT | wxRIGHT | wxDOWN, 10);

	vSizer->Add(primitiveTypeColorLabel, 0, wxCENTER | wxLEFT | wxRIGHT | wxUP, 10);
	vSizer->Add(primitiveTypeColorPickerCtrl, 0, wxCENTER | wxLEFT | wxRIGHT | wxDOWN, 10);

	vSizer->Add(keywordColorLabel, 0, wxCENTER | wxLEFT | wxRIGHT | wxUP, 10);
	vSizer->Add(keywordColorPickerCtrl, 0, wxCENTER | wxLEFT | wxRIGHT | wxDOWN, 10);

	vSizer->Add(stringColorLabel, 0, wxCENTER | wxLEFT | wxRIGHT | wxUP, 10);
	vSizer->Add(stringColorPickerCtrl, 0, wxCENTER | wxLEFT | wxRIGHT | wxDOWN, 10);

	vSizer->Add(numberColorLabel, 0, wxCENTER | wxLEFT | wxRIGHT | wxUP, 10);
	vSizer->Add(numberColorPickerCtrl, 0, wxCENTER | wxLEFT | wxRIGHT | wxDOWN, 10);

	vSizer->Add(applyButton, 0, wxCENTER | wxALL, 10);

	SetSizerAndFit(vSizer);

	ApplyColors();
}

void ColorsMenu::ApplyColors()
{
	if (decompilationTextCtrl) 
	{
		decompilationTextCtrl->StyleSetForeground(ColorsMenu::OPERATOR_COLOR, operatorColorPickerCtrl->GetColour());
		decompilationTextCtrl->StyleSetForeground(ColorsMenu::LOCAL_VAR_COLOR, localVarColorPickerCtrl->GetColour());
		decompilationTextCtrl->StyleSetForeground(ColorsMenu::ARGUMENT_COLOR, argumentColorPickerCtrl->GetColour());
		decompilationTextCtrl->StyleSetForeground(ColorsMenu::FUNCTION_COLOR, functionColorPickerCtrl->GetColour());
		decompilationTextCtrl->StyleSetForeground(ColorsMenu::IMPORT_COLOR, importColorPickerCtrl->GetColour());
		decompilationTextCtrl->StyleSetForeground(ColorsMenu::PRIMITIVE_COLOR, primitiveTypeColorPickerCtrl->GetColour());
		decompilationTextCtrl->StyleSetForeground(ColorsMenu::KEYWORD_COLOR, keywordColorPickerCtrl->GetColour());
		decompilationTextCtrl->StyleSetForeground(ColorsMenu::STRING_COLOR, stringColorPickerCtrl->GetColour());
		decompilationTextCtrl->StyleSetForeground(ColorsMenu::NUMBER_COLOR, numberColorPickerCtrl->GetColour());
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
