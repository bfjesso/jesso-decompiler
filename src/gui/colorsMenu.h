#pragma once
#include "jdcTextCtrl.h"
#include <wx/clrpicker.h>

class ColorsMenu : public wxFrame
{
public:
	ColorsMenu();

	std::vector<JdcTextCtrl*> textCtrls;

	wxStaticText* disassemblyLabel;
	wxStaticText* decompilationLabel;

	std::vector<wxColourPickerCtrl*> disassemblyColorPickerCtrls;
	std::vector<wxColourPickerCtrl*> decompilationColorPickerCtrls;

	wxButton* applyButton;

	wxBoxSizer* disassemblySizer = nullptr;
	wxBoxSizer* decompilationSizer = nullptr;
	wxBoxSizer* hSizer = nullptr;
	wxBoxSizer* vSizer = nullptr;

	wxScrolledWindow* disassemblyScrollWindow = nullptr;
	wxScrolledWindow* decompilationScrollWindow = nullptr;

	const char* disassemblyColorNames[8] =
	{
		"Punctuation",
		"Addresses",
		"Opcodes",
		"Registers",
		"Segments",
		"Pointer sizes",
		"Comments",
		"Constants"
	};
	wxColour defaultDisassemblyColors[8] =
	{
		wxColour(180, 180, 180),
		wxColour(154, 154, 154),
		wxColour(220, 220, 170),
		wxColour(156, 220, 254),
		wxColour(190, 183, 255),
		wxColour(86, 156, 214),
		wxColour(87, 166, 74),
		wxColour(181, 206, 168)
	};
	const int numberOfDisassemblyColors = 8;

	const char* decompColorNames[13] = 
	{ 
		"Operators",
		"Local variables",
		"Arguments",
		"Functions",
		"Imports",
		"Intrinsic Functions",
		"Primitive types",
		"Keywords",
		"Strings",
		"Numbers",
		"Comments",
		"Labels",
		"Errors"
	};
	wxColour defaultDecompColors[13] = 
	{
		wxColour(180, 180, 180),
		wxColour(156, 220, 254),
		wxColour(154, 154, 154),
		wxColour(220, 220, 170),
		wxColour(190, 183, 255),
		wxColour(220, 220, 170),
		wxColour(86, 156, 214),
		wxColour(216, 160, 223),
		wxColour(232, 201, 187),
		wxColour(181, 206, 168),
		wxColour(87, 166, 74),
		wxColour(200, 200, 200),
		wxColour(250, 50, 50)
	};
	const int numberOfDecompColors = 13;
	
	enum ids
	{
		MainWindowID,
		ApplyButtonID
	};

	void AddTextCtrl(JdcTextCtrl* ctrl);

	void ApplyColors();

	void OpenMenu(wxPoint position);

	void CloseMenu(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};
