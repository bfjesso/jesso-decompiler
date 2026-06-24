#pragma once
#include "jdcTextCtrl.h"
#include "colorsMenu.h"

#include "../decompiler/decompilationStructs.h"

class FunctionsTextCtrl : public JdcTextCtrl
{
public:
	FunctionsTextCtrl(wxWindow* parent, const wxSize& size, ColorsMenu* colorMenu, wxStaticText* statusText);

	ColorsMenu* colorsMenu = nullptr;

	void ShowAllFunctions(struct DecompilationParameters* params);

	void ApplyFunctionsHighlighting();
};