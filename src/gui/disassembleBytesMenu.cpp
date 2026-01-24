#include "disassembleBytesMenu.h"
#include "../disassembler/disassembler.h"

wxBEGIN_EVENT_TABLE(BytesDisassembler, wxFrame)
EVT_CLOSE(BytesDisassembler::CloseMenu)
EVT_BUTTON(DisassembleButtonID, BytesDisassembler::DisassembleBytes)
wxEND_EVENT_TABLE()

BytesDisassembler::BytesDisassembler() : wxFrame(nullptr, MainWindowID, "Bytes Disassembler", wxPoint(50, 50), wxSize(300, 175))
{
	SetOwnBackgroundColour(backgroundColor);
	
	bytesTextCtrl = new wxTextCtrl(this, wxID_ANY, "00", wxPoint(0, 0), wxSize(300, 25));
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

	row1Sizer = new wxBoxSizer(wxHORIZONTAL);
	row2Sizer = new wxBoxSizer(wxHORIZONTAL);
	row3Sizer = new wxBoxSizer(wxHORIZONTAL);
	vSizer = new wxBoxSizer(wxVERTICAL);

	row1Sizer->Add(bytesTextCtrl, 1, wxTOP | wxLEFT | wxRIGHT | wxCENTER, 10);

	row2Sizer->Add(disassembleButton, 1, wxLEFT | wxRIGHT | wxCENTER, 10);
	row2Sizer->Add(is64BitModeCheckBox, 1, wxLEFT | wxRIGHT | wxCENTER, 10);

	row3Sizer->Add(disassemblyStaticText, 1, wxLEFT | wxBOTTOM | wxRIGHT | wxCENTER, 10);

	vSizer->Add(row1Sizer, 1, wxCENTER);
	vSizer->Add(row2Sizer, 1, wxCENTER);
	vSizer->Add(row3Sizer, 1, wxCENTER);

	SetSizerAndFit(vSizer);
}

void BytesDisassembler::DisassembleBytes(wxCommandEvent& e)
{
	unsigned char bytes[15] = { 0 };
	int numOfBytes = ParseStringBytes(bytesTextCtrl->GetValue(), bytes, 15);
	if (numOfBytes == 0)
	{
		wxMessageBox("Failed to parse bytes", "Can't disassemble");
		return;
	}

	struct DisassemblerOptions options;
	options.is64BitMode = is64BitModeCheckBox->IsChecked();

	struct DisassembledInstruction result;
	if (disassembleInstruction(bytes, bytes + numOfBytes, &options, &result, 0))
	{
		char buffer[255] = { 0 };
		if (instructionToStr(&result, buffer, 255))
		{
			if (result.isInvalid)
			{
				disassemblyStaticText->SetLabel(wxString(buffer) + wxString(" ; invalid opcode"));
			}
			else 
			{
				disassemblyStaticText->SetLabel(buffer);
			}
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

int BytesDisassembler::ParseStringBytes(wxString str, unsigned char* bytesBuffer, unsigned char bytesBufferLen)
{
	str.Replace(" ", "", true);
	str.Replace("\\", "", true);
	str.Replace("x", "", true);
	str.Replace("X", "", true);

	int strLen = str.Length();
	if (strLen < 2 || strLen % 2 != 0)
	{
		return 0;
	}

	int currentByte = 0;
	for (int i = 0; i < strLen; i += 2)
	{
		if (currentByte > bytesBufferLen - 1)
		{
			return 0;
		}

		unsigned int byte = 0;
		if (!str.SubString(i, i + 1).ToUInt(&byte, 16))
		{
			return 0;
		}

		bytesBuffer[currentByte] = (unsigned char)byte;

		currentByte++;
	}

	return currentByte;
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
