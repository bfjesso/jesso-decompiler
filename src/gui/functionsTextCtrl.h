#pragma once
#include "jdcTextCtrl.h"

class MainGui;
class DecompilationTextCtrl;

class FunctionsTextCtrl : public JdcTextCtrl
{
public:
	FunctionsTextCtrl(MainGui* parent);

	MainGui* mainGui = nullptr;

	void ShowAllFunctions();

	void ApplyFunctionsHighlighting();
};