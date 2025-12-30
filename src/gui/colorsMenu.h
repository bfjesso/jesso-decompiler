#pragma once
#include "guiUtils.h"
#include <wx/clrpicker.h>
#include <wx/stc/stc.h>

class ColorsMenu : public wxFrame, public Utils
{
public:
	ColorsMenu(wxStyledTextCtrl* textCtrl, wxStyledTextCtrl* textCtrl2);

	wxStyledTextCtrl* disassemblyTextCtrl = nullptr;
	wxStyledTextCtrl* decompilationTextCtrl = nullptr;

	std::vector<wxColourPickerCtrl*> colorPickerCtrls;

	wxButton* applyButton;

	wxBoxSizer* vSizer = nullptr;

	enum DecompilationColors
	{
		OPERATOR_COLOR,
		LOCAL_VAR_COLOR,
		ARGUMENT_COLOR,
		FUNCTION_COLOR,
		IMPORT_COLOR,
		PRIMITIVE_COLOR,
		KEYWORD_COLOR,
		STRING_COLOR,
		NUMBER_COLOR 
	};
	const char* decompColorNames[9] = 
	{ 
		"Operators",
		"Local variables",
		"Arguments",
		"Functions",
		"Imports",
		"Primitive types",
		"Keywords",
		"Strings",
		"Numbers"
	};
	wxColour defaultDecompColors[9] = 
	{
		wxColour(180, 180, 180),
		wxColour(156, 220, 254),
		wxColour(154, 154, 154),
		wxColour(220, 220, 170),
		wxColour(190, 183, 255),
		wxColour(86, 156, 214),
		wxColour(216, 160, 223),
		wxColour(232, 201, 187),
		wxColour(181, 206, 168)
	};
	const int numberOfDecompColors = 9;
	
	enum ids
	{
		MainWindowID,
		ApplyButtonID
	};

	void ApplyColors(wxStyledTextCtrl* ctrl);

	void OpenMenu(wxPoint position);

	void CloseMenu(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};
