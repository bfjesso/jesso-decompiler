#include "logTextCtrl.h"

LogTextCtrl::LogTextCtrl(wxWindow* parent, MainGui* mainGuiRef) : JdcTextCtrl(parent, mainGuiRef, "Log")
{
	Bind(wxEVT_CONTEXT_MENU, &LogTextCtrl::LogRightClickOptions, this);
	Bind(wxEVT_STC_UPDATEUI, &LogTextCtrl::OnUpdateLogUI, this);

	highlightSelectedLines = 0;
	Hide();
	Log("JDC started", 0);
}

void LogTextCtrl::Log(wxString text, bool isError)
{
	SetReadOnly(false);
	AppendText(wxDateTime::Now().Format(wxT("%X")) + ": ");

	int32_t textStart = GetLength();
	AppendText(text + "\n");

	if (isError)
	{
		SetIndicatorCurrent(RED_INDICATOR);
		IndicatorFillRange(textStart, GetLength() - textStart);
	}

	SetReadOnly(true);
	Refresh();
	Update();
}

void LogTextCtrl::LogHexNum(wxString label, uint64_t num, bool isError)
{
	char numStr[20] = { 0 };
	sprintf(numStr, "0x%llX", num);

	Log(label + ": " + wxString(numStr), isError);
}

void LogTextCtrl::LogProgress(uint64_t current, uint64_t max)
{
	SetReadOnly(false);

	char numStr[20] = { 0 };
	if (max == 0) // update existing progress
	{
		sprintf(numStr, "0x%llX", current);

		int32_t endPos = GetText().find('/', progressPos);
		Replace(progressPos, endPos, numStr);
		Refresh();
		Update();
	}
	else 
	{
		progressPos = GetLength() + 1;
		sprintf(numStr, "0x%llX", current);
		AppendText("\t" + wxString(numStr) + "/");
		sprintf(numStr, "0x%llX", max);
		AppendText(wxString(numStr) + "\n");
	}

	SetReadOnly(true);
}

void LogTextCtrl::LogRightClickOptions(wxContextMenuEvent& e)
{
	wxMenu menu;

	AddDefaultRightClickOptions(&menu);

	const int32_t ID_CLEAR = 100;

	menu.Append(ID_CLEAR, "Clear");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
		ClearText();
	}, ID_CLEAR);

	PopupMenu(&menu, ScreenToClient(e.GetPosition()));
}

void LogTextCtrl::OnUpdateLogUI(wxStyledTextEvent& e) 
{
	if (!HasFocus())
	{
		return;
	}

	for (int32_t i = 0; i < NUM_OF_INDICATORS; i++)
	{
		if (i != RED_INDICATOR)
		{
			SetIndicatorCurrent(i);
			IndicatorClearRange(0, GetTextLength());
		}
	}

	if (highlightSelectedLines)
	{
		HighlightLine(GetCurrentLine(), YELLOW_INDICATOR, 0);
	}

	HighlightSelectedBraces();
	HighlightSelectionInstances();
}