#include "functionInfoMenu.h"
#include "../decompiler/dataTypes.h"
#include "../disassembler/registers.h"
#include "../jdc-str/jdcStr.h"

wxBEGIN_EVENT_TABLE(FunctionInfoMenu, wxFrame)
EVT_GRID_CELL_RIGHT_CLICK(FunctionInfoMenu::GridRightClickOptions)
wxEND_EVENT_TABLE()

FunctionInfoMenu::FunctionInfoMenu(wxWindow* parent, wxPoint position, DisassembledInstruction* instructions, Function* function) : wxFrame(parent, MainWindowID, "Function Info", wxPoint(50, 50), wxSize(900, 600))
{
    SetOwnBackgroundColour(backgroundColor);

	row1Sizer = new wxBoxSizer(wxHORIZONTAL);
	row2Sizer = new wxBoxSizer(wxHORIZONTAL);
	row3Sizer = new wxBoxSizer(wxHORIZONTAL);
	vSizer = new wxBoxSizer(wxVERTICAL);

	infoGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(400, 200));
	infoGrid->SetLabelBackgroundColour(foregroundColor);
	infoGrid->SetLabelTextColour(textColor);
	infoGrid->SetDefaultCellBackgroundColour(gridColor);
	infoGrid->SetDefaultCellTextColour(textColor);
	infoGrid->CreateGrid(0, 2);
	infoGrid->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
	infoGrid->SetScrollRate(0, 10);
	infoGrid->DisableDragRowSize();
	infoGrid->EnableEditing(false);
	infoGrid->SetColLabelValue(0, "Function property");
	infoGrid->SetColLabelValue(1, "Value");
	infoGrid->HideRowLabels();
	infoGrid->SetColSize(0, 200);
	infoGrid->SetColSize(1, 9999);
	infoGrid->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);
	infoGrid->AppendRows(7);

    struct JdcStr dataTypeStr = initializeJdcStr();
    dataTypeToStr(function->returnType, &dataTypeStr);
	infoGrid->SetCellValue(0, 0, "Return type");
	infoGrid->SetCellValue(0, 1, dataTypeStr.buffer);

	infoGrid->SetCellValue(1, 0, "Return register");
	infoGrid->SetCellValue(1, 1, registerStrs[function->returnReg]);

    char hexNumStr[10] = { 0 };
    sprintf(hexNumStr, "0x%llX", function->returningFunctionAddress);
	infoGrid->SetCellValue(2, 0, "Returning function address");
	infoGrid->SetCellValue(2, 1, hexNumStr);

    sprintf(hexNumStr, "0x%llX", instructions[function->firstFuncCallInstructionIndex].address);
	infoGrid->SetCellValue(3, 0, "First call instruction index");
	infoGrid->SetCellValue(3, 1, wxString(std::to_string(function->firstFuncCallInstructionIndex)) + " (" + wxString(hexNumStr) + ")");

	infoGrid->SetCellValue(4, 0, "Calling convention");
	infoGrid->SetCellValue(4, 1, callingConventionStrs[function->callingConvention]);

	infoGrid->SetCellValue(5, 0, "Function name");
	infoGrid->SetCellValue(5, 1, function->name.buffer);

	infoGrid->SetCellValue(6, 0, "Number of instructions");
	infoGrid->SetCellValue(6, 1, std::to_string(function->lastInstructionIndex - function->firstInstructionIndex + 1));

	row1Sizer->Add(infoGrid, 1, wxEXPAND | wxLEFT | wxRIGHT, 10);

	returnedVarsGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(900, 200));
	returnedVarsGrid->SetLabelBackgroundColour(foregroundColor);
	returnedVarsGrid->SetLabelTextColour(textColor);
	returnedVarsGrid->SetDefaultCellBackgroundColour(gridColor);
	returnedVarsGrid->SetDefaultCellTextColour(textColor);
	returnedVarsGrid->CreateGrid(0, 5);
	returnedVarsGrid->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
	returnedVarsGrid->SetScrollRate(0, 10);
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
	returnedVarsGrid->SetColSize(3, 150);
	returnedVarsGrid->SetColSize(4, 9999);
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

	row1Sizer->Add(returnedVarsGrid, 2, wxEXPAND | wxRIGHT, 10);

    regVarsGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(400, 200));
	regVarsGrid->SetLabelBackgroundColour(foregroundColor);
	regVarsGrid->SetLabelTextColour(textColor);
	regVarsGrid->SetDefaultCellBackgroundColour(gridColor);
	regVarsGrid->SetDefaultCellTextColour(textColor);
	regVarsGrid->CreateGrid(0, 4);
	regVarsGrid->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
	regVarsGrid->SetScrollRate(0, 10);
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
	regVarsGrid->SetColSize(3, 9999);
	regVarsGrid->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	row2Sizer->Add(regVarsGrid, 1, wxEXPAND | wxLEFT | wxRIGHT, 10);

	for (int i = 0; i < function->numOfRegArgs; i++)
	{
		struct RegisterVariable* regArg = &function->regArgs[i];
		regVarsGrid->AppendRows(1);
		dataTypeToStr(regArg->dataType, &dataTypeStr);
		regVarsGrid->SetCellValue(i, 0, wxString(dataTypeStr.buffer));
		regVarsGrid->SetCellValue(i, 1, wxString(regArg->name.buffer));
		regVarsGrid->SetCellValue(i, 2, wxString(registerStrs[regArg->reg]));
		regVarsGrid->SetCellValue(i, 3, "Yes");
	}
	for (int i = 0; i < function->numOfRegVars; i++)
	{
		struct RegisterVariable* regVar = &function->regVars[i];
		regVarsGrid->AppendRows(1);
		dataTypeToStr(regVar->dataType, &dataTypeStr);
		regVarsGrid->SetCellValue(i + function->numOfRegArgs, 0, wxString(dataTypeStr.buffer));
		regVarsGrid->SetCellValue(i + function->numOfRegArgs, 1, wxString(regVar->name.buffer));
		regVarsGrid->SetCellValue(i + function->numOfRegArgs, 2, wxString(registerStrs[regVar->reg]));
		regVarsGrid->SetCellValue(i + function->numOfRegArgs, 3, "No");
	}

	stackVarsGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(400, 200));
	stackVarsGrid->SetLabelBackgroundColour(foregroundColor);
	stackVarsGrid->SetLabelTextColour(textColor);
	stackVarsGrid->SetDefaultCellBackgroundColour(gridColor);
	stackVarsGrid->SetDefaultCellTextColour(textColor);
	stackVarsGrid->CreateGrid(0, 4);
	stackVarsGrid->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
	stackVarsGrid->SetScrollRate(0, 10);
	stackVarsGrid->DisableDragRowSize();
	stackVarsGrid->EnableEditing(false);
	stackVarsGrid->SetColLabelValue(0, "Stack var type");
	stackVarsGrid->SetColLabelValue(1, "Name");
	stackVarsGrid->SetColLabelValue(2, "BP offset");
	stackVarsGrid->SetColLabelValue(3, "Is argument");
	stackVarsGrid->HideRowLabels();
	stackVarsGrid->SetColSize(0, 100);
	stackVarsGrid->SetColSize(1, 100);
	stackVarsGrid->SetColSize(2, 100);
	stackVarsGrid->SetColSize(3, 9999);
	stackVarsGrid->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	row3Sizer->Add(stackVarsGrid, 1, wxEXPAND | wxLEFT | wxRIGHT, 10);

	for (int i = 0; i < function->numOfStackArgs; i++)
	{
		struct StackVariable* stackArg = &function->stackArgs[i];
		stackVarsGrid->AppendRows(1);
		dataTypeToStr(stackArg->dataType, &dataTypeStr);
		if (stackArg->stackOffset > 0)
		{
			sprintf(hexNumStr, "0x%llX", stackArg->stackOffset);
		}
		else
		{
			sprintf(hexNumStr, "-0x%llX", -stackArg->stackOffset);
		}
		stackVarsGrid->SetCellValue(i, 0, wxString(dataTypeStr.buffer));
		stackVarsGrid->SetCellValue(i, 1, wxString(stackArg->name.buffer));
		stackVarsGrid->SetCellValue(i, 2, wxString(hexNumStr));
		stackVarsGrid->SetCellValue(i, 3, "Yes");
	}
	for (int i = 0; i < function->numOfStackVars; i++)
	{
		struct StackVariable* stackVar = &function->stackVars[i];
		stackVarsGrid->AppendRows(1);
		dataTypeToStr(stackVar->dataType, &dataTypeStr);
		if (stackVar->stackOffset > 0)
		{
			sprintf(hexNumStr, "0x%llX", stackVar->stackOffset);
		}
		else
		{
			sprintf(hexNumStr, "-0x%llX", -stackVar->stackOffset);
		}
		stackVarsGrid->SetCellValue(i + function->numOfStackArgs, 0, wxString(dataTypeStr.buffer));
		stackVarsGrid->SetCellValue(i + function->numOfStackArgs, 1, wxString(stackVar->name.buffer));
		stackVarsGrid->SetCellValue(i + function->numOfStackArgs, 2, wxString(hexNumStr));
		stackVarsGrid->SetCellValue(i + function->numOfStackArgs, 3, "No");
	}

	freeJdcStr(&dataTypeStr);

	conditionsGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(900, 200));
	conditionsGrid->SetLabelBackgroundColour(foregroundColor);
	conditionsGrid->SetLabelTextColour(textColor);
	conditionsGrid->SetDefaultCellBackgroundColour(gridColor);
	conditionsGrid->SetDefaultCellTextColour(textColor);
	conditionsGrid->CreateGrid(0, 8);
	conditionsGrid->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
	conditionsGrid->SetScrollRate(0, 10);
	conditionsGrid->DisableDragRowSize();
	conditionsGrid->EnableEditing(false);
	conditionsGrid->SetColLabelValue(0, "Condition type");
	conditionsGrid->SetColLabelValue(1, "Start index");
	conditionsGrid->SetColLabelValue(2, "End index");
	conditionsGrid->SetColLabelValue(3, "Jcc index");
	conditionsGrid->SetColLabelValue(4, "Dst index");
	conditionsGrid->SetColLabelValue(5, "Exit index");
	conditionsGrid->SetColLabelValue(6, "Combined jcc indexes");
	conditionsGrid->SetColLabelValue(7, "Jcc combination type");
	conditionsGrid->HideRowLabels();
	conditionsGrid->SetColSize(0, 100);
	conditionsGrid->SetColSize(1, 100);
	conditionsGrid->SetColSize(2, 100);
	conditionsGrid->SetColSize(3, 100);
	conditionsGrid->SetColSize(4, 100);
	conditionsGrid->SetColSize(5, 100);
	conditionsGrid->SetColSize(6, 150);
	conditionsGrid->SetColSize(7, 9999);
	conditionsGrid->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	row2Sizer->Add(conditionsGrid, 2, wxEXPAND | wxRIGHT, 10);

	for (int i = 0; i < function->numOfConditions; i++)
	{
		struct Condition* condition = &function->conditions[i];
		conditionsGrid->AppendRows(1);
		conditionsGrid->SetCellValue(i, 0, wxString(conditionTypeStrs[condition->conditionType]));

		sprintf(hexNumStr, "0x%llX", instructions[condition->startIndex].address);
		conditionsGrid->SetCellValue(i, 1, wxString(std::to_string(condition->startIndex)) + " (" + wxString(hexNumStr) + ")");
		sprintf(hexNumStr, "0x%llX", instructions[condition->endIndex].address);
		conditionsGrid->SetCellValue(i, 2, wxString(std::to_string(condition->endIndex)) + " (" + wxString(hexNumStr) + ")");

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
	directJmpsGrid->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
	directJmpsGrid->SetScrollRate(0, 10);
	directJmpsGrid->DisableDragRowSize();
	directJmpsGrid->EnableEditing(false);
	directJmpsGrid->SetColLabelValue(0, "Direct jmp type");
	directJmpsGrid->SetColLabelValue(1, "Jmp index");
	directJmpsGrid->SetColLabelValue(2, "Dst index");
	directJmpsGrid->HideRowLabels();
	directJmpsGrid->SetColSize(0, 100);
	directJmpsGrid->SetColSize(1, 100);
	directJmpsGrid->SetColSize(2, 9999);
	directJmpsGrid->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	row3Sizer->Add(directJmpsGrid, 2, wxEXPAND | wxRIGHT, 10);

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

	vSizer->Add(row1Sizer, 0, wxEXPAND | wxBOTTOM | wxUP, 10);
	vSizer->Add(row2Sizer, 0, wxEXPAND | wxBOTTOM, 10);
	vSizer->Add(row3Sizer, 0, wxEXPAND | wxBOTTOM, 10);

	SetSizerAndFit(vSizer);

	position.x += 10;
	position.y += 10;
	SetPosition(position);
	Show();
	Raise();
}

void FunctionInfoMenu::GridRightClickOptions(wxGridEvent& e)
{
	wxMenu menu;

	wxGrid* grid = (wxGrid*)(e.GetEventObject());

	int row = e.GetRow();
	int col = e.GetCol();

	const int ID_COPY = 100;
	
	menu.Append(ID_COPY, "Copy");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(grid->GetCellValue(row, col)); }, ID_COPY);

	PopupMenu(&menu, ScreenToClient(wxGetMousePosition()));
	e.Skip();
}
