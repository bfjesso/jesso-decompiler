#pragma once
#include "guiUtils.h"
#include <wx/grid.h>
#include "../fileStructs.h"

class SectionsViewer : public wxFrame, public Utils
{
public:
	SectionsViewer(wxWindow* parent, wxPoint position, FileSection* sections, int numOfSections);

	wxGrid* sectionsGrid = nullptr;

	wxBoxSizer* vSizer = nullptr;

	enum ids
	{
		MainWindowID
	};

	void GridRightClickOptions(wxGridEvent& e);

	wxDECLARE_EVENT_TABLE();
};
