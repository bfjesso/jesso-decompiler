#pragma once
#include "jdcTextCtrl.h"
#include "colorsMenu.h"

#include "../decompiler/decompilationStructs.h"

class DecompilationTextCtrl;

class FunctionsTextCtrl : public JdcTextCtrl
{
public:
	FunctionsTextCtrl(wxWindow* parent, const wxSize& size, struct DecompilationParameters* decompParams, ColorsMenu* colorMenu, wxStaticText* statusText);

	ColorsMenu* colorsMenu = nullptr;

	DecompilationTextCtrl* decompilationTextCtrl = nullptr;

	struct DecompilationParameters* params = nullptr;

	void SetAssociatedDecompilationTextCtrl(DecompilationTextCtrl* window);

	void ShowAllFunctions();

	void ApplyFunctionsHighlighting();
};