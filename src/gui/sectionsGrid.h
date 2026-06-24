#pragma once
#include "guiUtils.h"
#include <wx/grid.h>
#include "../fileStructs.h"

class SectionsGrid : public wxGrid
{
public:
	SectionsGrid(wxWindow* parent, FileSection* sections, int numOfSections);

	void RightClickOptions(wxGridEvent& e);

	wxDECLARE_EVENT_TABLE();
};
