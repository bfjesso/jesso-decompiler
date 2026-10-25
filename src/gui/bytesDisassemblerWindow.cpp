#include "bytesDisassemblerWindow.h"
#include "../disassembler/disassembler.h"
#include "mainGui.h"

wxBEGIN_EVENT_TABLE(BytesDisassemblerWindow, wxWindow)
EVT_BUTTON(DisassembleButtonID, BytesDisassemblerWindow::DisassembleBytes)
wxEND_EVENT_TABLE()

BytesDisassemblerWindow::BytesDisassemblerWindow(wxWindow* parent, MainGui* mainGuiRef) : wxWindow(parent, wxID_ANY)
{
	SetMinSize(wxSize(150, 100));
	SetOwnBackgroundColour(backgroundColor);

	mainGui = mainGuiRef;

	bytesTextCtrl = new wxTextCtrl(this, wxID_ANY, "00", wxPoint(0, 0), wxSize(400, 25));
	bytesTextCtrl->SetOwnBackgroundColour(foregroundColor);
	bytesTextCtrl->SetOwnForegroundColour(textColor);
	bytesTextCtrl->SetToolTip("Bytes to disassemble");

	disassembleButton = new wxButton(this, DisassembleButtonID, "Disassemble", wxPoint(0, 0), wxSize(75, 25));
	disassembleButton->SetOwnBackgroundColour(foregroundColor);
	disassembleButton->SetOwnForegroundColour(textColor);

	is64BitModeCheckBox = new wxCheckBox(this, wxID_ANY, "64-bit mode");
	is64BitModeCheckBox->SetOwnForegroundColour(textColor);

	disassemblyTextCtrl = new DisassemblyTextCtrl(this, mainGui);

	row1Sizer = new wxBoxSizer(wxHORIZONTAL);
	row2Sizer = new wxBoxSizer(wxHORIZONTAL);
	vSizer = new wxBoxSizer(wxVERTICAL);

	row1Sizer->Add(bytesTextCtrl, 1, wxTOP | wxLEFT | wxRIGHT | wxCENTER, 10);

	row2Sizer->Add(disassembleButton, 1, wxLEFT | wxRIGHT | wxCENTER, 10);
	row2Sizer->Add(is64BitModeCheckBox, 1, wxLEFT | wxRIGHT | wxCENTER, 10);

	vSizer->Add(row1Sizer, 0, wxCENTER);
	vSizer->Add(row2Sizer, 0, wxCENTER);
	vSizer->Add(disassemblyTextCtrl, 1, wxEXPAND | wxALL, 10);

	SetSizerAndFit(vSizer);

	memset(&currentInstruction, 0, sizeof(struct DisassembledInstruction));
}

void BytesDisassemblerWindow::DisassembleBytes(wxCommandEvent& e)
{
	uint8_t bytes[15] = { 0 };
	int32_t numOfBytes = ParseStringBytes(bytesTextCtrl->GetValue(), bytes, 15);
	if (numOfBytes == 0)
	{
		wxMessageBox("Failed to parse bytes", "Can't disassemble");
		return;
	}

	struct DisassemblerOptions options;
	options.is64BitMode = is64BitModeCheckBox->IsChecked();

	memset(&currentInstruction, 0, sizeof(struct DisassembledInstruction));
	if (disassembleInstruction(bytes, bytes + numOfBytes, &options, &currentInstruction))
	{
		disassemblyTextCtrl->Initialize(&currentInstruction, 1, 0, -1);
	}
	else
	{
		wxMessageBox("Error disassembling bytes", "Can't disassemble");
		disassemblyTextCtrl->Clear();
	}
}

int32_t BytesDisassemblerWindow::ParseStringBytes(wxString str, uint8_t* bytesBuffer, uint8_t bytesBufferLen)
{
	str.Replace(" ", "", true);
	str.Replace("\\", "", true);
	str.Replace("0x", "", true);
	str.Replace("0X", "", true);
	str.Replace("x", "", true);
	str.Replace("X", "", true);

	size_t strLen = str.Length();
	if (strLen < 2 || strLen % 2 != 0)
	{
		return 0;
	}

	int32_t currentByte = 0;
	for (int32_t i = 0; i < strLen; i += 2)
	{
		if (currentByte > bytesBufferLen - 1)
		{
			return 0;
		}

		uint32_t byte = 0;
		if (!str.SubString(i, i + 1).ToUInt(&byte, 16))
		{
			return 0;
		}

		bytesBuffer[currentByte] = (uint8_t)byte;

		currentByte++;
	}

	return currentByte;
}