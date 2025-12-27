#pragma once
#include <wx/wx.h>
#include <wx/clipbrd.h>

class Utils
{
public:
	const wxColour backgroundColor = wxColour(35, 35, 35);
	const wxColour foregroundColor = wxColour(60, 60, 60);
	const wxColour textColor = wxColour(220, 220, 220);

	const wxColour localVarColor = wxColour(0, 240, 255);
	const wxColour argumentColor = wxColour(150, 150, 150);
	const wxColour functionColor = wxColour(255, 220, 70);
	const wxColour importColor = wxColour(255, 70, 70);
	const wxColour primitiveTypeColor = wxColour(0, 150, 255);
	const wxColour keywordColor = wxColour(255, 150, 255);
	const wxColour stringColor = wxColour(200, 130, 0);
	const wxColour numberColor = wxColour(200, 230, 150);

	void CopyToClipboard(const char* txt);
};