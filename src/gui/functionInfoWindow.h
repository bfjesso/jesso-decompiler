#include "guiUtils.h"
#include <wx/grid.h>
#include "../decompiler/decompilationStructs.h"

class FunctionInfoWindow : public wxScrolledWindow
{
public:
	FunctionInfoWindow(wxWindow* parent, DisassembledInstruction* instructionsPtr, Function* theFunction);

	Function* function = nullptr;
	DisassembledInstruction* instructions = nullptr;

	wxGrid* infoGrid = nullptr;
	wxGrid* regVarsGrid = nullptr;
	wxGrid* stackVarsGrid = nullptr;
	wxGrid* returnedVarsGrid = nullptr;
	wxGrid* conditionsGrid = nullptr;
	wxGrid* directJmpsGrid = nullptr;

	wxBoxSizer* vSizer = nullptr;

	void GridRightClickOptions(wxGridEvent& e);

	wxDECLARE_EVENT_TABLE();
};

class ScopesDialog : public wxDialog
{
public:
	ScopesDialog(wxWindow* parent, DisassembledInstruction* instructions, const RegisterVariable* regVar);
};