#pragma once
#include "guiUtils.h"

class FileHeadersWindow : public wxScrolledWindow
{
public:
	FileHeadersWindow(wxWindow* parent, wxString filePath);

	wxBoxSizer* vSizer = nullptr;
};
