#pragma once
#include "guiUtils.h"
#include <wx/grid.h>
#include "../fileStructs.h"

class ImportsWindow : public wxGrid
{
public:
	ImportsWindow(wxWindow* parent, ImportedFunction* imports, int numOfImports);

	void RightClickOptions(wxGridEvent& e);
};