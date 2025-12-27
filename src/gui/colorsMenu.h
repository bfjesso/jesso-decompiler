#pragma once
#include "guiUtils.h"
#include <wx/clrpicker.h>

class ColorsMenu : public wxFrame, public Utils
{
public:
	ColorsMenu();

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

	wxColour localVarColor = wxColour(0, 240, 255);
	wxColour argumentColor = wxColour(150, 150, 150);
	wxColour functionColor = wxColour(255, 220, 70);
	wxColour importColor = wxColour(255, 70, 70);
	wxColour primitiveTypeColor = wxColour(0, 150, 255);
	wxColour keywordColor = wxColour(255, 150, 255);
	wxColour stringColor = wxColour(200, 130, 0);
	wxColour numberColor = wxColour(200, 230, 150);

	enum ids
	{
		MainWindowID,
		ApplyButtonID
	};

	void ApplyColors(wxCommandEvent& e);

	void OpenMenu(wxPoint position);

	void CloseMenu(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};
