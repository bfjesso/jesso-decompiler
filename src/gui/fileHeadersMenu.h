#pragma once
#include "guiUtils.h"

class FileHeadersMenu : public wxFrame, public Utils
{
public:
	FileHeadersMenu(wxWindow* parent, wxPoint position, wxString filePath);

	wxBoxSizer* vSizer = nullptr;

	enum ids
	{
		MainWindowID
	};
};
