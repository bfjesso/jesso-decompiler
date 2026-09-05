#include "functionInfoWindow.h"
#include "../decompiler/dataTypes.h"
#include "../disassembler/registers.h"
#include "../jdc-str/jdcStr.h"

wxBEGIN_EVENT_TABLE(FunctionInfoWindow, wxScrolledWindow)
EVT_GRID_CELL_RIGHT_CLICK(FunctionInfoWindow::GridRightClickOptions)
wxEND_EVENT_TABLE()

FunctionInfoWindow::FunctionInfoWindow(wxWindow* parent, DisassembledInstruction* instructionsPtr, Function* theFunction) : wxScrolledWindow(parent, wxID_ANY)
{
	SetOwnBackgroundColour(backgroundColor);
	SetScrollRate(10, 10);

	function = theFunction;
	instructions = instructionsPtr;

	vSizer = new wxBoxSizer(wxVERTICAL);

	infoGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(200, 200));
	infoGrid->SetLabelBackgroundColour(foregroundColor);
	infoGrid->SetLabelTextColour(textColor);
	infoGrid->SetDefaultCellBackgroundColour(gridColor);
	infoGrid->SetDefaultCellTextColour(textColor);
	infoGrid->CreateGrid(0, 2);
	infoGrid->SetScrollRate(10, 10);
	infoGrid->DisableDragRowSize();
	infoGrid->EnableEditing(false);
	infoGrid->SetColLabelValue(0, "Function property");
	infoGrid->SetColLabelValue(1, "Value");
	infoGrid->HideRowLabels();
	infoGrid->SetColSize(0, 200);
	infoGrid->SetColSize(1, 200);
	infoGrid->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);
	infoGrid->AppendRows(8);

	struct JdcStr dataTypeStr = initializeJdcStr();
	dataTypeToStr(function->returnType, &dataTypeStr);
	infoGrid->SetCellValue(0, 0, "Return type");
	infoGrid->SetCellValue(0, 1, dataTypeStr.buffer);

	infoGrid->SetCellValue(1, 0, "Return register");
	infoGrid->SetCellValue(1, 1, registerStrs[function->returnReg]);

	infoGrid->SetCellValue(2, 0, "Calling convention");
	infoGrid->SetCellValue(2, 1, callingConventionStrs[function->callingConvention]);

	infoGrid->SetCellValue(3, 0, "Function name");
	infoGrid->SetCellValue(3, 1, function->name.buffer);

	infoGrid->SetCellValue(4, 0, "Number of instructions");
	infoGrid->SetCellValue(4, 1, std::to_string(function->lastInstructionIndex - function->firstInstructionIndex + 1));

	char hexNumStr[10] = { 0 };
	sprintf(hexNumStr, "0x%llX", instructions[function->firstInstructionIndex].address);
	infoGrid->SetCellValue(5, 0, "First instruction index");
	infoGrid->SetCellValue(5, 1, wxString(std::to_string(function->firstInstructionIndex)) + " (" + wxString(hexNumStr) + ")");

	sprintf(hexNumStr, "0x%llX", instructions[function->lastInstructionIndex].address);
	infoGrid->SetCellValue(6, 0, "Last instruction index");
	infoGrid->SetCellValue(6, 1, wxString(std::to_string(function->lastInstructionIndex)) + " (" + wxString(hexNumStr) + ")");

	infoGrid->SetCellValue(7, 0, "Number of lines");
	infoGrid->SetCellValue(7, 1, std::to_string(function->numOfLines));

	vSizer->Add(infoGrid, 1, wxEXPAND | wxALL, 10);

	returnedVarsGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(900, 200));
	returnedVarsGrid->SetLabelBackgroundColour(foregroundColor);
	returnedVarsGrid->SetLabelTextColour(textColor);
	returnedVarsGrid->SetDefaultCellBackgroundColour(gridColor);
	returnedVarsGrid->SetDefaultCellTextColour(textColor);
	returnedVarsGrid->CreateGrid(0, 5);
	returnedVarsGrid->SetScrollRate(10, 10);
	returnedVarsGrid->DisableDragRowSize();
	returnedVarsGrid->EnableEditing(false);
	returnedVarsGrid->SetColLabelValue(0, "Returned var type");
	returnedVarsGrid->SetColLabelValue(1, "Name");
	returnedVarsGrid->SetColLabelValue(2, "Callee address");
	returnedVarsGrid->SetColLabelValue(3, "Call instruction address");
	returnedVarsGrid->SetColLabelValue(4, "Return reg");
	returnedVarsGrid->HideRowLabels();
	returnedVarsGrid->SetColSize(0, 150);
	returnedVarsGrid->SetColSize(1, 150);
	returnedVarsGrid->SetColSize(2, 150);
	returnedVarsGrid->SetColSize(3, 200);
	returnedVarsGrid->SetColSize(4, 100);
	returnedVarsGrid->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	for (int i = 0; i < function->numOfReturnedVars; i++)
	{
		struct ReturnedVariable* returnedVar = &function->returnedVars[i];
		returnedVarsGrid->AppendRows(1);
		dataTypeToStr(returnedVar->dataType, &dataTypeStr);
		returnedVarsGrid->SetCellValue(i, 0, wxString(dataTypeStr.buffer));
		returnedVarsGrid->SetCellValue(i, 1, wxString(returnedVar->name.buffer));
		sprintf(hexNumStr, "0x%llX", returnedVar->calleeAddress);
		returnedVarsGrid->SetCellValue(i, 2, wxString(hexNumStr));
		sprintf(hexNumStr, "0x%llX", returnedVar->callInstructionAddress);
		returnedVarsGrid->SetCellValue(i, 3, wxString(hexNumStr));
		returnedVarsGrid->SetCellValue(i, 4, wxString(registerStrs[returnedVar->returnReg]));
	}

	vSizer->Add(returnedVarsGrid, 1, wxEXPAND | wxRIGHT | wxLEFT | wxBOTTOM, 10);

	regVarsGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(400, 200));
	regVarsGrid->SetLabelBackgroundColour(foregroundColor);
	regVarsGrid->SetLabelTextColour(textColor);
	regVarsGrid->SetDefaultCellBackgroundColour(gridColor);
	regVarsGrid->SetDefaultCellTextColour(textColor);
	regVarsGrid->CreateGrid(0, 4);
	regVarsGrid->SetScrollRate(10, 10);
	regVarsGrid->DisableDragRowSize();
	regVarsGrid->EnableEditing(false);
	regVarsGrid->SetColLabelValue(0, "Reg var type");
	regVarsGrid->SetColLabelValue(1, "Name");
	regVarsGrid->SetColLabelValue(2, "Register");
	regVarsGrid->SetColLabelValue(3, "Is argument");
	regVarsGrid->HideRowLabels();
	regVarsGrid->SetColSize(0, 100);
	regVarsGrid->SetColSize(1, 100);
	regVarsGrid->SetColSize(2, 100);
	regVarsGrid->SetColSize(3, 100);
	regVarsGrid->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	vSizer->Add(regVarsGrid, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

	for (int i = 0; i < function->numOfRegVars; i++)
	{
		struct RegisterVariable* regVar = &function->regVars[i];
		regVarsGrid->AppendRows(1);
		dataTypeToStr(regVar->dataType, &dataTypeStr);
		regVarsGrid->SetCellValue(i, 0, wxString(dataTypeStr.buffer));
		regVarsGrid->SetCellValue(i, 1, wxString(regVar->name.buffer));
		regVarsGrid->SetCellValue(i, 2, wxString(registerStrs[regVar->reg]));
		regVarsGrid->SetCellValue(i, 3, regVar->isArgument ? "Yes" : "No");
	}

	stackVarsGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(400, 200));
	stackVarsGrid->SetLabelBackgroundColour(foregroundColor);
	stackVarsGrid->SetLabelTextColour(textColor);
	stackVarsGrid->SetDefaultCellBackgroundColour(gridColor);
	stackVarsGrid->SetDefaultCellTextColour(textColor);
	stackVarsGrid->CreateGrid(0, 4);
	stackVarsGrid->SetScrollRate(10, 10);
	stackVarsGrid->DisableDragRowSize();
	stackVarsGrid->EnableEditing(false);
	stackVarsGrid->SetColLabelValue(0, "Stack var type");
	stackVarsGrid->SetColLabelValue(1, "Name");
	stackVarsGrid->SetColLabelValue(2, "Initial SP offset");
	stackVarsGrid->SetColLabelValue(3, "Is argument");
	stackVarsGrid->HideRowLabels();
	stackVarsGrid->SetColSize(0, 150);
	stackVarsGrid->SetColSize(1, 100);
	stackVarsGrid->SetColSize(2, 150);
	stackVarsGrid->SetColSize(3, 100);
	stackVarsGrid->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	vSizer->Add(stackVarsGrid, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

	for (int i = 0; i < function->numOfStackVars; i++)
	{
		struct StackVariable* stackVar = &function->stackVars[i];
		stackVarsGrid->AppendRows(1);
		dataTypeToStr(stackVar->dataType, &dataTypeStr);
		if (stackVar->offsetFromInitSP > 0)
		{
			sprintf(hexNumStr, "0x%llX", stackVar->offsetFromInitSP);
		}
		else
		{
			sprintf(hexNumStr, "-0x%llX", -stackVar->offsetFromInitSP);
		}
		stackVarsGrid->SetCellValue(i, 0, wxString(dataTypeStr.buffer));
		stackVarsGrid->SetCellValue(i, 1, wxString(stackVar->name.buffer));
		stackVarsGrid->SetCellValue(i, 2, wxString(hexNumStr));
		stackVarsGrid->SetCellValue(i, 3, stackVar->isArgument ? "Yes" : "No");
	}

	freeJdcStr(&dataTypeStr);

	conditionsGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(900, 200));
	conditionsGrid->SetLabelBackgroundColour(foregroundColor);
	conditionsGrid->SetLabelTextColour(textColor);
	conditionsGrid->SetDefaultCellBackgroundColour(gridColor);
	conditionsGrid->SetDefaultCellTextColour(textColor);
	conditionsGrid->CreateGrid(0, 8);
	conditionsGrid->SetScrollRate(10, 10);
	conditionsGrid->DisableDragRowSize();
	conditionsGrid->EnableEditing(false);
	conditionsGrid->SetColLabelValue(0, "Condition type");
	conditionsGrid->SetColLabelValue(1, "First body index");
	conditionsGrid->SetColLabelValue(2, "Last body index");
	conditionsGrid->SetColLabelValue(3, "Jcc index");
	conditionsGrid->SetColLabelValue(4, "Dst index");
	conditionsGrid->SetColLabelValue(5, "Exit index");
	conditionsGrid->SetColLabelValue(6, "Combined jcc indexes");
	conditionsGrid->SetColLabelValue(7, "Jcc combination type");
	conditionsGrid->HideRowLabels();
	conditionsGrid->SetColSize(0, 150);
	conditionsGrid->SetColSize(1, 200);
	conditionsGrid->SetColSize(2, 200);
	conditionsGrid->SetColSize(3, 200);
	conditionsGrid->SetColSize(4, 200);
	conditionsGrid->SetColSize(5, 200);
	conditionsGrid->SetColSize(6, 200);
	conditionsGrid->SetColSize(7, 200);
	conditionsGrid->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	vSizer->Add(conditionsGrid, 1, wxEXPAND | wxRIGHT | wxLEFT | wxBOTTOM, 10);

	for (int i = 0; i < function->numOfConditions; i++)
	{
		struct Condition* condition = &function->conditions[i];
		conditionsGrid->AppendRows(1);
		conditionsGrid->SetCellValue(i, 0, wxString(conditionTypeStrs[condition->conditionType]));

		if (condition->firstBodyIndex == -1)
		{
			strcpy(hexNumStr, "none");
		}
		else
		{
			sprintf(hexNumStr, "0x%llX", instructions[condition->firstBodyIndex].address);
		}
		conditionsGrid->SetCellValue(i, 1, wxString(std::to_string(condition->firstBodyIndex)) + " (" + wxString(hexNumStr) + ")");

		if (condition->lastBodyIndex == -1)
		{
			strcpy(hexNumStr, "none");
		}
		else
		{
			sprintf(hexNumStr, "0x%llX", instructions[condition->lastBodyIndex].address);
		}
		conditionsGrid->SetCellValue(i, 2, wxString(std::to_string(condition->lastBodyIndex)) + " (" + wxString(hexNumStr) + ")");

		sprintf(hexNumStr, "0x%llX", instructions[condition->jccIndex].address);
		conditionsGrid->SetCellValue(i, 3, wxString(std::to_string(condition->jccIndex)) + " (" + wxString(hexNumStr) + ")");
		sprintf(hexNumStr, "0x%llX", instructions[condition->dstIndex].address);
		conditionsGrid->SetCellValue(i, 4, wxString(std::to_string(condition->dstIndex)) + " (" + wxString(hexNumStr) + ")");

		if (condition->exitIndex == -1)
		{
			strcpy(hexNumStr, "none");
		}
		else
		{
			sprintf(hexNumStr, "0x%llX", instructions[condition->exitIndex].address);
		}
		conditionsGrid->SetCellValue(i, 5, wxString(std::to_string(condition->exitIndex)) + " (" + wxString(hexNumStr) + ")");

		wxString combinedJccsStr = "";
		for (int j = 0; j < condition->numOfCombinedJccs; j++)
		{
			sprintf(hexNumStr, "0x%llX", instructions[condition->combinedJccIndexes[j]].address);
			combinedJccsStr += wxString(std::to_string(condition->combinedJccIndexes[j]) + " (" + wxString(hexNumStr) + ")");
			if (j != condition->numOfCombinedJccs - 1)
			{
				combinedJccsStr += ", ";
			}
		}
		conditionsGrid->SetCellValue(i, 6, combinedJccsStr);
		conditionsGrid->SetCellValue(i, 7, wxString(logicalTypeStrs[condition->combinedJccsLogicType]));
	}

	directJmpsGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(800, 200));
	directJmpsGrid->SetLabelBackgroundColour(foregroundColor);
	directJmpsGrid->SetLabelTextColour(textColor);
	directJmpsGrid->SetDefaultCellBackgroundColour(gridColor);
	directJmpsGrid->SetDefaultCellTextColour(textColor);
	directJmpsGrid->CreateGrid(0, 3);
	directJmpsGrid->SetScrollRate(10, 10);
	directJmpsGrid->DisableDragRowSize();
	directJmpsGrid->EnableEditing(false);
	directJmpsGrid->SetColLabelValue(0, "Direct jmp type");
	directJmpsGrid->SetColLabelValue(1, "Jmp index");
	directJmpsGrid->SetColLabelValue(2, "Dst index");
	directJmpsGrid->HideRowLabels();
	directJmpsGrid->SetColSize(0, 150);
	directJmpsGrid->SetColSize(1, 200);
	directJmpsGrid->SetColSize(2, 200);
	directJmpsGrid->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	vSizer->Add(directJmpsGrid, 1, wxEXPAND | wxRIGHT | wxLEFT | wxBOTTOM, 10);

	for (int i = 0; i < function->numOfDirectJmps; i++)
	{
		struct DirectJmp* directJmp = &function->directJmps[i];
		directJmpsGrid->AppendRows(1);
		directJmpsGrid->SetCellValue(i, 0, wxString(directJmpTypeStrs[directJmp->type]));
		sprintf(hexNumStr, "0x%llX", instructions[directJmp->jmpIndex].address);
		directJmpsGrid->SetCellValue(i, 1, wxString(std::to_string(directJmp->jmpIndex)) + " (" + wxString(hexNumStr) + ")");
		sprintf(hexNumStr, "0x%llX", instructions[directJmp->dstIndex].address);
		directJmpsGrid->SetCellValue(i, 2, wxString(std::to_string(directJmp->dstIndex)) + " (" + wxString(hexNumStr) + ")");
	}

	SetSizerAndFit(vSizer);
	SetMinSize(wxSize(200, 200));
}

void FunctionInfoWindow::GridRightClickOptions(wxGridEvent& e)
{
	wxMenu menu;

	wxGrid* grid = (wxGrid*)(e.GetEventObject());

	int row = e.GetRow();
	int col = e.GetCol();

	const int ID_COPY = 100;
	const int ID_SHOW_SCOPES = 101;
	
	menu.Append(ID_COPY, "Copy");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(grid->GetCellValue(row, col)); }, ID_COPY);

	if (grid == regVarsGrid) 
	{
		struct RegisterVariable* regVar = &function->regVars[row];
		if (regVar->numOfScopes > 0) 
		{
			menu.Append(ID_SHOW_SCOPES, "Show scopes");
			menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void {
				ScopesDialog* dlg = new ScopesDialog(this, instructions, regVar);
				dlg->Bind(wxEVT_CLOSE_WINDOW, [dlg](wxCloseEvent& evt) {
					dlg->Destroy();
					evt.Skip();
				});

				dlg->Show();
			}, ID_SHOW_SCOPES);
		}
	}

	PopupMenu(&menu, ScreenToClient(wxGetMousePosition()));
	e.Skip();
}

ScopesDialog::ScopesDialog(wxWindow* parent, DisassembledInstruction* instructions, const RegisterVariable* regVar) : wxDialog(parent, wxID_ANY, wxString(regVar->name.buffer) + " scopes", wxDefaultPosition, wxSize(700, 300), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
{
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	wxGrid* grid = new wxGrid(this, wxID_ANY);

	grid->SetLabelBackgroundColour(foregroundColor);
	grid->SetLabelTextColour(textColor);
	grid->SetDefaultCellBackgroundColour(gridColor);
	grid->SetDefaultCellTextColour(textColor);
	grid->CreateGrid(0, 3);
	grid->SetScrollRate(10, 10);
	grid->DisableDragRowSize();
	grid->EnableEditing(false);
	grid->SetColLabelValue(0, "Scope index");
	grid->SetColLabelValue(1, "Start index");
	grid->SetColLabelValue(2, "End index");
	grid->HideRowLabels();
	grid->SetColSize(0, 100);
	grid->SetColSize(1, 200);
	grid->SetColSize(2, 200);
	grid->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	char hexNumStr[10] = { 0 };
	for (int i = 0; i < regVar->numOfScopes; i++)
	{
		grid->AppendRows(1);

		grid->SetCellValue(i, 0, std::to_string(i));

		sprintf(hexNumStr, "0x%llX", instructions[regVar->scopes[i].startIndex].address);
		grid->SetCellValue(i, 1, std::to_string(regVar->scopes[i].startIndex) + " (" + wxString(hexNumStr) + ")");

		sprintf(hexNumStr, "0x%llX", instructions[regVar->scopes[i].endIndex].address);
		grid->SetCellValue(i, 2, std::to_string(regVar->scopes[i].endIndex) + " (" + wxString(hexNumStr) + ")");
	}

	sizer->Add(grid, 1, wxEXPAND | wxALL, 10);
	SetSizerAndFit(sizer);
	SetMinSize(wxSize(200, 200));
}