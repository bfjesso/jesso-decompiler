#pragma once
#include "jdcTextCtrl.h"

class StringsTextCtrl : public JdcTextCtrl
{
public:
	StringsTextCtrl(wxWindow* parent, MainGui* mainGuiRef);

	std::vector<uint64_t> foundAddresses;

	void ShowFindAddressDialog();

	void StringsRightClickOptions(wxContextMenuEvent& e);

	void OnStringsKeyDown(wxKeyEvent& e);

	void LoadStrings();

	void ApplyStringsHighlighting();
};