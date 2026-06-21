#pragma once
#include <wx/wx.h>

const wxColour backgroundColor = wxColour(35, 35, 35);
const wxColour foregroundColor = wxColour(60, 60, 60);
const wxColour gridColor = wxColour(30, 30, 30);
const wxColour textColor = wxColour(220, 220, 220);
const wxColour darkerTextColor = wxColour(154, 154, 154);
const wxFont codeFont = wxFontInfo(10).FaceName("Cascadia Mono").Bold();

#define NUM_OF_DISASSEMBLY_COLORS 8
#define NUM_OF_DECOMP_COLORS 13
#define NUM_OF_DATA_COLORS 2

enum DisassemblyColor
{
	OPERATOR_ASM_COLOR,
	ADDRESS_ASM_COLOR,
	OPCODE_ASM_COLOR,
	REGISTER_ASM_COLOR,
	SEGMENT_ASM_COLOR,
	PTR_SIZE_ASM_COLOR,
	COMMENT_ASM_COLOR,
	NUMBER_ASM_COLOR
};

enum DecompilationColor
{
	OPERATOR_DECOMP_COLOR,
	LOCAL_VAR_DECOMP_COLOR,
	ARGUMENT_DECOMP_COLOR,
	FUNCTION_DECOMP_COLOR,
	IMPORT_DECOMP_COLOR,
	INTRINSIC_DECOMP_COLOR,
	PRIMITIVE_DECOMP_COLOR,
	KEYWORD_DECOMP_COLOR,
	STRING_DECOMP_COLOR,
	NUMBER_DECOMP_COLOR,
	COMMENT_DECOMP_COLOR,
	LABEL_DECOMP_COLOR,
	ERROR_DECOMP_COLOR,
};

enum DataColor
{
	ADDRESS_DATA_COLOR,
	VALUE_DATA_COLOR
};

void CopyToClipboard(const char* txt);