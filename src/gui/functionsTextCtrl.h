#pragma once
#include "jdcTextCtrl.h"

class MainGui;
class DecompilationTextCtrl;

class FunctionsTextCtrl : public JdcTextCtrl
{
public:
	FunctionsTextCtrl(MainGui* parent, wxString name);

	MainGui* mainGui = nullptr;

	void ShowAllFunctions();

	void ApplyFunctionsHighlighting();
};