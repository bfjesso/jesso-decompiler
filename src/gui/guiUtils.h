#pragma once
#include <wx/wx.h>
#include <wx/clipbrd.h>
#include <wx/stc/stc.h>

class Utils
{
public:
	const wxColour backgroundColor = wxColour(35, 35, 35);
	const wxColour foregroundColor = wxColour(60, 60, 60);
	const wxColour gridColor = wxColour(30, 30, 30);
	const wxColour textColor = wxColour(220, 220, 220);
	const wxFont codeFont = wxFontInfo(10).FaceName("Cascadia Mono").Bold();

	void CopyToClipboard(const char* txt);

	void SetUpStyledTextCtrl(wxStyledTextCtrl* ctrl);

	void ClearStyledTextCtrl(wxStyledTextCtrl* ctrl);
};