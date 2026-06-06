#include "guiUtils.h"
#include <wx/grid.h>
#include "../decompiler/decompilationStructs.h"

class FunctionInfoMenu : public wxFrame, public Utils
{
public:
	FunctionInfoMenu(wxWindow* parent, wxPoint position, DisassembledInstruction* instructions, Function* function);

	wxGrid* infoGrid = nullptr;
	wxGrid* regVarsGrid = nullptr;
	wxGrid* stackVarsGrid = nullptr;
	wxGrid* returnedVarsGrid = nullptr;
	wxGrid* conditionsGrid = nullptr;
	wxGrid* directJmpsGrid = nullptr;

	wxBoxSizer* row1Sizer = nullptr;
	wxBoxSizer* row2Sizer = nullptr;
	wxBoxSizer* row3Sizer = nullptr;
	wxBoxSizer* vSizer = nullptr;

	enum ids
	{
		MainWindowID
	};

	void GridRightClickOptions(wxGridEvent& e);

	wxDECLARE_EVENT_TABLE();
};
