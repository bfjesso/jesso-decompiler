#pragma once
#include <wx/wx.h>
#include <wx/clipbrd.h>
#include <wx/stc/stc.h>

const wxColour backgroundColor = wxColour(35, 35, 35);
const wxColour foregroundColor = wxColour(60, 60, 60);
const wxColour gridColor = wxColour(30, 30, 30);
const wxColour textColor = wxColour(220, 220, 220);
const wxColour darkerTextColor = wxColour(154, 154, 154);
const wxFont codeFont = wxFontInfo(10).FaceName("Cascadia Mono").Bold();

enum IndicatorColor
{
	PURPLE_INDICATOR,
	GRAY_INDICATOR,
	YELLOW_INDICATOR,
	RED_INDICATOR
};
const int numOfIndicators = 4;

void CopyToClipboard(const char* txt);

void SetUpStyledTextCtrl(wxStyledTextCtrl* ctrl);

void ClearStyledTextCtrl(wxStyledTextCtrl* ctrl);

void CenterLineStyledTextCtrl(wxStyledTextCtrl* ctrl, int line);

void HighlightLineStyledTextCtrl(wxStyledTextCtrl* ctrl, int line, enum IndicatorColor color, unsigned char gotoLine);