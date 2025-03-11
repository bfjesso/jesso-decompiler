#pragma once
#include "../Headers/guiUtils.h"

class DataViewer : public wxFrame, public Utils
{
public:
	DataViewer();

	wxChoice* dataTypeChoice = nullptr;
	wxTextCtrl* numOfbytesInputTextCtrl = nullptr;
	wxCheckBox* hexCheckBox = nullptr;
	wxButton* loadDataButton = nullptr;
	wxListBox* dataListBox = nullptr;

	wxBoxSizer* row1Sizer = nullptr;
	wxBoxSizer* row2Sizer = nullptr;
	wxBoxSizer* row3Sizer = nullptr;
	wxBoxSizer* vSizer = nullptr;

	wxString currentFilePath = "";

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
		LoadDataButtonID
	};

	void LoadData(wxCommandEvent& e);

	void OpenMenu(wxPoint position);

	void CloseMenu(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};