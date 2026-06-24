#pragma once
#include "guiUtils.h"

class FileHeadersWindow : public wxWindow
{
public:
	FileHeadersWindow(wxWindow* parent, wxString filePath);

	wxBoxSizer* vSizer = nullptr;
};
