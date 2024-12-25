#include "../Headers/mainGui.h"
#include "../../PEHandler/Headers/peHandler.h"

wxBEGIN_EVENT_TABLE(MainGui, wxFrame)
EVT_CLOSE(CloseApp)
EVT_BUTTON(DisassembleTestBytesButtonID, DisassembleTestBytes)
EVT_BUTTON(DisassembleFileButtonID, DisassembleCodeSection)
EVT_BUTTON(OpenFileButtonID, GetFilePath)
EVT_GRID_CELL_RIGHT_CLICK(RightClickOptions)
wxEND_EVENT_TABLE()

MainGui::MainGui() : wxFrame(nullptr, MainWindowID, "Jesso Decompiler x64", wxPoint(50, 50), wxSize(850, 850))
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

	disassemblyListBox = new wxListBox(this, wxID_ANY, wxPoint(0, 0), wxSize(400, 400));
	disassemblyListBox->SetOwnBackgroundColour(foregroundColor);
	disassemblyListBox->SetOwnForegroundColour(textColor);

	// ---------------------

	decompilationListBox = new wxListBox(this, wxID_ANY, wxPoint(0, 0), wxSize(400, 400));
	decompilationListBox->SetOwnBackgroundColour(foregroundColor);
	decompilationListBox->SetOwnForegroundColour(textColor);

	// ---------------------

	functionsGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(9999, 9999));
	functionsGrid->SetLabelBackgroundColour(backgroundColor);
	functionsGrid->SetLabelTextColour(textColor);
	functionsGrid->SetDefaultCellBackgroundColour(foregroundColor);
	functionsGrid->SetDefaultCellTextColour(textColor);

	functionsGrid->CreateGrid(0, 3);
	functionsGrid->EnableGridLines(false);
	functionsGrid->SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectRows);
	functionsGrid->SetScrollRate(0, 10);
	functionsGrid->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
	functionsGrid->DisableDragRowSize();
	functionsGrid->EnableEditing(false);
	functionsGrid->SetColLabelValue(0, "Address");
	functionsGrid->SetColLabelValue(1, "Name");
	functionsGrid->SetColLabelValue(2, "Number of Instructions");
	functionsGrid->HideRowLabels();
	functionsGrid->SetColSize(0, 200);
	functionsGrid->SetColSize(1, 200);
	functionsGrid->SetColSize(2, 9999);
	functionsGrid->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	// ---------------------

	row1Sizer = new wxBoxSizer(wxHORIZONTAL);
	row2Sizer = new wxBoxSizer(wxHORIZONTAL);
	row3Sizer = new wxBoxSizer(wxHORIZONTAL);
	row4Sizer = new wxBoxSizer(wxHORIZONTAL);
	row5Sizer = new wxBoxSizer(wxHORIZONTAL);
	row6Sizer = new wxBoxSizer(wxHORIZONTAL);
	row7Sizer = new wxBoxSizer(wxHORIZONTAL);
	vSizer = new wxBoxSizer(wxVERTICAL);

	row1Sizer->Add(testBytesTextCtrl, 0, wxALL, 10);
	
	row2Sizer->Add(disassembleTestBytesButton, 0, wxLEFT | wxBOTTOM | wxRIGHT, 10);
	row2Sizer->Add(testDisassemblyStaticText, 0, wxBOTTOM | wxRIGHT, 10);

	row3Sizer->Add(is64BitModeCheckBox, 0, wxLEFT | wxBOTTOM | wxRIGHT, 10);

	row4Sizer->Add(openFileButton, 0, wxLEFT | wxBOTTOM | wxRIGHT, 10);
	row4Sizer->Add(numOfbytesInputTextCtrl, 0, wxRIGHT | wxBOTTOM, 10);
	row4Sizer->AddStretchSpacer();

	row5Sizer->Add(disassembleFileButton, 0, wxLEFT | wxBOTTOM | wxRIGHT, 10);
	row5Sizer->AddStretchSpacer();

	row6Sizer->Add(disassemblyListBox, 0, wxBOTTOM | wxRIGHT | wxLEFT, 10);
	row6Sizer->AddStretchSpacer();
	row6Sizer->Add(decompilationListBox, 0, wxBOTTOM | wxRIGHT, 10);

	row7Sizer->Add(functionsGrid, 0, wxBOTTOM | wxRIGHT | wxLEFT, 10);

	vSizer->Add(row1Sizer, 0, wxEXPAND);
	vSizer->Add(row2Sizer, 0, wxEXPAND);
	vSizer->Add(row3Sizer, 0, wxEXPAND);
	vSizer->Add(row4Sizer, 0, wxEXPAND);
	vSizer->Add(row5Sizer, 0, wxEXPAND);
	vSizer->Add(row6Sizer, 0, wxEXPAND);
	vSizer->Add(row7Sizer, 0, wxEXPAND);

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

void MainGui::GetFilePath(wxCommandEvent& e)
{
	currentFilePath = "";
	
	wxFileDialog openDllDialog(this, "Choose PE file", "", "", "EXE and DLL files (*.exe;*.dll)|*.exe;*.dll", wxFD_FILE_MUST_EXIST);

	if (openDllDialog.ShowModal() != wxID_CANCEL)
	{
		currentFilePath = openDllDialog.GetPath();

		if (!currentFilePath.empty())
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

	openDllDialog.Close(true);
}

void MainGui::DisassembleCodeSection(wxCommandEvent& e)
{
	if (currentFilePath.empty())
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
	const wchar_t* filePath = currentFilePath.c_str().AsWChar();
	if (!readCodeSection(filePath, bytes, numOfBytesToRead, &codeSection, &imageBase))
	{
		wxMessageBox("Error reading bytes from file code section", "Can't disassemble");

		delete[] bytes;
		return;
	}

	struct DisassemblerOptions options;
	options.is64BitMode = is64BitModeCheckBox->IsChecked();

	struct DisassembledInstruction currentInstruction;
	unsigned int currentIndex = 0;
	unsigned int instructionNum = 1;
	while (disassembleInstruction(&bytes[currentIndex], bytes + numOfBytesToRead - 1, &options, &currentInstruction))
	{
		uintptr_t address = imageBase + codeSection.VirtualAddress + currentIndex;

		char addressStr[10];
		sprintf(addressStr, "%X", address);

		currentIndex += currentInstruction.numOfBytes;
		
		char buffer[50];
		if (instructionToStr(&currentInstruction, buffer, 50))
		{
			disassemblyListBox->AppendString(std::to_string(instructionNum) + "\t" + wxString(addressStr) + "\t" + wxString(buffer));

			instructionAddresses.push_back(address);
			disassembledInstructions.push_back(currentInstruction);
		}
		else 
		{
			disassemblyListBox->AppendString(std::to_string(instructionNum) + "\t" + wxString(addressStr) + "\tERROR");
			break;
		}

		instructionNum++;
	}

	delete[] bytes;

	FindAllFunctions();
}

void MainGui::DecompileFunction(Function* function)
{
	if (currentFilePath.empty())
	{
		wxMessageBox("No file opened", "Can't decompile");
		return;
	}

	decompilationListBox->Clear();

	LineOfC decompiledFunction[10];
	unsigned short numOfLinesDecompiled = decompileFunction(function, decompiledFunction, 10);
	if (numOfLinesDecompiled == 0)
	{
		wxMessageBox("Error decompiling function", "Can't decompile");
		return;
	}

	for (int i = 0; i < numOfLinesDecompiled; i++)
	{
		wxString str = wxString(decompiledFunction[i].line);

		int symbolIndex = 0;
		while (str.Replace("\\", wxString(decompiledFunction[i].symbols[symbolIndex]), false))
		{
			symbolIndex++;
		}

		str.Replace("\t", "    ");

		decompilationListBox->InsertItems(1, &str, 0);
	}
}

void MainGui::FindAllFunctions() 
{
	functions.clear();
	functions.shrink_to_fit();
	functionsGrid->ClearGrid();
	
	int numOfInstructions = disassembledInstructions.size();

	int functionNum = 0;

	Function currentFunction = {};
	int instructionIndex = 0;
	while (findNextFunction(&disassembledInstructions[instructionIndex], &instructionAddresses[instructionIndex], numOfInstructions, &currentFunction, &instructionIndex))
	{
		functions.push_back(currentFunction);
		numOfInstructions -= currentFunction.numOfInstructions;

		functionsGrid->AppendRows(1);

		char addressStr[10];
		sprintf(addressStr, "%X", *currentFunction.address);
		functionsGrid->SetCellValue(functionNum, 0, wxString(addressStr));

		sprintf(addressStr, "%X", (*currentFunction.address) - imageBase);
		functionsGrid->SetCellValue(functionNum, 1, "func" + wxString(addressStr));

		functionsGrid->SetCellValue(functionNum, 2, std::to_string(currentFunction.numOfInstructions));
		functionNum++;
	}
}

void MainGui::RightClickOptions(wxGridEvent& e)
{
	wxMenu menu;

	int row = e.GetRow(); // row right-clicked on

	wxMenuItem* decompile = new wxMenuItem(0, 100, "Decompile");
	decompile->SetBackgroundColour(foregroundColor);
	decompile->SetTextColour(textColor);
	menu.Append(decompile);
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void
		{
			DecompileFunction(&functions[row]);
		}, 100);

	wxMenuItem* cpyAddr = menu.Append(101, "Copy Address");
	cpyAddr->SetBackgroundColour(foregroundColor);
	cpyAddr->SetTextColour(textColor);
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(functionsGrid->GetCellValue(row, 0)); }, 101);

	wxPoint pos = wxGetMousePosition() - this->GetPosition();
	PopupMenu(&menu, wxPoint(pos.x - 8, pos.y - 30));
	e.Skip();
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

void MainGui::CopyToClipboard(const char* txt)
{
	if (wxTheClipboard->Open())
	{
		wxTheClipboard->SetData(new wxTextDataObject(txt));
		wxTheClipboard->Close();
	}
}

void MainGui::CloseApp(wxCloseEvent& e)
{
	Destroy();
}