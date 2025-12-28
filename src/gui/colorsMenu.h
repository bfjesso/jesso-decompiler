#pragma once
#include "guiUtils.h"
#include <wx/clrpicker.h>
#include <wx/stc/stc.h>

class ColorsMenu : public wxFrame, public Utils
{
public:
	ColorsMenu(wxStyledTextCtrl* textCtrl);

	wxStyledTextCtrl* decompilationTextCtrl;

	wxStaticText* operatorColorLabel = nullptr;
	wxColourPickerCtrl* operatorColorPickerCtrl = nullptr;

	wxStaticText* localVarColorLabel = nullptr;
	wxColourPickerCtrl* localVarColorPickerCtrl = nullptr;

	wxStaticText* argumentColorLabel = nullptr;
	wxColourPickerCtrl* argumentColorPickerCtrl = nullptr;

	wxStaticText* functionColorLabel = nullptr;
	wxColourPickerCtrl* functionColorPickerCtrl = nullptr;

	wxStaticText* importColorLabel = nullptr;
	wxColourPickerCtrl* importColorPickerCtrl = nullptr;

	wxStaticText* primitiveTypeColorLabel = nullptr;
	wxColourPickerCtrl* primitiveTypeColorPickerCtrl = nullptr;

	wxStaticText* keywordColorLabel = nullptr;
	wxColourPickerCtrl* keywordColorPickerCtrl = nullptr;

	wxStaticText* stringColorLabel = nullptr;
	wxColourPickerCtrl* stringColorPickerCtrl = nullptr;

	wxStaticText* numberColorLabel = nullptr;
	wxColourPickerCtrl* numberColorPickerCtrl = nullptr;

	wxButton* applyButton;

	wxBoxSizer* vSizer = nullptr;

	enum SyntaxHighlights
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
	
	enum ids
	{
		MainWindowID,
		ApplyButtonID
	};

	void ApplyColors();

	void OpenMenu(wxPoint position);

	void CloseMenu(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};
