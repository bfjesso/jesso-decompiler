#include "functionInfoMenu.h"
#include "../decompiler/dataTypes.h"
#include "../disassembler/registers.h"
#include "../jdc-str/jdcStr.h"

FunctionInfoMenu::FunctionInfoMenu(wxPoint position, Function* function) : wxFrame(nullptr, MainWindowID, "Function Info", wxPoint(50, 50), wxSize(600, 600))
{
    SetOwnBackgroundColour(backgroundColor);

    struct JdcStr returnTypeStr = initializeJdcStr();
    varTypeToStr(function->returnType, &returnTypeStr);
    returnTypeStaticTxt = new wxStaticText(this, wxID_ANY, "Return Type: " + wxString(returnTypeStr.buffer));
	returnTypeStaticTxt->SetOwnForegroundColour(textColor);
    freeJdcStr(&returnTypeStr);

    returnRegStaticTxt = new wxStaticText(this, wxID_ANY, "Return Reg: " + wxString(registerStrs[function->returnReg]));
	returnRegStaticTxt->SetOwnForegroundColour(textColor);

    char addressOfReturnFunctionStr[10] = { 0 };
    sprintf(addressOfReturnFunctionStr, "%llX", function->addressOfReturnFunction);
    returnFunctionAddrStaticTxt = new wxStaticText(this, wxID_ANY, "Address of Return Function: 0x" + wxString(addressOfReturnFunctionStr));
	returnFunctionAddrStaticTxt->SetOwnForegroundColour(textColor);

    char addressOfFirstFuncCallStr[10] = { 0 };
    sprintf(addressOfFirstFuncCallStr, "%llX", function->addressOfFirstFuncCall);
    firstFuncCallAddrStaticTxt = new wxStaticText(this, wxID_ANY, "Address of First Function Call: 0x" + wxString(addressOfFirstFuncCallStr));
	firstFuncCallAddrStaticTxt->SetOwnForegroundColour(textColor);

    indexOfFirstFuncCallStaticTxt = new wxStaticText(this, wxID_ANY, "Index of First Function Call: " + wxString(std::to_string(function->indexOfFirstFuncCall)));
	indexOfFirstFuncCallStaticTxt->SetOwnForegroundColour(textColor);

    callingConventionStaticTxt = new wxStaticText(this, wxID_ANY, "Calling Convention: " + wxString(callingConventionStrs[function->callingConvention]));
	callingConventionStaticTxt->SetOwnForegroundColour(textColor);

	functionNameStaticTxt = new wxStaticText(this, wxID_ANY, "Function Name: " + wxString(function->name.buffer));
	functionNameStaticTxt->SetOwnForegroundColour(textColor);

    vSizer = new wxBoxSizer(wxVERTICAL);

    vSizer->Add(returnTypeStaticTxt, 0, wxEXPAND);
    vSizer->Add(returnRegStaticTxt, 0, wxEXPAND);
    vSizer->Add(returnFunctionAddrStaticTxt, 0, wxEXPAND);
    vSizer->Add(firstFuncCallAddrStaticTxt, 0, wxEXPAND);
    vSizer->Add(indexOfFirstFuncCallStaticTxt, 0, wxEXPAND);
    vSizer->Add(callingConventionStaticTxt, 0, wxEXPAND);
	vSizer->Add(functionNameStaticTxt, 0, wxEXPAND);

    if (function->numOfRegArgs > 0)
	{
		wxStaticText* regArgLabel = new wxStaticText(this, wxID_ANY, "Reg Arg Names:");
		regArgLabel->SetOwnForegroundColour(textColor);
		vSizer->Add(regArgLabel, 0, wxEXPAND);
		for (int i = 0; i < function->numOfRegArgs; i++)
		{
			wxStaticText* regArgStaticTxt = new wxStaticText(this, wxID_ANY, function->regArgs[i].name.buffer);
			regArgStaticTxt->SetOwnForegroundColour(textColor);
			vSizer->Add(regArgStaticTxt, 0, wxEXPAND);
		}
	}

	if (function->numOfRegVars > 0)
	{
		wxStaticText* regVarLabel = new wxStaticText(this, wxID_ANY, "Reg Var Names:");
		regVarLabel->SetOwnForegroundColour(textColor);
		vSizer->Add(regVarLabel, 0, wxEXPAND);
		for (int i = 0; i < function->numOfRegVars; i++)
		{
			wxStaticText* regVarStaticTxt = new wxStaticText(this, wxID_ANY, function->regVars[i].name.buffer);
			regVarStaticTxt->SetOwnForegroundColour(textColor);
			vSizer->Add(regVarStaticTxt, 0, wxEXPAND);
		}
	}

	if (function->numOfStackArgs > 0)
	{
		wxStaticText* stackArgLabel = new wxStaticText(this, wxID_ANY, "Stack Arg Names:");
		stackArgLabel->SetOwnForegroundColour(textColor);
		vSizer->Add(stackArgLabel, 0, wxEXPAND);
		for (int i = 0; i < function->numOfStackArgs; i++)
		{
			wxStaticText* stackArgStaticTxt = new wxStaticText(this, wxID_ANY, function->stackArgs[i].name.buffer);
			stackArgStaticTxt->SetOwnForegroundColour(textColor);
			vSizer->Add(stackArgStaticTxt, 0, wxEXPAND);
		}
	}

	if (function->numOfStackVars > 0)
	{
		wxStaticText* stackVarLabel = new wxStaticText(this, wxID_ANY, "Stack Var Names:");
		stackVarLabel->SetOwnForegroundColour(textColor);
		vSizer->Add(stackVarLabel, 0, wxEXPAND);
		for (int i = 0; i < function->numOfStackVars; i++)
		{
			wxStaticText* stackVarStaticTxt = new wxStaticText(this, wxID_ANY, function->stackVars[i].name.buffer);
			stackVarStaticTxt->SetOwnForegroundColour(textColor);
			vSizer->Add(stackVarStaticTxt, 0, wxEXPAND);
		}
	}

	if (function->numOfReturnedVars > 0)
	{
		wxStaticText* retVarLabel = new wxStaticText(this, wxID_ANY, "Returned Var Names:");
		retVarLabel->SetOwnForegroundColour(textColor);
		vSizer->Add(retVarLabel, 0, wxEXPAND);
		for (int i = 0; i < function->numOfReturnedVars; i++)
		{
			wxStaticText* retVarStaticTxt = new wxStaticText(this, wxID_ANY, function->returnedVars[i].name.buffer);
			retVarStaticTxt->SetOwnForegroundColour(textColor);
			vSizer->Add(retVarStaticTxt, 0, wxEXPAND);
		}
	}

	numOfInstructionsStaticTxt = new wxStaticText(this, wxID_ANY, "Number of Instructions: " + wxString(std::to_string(function->numOfInstructions)));
	numOfInstructionsStaticTxt->SetOwnForegroundColour(textColor);
    vSizer->Add(numOfInstructionsStaticTxt, 0, wxEXPAND);

	SetSizerAndFit(vSizer);

	position.x += 10;
	position.y += 10;
	SetPosition(position);
	Show();
	Raise();
}
