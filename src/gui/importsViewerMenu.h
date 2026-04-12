#pragma once
#include "guiUtils.h"
#include <wx/grid.h>
#include "../fileStructs.h"

class ImportsViewer : public wxFrame, public Utils
{
public:
	ImportsViewer(wxWindow* parent, wxPoint position, ImportedFunction* imports, int numOfImports);

	wxGrid* importsGrid = nullptr;

	wxBoxSizer* vSizer = nullptr;

	enum ids
	{
		MainWindowID
	};

	void GridRightClickOptions(wxGridEvent& e);

	wxDECLARE_EVENT_TABLE();
};
