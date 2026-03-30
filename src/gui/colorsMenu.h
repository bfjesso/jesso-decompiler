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
		COMMENT_DIS_COLOR,
		CONSTANT_COLOR
	};
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

	enum DecompilationColor
	{
		OPERATOR_COLOR,
		LOCAL_VAR_COLOR,
		ARGUMENT_COLOR,
		FUNCTION_COLOR,
		IMPORT_COLOR,
		INTRINSIC_COLOR,
		PRIMITIVE_COLOR,
		KEYWORD_COLOR,
		STRING_COLOR,
		NUMBER_COLOR,
		COMMENT_DECOMP_COLOR,
		LABEL_COLOR,
		ERROR_COLOR,
	};
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

	void ApplyColors();

	void OpenMenu(wxPoint position);

	void CloseMenu(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};
