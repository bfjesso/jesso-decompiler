#include "codeReferencesWindow.h"
#include "mainGui.h"

wxBEGIN_EVENT_TABLE(CodeReferencesWindow, wxWindow)
EVT_BUTTON(FindReferencesID, CodeReferencesWindow::FindCodeReferences)
wxEND_EVENT_TABLE()

CodeReferencesWindow::CodeReferencesWindow(MainGui* parent) : wxWindow(parent, wxID_ANY)
{
	mainGui = parent;
	
	SetMinSize(wxSize(200, 100));
	SetOwnBackgroundColour(backgroundColor);

	hexCheckBox = new wxCheckBox(this, wxID_ANY, "Hex");
	hexCheckBox->SetOwnForegroundColour(textColor);
	hexCheckBox->SetValue(true);

	valueTextCtrl = new wxTextCtrl(this, wxID_ANY, "0x0", wxPoint(0, 0), wxSize(150, 25));
	valueTextCtrl->SetOwnBackgroundColour(foregroundColor);
	valueTextCtrl->SetOwnForegroundColour(textColor);

	findReferencesButton = new wxButton(this, FindReferencesID, "Find code references", wxPoint(0, 0), wxSize(50, 25));
	findReferencesButton->SetOwnBackgroundColour(foregroundColor);
	findReferencesButton->SetOwnForegroundColour(textColor);

	disassemblyTextCtrl = new DisassemblyTextCtrl(this, mainGui, "disassembly", 0, 0);

	row1Sizer = new wxBoxSizer(wxHORIZONTAL);
	vSizer = new wxBoxSizer(wxVERTICAL);

	row1Sizer->Add(hexCheckBox, 0, wxTOP | wxLEFT, 10);
	row1Sizer->Add(valueTextCtrl, 1, wxEXPAND | wxTOP | wxRIGHT, 10);

	vSizer->Add(row1Sizer, 0, wxEXPAND);
	vSizer->Add(findReferencesButton, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
	vSizer->Add(disassemblyTextCtrl, 1, wxEXPAND | wxALL, 10);

	SetSizerAndFit(vSizer);
}

void CodeReferencesWindow::FindCodeReferences(wxCommandEvent& e)
{
	long long value = 0;
	if (hexCheckBox->IsChecked())
	{
		if (!valueTextCtrl->GetValue().ToLongLong(&value, 16))
		{
			return;
		}
	}
	else
	{
		if (!valueTextCtrl->GetValue().ToLongLong(&value, 10))
		{
			return;
		}
	}

	foundInstructions.clear();
	foundInstructions.shrink_to_fit();
	for (int i = 0; i < mainGui->decompParams.numOfInstructions; i++) 
	{
		struct DisassembledInstruction* instruction = &mainGui->decompParams.instructions[i];
		for (int j = 0; j < instruction->numOfOperands; j++) 
		{
			if (instruction->operands[j].type == IMMEDIATE)
			{
				if (instruction->operands[j].immediate.value == value) 
				{
					foundInstructions.push_back(*instruction);
					break;
				}
			}
			else if (instruction->operands[j].type == MEM_ADDRESS)
			{
				if (instruction->operands[j].memoryAddress.constDisplacement == value) 
				{
					foundInstructions.push_back(*instruction);
					break;
				}
				else if (compareRegisters(instruction->operands[j].memoryAddress.reg, IP) &&
					instruction->address + instruction->numOfBytes + instruction->operands[j].memoryAddress.constDisplacement == value) 
				{
					foundInstructions.push_back(*instruction);
					break;
				}
			}
		}
	}

	if (foundInstructions.size() == 0)
	{
		wxMessageBox("No instructions found", "Failed to find code references");
		disassemblyTextCtrl->ClearText();
	}
	else 
	{
		disassemblyTextCtrl->Initialize(foundInstructions.data(), foundInstructions.size(), 0);
	}
}