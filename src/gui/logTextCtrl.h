#pragma once
#include "jdcTextCtrl.h"

class LogTextCtrl : public JdcTextCtrl
{
public:
	LogTextCtrl(wxWindow* parent, MainGui* mainGuiRef);

	int32_t progressPos = 0;

	void Log(wxString text, bool isError);

	void LogHexNum(wxString label, uint64_t num, bool isError);

	void LogProgress(uint64_t current, uint64_t max);

	void LogRightClickOptions(wxContextMenuEvent& e);

	void OnUpdateLogUI(wxStyledTextEvent& e);
};