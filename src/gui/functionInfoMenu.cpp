#include "functionInfoMenu.h"
#include "../decompiler/dataTypes.h"
#include "../disassembler/registers.h"
#include "../jdc-str/jdcStr.h"

wxBEGIN_EVENT_TABLE(FunctionInfoMenu, wxFrame)
EVT_GRID_CELL_RIGHT_CLICK(FunctionInfoMenu::GridRightClickOptions)
wxEND_EVENT_TABLE()

FunctionInfoMenu::FunctionInfoMenu(wxWindow* parent, wxPoint position, DisassembledInstruction* instructions, Function* function) : wxFrame(parent, MainWindowID, "Function Info", wxPoint(50, 50), wxSize(800, 600))
{
    SetOwnBackgroundColour(backgroundColor);

    struct JdcStr typeStr = initializeJdcStr();
    varTypeToStr(function->returnType, &typeStr);
    returnTypeStaticTxt = new wxStaticText(this, wxID_ANY, "Return Type: " + wxString(typeStr.buffer));
	returnTypeStaticTxt->SetOwnForegroundColour(textColor);

    returnRegStaticTxt = new wxStaticText(this, wxID_ANY, "Return Reg: " + wxString(registerStrs[function->returnReg]));
	returnRegStaticTxt->SetOwnForegroundColour(textColor);

    char hexNumStr[10] = { 0 };
    sprintf(hexNumStr, "%llX", function->addressOfReturnFunction);
    returnFunctionAddrStaticTxt = new wxStaticText(this, wxID_ANY, "Address of Return Function: 0x" + wxString(hexNumStr));
	returnFunctionAddrStaticTxt->SetOwnForegroundColour(textColor);

    sprintf(hexNumStr, "%llX", function->addressOfFirstFuncCall);
    firstFuncCallAddrStaticTxt = new wxStaticText(this, wxID_ANY, "Address of First Function Call: 0x" + wxString(hexNumStr));
	firstFuncCallAddrStaticTxt->SetOwnForegroundColour(textColor);

    indexOfFirstFuncCallStaticTxt = new wxStaticText(this, wxID_ANY, "Index of First Function Call: " + wxString(std::to_string(function->indexOfFirstFuncCall)));
	indexOfFirstFuncCallStaticTxt->SetOwnForegroundColour(textColor);

    callingConventionStaticTxt = new wxStaticText(this, wxID_ANY, "Calling Convention: " + wxString(callingConventionStrs[function->callingConvention]));
	callingConventionStaticTxt->SetOwnForegroundColour(textColor);

	functionNameStaticTxt = new wxStaticText(this, wxID_ANY, "Function Name: " + wxString(function->name.buffer));
	functionNameStaticTxt->SetOwnForegroundColour(textColor);

	numOfInstructionsStaticTxt = new wxStaticText(this, wxID_ANY, "Number of Instructions: " + wxString(std::to_string(function->lastInstructionIndex - function->firstInstructionIndex + 1)));
	numOfInstructionsStaticTxt->SetOwnForegroundColour(textColor);

	row1Sizer = new wxBoxSizer(wxHORIZONTAL);
	row2Sizer = new wxBoxSizer(wxHORIZONTAL);
	vSizer = new wxBoxSizer(wxVERTICAL);

    if (function->numOfRegArgs > 0 || function->numOfRegVars > 0)
	{
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

		row1Sizer->Add(regVarsGrid, 1, wxEXPAND | wxLEFT | wxRIGHT, 10);
		
		for (int i = 0; i < function->numOfRegArgs; i++)
		{
			struct RegisterVariable* regArg = &function->regArgs[i];
			regVarsGrid->AppendRows(1);
			varTypeToStr(regArg->type, &typeStr);
			regVarsGrid->SetCellValue(i, 0, wxString(typeStr.buffer));
			regVarsGrid->SetCellValue(i, 1, wxString(regArg->name.buffer));
			regVarsGrid->SetCellValue(i, 2, wxString(registerStrs[regArg->reg]));
			regVarsGrid->SetCellValue(i, 3, "Yes");
		}
		for (int i = 0; i < function->numOfRegVars; i++)
		{
			struct RegisterVariable* regVar = &function->regVars[i];
			regVarsGrid->AppendRows(1);
			varTypeToStr(regVar->type, &typeStr);
			regVarsGrid->SetCellValue(i + function->numOfRegArgs, 0, wxString(typeStr.buffer));
			regVarsGrid->SetCellValue(i + function->numOfRegArgs, 1, wxString(regVar->name.buffer));
			regVarsGrid->SetCellValue(i + function->numOfRegArgs, 2, wxString(registerStrs[regVar->reg]));
			regVarsGrid->SetCellValue(i + function->numOfRegArgs, 3, "No");
		}
	}

	if (function->numOfStackArgs > 0 || function->numOfStackVars > 0)
	{
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

		row2Sizer->Add(stackVarsGrid, 1, wxEXPAND | wxLEFT | wxRIGHT, 10);
		
		for (int i = 0; i < function->numOfStackArgs; i++)
		{
			struct StackVariable* stackArg = &function->stackArgs[i];
			stackVarsGrid->AppendRows(1);
			varTypeToStr(stackArg->type, &typeStr);
			if (stackArg->stackOffset > 0)
			{
				sprintf(hexNumStr, "0x%X", stackArg->stackOffset);
			}
			else
			{
				sprintf(hexNumStr, "-0x%X", -stackArg->stackOffset);
			}
			stackVarsGrid->SetCellValue(i, 0, wxString(typeStr.buffer));
			stackVarsGrid->SetCellValue(i, 1, wxString(stackArg->name.buffer));
			stackVarsGrid->SetCellValue(i, 2, wxString(hexNumStr));
			stackVarsGrid->SetCellValue(i, 3, "Yes");
		}
		for (int i = 0; i < function->numOfStackVars; i++)
		{
			struct StackVariable* stackVar = &function->stackVars[i];
			stackVarsGrid->AppendRows(1);
			varTypeToStr(stackVar->type, &typeStr);
			if (stackVar->stackOffset > 0)
			{
				sprintf(hexNumStr, "0x%X", stackVar->stackOffset);
			}
			else
			{
				sprintf(hexNumStr, "-0x%X", -stackVar->stackOffset);
			}
			stackVarsGrid->SetCellValue(i + function->numOfStackArgs, 0, wxString(typeStr.buffer));
			stackVarsGrid->SetCellValue(i + function->numOfStackArgs, 1, wxString(stackVar->name.buffer));
			stackVarsGrid->SetCellValue(i + function->numOfStackArgs, 2, wxString(hexNumStr));
			stackVarsGrid->SetCellValue(i + function->numOfStackArgs, 3, "No");
		}
	}

	if (function->numOfReturnedVars > 0)
	{
		returnedVarsGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(600, 200));
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
			varTypeToStr(returnedVar->type, &typeStr);
			returnedVarsGrid->SetCellValue(i, 0, wxString(typeStr.buffer));
			returnedVarsGrid->SetCellValue(i, 1, wxString(returnedVar->name.buffer));
			sprintf(hexNumStr, "0x%llX", returnedVar->calleeAddress);
			returnedVarsGrid->SetCellValue(i, 2, wxString(hexNumStr));
			sprintf(hexNumStr, "0x%llX", returnedVar->callInstructionAddress);
			returnedVarsGrid->SetCellValue(i, 3, wxString(hexNumStr));
			returnedVarsGrid->SetCellValue(i, 4, wxString(registerStrs[returnedVar->returnReg]));
		}
	}

	freeJdcStr(&typeStr);

	if (function->numOfConditions > 0) 
	{
		conditionsGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(800, 200));
		conditionsGrid->SetLabelBackgroundColour(foregroundColor);
		conditionsGrid->SetLabelTextColour(textColor);
		conditionsGrid->SetDefaultCellBackgroundColour(gridColor);
		conditionsGrid->SetDefaultCellTextColour(textColor);
		conditionsGrid->CreateGrid(0, 7);
		conditionsGrid->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
		conditionsGrid->SetScrollRate(0, 10);
		conditionsGrid->DisableDragRowSize();
		conditionsGrid->EnableEditing(false);
		conditionsGrid->SetColLabelValue(0, "Condition type");
		conditionsGrid->SetColLabelValue(1, "Jcc index");
		conditionsGrid->SetColLabelValue(2, "Dst index");
		conditionsGrid->SetColLabelValue(3, "Exit index");
		conditionsGrid->SetColLabelValue(4, "Decompilation type");
		conditionsGrid->SetColLabelValue(5, "Combined jcc indexes");
		conditionsGrid->SetColLabelValue(6, "Jcc combination type");
		conditionsGrid->HideRowLabels();
		conditionsGrid->SetColSize(0, 100);
		conditionsGrid->SetColSize(1, 100);
		conditionsGrid->SetColSize(2, 100);
		conditionsGrid->SetColSize(3, 100);
		conditionsGrid->SetColSize(4, 150);
		conditionsGrid->SetColSize(5, 150);
		conditionsGrid->SetColSize(6, 9999);
		conditionsGrid->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

		row1Sizer->Add(conditionsGrid, 2, wxEXPAND | wxRIGHT, 10);

		for (int i = 0; i < function->numOfConditions; i++)
		{
			struct Condition* condition = &function->conditions[i];
			conditionsGrid->AppendRows(1);
			conditionsGrid->SetCellValue(i, 0, wxString(conditionTypeStrs[condition->conditionType]));
			sprintf(hexNumStr, "0x%llX", instructions[condition->jccIndex].address);
			conditionsGrid->SetCellValue(i, 1, wxString(std::to_string(condition->jccIndex)) + " (" + wxString(hexNumStr) + ")");
			sprintf(hexNumStr, "0x%llX", instructions[condition->dstIndex].address);
			conditionsGrid->SetCellValue(i, 2, wxString(std::to_string(condition->dstIndex)) + " (" + wxString(hexNumStr) + ")");

			if (condition->exitIndex == -1)
			{
				strcpy(hexNumStr, "none");
			}
			else
			{
				sprintf(hexNumStr, "0x%llX", instructions[condition->exitIndex].address);
			}
			conditionsGrid->SetCellValue(i, 3, wxString(std::to_string(condition->exitIndex)) + " (" + wxString(hexNumStr) + ")");

			if (condition->decompileAsGoTo) 
			{
				conditionsGrid->SetCellValue(i, 4, "Conditional goto");
			}
			else if (condition->decompileAsReturn)
			{
				conditionsGrid->SetCellValue(i, 4, "Conditional return");
			}
			else
			{
				conditionsGrid->SetCellValue(i, 4, "Condition");
			}

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
			conditionsGrid->SetCellValue(i, 5, combinedJccsStr);
			conditionsGrid->SetCellValue(i, 6, wxString(logicalTypeStrs[condition->combinedJccsLogicType]));
		}
	}

	if (function->numOfDirectJmps > 0)
	{
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

		row2Sizer->Add(directJmpsGrid, 2, wxEXPAND | wxRIGHT, 10);

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
	}

	vSizer->Add(returnTypeStaticTxt, 0, wxEXPAND | wxLEFT | wxTOP, 10);
	vSizer->Add(returnRegStaticTxt, 0, wxEXPAND | wxLEFT, 10);
	vSizer->Add(returnFunctionAddrStaticTxt, 0, wxEXPAND | wxLEFT, 10);
	vSizer->Add(firstFuncCallAddrStaticTxt, 0, wxEXPAND | wxLEFT, 10);
	vSizer->Add(indexOfFirstFuncCallStaticTxt, 0, wxEXPAND | wxLEFT, 10);
	vSizer->Add(callingConventionStaticTxt, 0, wxEXPAND | wxLEFT, 10);
	vSizer->Add(functionNameStaticTxt, 0, wxEXPAND | wxLEFT, 10);
	vSizer->Add(numOfInstructionsStaticTxt, 0, wxEXPAND | wxLEFT | wxBOTTOM, 10);

	vSizer->Add(row1Sizer, 0, wxEXPAND | wxBOTTOM, 10);
	vSizer->Add(row2Sizer, 0, wxEXPAND | wxBOTTOM, 10);

	vSizer->Add(returnedVarsGrid, 0, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 10);

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