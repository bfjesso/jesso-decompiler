#include "functionInfoMenu.h"
#include "../decompiler/dataTypes.h"
#include "../disassembler/registers.h"
#include "../jdc-str/jdcStr.h"

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
		wxStaticText* regArgLabel = new wxStaticText(this, wxID_ANY, "Reg Args:");
		regArgLabel->SetOwnForegroundColour(textColor);
		vSizer->Add(regArgLabel, 0, wxEXPAND);
		for (int i = 0; i < function->numOfRegArgs; i++)
		{
			varTypeToStr(function->regArgs[i].type, &typeStr);
			wxStaticText* regArgStaticTxt = new wxStaticText(this, wxID_ANY, wxString(typeStr.buffer) + " " + wxString(function->regArgs[i].name.buffer) + "; (Register: " + wxString(registerStrs[function->regArgs[i].reg]) + ")");
			regArgStaticTxt->SetOwnForegroundColour(textColor);
			vSizer->Add(regArgStaticTxt, 0, wxEXPAND);
		}
	}

	if (function->numOfRegVars > 0)
	{
		wxStaticText* regVarLabel = new wxStaticText(this, wxID_ANY, "Reg Vars:");
		regVarLabel->SetOwnForegroundColour(textColor);
		vSizer->Add(regVarLabel, 0, wxEXPAND);
		for (int i = 0; i < function->numOfRegVars; i++)
		{
			varTypeToStr(function->regVars[i].type, &typeStr);
			wxStaticText* regVarStaticTxt = new wxStaticText(this, wxID_ANY, wxString(typeStr.buffer) + " " + wxString(function->regVars[i].name.buffer) + "; (Register: " + wxString(registerStrs[function->regVars[i].reg]) + ")");
			regVarStaticTxt->SetOwnForegroundColour(textColor);
			vSizer->Add(regVarStaticTxt, 0, wxEXPAND);
		}
	}

	if (function->numOfStackArgs > 0)
	{
		wxStaticText* stackArgLabel = new wxStaticText(this, wxID_ANY, "Stack Args:");
		stackArgLabel->SetOwnForegroundColour(textColor);
		vSizer->Add(stackArgLabel, 0, wxEXPAND);
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
			wxStaticText* stackArgStaticTxt = new wxStaticText(this, wxID_ANY, wxString(typeStr.buffer) + " " + wxString(function->stackArgs[i].name.buffer) + "; (Offset from BP: " + wxString(hexNumStr) + ")");
			stackArgStaticTxt->SetOwnForegroundColour(textColor);
			vSizer->Add(stackArgStaticTxt, 0, wxEXPAND);
		}
	}

	if (function->numOfStackVars > 0)
	{
		wxStaticText* stackVarLabel = new wxStaticText(this, wxID_ANY, "Stack Vars:");
		stackVarLabel->SetOwnForegroundColour(textColor);
		vSizer->Add(stackVarLabel, 0, wxEXPAND);
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
			wxStaticText* stackVarStaticTxt = new wxStaticText(this, wxID_ANY, wxString(typeStr.buffer) + " " + wxString(function->stackVars[i].name.buffer) + "; (Offset from BP: " + wxString(hexNumStr) + ")");
			stackVarStaticTxt->SetOwnForegroundColour(textColor);
			vSizer->Add(stackVarStaticTxt, 0, wxEXPAND);
		}
	}

	if (function->numOfReturnedVars > 0)
	{
		wxStaticText* retVarLabel = new wxStaticText(this, wxID_ANY, "Returned Vars:");
		retVarLabel->SetOwnForegroundColour(textColor);
		vSizer->Add(retVarLabel, 0, wxEXPAND);
		for (int i = 0; i < function->numOfReturnedVars; i++)
		{
			varTypeToStr(function->returnedVars[i].type, &typeStr);
			sprintf(hexNumStr, "0x%llX", function->returnedVars[i].callAddr);
			wxStaticText* retVarStaticTxt = new wxStaticText(this, wxID_ANY, wxString(typeStr.buffer) + " " + wxString(function->returnedVars[i].name.buffer) + "; (Return reg: " + wxString(registerStrs[function->returnedVars[i].returnReg]) + ", called address: " + wxString(hexNumStr) + ", call number: " + wxString(std::to_string(function->returnedVars[i].callNum)) + ")");
			retVarStaticTxt->SetOwnForegroundColour(textColor);
			vSizer->Add(retVarStaticTxt, 0, wxEXPAND);
		}
	}

	freeJdcStr(&typeStr);

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
