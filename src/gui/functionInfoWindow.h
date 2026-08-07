#include "guiUtils.h"
#include <wx/grid.h>
#include "../decompiler/decompilationStructs.h"

class FunctionInfoWindow : public wxWindow
{
public:
	FunctionInfoWindow(wxWindow* parent, DisassembledInstruction* instructions, Function* function);

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

	void GridRightClickOptions(wxGridEvent& e);

	wxDECLARE_EVENT_TABLE();
};