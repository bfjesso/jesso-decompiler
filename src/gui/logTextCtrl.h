#pragma once
#include "jdcTextCtrl.h"

class LogTextCtrl : public JdcTextCtrl
{
public:
	LogTextCtrl(wxWindow* parent, MainGui* mainGuiRef);

	int progressPos = 0;

	void Log(wxString text, unsigned char isError);

	void LogHexNum(wxString label, unsigned long long num, unsigned char isError);

	void LogProgress(unsigned long long current, unsigned long long max);

	void LogRightClickOptions(wxContextMenuEvent& e);

	void OnUpdateLogUI(wxStyledTextEvent& e);
};