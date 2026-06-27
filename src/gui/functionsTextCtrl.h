#pragma once
#include "jdcTextCtrl.h"
#include "colorsMenu.h"

#include "../decompiler/decompilationStructs.h"

class DecompilationTextCtrl;

class FunctionsTextCtrl : public JdcTextCtrl
{
public:
	FunctionsTextCtrl(wxWindow* parent, struct DecompilationParameters* decompParams, ColorsMenu* colorMenu, const std::function<DecompilationTextCtrl* ()>& getDecompTextCtrl);

	ColorsMenu* colorsMenu = nullptr;

	std::function<DecompilationTextCtrl* ()> getDecompilationTextCtrl;

	struct DecompilationParameters* params = nullptr;

	void ShowAllFunctions();

	void ApplyFunctionsHighlighting();
};