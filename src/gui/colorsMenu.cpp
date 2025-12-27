#include "colorsMenu.h"

wxBEGIN_EVENT_TABLE(ColorsMenu, wxFrame)
EVT_BUTTON(ApplyButtonID, ColorsMenu::ApplyColors)
wxEND_EVENT_TABLE()

ColorsMenu::ColorsMenu() : wxFrame(nullptr, MainWindowID, "Colors Menu", wxPoint(50, 50), wxSize(400, 600))
{
	SetOwnBackgroundColour(backgroundColor);

	localVarColorLabel = new wxStaticText(this, wxID_ANY, "Local variables");
	localVarColorLabel->SetOwnForegroundColour(textColor);
	localVarColorPickerCtrl = new wxColourPickerCtrl(this, wxID_ANY, localVarColor, wxPoint(0, 0), wxSize(150, 25));

	argumentColorLabel = new wxStaticText(this, wxID_ANY, "Arguments");
	argumentColorLabel->SetOwnForegroundColour(textColor);
	argumentColorPickerCtrl = new wxColourPickerCtrl(this, wxID_ANY, argumentColor, wxPoint(0, 0), wxSize(150, 25));

	functionColorLabel = new wxStaticText(this, wxID_ANY, "Functions");
	functionColorLabel->SetOwnForegroundColour(textColor);
	functionColorPickerCtrl = new wxColourPickerCtrl(this, wxID_ANY, functionColor, wxPoint(0, 0), wxSize(150, 25));

	importColorLabel = new wxStaticText(this, wxID_ANY, "Imports");
	importColorLabel->SetOwnForegroundColour(textColor);
	importColorPickerCtrl = new wxColourPickerCtrl(this, wxID_ANY, importColor, wxPoint(0, 0), wxSize(150, 25));

	primitiveTypeColorLabel = new wxStaticText(this, wxID_ANY, "Primitive types");
	primitiveTypeColorLabel->SetOwnForegroundColour(textColor);
	primitiveTypeColorPickerCtrl = new wxColourPickerCtrl(this, wxID_ANY, primitiveTypeColor, wxPoint(0, 0), wxSize(150, 25));

	keywordColorLabel = new wxStaticText(this, wxID_ANY, "Keywords");
	keywordColorLabel->SetOwnForegroundColour(textColor);
	keywordColorPickerCtrl = new wxColourPickerCtrl(this, wxID_ANY, keywordColor, wxPoint(0, 0), wxSize(150, 25));

	stringColorLabel = new wxStaticText(this, wxID_ANY, "Strings");
	stringColorLabel->SetOwnForegroundColour(textColor);
	stringColorPickerCtrl = new wxColourPickerCtrl(this, wxID_ANY, stringColor, wxPoint(0, 0), wxSize(150, 25));

	numberColorLabel = new wxStaticText(this, wxID_ANY, "Numbers");
	numberColorLabel->SetOwnForegroundColour(textColor);
	numberColorPickerCtrl = new wxColourPickerCtrl(this, wxID_ANY, numberColor, wxPoint(0, 0), wxSize(150, 25));

	applyButton = new wxButton(this, ApplyButtonID, "Apply", wxPoint(0, 0), wxSize(150, 25));
	applyButton->SetOwnBackgroundColour(foregroundColor);
	applyButton->SetOwnForegroundColour(textColor);

	vSizer = new wxBoxSizer(wxVERTICAL);

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
}

void ColorsMenu::ApplyColors(wxCommandEvent& e)
{
	localVarColor = localVarColorPickerCtrl->GetColour();
	argumentColor = argumentColorPickerCtrl->GetColour();
	functionColor = functionColorPickerCtrl->GetColour();
	importColor = importColorPickerCtrl->GetColour();
	primitiveTypeColor = primitiveTypeColorPickerCtrl->GetColour();
	keywordColor = keywordColorPickerCtrl->GetColour();
	stringColor = stringColorPickerCtrl->GetColour();
	numberColor = numberColorPickerCtrl->GetColour();
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
