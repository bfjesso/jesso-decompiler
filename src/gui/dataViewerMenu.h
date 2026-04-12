#pragma once
#include "guiUtils.h"
#include "../file-handler/fileHandler.h"

class DataViewer : public wxFrame, public Utils
{
public:
	DataViewer();

	wxChoice* dataTypeChoice = nullptr;
	wxChoice* sectionChoice = nullptr;
	wxCheckBox* hexCheckBox = nullptr;
	wxStyledTextCtrl* dataTextCtrl = nullptr;

	wxBoxSizer* row1Sizer = nullptr;
	wxBoxSizer* row2Sizer = nullptr;
	wxBoxSizer* vSizer = nullptr;

	const int bytesPerLine = 8;

	unsigned char* fileBytes = nullptr;
	uintptr_t imageBase = 0;
	FileSection* sections = nullptr;
	int numOfSections = 0;

	const char* dataTypeStrs[6] = 
	{
		"1-byte int",
		"2-byte int",
		"4-byte int",
		"8-byte int",
		"float",
		"double"
	};

	const int typeSizes[6] =
	{
		1,
		2,
		4,
		8,
		4,
		8
	};

	enum ids
	{
		MainWindowID,
		DataTypeChoiceID,
		SectionChoiceID,
		HexCheckBoxID
	};

	void UpdateDataList(wxCommandEvent& e);

	void LoadData();

	void StyledTextCtrlRightClickOptions(wxContextMenuEvent& e);

	void ClearData();

	void OpenMenu(wxPoint position, uintptr_t imageBas, FileSection* secs, int numOfSecs, unsigned char* bytes);

	void CloseMenu(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};
