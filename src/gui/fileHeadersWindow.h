#pragma once
#include "guiUtils.h"
#include "../fileStructs.h"

class FileHeadersWindow : public wxScrolledWindow
{
public:
	FileHeadersWindow(wxWindow* parent, wxString filePath, enum FileFormat fileFormat);

	wxBoxSizer* vSizer = nullptr;
};
