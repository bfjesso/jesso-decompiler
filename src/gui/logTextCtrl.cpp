#include "logTextCtrl.h"

LogTextCtrl::LogTextCtrl(wxWindow* parent, MainGui* mainGuiRef) : JdcTextCtrl(parent, mainGuiRef, "Log")
{
	Bind(wxEVT_STC_UPDATEUI, &LogTextCtrl::OnUpdateLogUI, this);

	highlightSelectedLines = 0;
	Hide();
	Log("JDC started", 0);
}

void LogTextCtrl::Log(wxString text, unsigned char isError)
{
	SetReadOnly(false);
	AppendText(wxDateTime::Now().Format(wxT("%X")) + ": ");

	int textStart = GetLength();
	AppendText(text + "\n");

	if (isError)
	{
		SetIndicatorCurrent(RED_INDICATOR);
		IndicatorFillRange(textStart, GetLength() - textStart);
	}

	SetReadOnly(true);
}

void LogTextCtrl::LogHexNum(wxString label, unsigned long long num, unsigned char isError)
{
	char numStr[20] = { 0 };
	sprintf(numStr, "0x%llX", num);

	Log(label + ": " + wxString(numStr), isError);
}

void LogTextCtrl::OnUpdateLogUI(wxStyledTextEvent& e) 
{
	if (!HasFocus())
	{
		return;
	}

	for (int i = 0; i < NUM_OF_INDICATORS; i++)
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