#pragma once
#include <wx/wx.h>
#include <wx/clipbrd.h>

class Utils
{
public:
	const wxColour backgroundColor = wxColour(35, 35, 35);
	const wxColour foregroundColor = wxColour(60, 60, 60);
	const wxColour textColor = wxColour(220, 220, 220);

	void CopyToClipboard(const char* txt);
};