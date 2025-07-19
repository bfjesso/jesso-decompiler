#include "disassembleBytesMenu.h"
#include "../disassembler/disassembler.h"

wxBEGIN_EVENT_TABLE(BytesDisassembler, wxFrame)
EVT_CLOSE(CloseMenu)
EVT_BUTTON(DisassembleButtonID, DisassembleBytes)
wxEND_EVENT_TABLE()

BytesDisassembler::BytesDisassembler() : wxFrame(nullptr, MainWindowID, "Bytes Disassembler", wxPoint(50, 50), wxSize(300, 175))
{
	SetOwnBackgroundColour(backgroundColor);
	
	bytesTextCtrl = new wxTextCtrl(this, wxID_ANY, "00", wxPoint(0, 0), wxSize(200, 25));
	bytesTextCtrl->SetOwnBackgroundColour(foregroundColor);
	bytesTextCtrl->SetOwnForegroundColour(textColor);
	bytesTextCtrl->SetToolTip("Bytes to disassemble");

	disassembleButton = new wxButton(this, DisassembleButtonID, "Disassemble", wxPoint(0, 0), wxSize(75, 25));
	disassembleButton->SetOwnBackgroundColour(foregroundColor);
	disassembleButton->SetOwnForegroundColour(textColor);

	is64BitModeCheckBox = new wxCheckBox(this, wxID_ANY, "64-bit mode");
	is64BitModeCheckBox->SetOwnForegroundColour(textColor);

	disassemblyStaticText = new wxStaticText(this, wxID_ANY, "Disassebled bytes");
	disassemblyStaticText->SetOwnForegroundColour(textColor);

	// ---------------------

	row1Sizer = new wxBoxSizer(wxHORIZONTAL);
	row2Sizer = new wxBoxSizer(wxHORIZONTAL);
	row3Sizer = new wxBoxSizer(wxHORIZONTAL);
	vSizer = new wxBoxSizer(wxVERTICAL);

	row1Sizer->Add(bytesTextCtrl, 0, wxALL, 10);

	row2Sizer->Add(disassembleButton, 0, wxLEFT | wxBOTTOM | wxRIGHT, 10);
	row2Sizer->Add(is64BitModeCheckBox, 0, wxBOTTOM | wxRIGHT, 10);

	row3Sizer->Add(disassemblyStaticText, 0, wxLEFT | wxBOTTOM | wxRIGHT, 10);

	vSizer->Add(row1Sizer, 0, wxEXPAND);
	vSizer->Add(row2Sizer, 0, wxEXPAND);
	vSizer->Add(row3Sizer, 0, wxEXPAND);

	SetSizer(vSizer);
}

void BytesDisassembler::DisassembleBytes(wxCommandEvent& e)
{
	unsigned char bytes[15];
	if (!ParseStringBytes(bytesTextCtrl->GetValue(), bytes, 15))
	{
		wxMessageBox("Failed to parse bytes", "Can't disassemble");
		return;
	}

	struct DisassemblerOptions options;
	options.is64BitMode = is64BitModeCheckBox->IsChecked();

	struct DisassembledInstruction result;
	if (disassembleInstruction(bytes, bytes + 15, &options, &result))
	{
		char buffer[50];
		if (instructionToStr(&result, buffer, 50))
		{
			disassemblyStaticText->SetLabel(buffer);
		}
		else
		{
			disassemblyStaticText->SetLabel("Error converting disassembled instruction to string");
		}
	}
	else
	{
		disassemblyStaticText->SetLabel("Error disassembling instruction");
	}
}

bool BytesDisassembler::ParseStringBytes(wxString str, unsigned char* bytesBuffer, unsigned char bytesBufferLen)
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

void BytesDisassembler::OpenMenu(wxPoint position)
{
	position.x += 10;
	position.y += 10;
	SetPosition(position);
	Show();
	Raise();
}

void BytesDisassembler::CloseMenu(wxCloseEvent& e) // stops this frame from being destroyed and the data being lost
{
	Hide();
}
