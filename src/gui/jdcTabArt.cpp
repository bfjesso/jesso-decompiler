#include "jdcTabArt.h"

wxAuiTabArt* JdcTabArt::Clone()
{
	return new JdcTabArt(*this);
}

void JdcTabArt::DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect)
{
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.SetBrush(wxBrush(foregroundColor));
	dc.DrawRectangle(rect);
}

void JdcTabArt::DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& page, const wxRect& in_rect, int32_t closeButtonState, wxRect* outTabRect, wxRect* outButtonRect, int32_t* xExtent)
{
	int32_t textWidth = 0;
	int32_t textHeight = 0;
	dc.GetTextExtent(page.caption, &textWidth, &textHeight);

	const int32_t tabWidth = textWidth + 32;
	wxRect rect = in_rect;
	rect.width = tabWidth;
	rect.Deflate(1, 2);

	const wxColour fill = page.active ? backgroundColor : foregroundColor;
	const wxColour border = fill.ChangeLightness(page.active ? 85 : 70);

	dc.SetPen(wxPen(border));
	dc.SetBrush(wxBrush(fill));
	dc.DrawRoundedRectangle(rect, 2);

	dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
	dc.SetTextForeground(textColor);

	dc.DrawText(page.caption, rect.x + 8, rect.y + (rect.height - dc.GetCharHeight()) / 2);

	if (outTabRect) { *outTabRect = rect; }
	if (xExtent) { *xExtent = tabWidth; }
	if (outButtonRect && closeButtonState != wxAUI_BUTTON_STATE_HIDDEN)
	{
		wxRect btn(rect.GetRight() - 16, rect.y + 4, 12, 12);
		*outButtonRect = btn;

		wxColour btnColor = textColor;
		if (closeButtonState == wxAUI_BUTTON_STATE_HOVER) { btnColor = wxColour(255, 255, 255); }
		else if (closeButtonState == wxAUI_BUTTON_STATE_PRESSED) { btnColor = wxColour(255, 50, 50); }

		dc.SetPen(wxPen(btnColor, 2));
		dc.DrawLine(btn.x + 1, btn.y + 1, btn.x + 9, btn.y + 9);
		dc.DrawLine(btn.x + 9, btn.y + 1, btn.x + 1, btn.y + 9);
	}
}