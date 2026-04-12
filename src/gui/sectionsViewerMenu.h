#pragma once
#include "guiUtils.h"
#include <wx/grid.h>
#include "../fileStructs.h"

class SectionsViewer : public wxFrame, public Utils
{
public:
	SectionsViewer();

	wxGrid* sectionsGrid = nullptr;

	wxBoxSizer* vSizer = nullptr;

	enum ids
	{
		MainWindowID
	};

	void OpenMenu(wxPoint position, FileSection* sections, int numOfSections);

	void GridRightClickOptions(wxGridEvent& e);

	void CloseMenu(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};
