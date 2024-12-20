#include "../Headers/mainGui.h"
#include "../../PEHandler/Headers/peHandler.h"
#include "../../Decompiler/Headers/decompiler.h"
#include "sstream"

wxBEGIN_EVENT_TABLE(MainGui, wxFrame)
EVT_CLOSE(CloseApp)
EVT_BUTTON(DisassembleTestBytesButtonID, DisassembleTestBytes)
EVT_BUTTON(DisassembleFileButtonID, DisassembleCodeSection)
EVT_BUTTON(OpenFileButtonID, GetFileHandle)
EVT_BUTTON(DecompileButtonID, DecompileInstructions)
wxEND_EVENT_TABLE()

MainGui::MainGui() : wxFrame(nullptr, MainWindowID, "Jesso Decompiler x64", wxPoint(50, 50), wxSize(850, 800))
{
	SetOwnBackgroundColour(backgroundColor);

	testBytesTextCtrl = new wxTextCtrl(this, wxID_ANY, "0", wxPoint(0, 0), wxSize(100, 25));
	testBytesTextCtrl->SetOwnBackgroundColour(foregroundColor);
	testBytesTextCtrl->SetOwnForegroundColour(textColor);
	testBytesTextCtrl->SetToolTip("Bytes to disassemble (debugging)");

	disassembleTestBytesButton = new wxButton(this, DisassembleTestBytesButtonID, "Disassemble", wxPoint(0, 0), wxSize(75, 25));
	disassembleTestBytesButton->SetOwnBackgroundColour(foregroundColor);
	disassembleTestBytesButton->SetOwnForegroundColour(textColor);

	testDisassemblyStaticText = new wxStaticText(this, wxID_ANY, "Disassebled bytes");
	testDisassemblyStaticText->SetOwnForegroundColour(textColor);

	// ---------------------

	openFileButton = new wxButton(this, OpenFileButtonID, "Open File", wxPoint(0, 0), wxSize(75, 25));
	openFileButton->SetOwnBackgroundColour(foregroundColor);
	openFileButton->SetOwnForegroundColour(textColor);

	numOfbytesInputTextCtrl = new wxTextCtrl(this, wxID_ANY, "4096", wxPoint(0, 0), wxSize(100, 25));
	numOfbytesInputTextCtrl->SetOwnBackgroundColour(foregroundColor);
	numOfbytesInputTextCtrl->SetOwnForegroundColour(textColor);
	numOfbytesInputTextCtrl->SetToolTip("Number of bytes to read from the file's code section");

	disassembleFileButton = new wxButton(this, DisassembleFileButtonID, "Disassemble", wxPoint(0, 0), wxSize(75, 25));
	disassembleFileButton->SetOwnBackgroundColour(foregroundColor);
	disassembleFileButton->SetOwnForegroundColour(textColor);

	is64BitModeCheckBox = new wxCheckBox(this, wxID_ANY, "64-bit mode");
	is64BitModeCheckBox->SetOwnForegroundColour(textColor);

	disassemblyListBox = new wxListBox(this, wxID_ANY, wxPoint(0, 0), wxSize(400, 700));
	disassemblyListBox->SetOwnBackgroundColour(foregroundColor);
	disassemblyListBox->SetOwnForegroundColour(textColor);

	// ---------------------

	startDecompAddressTextCtrl = new wxTextCtrl(this, wxID_ANY, "1220", wxPoint(0, 0), wxSize(100, 25));
	startDecompAddressTextCtrl->SetOwnBackgroundColour(foregroundColor);
	startDecompAddressTextCtrl->SetOwnForegroundColour(textColor);
	startDecompAddressTextCtrl->SetToolTip("Address to begin decompiling from");

	numOfInstructionsDecompTextCtrl = new wxTextCtrl(this, wxID_ANY, "26", wxPoint(0, 0), wxSize(100, 25));
	numOfInstructionsDecompTextCtrl->SetOwnBackgroundColour(foregroundColor);
	numOfInstructionsDecompTextCtrl->SetOwnForegroundColour(textColor);
	numOfInstructionsDecompTextCtrl->SetToolTip("Number of instructions to decompile");

	decompileButton = new wxButton(this, DecompileButtonID, "Decompile", wxPoint(0, 0), wxSize(75, 25));
	decompileButton->SetOwnBackgroundColour(foregroundColor);
	decompileButton->SetOwnForegroundColour(textColor);

	decompilationListBox = new wxListBox(this, wxID_ANY, wxPoint(0, 0), wxSize(400, 700));
	decompilationListBox->SetOwnBackgroundColour(foregroundColor);
	decompilationListBox->SetOwnForegroundColour(textColor);

	// ---------------------

	row1Sizer = new wxBoxSizer(wxHORIZONTAL);
	row2Sizer = new wxBoxSizer(wxHORIZONTAL);
	row3Sizer = new wxBoxSizer(wxHORIZONTAL);
	row4Sizer = new wxBoxSizer(wxHORIZONTAL);
	row5Sizer = new wxBoxSizer(wxHORIZONTAL);
	row6Sizer = new wxBoxSizer(wxHORIZONTAL);
	vSizer = new wxBoxSizer(wxVERTICAL);

	row1Sizer->Add(testBytesTextCtrl, 0, wxALL, 10);
	
	row2Sizer->Add(disassembleTestBytesButton, 0, wxLEFT | wxBOTTOM | wxRIGHT, 10);
	row2Sizer->Add(testDisassemblyStaticText, 0, wxBOTTOM | wxRIGHT, 10);

	row3Sizer->Add(is64BitModeCheckBox, 0, wxLEFT | wxBOTTOM | wxRIGHT, 10);

	row4Sizer->Add(openFileButton, 0, wxLEFT | wxBOTTOM | wxRIGHT, 10);
	row4Sizer->Add(numOfbytesInputTextCtrl, 0, wxRIGHT | wxBOTTOM, 10);
	row4Sizer->AddStretchSpacer();
	row4Sizer->Add(startDecompAddressTextCtrl, 0, wxRIGHT | wxBOTTOM, 10);
	row4Sizer->Add(numOfInstructionsDecompTextCtrl, 0,wxRIGHT | wxBOTTOM, 10);

	row5Sizer->Add(disassembleFileButton, 0, wxLEFT | wxBOTTOM | wxRIGHT, 10);
	row5Sizer->AddStretchSpacer();
	row5Sizer->Add(decompileButton, 0, wxBOTTOM | wxRIGHT, 10);

	row6Sizer->Add(disassemblyListBox, 0, wxBOTTOM | wxRIGHT | wxLEFT, 10);
	row6Sizer->AddStretchSpacer();
	row6Sizer->Add(decompilationListBox, 0, wxBOTTOM | wxRIGHT, 10);

	vSizer->Add(row1Sizer, 0, wxEXPAND);
	vSizer->Add(row2Sizer, 0, wxEXPAND);
	vSizer->Add(row3Sizer, 0, wxEXPAND);
	vSizer->Add(row4Sizer, 0, wxEXPAND);
	vSizer->Add(row5Sizer, 0, wxEXPAND);
	vSizer->Add(row6Sizer, 0, wxEXPAND);

	SetSizer(vSizer);
}

void MainGui::DisassembleTestBytes(wxCommandEvent& e)
{
	unsigned char bytes[15];
	if (!ParseStringBytes(testBytesTextCtrl->GetValue(), bytes, 15)) 
	{
		wxMessageBox("Failed to parse bytes", "Can't disassemble");
		return;
	}

	struct DisassemblerOptions options;
	options.is64BitMode = is64BitModeCheckBox->IsChecked();

	struct DisassembledInstruction result;
	if(disassembleInstruction(bytes, bytes + 15, &options, &result))
	{
		char buffer[50];
		if (instructionToStr(&result, buffer, 50))
		{
			testDisassemblyStaticText->SetLabel(buffer);
		}
		else 
		{
			testDisassemblyStaticText->SetLabel("Error converting disassembled instruction to string");
		}
	}
	else 
	{
		testDisassemblyStaticText->SetLabel("Error disassembling instruction");
	}
}

void MainGui::GetFileHandle(wxCommandEvent& e)
{
	wxFileDialog openDllDialog(this, "Choose PE file", "", "", "EXE and DLL files (*.exe;*.dll)|*.exe;*.dll", wxFD_FILE_MUST_EXIST);

	if (openDllDialog.ShowModal() != wxID_CANCEL)
	{
		currentFile = CreateFile(openDllDialog.GetPath().c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (currentFile != INVALID_HANDLE_VALUE)
		{
			this->SetTitle("Jesso Decompiler x64 - opened file " + openDllDialog.GetPath().Mid(openDllDialog.GetPath().Last('\\') + 1));
		}
		else
		{
			this->SetTitle("Jesso Decompiler x64");
			wxMessageBox("Error opening the file", "Failed to open file");
			return;
		}
	}

	openDllDialog.Close();
}

void MainGui::DisassembleCodeSection(wxCommandEvent& e)
{
	if (currentFile == INVALID_HANDLE_VALUE)
	{
		wxMessageBox("No file opened", "Can't disassemble");
		return;
	}
	
	unsigned int numOfBytesToRead = 1;
	if (!numOfbytesInputTextCtrl->GetValue().ToUInt(&numOfBytesToRead)) 
	{
		wxMessageBox("Invalid number of bytes to read", "Can't read number of bytes input");
		return;
	}

	disassemblyListBox->Clear();

	instructionAddresses.clear();
	instructionAddresses.shrink_to_fit();

	disassembledInstructions.clear();
	disassembledInstructions.shrink_to_fit();

	unsigned char* bytes = new unsigned char[numOfBytesToRead];
	IMAGE_SECTION_HEADER codeSection = { 0 };
	if (!readCodeSection(currentFile, bytes, numOfBytesToRead, &codeSection))
	{
		wxMessageBox("Error reading bytes from file code section", "Can't disassemble");

		delete[] bytes;
		return;
	}

	struct DisassemblerOptions options;
	options.is64BitMode = is64BitModeCheckBox->IsChecked();

	struct DisassembledInstruction currentInstruction;
	unsigned int currentIndex = 0;
	while (disassembleInstruction(&bytes[currentIndex], bytes + numOfBytesToRead - 1, &options, &currentInstruction))
	{
		uintptr_t address = codeSection.VirtualAddress + currentIndex;

		std::stringstream adressToHex;
		adressToHex << std::hex << address;
		currentIndex += currentInstruction.numOfBytes;
		
		char buffer[50];
		if (instructionToStr(&currentInstruction, buffer, 50))
		{
			disassemblyListBox->AppendString(adressToHex.str() + "\t" + wxString(buffer));

			instructionAddresses.push_back(address);
			disassembledInstructions.push_back(currentInstruction);
		}
		else 
		{
			disassemblyListBox->AppendString(adressToHex.str() + "\tERROR");
			break;
		}
	}

	delete[] bytes;
}

void MainGui::DecompileInstructions(wxCommandEvent& e)
{
	uintptr_t startAddress = 0;
	if (!startDecompAddressTextCtrl->GetValue().ToULongLong(&startAddress, 16))
	{
		wxMessageBox("Invalid start address", "Can't read start address input");
		return;
	}
	
	unsigned int numOfInstructions = 1;
	if (!numOfInstructionsDecompTextCtrl->GetValue().ToUInt(&numOfInstructions))
	{
		wxMessageBox("Invalid number of instructions to read", "Can't read number of instructions input");
		return;
	}

	decompilationListBox->Clear();

	DisassembledInstruction* startInstruction = GetInstructionAtAddress(startAddress, 0, instructionAddresses.size() - 1);
	if (startInstruction == nullptr) 
	{
		wxMessageBox("There is no instruction at that address", "Can't find instruction at address");
		return;
	}

	LineOfC decompiledFunction[10];
	unsigned short numOfLinesDecompiled = decompileFunction(startInstruction, numOfInstructions, decompiledFunction, 10);
	if (numOfLinesDecompiled == 0)
	{
		wxMessageBox("Error decompiling instructions", "Can't decompile");
		return;
	}

	for (int i = 0; i < numOfLinesDecompiled; i++)
	{
		decompilationListBox->AppendString(wxString(decompiledFunction[i].line));
	}
}

DisassembledInstruction* MainGui::GetInstructionAtAddress(uintptr_t address, int low, int high)
{
	if (high < low) { return nullptr; }
	
	int mid = low + (high - low) / 2;

	if (instructionAddresses[mid] == address)
	{
		return &disassembledInstructions[mid];
	}

	if (instructionAddresses[mid] > address)
	{
		return GetInstructionAtAddress(address, low, mid - 1);
	}

	return GetInstructionAtAddress(address, mid + 1, high);
}

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