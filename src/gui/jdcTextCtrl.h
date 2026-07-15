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

class MainGui;

class JdcTextCtrl : public wxStyledTextCtrl 
{
private:
	char IsCharDigit(char c);

public:
	JdcTextCtrl(wxWindow* parent, MainGui* mainGuiRef, wxString name);

	MainGui* mainGui = nullptr;

	wxFindReplaceData findData;
	wxFindReplaceDialog* findDialog = nullptr;
	wxString lastFindText = "";
	int totalFindResults = 0;

	unsigned char highlightSelectedLines = 1;

	void EnableLineNumbers();

	void ClearText();

	void CenterLine(int line);

	void HighlightLine(int line, enum IndicatorColor color, unsigned char gotoLine);

	void ClearIndicators();

	void ShowRenameDialog(struct Function* function, struct JdcStr* currentName);

	void ShowFindDialog();

	void OnFindDialog(wxFindDialogEvent& e);

	int FindInRange(const wxString& text, int start, int end, int flags, unsigned char forward);

	int CountNumOfResults(const wxString& text, int end, int flags);

	void OnFindDialogClose(wxFindDialogEvent& e);

	void AddDefaultRightClickOptions(wxMenu* menu);

	void RightClickOptions(wxContextMenuEvent& e);

	void OnKeyDown(wxKeyEvent& e);

	void OnUpdateUI(wxStyledTextEvent& e);

	void HighlightSelectedBraces();

	void HighlightSelectionInstances();
};