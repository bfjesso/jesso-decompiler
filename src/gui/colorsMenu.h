#pragma once
#include "guiUtils.h"
#include <wx/clrpicker.h>

class ColorsMenu : public wxFrame, public Utils
{
public:
	ColorsMenu(wxStyledTextCtrl* disassemblyCtrl, wxStyledTextCtrl* decompilationCtrl, wxStyledTextCtrl* dataCtrl);

	wxStyledTextCtrl* disassemblyTextCtrl = nullptr;
	wxStyledTextCtrl* decompilationTextCtrl = nullptr;
	wxStyledTextCtrl* dataTextCtrl = nullptr;

	wxStaticText* disassemblyLabel;
	wxStaticText* decompilationLabel;

	std::vector<wxColourPickerCtrl*> disassemblyColorPickerCtrls;
	std::vector<wxColourPickerCtrl*> decompilationColorPickerCtrls;

	wxButton* applyButton;

	wxBoxSizer* disassemblySizer = nullptr;
	wxBoxSizer* decompilationSizer = nullptr;
	wxBoxSizer* hSizer = nullptr;
	wxBoxSizer* vSizer = nullptr;

	enum DisassemblyColor
	{
		PUNCTUATION_COLOR,
		ADDRESS_COLOR,
		OPCODE_COLOR,
		REGISTER_COLOR,
		SEGMENT_COLOR,
		PTR_SIZE_COLOR,
		CONSTANT_COLOR
	};
	const char* disassemblyColorNames[7] =
	{
		"Punctuation",
		"Addresses",
		"Opcodes",
		"Registers",
		"Segments",
		"Pointer sizes",
		"Constants"
	};
	wxColour defaultDisassemblyColors[7] =
	{
		wxColour(180, 180, 180),
		wxColour(154, 154, 154),
		wxColour(220, 220, 170),
		wxColour(156, 220, 254),
		wxColour(190, 183, 255),
		wxColour(86, 156, 214),
		wxColour(181, 206, 168)
	};
	const int numberOfDisassemblyColors = 7;

	enum DecompilationColor
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

	void ApplyColors();

	void OpenMenu(wxPoint position);

	void CloseMenu(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};
