#pragma once
#include "guiUtils.h"

class CalculatorMenu : public wxFrame, public Utils
{
public:
	CalculatorMenu(wxWindow* parent, wxPoint position);

	wxCheckBox* firstHexCheckBox = nullptr;
	wxTextCtrl* firstValueInput = nullptr;

	wxChoice* selectOperation = nullptr;

	wxCheckBox* secondHexCheckBox = nullptr;
	wxTextCtrl* secondValueInput = nullptr;

	wxButton* copyHexResult = nullptr;
	wxStaticText* hexResultTxt = nullptr;

	wxButton* copyDecResult = nullptr;
	wxStaticText* decResultTxt = nullptr;

	// these are for copying to clipboard
	wxString hexResultStr;
	wxString decResultStr;

	wxBoxSizer* row1Sizer = nullptr;
	wxBoxSizer* row2Sizer = nullptr;
	wxBoxSizer* row3Sizer = nullptr;
	wxBoxSizer* row4Sizer = nullptr;
	wxBoxSizer* vSizer = nullptr;

	enum ids
	{
		MainWindowID,
		FirstHexCheckBoxID,
		FirstValueInputID,
		SelectOperationID,
		SecondHexCheckBoxID,
		SecondValueInputID,
		CopyHexResultID,
		CopyDecResultID
	};

	const char* operationChars[4]
	{
		"+",
		"-",
		"*",
		"/"
	};

	void CalculateResult(wxCommandEvent& e);

	void CopyHexResult(wxCommandEvent& e);

	void CopyDecResult(wxCommandEvent& e);

	wxDECLARE_EVENT_TABLE();
};