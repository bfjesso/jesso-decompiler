#pragma once
#include "jdcTextCtrl.h"

class LogTextCtrl : public JdcTextCtrl
{
public:
	LogTextCtrl(wxWindow* parent, MainGui* mainGuiRef);

	void Log(wxString text, unsigned char isError);

	void LogHexNum(wxString label, unsigned long long num, unsigned char isError);

	void OnUpdateLogUI(wxStyledTextEvent& e);
};