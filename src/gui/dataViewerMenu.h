#pragma once
#include "guiUtils.h"
#include "../file-handler/fileHandler.h"

class DataViewer : public wxFrame, public Utils
{
public:
	DataViewer();

	wxChoice* dataTypeChoice = nullptr;
	wxCheckBox* hexCheckBox = nullptr;
	wxListBox* dataListBox = nullptr;

	wxBoxSizer* row1Sizer = nullptr;
	wxBoxSizer* row2Sizer = nullptr;
	wxBoxSizer* vSizer = nullptr;

	unsigned char* bytes = nullptr;
	uintptr_t imageBase = 0;
	FileSection* dataSections = nullptr;
	int numOfDataSections = 0;

	const char* dataTypeStrs[6] = 
	{
		"1-byte int",
		"2-byte int",
		"4-byte int",
		"8-byte int",
		"float",
		"double"
	};

	enum ids
	{
		MainWindowID,
		DataTypeChoiceID,
		HexCheckBoxID
	};

	void UpdateDataList(wxCommandEvent& e);

	void LoadData();

	void OpenMenu(wxPoint position, uintptr_t imageBas, FileSection* dataSecs, int numOfDataSecs, unsigned char* dataBytes);

	void CloseMenu(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};
