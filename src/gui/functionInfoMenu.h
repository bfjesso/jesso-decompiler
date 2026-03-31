#include "guiUtils.h"
#include <wx/grid.h>
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

	wxGrid* conditionsGrid = nullptr;

	wxBoxSizer* vSizer1 = nullptr;
	wxBoxSizer* vSizer2 = nullptr;
	wxBoxSizer* hSizer = nullptr;

	enum ids
	{
		MainWindowID
	};

	void GridRightClickOptions(wxGridEvent& e);

	wxDECLARE_EVENT_TABLE();
};
