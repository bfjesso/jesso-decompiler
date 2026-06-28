#pragma once
#include "jdcTextCtrl.h"
#include "colorsMenu.h"

#include "../decompiler/decompilationStructs.h"

class StringsTextCtrl : public JdcTextCtrl
{
public:
	StringsTextCtrl(wxWindow* parent, struct DecompilationParameters* decompParams, ColorsMenu* colorMenu);

	ColorsMenu* colorsMenu = nullptr;

	struct DecompilationParameters* params;

	void LoadStrings();

	void ApplyStringsHighlighting();
};