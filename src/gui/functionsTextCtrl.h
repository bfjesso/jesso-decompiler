#pragma once
#include "jdcTextCtrl.h"

class MainGui;
class DecompilationTextCtrl;

class FunctionsTextCtrl : public JdcTextCtrl
{
public:
	FunctionsTextCtrl(MainGui* parent, wxString name);

	MainGui* mainGui = nullptr;

	void ShowFindAddressDialog();

	void FunctionsRightClickOptions(wxContextMenuEvent& e);

	void OnFunctionsKeyDown(wxKeyEvent& e);

	void ShowAllFunctions();

	void ApplyFunctionsHighlighting();
};