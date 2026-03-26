#include "guiUtils.h"
#include "../decompiler/decompilationStructs.h"

class FunctionInfoMenu : public wxFrame, public Utils
{
public:
	FunctionInfoMenu(wxPoint position, Function* function);

    wxStaticText* returnTypeStaticTxt = nullptr;
    wxStaticText* returnRegStaticTxt = nullptr;
    wxStaticText* returnFunctionAddrStaticTxt = nullptr;
    wxStaticText* firstFuncCallAddrStaticTxt = nullptr;
    wxStaticText* indexOfFirstFuncCallStaticTxt = nullptr;
    wxStaticText* callingConventionStaticTxt = nullptr;
	wxStaticText* functionNameStaticTxt = nullptr;
    wxStaticText* numOfInstructionsStaticTxt = nullptr;

	wxBoxSizer* vSizer = nullptr;

	enum ids
	{
		MainWindowID
	};
};
