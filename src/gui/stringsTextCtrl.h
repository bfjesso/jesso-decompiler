#pragma once
#include "jdcTextCtrl.h"
#include "colorsMenu.h"

#include "../decompiler/decompilationStructs.h"

class StringsTextCtrl : public JdcTextCtrl
{
public:
	StringsTextCtrl(wxWindow* parent, wxString name, struct DecompilationParameters* decompParams, ColorsMenu* colorMenu);

	ColorsMenu* colorsMenu = nullptr;

	struct DecompilationParameters* params;
	std::vector<unsigned long long> addresses;

	void ShowFindAddressDialog();

	void StringsRightClickOptions(wxContextMenuEvent& e);

	void OnStringsKeyDown(wxKeyEvent& e);

	void LoadStrings();

	void ApplyStringsHighlighting();
};