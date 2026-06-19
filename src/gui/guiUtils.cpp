#include "guiUtils.h"

void CopyToClipboard(const char* txt)
{
	if (wxTheClipboard->Open())
	{
		wxTheClipboard->SetData(new wxTextDataObject(txt));
		wxTheClipboard->Close();
	}
}

void SetUpStyledTextCtrl(wxStyledTextCtrl* ctrl)
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

	ctrl->IndicatorSetStyle(PURPLE_INDICATOR, wxSTC_INDIC_ROUNDBOX);
	ctrl->IndicatorSetForeground(PURPLE_INDICATOR, wxColour(255, 0, 255));
	ctrl->IndicatorSetAlpha(PURPLE_INDICATOR, 40);

	ctrl->IndicatorSetStyle(GRAY_INDICATOR, wxSTC_INDIC_ROUNDBOX);
	ctrl->IndicatorSetForeground(GRAY_INDICATOR, wxColour(200, 200, 200));
	ctrl->IndicatorSetAlpha(GRAY_INDICATOR, 40);

	ctrl->IndicatorSetStyle(YELLOW_INDICATOR, wxSTC_INDIC_ROUNDBOX);
	ctrl->IndicatorSetForeground(YELLOW_INDICATOR, wxColour(255, 200, 0));
	ctrl->IndicatorSetAlpha(YELLOW_INDICATOR, 40);

	ctrl->IndicatorSetStyle(RED_INDICATOR, wxSTC_INDIC_ROUNDBOX);
	ctrl->IndicatorSetForeground(RED_INDICATOR, wxColour(255, 0, 0));
	ctrl->IndicatorSetAlpha(RED_INDICATOR, 40);
}

void ClearStyledTextCtrl(wxStyledTextCtrl* ctrl)
{
	ctrl->SetReadOnly(false);
	ctrl->SetText("");
	ctrl->SetReadOnly(true);
}

void CenterLineStyledTextCtrl(wxStyledTextCtrl* ctrl, int line)
{
	ctrl->GotoLine(line);
	ctrl->LineScroll(0, line - ctrl->GetFirstVisibleLine() - (ctrl->LinesOnScreen() / 2)); // this centers it
}

void HighlightLineStyledTextCtrl(wxStyledTextCtrl* ctrl, int line, enum IndicatorColor color, unsigned char gotoLine)
{
	if(gotoLine)
	{
		CenterLineStyledTextCtrl(ctrl, line);
	}
	
	ctrl->SetIndicatorCurrent(color);
	int start = ctrl->PositionFromLine(line);
	int len = ctrl->GetLineLength(line);
	ctrl->IndicatorFillRange(start, len);
}