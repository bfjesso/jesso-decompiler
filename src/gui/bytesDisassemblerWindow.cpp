#include "bytesDisassemblerWindow.h"
#include "../disassembler/disassembler.h"
#include "mainGui.h"

wxBEGIN_EVENT_TABLE(BytesDisassemblerWindow, wxWindow)
EVT_CHOICE(ISAChoiceID, BytesDisassemblerWindow::UpdateISA)
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

	dataInputTypeChoice = new wxChoice(this, wxID_ANY, wxPoint(0, 0), wxSize(125, 30), wxArrayString(2, dataInputTypeStrs));
	dataInputTypeChoice->SetSelection(HEX_DIT);
	dataInputTypeChoice->SetOwnBackgroundColour(foregroundColor);
	dataInputTypeChoice->SetOwnForegroundColour(textColor);

	disassembleButton = new wxButton(this, DisassembleButtonID, "Disassemble", wxPoint(0, 0), wxSize(100, 25));
	disassembleButton->SetOwnBackgroundColour(foregroundColor);
	disassembleButton->SetOwnForegroundColour(textColor);

	isaChoice = new wxChoice(this, ISAChoiceID, wxPoint(0, 0), wxSize(75, 30), wxArrayString(NUM_OF_ISAS, isaStrs));
	isaChoice->SetSelection(X86_ISA);
	isaChoice->SetOwnBackgroundColour(foregroundColor);
	isaChoice->SetOwnForegroundColour(textColor);

	is64BitModeCheckBox = new wxCheckBox(this, wxID_ANY, "64-bit mode");
	is64BitModeCheckBox->SetOwnForegroundColour(textColor);

	disassemblyTextCtrl = new DisassemblyTextCtrl(this, mainGui);

	row1Sizer = new wxBoxSizer(wxHORIZONTAL);
	row2Sizer = new wxBoxSizer(wxHORIZONTAL);
	vSizer = new wxBoxSizer(wxVERTICAL);

	row1Sizer->Add(bytesTextCtrl, 0, wxTOP | wxLEFT | wxRIGHT | wxCENTER, 10);
	row1Sizer->Add(dataInputTypeChoice, 0, wxTOP | wxRIGHT | wxCENTER, 10);

	row2Sizer->Add(disassembleButton, 0, wxLEFT | wxRIGHT | wxCENTER, 10);
	row2Sizer->Add(isaChoice, 0, wxRIGHT | wxCENTER, 10);
	row2Sizer->Add(is64BitModeCheckBox, 0, wxRIGHT | wxCENTER, 10);

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

	uint8_t addressability = 1;
	if (isa == LC3_ISA)
	{
		addressability = 2;
	}

	uint64_t currentOffset = 0;
	uint64_t currentAddress = 0;
	while (currentOffset < bytes.size())
	{
		struct DisassembledInstruction currentInstruction;
		if (disassembleInstruction(isa, bytes.data() + currentOffset, bytes.data() + bytes.size() - 1, &options, &currentInstruction))
		{
			currentInstruction.address = currentAddress;
			instructions.push_back(currentInstruction);
			currentOffset += currentInstruction.numOfBytes;
			currentAddress += (currentInstruction.numOfBytes / addressability);
		}
		else
		{
			uint8_t numOfDataBytes = currentOffset + (addressability - 1) < bytes.size() ? addressability : 1;
			if (ERROR_JDC == generateDataInstruction(&currentInstruction, currentAddress, bytes.data() + currentOffset, numOfDataBytes))
			{
				wxMessageBox("Error disassembling bytes", "Can't disassemble");
				return;
			}

			instructions.push_back(currentInstruction);
			currentOffset += numOfDataBytes;
			currentAddress += (numOfDataBytes / addressability);
		}
	}

	disassemblyTextCtrl->Initialize(instructions.data(), instructions.size(), 0, -1);
}

enum JdcStatus BytesDisassemblerWindow::ParseStringBytes(wxString str, std::vector<uint8_t>& bytesBuffer)
{
	str.Replace(" ", "", true);
	str.Replace(",", "", true);
	str.Replace("\\x", "", true);
	str.Replace("\\X", "", true);
	str.Replace("0x", "", true);
	str.Replace("0X", "", true);

	enum DataInputType dit = (enum DataInputType)(dataInputTypeChoice->GetSelection());
	uint8_t digitsPerByte = 0;
	uint8_t base = 0;
	switch (dit)
	{
	case HEX_DIT:
		digitsPerByte = 2;
		base = 16;
		break;
	case BINARY_DIT:
		digitsPerByte = 8;
		base = 2;
		break;
	default:
		return ERROR_JDC;
	}

	size_t strLen = str.Length();
	if (strLen < digitsPerByte || strLen % digitsPerByte != 0)
	{
		return ERROR_JDC;
	}

	for (int32_t i = 0; i < strLen; i += digitsPerByte)
	{
		uint32_t byte = 0;
		if (!str.SubString(i, i + (digitsPerByte - 1)).ToUInt(&byte, base))
		{
			return ERROR_JDC;
		}

		bytesBuffer.push_back((uint8_t)byte);
	}

	return SUCCESS_JDC;
}

void BytesDisassemblerWindow::UpdateISA(wxCommandEvent& e)
{
	isa = (enum InstructionSetArchitecture)(isaChoice->GetSelection());
	if (isa == X86_ISA)
	{
		is64BitModeCheckBox->Show();
	}
	else
	{
		is64BitModeCheckBox->Hide();
	}

	Layout();
}