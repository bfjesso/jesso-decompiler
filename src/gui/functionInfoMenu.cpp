#include "functionInfoMenu.h"
#include "../decompiler/dataTypes.h"
#include "../disassembler/registers.h"
#include "../jdc-str/jdcStr.h"

wxBEGIN_EVENT_TABLE(FunctionInfoMenu, wxFrame)
EVT_GRID_CELL_RIGHT_CLICK(FunctionInfoMenu::GridRightClickOptions)
wxEND_EVENT_TABLE()

FunctionInfoMenu::FunctionInfoMenu(wxPoint position, Function* function) : wxFrame(nullptr, MainWindowID, "Function Info", wxPoint(50, 50), wxSize(600, 600))
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

	hSizer = new wxBoxSizer(wxHORIZONTAL);

    vSizer1 = new wxBoxSizer(wxVERTICAL);

    vSizer1->Add(returnTypeStaticTxt, 0, wxEXPAND);
    vSizer1->Add(returnRegStaticTxt, 0, wxEXPAND);
    vSizer1->Add(returnFunctionAddrStaticTxt, 0, wxEXPAND);
    vSizer1->Add(firstFuncCallAddrStaticTxt, 0, wxEXPAND);
    vSizer1->Add(indexOfFirstFuncCallStaticTxt, 0, wxEXPAND);
    vSizer1->Add(callingConventionStaticTxt, 0, wxEXPAND);
	vSizer1->Add(functionNameStaticTxt, 0, wxEXPAND);

    if (function->numOfRegArgs > 0)
	{
		wxStaticText* regArgLabel = new wxStaticText(this, wxID_ANY, "Reg Args:");
		regArgLabel->SetOwnForegroundColour(textColor);
		vSizer1->Add(regArgLabel, 0, wxEXPAND);
		for (int i = 0; i < function->numOfRegArgs; i++)
		{
			varTypeToStr(function->regArgs[i].type, &typeStr);
			wxStaticText* regArgStaticTxt = new wxStaticText(this, wxID_ANY, "\t" + wxString(typeStr.buffer) + " " + wxString(function->regArgs[i].name.buffer) + "; (Register: " + wxString(registerStrs[function->regArgs[i].reg]) + ")");
			regArgStaticTxt->SetOwnForegroundColour(textColor);
			vSizer1->Add(regArgStaticTxt, 0, wxEXPAND);
		}
	}

	if (function->numOfRegVars > 0)
	{
		wxStaticText* regVarLabel = new wxStaticText(this, wxID_ANY, "Reg Vars:");
		regVarLabel->SetOwnForegroundColour(textColor);
		vSizer1->Add(regVarLabel, 0, wxEXPAND);
		for (int i = 0; i < function->numOfRegVars; i++)
		{
			varTypeToStr(function->regVars[i].type, &typeStr);
			wxStaticText* regVarStaticTxt = new wxStaticText(this, wxID_ANY, "\t" + wxString(typeStr.buffer) + " " + wxString(function->regVars[i].name.buffer) + "; (Register: " + wxString(registerStrs[function->regVars[i].reg]) + ")");
			regVarStaticTxt->SetOwnForegroundColour(textColor);
			vSizer1->Add(regVarStaticTxt, 0, wxEXPAND);
		}
	}

	if (function->numOfStackArgs > 0)
	{
		wxStaticText* stackArgLabel = new wxStaticText(this, wxID_ANY, "Stack Args:");
		stackArgLabel->SetOwnForegroundColour(textColor);
		vSizer1->Add(stackArgLabel, 0, wxEXPAND);
		for (int i = 0; i < function->numOfStackArgs; i++)
		{
			varTypeToStr(function->stackArgs[i].type, &typeStr);
			if (function->stackArgs[i].stackOffset > 0)
			{
				sprintf(hexNumStr, "0x%X", function->stackArgs[i].stackOffset);
			}
			else
			{
				sprintf(hexNumStr, "-0x%X", -function->stackArgs[i].stackOffset);
			}
			wxStaticText* stackArgStaticTxt = new wxStaticText(this, wxID_ANY, "\t" + wxString(typeStr.buffer) + " " + wxString(function->stackArgs[i].name.buffer) + "; (Offset from BP: " + wxString(hexNumStr) + ")");
			stackArgStaticTxt->SetOwnForegroundColour(textColor);
			vSizer1->Add(stackArgStaticTxt, 0, wxEXPAND);
		}
	}

	if (function->numOfStackVars > 0)
	{
		wxStaticText* stackVarLabel = new wxStaticText(this, wxID_ANY, "Stack Vars:");
		stackVarLabel->SetOwnForegroundColour(textColor);
		vSizer1->Add(stackVarLabel, 0, wxEXPAND);
		for (int i = 0; i < function->numOfStackVars; i++)
		{
			varTypeToStr(function->stackVars[i].type, &typeStr);
			if (function->stackVars[i].stackOffset > 0) 
			{
				sprintf(hexNumStr, "0x%X", function->stackVars[i].stackOffset);
			}
			else 
			{
				sprintf(hexNumStr, "-0x%X", -function->stackVars[i].stackOffset);
			}
			wxStaticText* stackVarStaticTxt = new wxStaticText(this, wxID_ANY, "\t" + wxString(typeStr.buffer) + " " + wxString(function->stackVars[i].name.buffer) + "; (Offset from BP: " + wxString(hexNumStr) + ")");
			stackVarStaticTxt->SetOwnForegroundColour(textColor);
			vSizer1->Add(stackVarStaticTxt, 0, wxEXPAND);
		}
	}

	if (function->numOfReturnedVars > 0)
	{
		wxStaticText* retVarLabel = new wxStaticText(this, wxID_ANY, "Returned Vars:");
		retVarLabel->SetOwnForegroundColour(textColor);
		vSizer1->Add(retVarLabel, 0, wxEXPAND);
		for (int i = 0; i < function->numOfReturnedVars; i++)
		{
			varTypeToStr(function->returnedVars[i].type, &typeStr);
			sprintf(hexNumStr, "0x%llX", function->returnedVars[i].callAddr);
			wxStaticText* retVarStaticTxt = new wxStaticText(this, wxID_ANY, "\t" + wxString(typeStr.buffer) + " " + wxString(function->returnedVars[i].name.buffer) + "; (Return reg: " + wxString(registerStrs[function->returnedVars[i].returnReg]) + ", called address: " + wxString(hexNumStr) + ", call number: " + wxString(std::to_string(function->returnedVars[i].callNum)) + ")");
			retVarStaticTxt->SetOwnForegroundColour(textColor);
			vSizer1->Add(retVarStaticTxt, 0, wxEXPAND);
		}
	}

	freeJdcStr(&typeStr);

	numOfInstructionsStaticTxt = new wxStaticText(this, wxID_ANY, "Number of Instructions: " + wxString(std::to_string(function->numOfInstructions)));
	numOfInstructionsStaticTxt->SetOwnForegroundColour(textColor);
    vSizer1->Add(numOfInstructionsStaticTxt, 0, wxEXPAND);

	hSizer->Add(vSizer1);

	vSizer2 = new wxBoxSizer(wxVERTICAL);

	conditionsGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(600, 200));
	conditionsGrid->SetLabelBackgroundColour(foregroundColor);
	conditionsGrid->SetLabelTextColour(textColor);
	conditionsGrid->SetDefaultCellBackgroundColour(gridColor);
	conditionsGrid->SetDefaultCellTextColour(textColor);
	conditionsGrid->CreateGrid(0, 6);
	conditionsGrid->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
	conditionsGrid->SetScrollRate(0, 10);
	conditionsGrid->DisableDragRowSize();
	conditionsGrid->EnableEditing(false);
	conditionsGrid->SetColLabelValue(0, "Condition type");
	conditionsGrid->SetColLabelValue(1, "Jcc index");
	conditionsGrid->SetColLabelValue(2, "Dst index");
	conditionsGrid->SetColLabelValue(3, "Exit index");
	conditionsGrid->SetColLabelValue(4, "Combined jcc indexes");
	conditionsGrid->SetColLabelValue(5, "Jcc combination type");
	conditionsGrid->HideRowLabels();
	conditionsGrid->SetColSize(0, 100);
	conditionsGrid->SetColSize(1, 100);
	conditionsGrid->SetColSize(2, 100);
	conditionsGrid->SetColSize(3, 100);
	conditionsGrid->SetColSize(4, 100);
	conditionsGrid->SetColSize(5, 9999);
	conditionsGrid->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	vSizer2->Add(conditionsGrid, 0, wxEXPAND);

	for (int i = 0; i < function->numOfConditions; i++)
	{
		struct Condition* condition = &function->conditions[i];

		conditionsGrid->AppendRows(1);

		conditionsGrid->SetCellValue(i, 0, wxString(conditionTypeStrs[condition->conditionType]));

		sprintf(hexNumStr, "0x%llX", function->instructions[condition->jccIndex].address);
		conditionsGrid->SetCellValue(i, 1, wxString(std::to_string(condition->jccIndex)) + " (" + wxString(hexNumStr) + ")");

		sprintf(hexNumStr, "0x%llX", function->instructions[condition->dstIndex].address);
		conditionsGrid->SetCellValue(i, 2, wxString(std::to_string(condition->dstIndex)) + " (" + wxString(hexNumStr) + ")");

		if (condition->exitIndex == -1)
		{
			strcpy(hexNumStr, "none");
		}
		else 
		{
			sprintf(hexNumStr, "0x%llX", function->instructions[condition->exitIndex].address);
		}
		conditionsGrid->SetCellValue(i, 3, wxString(std::to_string(condition->exitIndex)) + " (" + wxString(hexNumStr) + ")");

		wxString combinedJccsStr = "";
		for (int j = 0; j < condition->numOfCombinedJccs; j++) 
		{
			sprintf(hexNumStr, "0x%llX", function->instructions[condition->combinedJccIndexes[j]].address);
			combinedJccsStr += wxString(std::to_string(condition->combinedJccIndexes[j]) + " (" + wxString(hexNumStr) + ")");
			if (j != condition->numOfCombinedJccs - 1) 
			{
				combinedJccsStr += ", ";
			}
		}
		conditionsGrid->SetCellValue(i, 4, combinedJccsStr);
		conditionsGrid->SetCellValue(i, 5, wxString(logicalTypeStrs[condition->combinedJccsLogicType]));
	}

	hSizer->Add(vSizer2, 1, wxEXPAND);

	SetSizerAndFit(hSizer);

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