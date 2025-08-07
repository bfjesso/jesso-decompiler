#include "functionPropertiesMenu.h"

wxBEGIN_EVENT_TABLE(FunctionPropertiesMenu, wxFrame)
EVT_CLOSE(CloseMenu)
wxEND_EVENT_TABLE()

FunctionPropertiesMenu::FunctionPropertiesMenu() : wxFrame(nullptr, MainWindowID, "Change Function Properties", wxPoint(50, 50), wxSize(600, 600))
{
	SetOwnBackgroundColour(backgroundColor);

	functionNameLabel = new wxStaticText(this, wxID_ANY, "Function Name");
	functionNameLabel->SetOwnForegroundColour(textColor);

	functionNameTextCtrl = new wxTextCtrl(this, wxID_ANY, "", wxPoint(0, 0), wxSize(100, 25));
	functionNameTextCtrl->SetOwnBackgroundColour(foregroundColor);
	functionNameTextCtrl->SetOwnForegroundColour(textColor);

	vSizer = new wxBoxSizer(wxVERTICAL);
}

void FunctionPropertiesMenu::OpenMenu(wxPoint position, Function* function)
{
	vSizer->Add(functionNameLabel, 0, wxEXPAND);
	functionNameTextCtrl->SetValue(function->name);
	vSizer->Add(functionNameTextCtrl, 0, wxEXPAND);
	
	if (function->numOfRegArgs > 0) 
	{
		wxStaticText* regArgLabel = new wxStaticText(this, wxID_ANY, "Reg Arg Names");
		regArgLabel->SetOwnForegroundColour(textColor);
		vSizer->Add(regArgLabel, 0, wxEXPAND);
		for (int i = 0; i < function->numOfRegArgs; i++)
		{
			wxTextCtrl* regArgTextCtrl = new wxTextCtrl(this, wxID_ANY, function->regArgs[i].name, wxPoint(0, 0), wxSize(100, 25));
			regArgTextCtrl->SetOwnBackgroundColour(foregroundColor);
			regArgTextCtrl->SetOwnForegroundColour(textColor);

			vSizer->Add(regArgTextCtrl, 0, wxEXPAND);

			regArgNameTextCtrls.push_back(regArgTextCtrl);
		}
	}

	if (function->numOfStackArgs > 0) 
	{
		wxStaticText* stackArgLabel = new wxStaticText(this, wxID_ANY, "Stack Arg Names");
		stackArgLabel->SetOwnForegroundColour(textColor);
		vSizer->Add(stackArgLabel, 0, wxEXPAND);
		for (int i = 0; i < function->numOfStackArgs; i++)
		{
			wxTextCtrl* stackArgTextCtrl = new wxTextCtrl(this, wxID_ANY, function->stackArgs[i].name, wxPoint(0, 0), wxSize(100, 25));
			stackArgTextCtrl->SetOwnBackgroundColour(foregroundColor);
			stackArgTextCtrl->SetOwnForegroundColour(textColor);

			vSizer->Add(stackArgTextCtrl, 0, wxEXPAND);

			stackArgNameTextCtrls.push_back(stackArgTextCtrl);
		}
	}

	if (function->numOfLocalVars > 0)
	{
		wxStaticText* localVarLabel = new wxStaticText(this, wxID_ANY, "Local Var Names");
		localVarLabel->SetOwnForegroundColour(textColor);
		vSizer->Add(localVarLabel, 0, wxEXPAND);
		for (int i = 0; i < function->numOfLocalVars; i++)
		{
			wxTextCtrl* localVarTextCtrl = new wxTextCtrl(this, wxID_ANY, function->localVars[i].name, wxPoint(0, 0), wxSize(100, 25));
			localVarTextCtrl->SetOwnBackgroundColour(foregroundColor);
			localVarTextCtrl->SetOwnForegroundColour(textColor);

			vSizer->Add(localVarTextCtrl, 0, wxEXPAND);

			localVarNameTextCtrls.push_back(localVarTextCtrl);
		}
	}

	SetSizer(vSizer);
	
	currentFunction = function;
	
	position.x += 10;
	position.y += 10;
	SetPosition(position);
	Show();
	Raise();
}

void FunctionPropertiesMenu::CloseMenu(wxCloseEvent& e)
{
	currentFunction->name[0] = 0;
	strcpy(currentFunction->name, functionNameTextCtrl->GetValue().c_str());
	
	for (int i = 0; i < currentFunction->numOfRegArgs; i++)
	{
		currentFunction->regArgs[i].name[0] = 0;
		strcpy(currentFunction->regArgs[i].name, regArgNameTextCtrls[i]->GetValue().c_str());
	}

	for (int i = 0; i < currentFunction->numOfStackArgs; i++)
	{
		currentFunction->stackArgs[i].name[0] = 0;
		strcpy(currentFunction->stackArgs[i].name, stackArgNameTextCtrls[i]->GetValue().c_str());
	}

	for (int i = 0; i < currentFunction->numOfLocalVars; i++)
	{
		currentFunction->localVars[i].name[0] = 0;
		strcpy(currentFunction->localVars[i].name, localVarNameTextCtrls[i]->GetValue().c_str());
	}

	Destroy();
}
