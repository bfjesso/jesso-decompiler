#pragma once
#include "guiUtils.h"
#include <wx/aui/aui.h>

class JdcTabArt final : public wxAuiDefaultTabArt
{
public:
	JdcTabArt() {}

	wxAuiTabArt* Clone() override;

	void DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect) override;

	void DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& page, const wxRect& in_rect, int32_t closeButtonState, wxRect* outTabRect, wxRect* outButtonRect, int32_t* xExtent) override;
};