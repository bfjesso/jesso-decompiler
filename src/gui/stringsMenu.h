#pragma once
#include "guiUtils.h"
#include "../fileStructs.h"
#include <wx/grid.h>

class StringsMenu : public wxFrame, public Utils
{
public:
	StringsMenu();

	wxStaticText* infoStaticText = nullptr;
	wxGrid* stringsGrid = nullptr;
	wxBoxSizer* vSizer = nullptr;

	unsigned char* fileBytes = nullptr;
	unsigned long long imageBase = 0;
	FileSection* sections = nullptr;
	int numOfSections = 0;

	enum ids
	{
		MainWindowID
	};

	void LoadStrings();

	void ClearData();

	void GridRightClickOptions(wxGridEvent& e);

	void OpenMenu(wxPoint position, unsigned long long imageBas, FileSection* secs, int numOfSecs, unsigned char* bytes);

	void CloseMenu(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};
