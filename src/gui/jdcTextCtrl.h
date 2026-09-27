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
	int32_t totalFindResults = 0;

	bool highlightSelectedLines = true;

	void EnableLineNumbers();

	void ClearText();

	void CenterLine(int32_t line);

	void HighlightLine(int32_t line, enum IndicatorColor color, bool gotoLine);

	void ClearIndicators();

	void ShowRenameDialog(int32_t functionIndex, struct JdcStr* currentName);

	void ShowFindDialog();

	void OnFindDialog(wxFindDialogEvent& e);

	int32_t FindInRange(const wxString& text, int32_t start, int32_t end, int32_t flags, bool forward);

	int32_t CountNumOfResults(const wxString& text, int32_t end, int32_t flags);

	void OnFindDialogClose(wxFindDialogEvent& e);

	void AddDefaultRightClickOptions(wxMenu* menu);

	void RightClickOptions(wxContextMenuEvent& e);

	void OnKeyDown(wxKeyEvent& e);

	void OnUpdateUI(wxStyledTextEvent& e);

	void HighlightSelectedBraces();

	void HighlightSelectionInstances();
};