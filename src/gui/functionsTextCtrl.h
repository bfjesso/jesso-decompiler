#pragma once
#include "jdcTextCtrl.h"

class FunctionsTextCtrl : public JdcTextCtrl
{
public:
	FunctionsTextCtrl(wxWindow* parent, MainGui* mainGuiRef, wxString name);

	void ShowFindAddressDialog();

	void FunctionsRightClickOptions(wxContextMenuEvent& e);

	void OnFunctionsKeyDown(wxKeyEvent& e);

	void ShowAllFunctions(int highlightIndex);

	void ApplyFunctionsHighlighting();
};