#pragma once
#include <wx/wx.h>

const wxColour backgroundColor = wxColour(35, 35, 35);
const wxColour foregroundColor = wxColour(60, 60, 60);
const wxColour gridColor = wxColour(30, 30, 30);
const wxColour textColor = wxColour(220, 220, 220);
const wxColour darkerTextColor = wxColour(154, 154, 154);
const wxFont codeFont = wxFontInfo(10).FaceName("Cascadia Mono").Bold();

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

void CopyToClipboard(const char* txt);