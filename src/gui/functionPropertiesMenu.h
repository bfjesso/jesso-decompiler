#pragma once
#include "guiUtils.h"
#include "../decompiler/functions.h"

class FunctionPropertiesMenu : public wxFrame, public Utils
{
public:
	FunctionPropertiesMenu();

	wxStaticText* functionNameLabel = nullptr;
	wxTextCtrl* functionNameTextCtrl;

	std::vector<wxTextCtrl*> regArgNameTextCtrls;
	std::vector<wxTextCtrl*> stackArgNameTextCtrls;
	std::vector<wxTextCtrl*> localVarNameTextCtrls;

	wxBoxSizer* vSizer = nullptr;

	Function* currentFunction = 0;

	enum ids
	{
		MainWindowID
	};

	void OpenMenu(wxPoint position, Function* function);

	void CloseMenu(wxCloseEvent& e);

	wxDECLARE_EVENT_TABLE();
};
