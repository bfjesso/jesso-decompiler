#pragma once
#include "jdcTextCtrl.h"

class FunctionsTextCtrl : public JdcTextCtrl
{
public:
	FunctionsTextCtrl(wxWindow* parent, MainGui* mainGuiRef, wxString name);

	int entryFunctionIndex = -1;

	void ShowFindAddressDialog();

	void FunctionsRightClickOptions(wxContextMenuEvent& e);

	void OnFunctionsKeyDown(wxKeyEvent& e);

	wxString GenerateFunctionDefinition(int functionIndex, struct JdcStr* functionHeaderBuffer);

	void UpdateFunctionHeader(int functionIndex);

	void ShowAllFunctions(int highlightIndex);

	void ApplyFunctionsHighlighting(int start, int end);
};