#include "calculatorMenu.h"

wxBEGIN_EVENT_TABLE(CalculatorMenu, wxFrame)
EVT_CHECKBOX(FirstHexCheckBoxID, CalculatorMenu::CalculateResult)
EVT_TEXT(FirstValueInputID, CalculatorMenu::CalculateResult)
EVT_CHOICE(SelectOperationID, CalculatorMenu::CalculateResult)
EVT_CHECKBOX(SecondHexCheckBoxID, CalculatorMenu::CalculateResult)
EVT_TEXT(SecondValueInputID, CalculatorMenu::CalculateResult)
EVT_BUTTON(CopyHexResultID, CalculatorMenu::CopyHexResult)
EVT_BUTTON(CopyDecResultID, CalculatorMenu::CopyDecResult)
wxEND_EVENT_TABLE()

CalculatorMenu::CalculatorMenu(wxWindow* parent, wxPoint position) : wxFrame(parent, MainWindowID, "Calculator", wxPoint(50, 50), wxSize(380, 150))
{
	position.x += 10;
	position.y += 10;
	SetPosition(position);
	Show();
	Raise();

	SetOwnBackgroundColour(backgroundColor);

	firstHexCheckBox = new wxCheckBox(this, FirstHexCheckBoxID, "Hex");
	firstHexCheckBox->SetOwnForegroundColour(textColor);
	firstHexCheckBox->SetValue(true);

	firstValueInput = new wxTextCtrl(this, FirstValueInputID, "0xC", wxPoint(0, 0), wxSize(150, 30));
	firstValueInput->SetOwnBackgroundColour(foregroundColor);
	firstValueInput->SetOwnForegroundColour(textColor);

	selectOperation = new wxChoice(this, SelectOperationID, wxPoint(0, 0), wxSize(50, 30), wxArrayString(4, operationChars));
	selectOperation->SetSelection(0);
	selectOperation->SetOwnBackgroundColour(foregroundColor);
	selectOperation->SetOwnForegroundColour(textColor);

	secondHexCheckBox = new wxCheckBox(this, SecondHexCheckBoxID, "Hex");
	secondHexCheckBox->SetOwnForegroundColour(textColor);
	secondHexCheckBox->SetValue(true);

	secondValueInput = new wxTextCtrl(this, SecondValueInputID, "0x4", wxPoint(0, 0), wxSize(150, 30));
	secondValueInput->SetOwnBackgroundColour(foregroundColor);
	secondValueInput->SetOwnForegroundColour(textColor);

	copyHexResult = new wxButton(this, CopyHexResultID, "Copy", wxPoint(0, 0), wxSize(50, 25));
	copyHexResult->SetOwnBackgroundColour(foregroundColor);
	copyHexResult->SetOwnForegroundColour(textColor);

	hexResultTxt = new wxStaticText(this, wxID_ANY, "Hex Result: 0x10");
	hexResultTxt->SetOwnForegroundColour(textColor);
	hexResultStr = "0x10";

	copyDecResult = new wxButton(this, CopyDecResultID, "Copy", wxPoint(0, 0), wxSize(50, 25));
	copyDecResult->SetOwnBackgroundColour(foregroundColor);
	copyDecResult->SetOwnForegroundColour(textColor);

	decResultTxt = new wxStaticText(this, wxID_ANY, "Dec Result: 16");
	decResultTxt->SetOwnForegroundColour(textColor);
	decResultStr = "16";

	row1Sizer = new wxBoxSizer(wxHORIZONTAL);
	row2Sizer = new wxBoxSizer(wxHORIZONTAL);
	row3Sizer = new wxBoxSizer(wxHORIZONTAL);
	row4Sizer = new wxBoxSizer(wxHORIZONTAL);
	vSizer = new wxBoxSizer(wxVERTICAL);

	row1Sizer->Add(firstHexCheckBox, 0, wxTOP | wxLEFT, 10);
	row1Sizer->AddStretchSpacer();
	row1Sizer->Add(secondHexCheckBox, 0, wxTOP | wxRIGHT, 10);

	row2Sizer->Add(firstValueInput, 1, wxEXPAND | wxALL, 10);
	row2Sizer->Add(selectOperation, 0, wxTOP | wxBOTTOM, 10);
	row2Sizer->Add(secondValueInput, 1, wxEXPAND | wxALL, 10);

	row3Sizer->Add(copyHexResult, 0, wxALL, 10);
	row3Sizer->Add(hexResultTxt, 0, wxTOP | wxBOTTOM, 10);

	row4Sizer->Add(copyDecResult, 0, wxALL, 10);
	row4Sizer->Add(decResultTxt, 0, wxTOP | wxBOTTOM, 10);

	vSizer->Add(row1Sizer, 0, wxEXPAND | wxCENTER);
	vSizer->Add(row2Sizer, 0, wxEXPAND | wxCENTER);
	vSizer->Add(row3Sizer, 0, wxEXPAND | wxCENTER);
	vSizer->Add(row4Sizer, 0, wxEXPAND | wxCENTER);

	SetSizerAndFit(vSizer);
}

void CalculatorMenu::CalculateResult(wxCommandEvent& e)
{
	hexResultTxt->SetLabelText("Hex Result: ");
	hexResultStr = "";

	decResultTxt->SetLabelText("Dec Result: ");
	decResultStr = "";
	
	long long firstValue = 0;
	long long secondValue = 0;

	if (firstHexCheckBox->IsChecked()) 
	{
		if (!firstValueInput->GetValue().ToLongLong(&firstValue, 16))
		{
			return;
		}
	}
	else 
	{
		if (!firstValueInput->GetValue().ToLongLong(&firstValue, 10))
		{
			return;
		}
	}

	if (secondHexCheckBox->IsChecked())
	{
		if (!secondValueInput->GetValue().ToLongLong(&secondValue, 16))
		{
			return;
		}
	}
	else
	{
		if (!secondValueInput->GetValue().ToLongLong(&secondValue, 10))
		{
			return;
		}
	}

	int operation = selectOperation->GetSelection();
	if(operation == 3 && secondValue == 0) // division by zero check
	{
		return;
	}

	long long result = 0;
	switch (operation)
	{
	case 0: // +
		result = firstValue + secondValue;
		break;
	case 1: // -
		result = firstValue - secondValue;
		break;
	case 2: // *
		result = firstValue * secondValue;
		break;
	case 3: // /
		result = firstValue / secondValue;
		break;
	}

	char hexStr[50] = { 0 };
	sprintf(hexStr, "0x%llX", result);
	hexResultStr = wxString(hexStr);
	hexResultTxt->SetLabelText("Hex Result: " + hexResultStr);

	decResultStr = std::to_string(result);
	decResultTxt->SetLabelText("Dec Result: " + decResultStr);
}

void CalculatorMenu::CopyHexResult(wxCommandEvent& e)
{
	CopyToClipboard(hexResultStr.c_str());
}

void CalculatorMenu::CopyDecResult(wxCommandEvent& e)
{
	CopyToClipboard(decResultStr.c_str());
}