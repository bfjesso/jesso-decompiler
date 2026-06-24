#pragma once
#include "guiUtils.h"
#include <wx/grid.h>
#include "../fileStructs.h"

class ImportsGrid : public wxGrid
{
public:
	ImportsGrid(wxWindow* parent, ImportedFunction* imports, int numOfImports);

	void RightClickOptions(wxGridEvent& e);

	wxDECLARE_EVENT_TABLE();
};