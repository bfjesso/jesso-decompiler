#include "logTextCtrl.h"

LogTextCtrl::LogTextCtrl(wxWindow* parent, MainGui* mainGuiRef) : JdcTextCtrl(parent, mainGuiRef, "Log")
{
	Bind(wxEVT_CONTEXT_MENU, &LogTextCtrl::LogRightClickOptions, this);
	Bind(wxEVT_STC_UPDATEUI, &LogTextCtrl::OnUpdateLogUI, this);

	highlightSelectedLines = 0;
	Hide();
	Log("JDC started", 0);
}

void LogTextCtrl::Log(wxString text, bool isError, bool addNewLine)
{
	SetReadOnly(false);
	AppendText(wxDateTime::Now().Format(wxT("%X")) + ": ");

	int32_t textStart = GetLength();

	if (addNewLine)
	{
		text += "\n";
	}

	AppendText(text);

	if (isError)
	{
		SetIndicatorCurrent(RED_INDICATOR);
		IndicatorFillRange(textStart, GetLength() - textStart);
	}

	SetReadOnly(true);
	Refresh();
	Update();
}

void LogTextCtrl::LogProgress(uint64_t value, bool updateCurrentProgress)
{
	int64_t now = wxGetLocalTimeMillis().GetValue();
	if (!updateCurrentProgress)
	{
		SetReadOnly(false);
		progressPos = GetLength();
		wxString numStr = wxString::Format("0x%llX\n", value);
		progressLen = (int32_t)(numStr.Length() - 1);
		AppendText(numStr);
		SetReadOnly(true);
	}
	else if (now - lastProgressUpdateMs > 1000)
	{
		SetReadOnly(false);

		wxString numStr = wxString::Format("0x%llX", value);
		SetTargetStart(progressPos);
		SetTargetEnd(progressPos + progressLen);
		ReplaceTarget(numStr);

		progressLen = (int32_t)numStr.Length();

		lastProgressUpdateMs = now;
		Refresh();
		Update();

		SetReadOnly(true);
	}
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