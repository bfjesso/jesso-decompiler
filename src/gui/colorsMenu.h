#pragma once
#include "guiUtils.h"
#include <wx/clrpicker.h>

class ColorsMenu : public wxFrame, public Utils
{
public:
	ColorsMenu();

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

	wxColour operatorColor = wxColour(180, 180, 180);
	wxColour localVarColor = wxColour(156, 220, 254);
	wxColour argumentColor = wxColour(154, 154, 154);
	wxColour functionColor = wxColour(220, 220, 170);
	wxColour importColor = wxColour(190, 183, 255);
	wxColour primitiveTypeColor = wxColour(86, 156, 214);
	wxColour keywordColor = wxColour(216, 160, 223);
	wxColour stringColor = wxColour(232, 201, 187);
	wxColour numberColor = wxColour(181, 206, 168);
	
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
