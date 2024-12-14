#include "../Headers/mainGui.h"

#include "../../Disassembler/Headers/disassembler.h"

wxBEGIN_EVENT_TABLE(MainGui, wxFrame)
EVT_CLOSE(CloseApp)
EVT_BUTTON(DisassembleButtonID, DisassembleBytesInput)
wxEND_EVENT_TABLE()

MainGui::MainGui() : wxFrame(nullptr, MainWindowID, "Jesso Decompiler x64", wxPoint(50, 50), wxSize(800, 800))
{
	SetOwnBackgroundColour(backgroundColor);

	bytesInputTextCtrl = new wxTextCtrl(this, wxID_ANY, "00", wxPoint(0, 0), wxSize(9999, 25));
	bytesInputTextCtrl->SetOwnBackgroundColour(foregroundColor);
	bytesInputTextCtrl->SetOwnForegroundColour(textColor);

	disassembleButton = new wxButton(this, DisassembleButtonID, "Disassemble", wxPoint(0, 0), wxSize(75, 25));
	disassembleButton->SetOwnBackgroundColour(foregroundColor);
	disassembleButton->SetOwnForegroundColour(textColor);

	is64BitModeCheckBox = new wxCheckBox(this, wxID_ANY, "64-bit mode");
	is64BitModeCheckBox->SetOwnForegroundColour(textColor);

	disassemblyStaticText = new wxStaticText(this, wxID_ANY, "RESULT: ");
	disassemblyStaticText->SetOwnForegroundColour(textColor);

	row1Sizer = new wxBoxSizer(wxHORIZONTAL);
	row2Sizer = new wxBoxSizer(wxHORIZONTAL);
	row3Sizer = new wxBoxSizer(wxHORIZONTAL);
	vSizer = new wxBoxSizer(wxVERTICAL);

	row1Sizer->Add(bytesInputTextCtrl, 0, wxALL, 10);

	row2Sizer->Add(disassembleButton, 0, wxRIGHT | wxLEFT, 10);
	row2Sizer->Add(is64BitModeCheckBox, 0, wxRIGHT, 10);

	row3Sizer->Add(disassemblyStaticText, 0, wxTOP | wxRIGHT | wxLEFT, 10);

	vSizer->Add(row1Sizer, 0, wxEXPAND);
	vSizer->Add(row2Sizer, 0, wxEXPAND);
	vSizer->Add(row3Sizer, 0, wxEXPAND);

	SetSizer(vSizer);
}

void MainGui::DisassembleBytesInput(wxCommandEvent& e)
{
	unsigned char bytes[255];
	if (!ParseStringBytes(bytesInputTextCtrl->GetValue(), bytes, 255)) { return; }

	struct DisassemblerOptions options;
	options.is64BitMode = is64BitModeCheckBox->IsChecked();

	struct DisassembledInstruction result;
	disassembleInstruction(bytes, bytes + 15, &options, &result);

	char buffer[50];
	instructionToStr(&result, buffer, 50);
	disassemblyStaticText->SetLabelText(buffer);
}

// allocates memory for bytes; needs to be deleted later
bool MainGui::ParseStringBytes(wxString str, unsigned char* bytesBuffer, unsigned char bytesBufferLen)
{
	str.Replace(" ", "", true);
	str.Replace("\\", "", true);
	str.Replace("x", "", true);
	str.Replace("X", "", true);
	
	int strLen = str.Length();
	if (strLen < 2 || strLen % 2 != 0) 
	{ 
		return false; 
	}

	int currentByte = 0;
	for (int i = 0; i < strLen; i += 2)
	{
		if (currentByte > bytesBufferLen - 1) 
		{ 
			return false; 
		}
		
		unsigned int byte = 0;
		if (!str.SubString(i, i + 1).ToUInt(&byte, 16))
		{
			return false;
		}

		bytesBuffer[currentByte] = (unsigned char)byte;

		currentByte++;
	}

	return true;
}

void MainGui::CloseApp(wxCloseEvent& e)
{
	Destroy();
}