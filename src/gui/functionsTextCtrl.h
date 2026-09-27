#pragma once
#include "jdcTextCtrl.h"

class FunctionsTextCtrl : public JdcTextCtrl
{
public:
	FunctionsTextCtrl(wxWindow* parent, MainGui* mainGuiRef, wxString name);

	int32_t entryFunctionIndex = -1;

	void ShowFindAddressDialog();

	void FunctionsRightClickOptions(wxContextMenuEvent& e);

	void OnFunctionsKeyDown(wxKeyEvent& e);

	wxString GenerateFunctionDefinition(int32_t functionIndex, struct JdcStr* functionHeaderBuffer);

	void UpdateFunctionHeader(int32_t functionIndex);

	void ShowAllFunctions(int32_t highlightIndex);

	void ApplyFunctionsHighlighting(int32_t start, int32_t end);
};