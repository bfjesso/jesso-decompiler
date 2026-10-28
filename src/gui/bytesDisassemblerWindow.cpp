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
}

void BytesDisassemblerWindow::DisassembleBytes(wxCommandEvent& e)
{
	instructions.clear();
	instructions.shrink_to_fit();

	std::vector<uint8_t> bytes;
	if (ERROR_JDC == ParseStringBytes(bytesTextCtrl->GetValue(), bytes))
	{
		wxMessageBox("Failed to parse bytes", "Can't disassemble");
		return;
	}

	struct DisassemblerOptions options;
	options.is64BitMode = is64BitModeCheckBox->IsChecked();

	uint64_t currentOffset = 0;
	while (currentOffset < bytes.size())
	{
		struct DisassembledInstruction currentInstruction;
		if (disassembleInstruction(bytes.data() + currentOffset, bytes.data() + bytes.size() - 1, &options, &currentInstruction))
		{
			currentInstruction.address = currentOffset;
			instructions.push_back(currentInstruction);
			currentOffset += currentInstruction.numOfBytes;
		}
		else
		{
			wxMessageBox("Error disassembling bytes", "Can't disassemble");
			disassemblyTextCtrl->Clear();
			return;
		}
	}

	disassemblyTextCtrl->Initialize(instructions.data(), instructions.size(), 0, -1);
}

enum JdcStatus BytesDisassemblerWindow::ParseStringBytes(wxString str, std::vector<uint8_t>& bytesBuffer)
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
		return ERROR_JDC;
	}

	for (int32_t i = 0; i < strLen; i += 2)
	{
		uint32_t byte = 0;
		if (!str.SubString(i, i + 1).ToUInt(&byte, 16))
		{
			return ERROR_JDC;
		}

		bytesBuffer.push_back((uint8_t)byte);
	}

	return SUCCESS_JDC;
}