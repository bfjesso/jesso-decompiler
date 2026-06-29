#pragma once
#include "guiUtils.h"
#include <wx/stc/stc.h>
#include <wx/fdrepdlg.h>

#define NUM_OF_INDICATORS 4

enum IndicatorColor
{
	PURPLE_INDICATOR,
	GRAY_INDICATOR,
	YELLOW_INDICATOR,
	RED_INDICATOR
};

struct RightClickOption 
{
	wxString name;
	char commandKey;
	unsigned char* check;
	std::function<void(wxCommandEvent&)> function;
};

class JdcTextCtrl : public wxStyledTextCtrl 
{
private:
	char IsCharDigit(char c);

public:
	JdcTextCtrl(wxWindow* parent, wxString name);

	std::vector<struct RightClickOption> additionalRightClickOptions;

	wxFindReplaceData findData;
	wxFindReplaceDialog* findDialog = nullptr;
	wxString lastFindText = "";
	int totalFindResults = 0;

	unsigned char highlightSelectedLines = 1;

	void EnableLineNumbers();

	void ClearText();

	void AddLine(wxString txt);

	void CenterLine(int line);

	void HighlightLine(int line, enum IndicatorColor color, unsigned char gotoLine);

	void ClearIndicators();

	void ShowFindDialog();

	void OnFindDialog(wxFindDialogEvent& e);

	int FindInRange(const wxString& text, int start, int end, int flags, unsigned char forward);

	int CountNumOfResults(const wxString& text, int end, int flags);

	void OnFindDialogClose(wxFindDialogEvent& e);

	void AddRightClickOption(wxString name, char commandKey, unsigned char* check, const std::function<void(wxCommandEvent&)>& function);

	void AddDefaultRightClickOptions(wxMenu* menu);

	void RightClickOptions(wxContextMenuEvent& e);

	void OnKeyDown(wxKeyEvent& e);

	void OnUpdateUI(wxStyledTextEvent& e);
};