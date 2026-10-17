#pragma once
#include "jdcTextCtrl.h"

class LogTextCtrl : public JdcTextCtrl
{
public:
	LogTextCtrl(wxWindow* parent, MainGui* mainGuiRef);

	int32_t progressPos = 0;
	int32_t progressLen = 0;
	int64_t lastProgressUpdateMs = 0;

	void Log(wxString text, bool isError = false, bool addNewLine = true);

	void LogProgress(uint64_t value, bool updateCurrentProgress);

	void LogRightClickOptions(wxContextMenuEvent& e);

	void OnUpdateLogUI(wxStyledTextEvent& e);
};