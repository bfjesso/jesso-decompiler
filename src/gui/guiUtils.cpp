#include "guiUtils.h"

void Utils::CopyToClipboard(const char* txt)
{
	if (wxTheClipboard->Open())
	{
		wxTheClipboard->SetData(new wxTextDataObject(txt));
		wxTheClipboard->Close();
	}
}

void Utils::SetUpStyledTextCtrl(wxStyledTextCtrl* ctrl)
{
	ctrl->SetReadOnly(true);
	ctrl->SetMarginWidth(1, 0);
	ctrl->StyleSetFont(wxSTC_STYLE_DEFAULT, codeFont);
	ctrl->StyleSetBackground(wxSTC_STYLE_DEFAULT, gridColor);
	ctrl->StyleClearAll();
	ctrl->SetSelBackground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
	ctrl->SetSelForeground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
	ctrl->SetCaretForeground(textColor);
	ctrl->SetCaretWidth(2);
	ctrl->SetWrapMode(wxSTC_WRAP_NONE);
	ctrl->SetScrollWidthTracking(true);
	ctrl->SetScrollWidth(1);
	ctrl->SetViewWhiteSpace(wxSTC_WS_INVISIBLE);

	ctrl->IndicatorSetStyle(0, wxSTC_INDIC_ROUNDBOX);
	ctrl->IndicatorSetForeground(0, wxColour(255, 0, 255));
	ctrl->IndicatorSetAlpha(0, 80);

	ctrl->IndicatorSetStyle(1, wxSTC_INDIC_ROUNDBOX);
	ctrl->IndicatorSetForeground(1, wxColour(200, 200, 200));
	ctrl->IndicatorSetAlpha(1, 80);
}

void Utils::ClearStyledTextCtrl(wxStyledTextCtrl* ctrl)
{
	ctrl->SetReadOnly(false);
	ctrl->SetText("");
	ctrl->SetReadOnly(true);
}