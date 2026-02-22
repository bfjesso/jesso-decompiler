#pragma once
#include "guiUtils.h"
#include <wx/grid.h>
#include "../fileStructs.h"

class ImportsViewer : public wxFrame, public Utils
{
public:
	ImportsViewer();

	wxGrid* importsGrid = nullptr;

	wxBoxSizer* vSizer = nullptr;

	enum ids
	{
		MainWindowID
	};


	void OpenMenu(wxPoint position, ImportedFunction* imports, int numOfImports);
};
