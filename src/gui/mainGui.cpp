#include "mainGui.h"
#include "../pe-handler/peHandler.h"

wxBEGIN_EVENT_TABLE(MainGui, wxFrame)
EVT_CLOSE(CloseApp)
EVT_BUTTON(DisassembleFileButtonID, DisassembleButton)
EVT_BUTTON(AnalyzeFileButtonID, AnalyzeButton)
EVT_BUTTON(OpenFileButtonID, OpenFileButton)
EVT_GRID_CELL_RIGHT_CLICK(RightClickOptions)
wxEND_EVENT_TABLE()

MainGui::MainGui() : wxFrame(nullptr, MainWindowID, "Jesso Decompiler x64", wxPoint(50, 50), wxSize(1300, 1000))
{
	SetOwnBackgroundColour(backgroundColor);

	menuBar = new wxMenuBar();
	bytesDisassemblerMenu = new BytesDisassembler();
	dataViewerMenu = new DataViewer();

	wxMenu* toolMenu = new wxMenu();

	wxMenuItem* openHexCalculator = toolMenu->Append(OpenBytesDisassemblerID, "Bytes Disassembler");
	openHexCalculator->SetBackgroundColour(foregroundColor);
	openHexCalculator->SetTextColour(textColor);
	toolMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& ce) -> void { bytesDisassemblerMenu->OpenMenu(GetPosition()); }, OpenBytesDisassemblerID);

	wxMenuItem* openDataViewer = toolMenu->Append(OpenDataViewerID, "Data Viewer");
	openHexCalculator->SetBackgroundColour(foregroundColor);
	openHexCalculator->SetTextColour(textColor);
	toolMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& ce) -> void { dataViewerMenu->OpenMenu(GetPosition()); }, OpenDataViewerID);

	menuBar->Append(toolMenu, "Tools");
	this->SetMenuBar(menuBar);

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

	analyzeFileButton = new wxButton(this, AnalyzeFileButtonID, "Analyze", wxPoint(0, 0), wxSize(75, 25));
	analyzeFileButton->SetOwnBackgroundColour(foregroundColor);
	analyzeFileButton->SetOwnForegroundColour(textColor);

	disassemblyListBox = new wxListBox(this, wxID_ANY, wxPoint(0, 0), wxSize(600, 600));
	disassemblyListBox->SetOwnBackgroundColour(foregroundColor);
	disassemblyListBox->SetOwnForegroundColour(textColor);

	// ---------------------

	decompilationListBox = new wxListBox(this, wxID_ANY, wxPoint(0, 0), wxSize(600, 600));
	decompilationListBox->SetOwnBackgroundColour(foregroundColor);
	decompilationListBox->SetOwnForegroundColour(textColor);

	// ---------------------

	functionsGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(9999, 9999));
	functionsGrid->SetLabelBackgroundColour(backgroundColor);
	functionsGrid->SetLabelTextColour(textColor);
	functionsGrid->SetDefaultCellBackgroundColour(foregroundColor);
	functionsGrid->SetDefaultCellTextColour(textColor);

	functionsGrid->CreateGrid(0, 4);
	functionsGrid->EnableGridLines(false);
	functionsGrid->SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectRows);
	functionsGrid->SetScrollRate(0, 10);
	functionsGrid->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
	functionsGrid->DisableDragRowSize();
	functionsGrid->EnableEditing(false);
	functionsGrid->SetColLabelValue(0, "Address");
	functionsGrid->SetColLabelValue(1, "Calling Convention");
	functionsGrid->SetColLabelValue(2, "Name");
	functionsGrid->SetColLabelValue(3, "Number of Instructions");
	functionsGrid->HideRowLabels();
	functionsGrid->SetColSize(0, 200);
	functionsGrid->SetColSize(1, 200);
	functionsGrid->SetColSize(2, 200);
	functionsGrid->SetColSize(3, 9999);
	functionsGrid->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	// ---------------------

	row1Sizer = new wxBoxSizer(wxHORIZONTAL);
	row2Sizer = new wxBoxSizer(wxHORIZONTAL);
	row3Sizer = new wxBoxSizer(wxHORIZONTAL);
	row4Sizer = new wxBoxSizer(wxHORIZONTAL);
	vSizer = new wxBoxSizer(wxVERTICAL);

	row1Sizer->Add(openFileButton, 0, wxALL, 10);
	row1Sizer->Add(numOfbytesInputTextCtrl, 0, wxRIGHT | wxBOTTOM | wxTOP, 10);
	row1Sizer->AddStretchSpacer();

	row2Sizer->Add(disassembleFileButton, 0, wxLEFT | wxBOTTOM | wxRIGHT, 10);
	row2Sizer->Add(analyzeFileButton, 0, wxLEFT | wxBOTTOM | wxRIGHT, 10);
	row2Sizer->AddStretchSpacer();

	row3Sizer->Add(disassemblyListBox, 0, wxBOTTOM | wxRIGHT | wxLEFT, 10);
	row3Sizer->AddStretchSpacer();
	row3Sizer->Add(decompilationListBox, 0, wxBOTTOM | wxRIGHT, 10);

	row4Sizer->Add(functionsGrid, 0, wxBOTTOM | wxRIGHT | wxLEFT, 10);

	vSizer->Add(row1Sizer, 0, wxEXPAND);
	vSizer->Add(row2Sizer, 0, wxEXPAND);
	vSizer->Add(row3Sizer, 0, wxEXPAND);
	vSizer->Add(row4Sizer, 0, wxEXPAND);

	SetSizer(vSizer);
}

void MainGui::OpenFileButton(wxCommandEvent& e)
{
	wxFileDialog openDllDialog(this, "Choose PE file", "", "", "EXE and DLL files (*.exe;*.dll)|*.exe;*.dll", wxFD_FILE_MUST_EXIST);

	if (openDllDialog.ShowModal() != wxID_CANCEL)
	{
		currentFilePath = openDllDialog.GetPath();
		dataViewerMenu->currentFilePath = currentFilePath;

		if (!currentFilePath.empty())
		{
			wxString fileName = openDllDialog.GetPath().Mid(openDllDialog.GetPath().Last('\\') + 1);
			this->SetTitle("Jesso Decompiler x64 - opened file " + fileName);
			wxMessageBox(fileName + " has been opened. It can now be disassembled and analyzed.", "Successfully opened the file");
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

void MainGui::DisassembleButton(wxCommandEvent& e) 
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
	
	DisassembleCodeSection(numOfBytesToRead);
}

void MainGui::AnalyzeButton(wxCommandEvent& e) 
{
	if (currentFilePath.empty())
	{
		wxMessageBox("No file opened", "Can't analyze");
		return;
	}

	functions.clear();
	functions.shrink_to_fit();
	functionsGrid->ClearGrid();
	
	FindAllFunctions();
}

void MainGui::DisassembleCodeSection(unsigned int numOfBytesToRead)
{
	unsigned char* bytes = new unsigned char[numOfBytesToRead];
	IMAGE_SECTION_HEADER codeSection = { 0 };
	const wchar_t* filePath = currentFilePath.c_str().AsWChar();
	unsigned char is64Bit = 0;
	if (!readCodeSection(filePath, bytes, numOfBytesToRead, &codeSection, &imageBase, &is64Bit))
	{
		wxMessageBox("Error reading bytes from file code section", "Can't disassemble");

		delete[] bytes;
		return;
	}

	struct DisassemblerOptions options;
	options.is64BitMode = is64Bit;

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
}

void MainGui::DecompileFunction(unsigned short functionIndex, const char* name)
{
	if (currentFilePath.empty())
	{
		wxMessageBox("No file opened", "Can't decompile");
		return;
	}

	decompilationListBox->Clear();

	DecompilationParameters params = {};
	params.functions = &functions[0];
	params.numOfFunctions = functions.size();
	params.currentFunc = &functions[functionIndex];
	params.startInstructionIndex = 0;

	LineOfC decompiledFunction[100];
	unsigned short numOfLinesDecompiled = decompileFunction(params, name, decompiledFunction, 100);
	if (numOfLinesDecompiled == 0)
	{
		wxMessageBox("Error decompiling function", "Can't decompile");
		return;
	}

	for (int i = 0; i < numOfLinesDecompiled; i++)
	{
		wxString str = wxString(decompiledFunction[i].line);

		for (int j = 0; j < decompiledFunction[i].indents; j++) 
		{
			str = "    " + str;
		}

		decompilationListBox->InsertItems(1, &str, 0);
	}
}

void MainGui::FindAllFunctions() 
{
	int numOfInstructions = disassembledInstructions.size();

	int functionNum = 0;

	Function currentFunction = {};
	int instructionIndex = 0;
	while (findNextFunction(&disassembledInstructions[instructionIndex], &instructionAddresses[instructionIndex], numOfInstructions, &currentFunction, &instructionIndex))
	{
		numOfInstructions -= currentFunction.numOfInstructions;

		functionsGrid->AppendRows(1);

		char addressStr[10];
		sprintf(addressStr, "%X", *currentFunction.addresses);
		functionsGrid->SetCellValue(functionNum, 0, wxString(addressStr));

		functionsGrid->SetCellValue(functionNum, 1, wxString(callingConventionStrs[currentFunction.callingConvention]));

		sprintf(addressStr, "%X", (*currentFunction.addresses) - imageBase);
		functionsGrid->SetCellValue(functionNum, 2, "func" + wxString(addressStr));

		currentFunction.name[0] = 0;
		strcpy(currentFunction.name, functionsGrid->GetCellValue(functionNum, 2).c_str().AsChar());

		functionsGrid->SetCellValue(functionNum, 3, std::to_string(currentFunction.numOfInstructions));
		
		functions.push_back(currentFunction);
		functionNum++;
	}

	if (functions.size() > 0) 
	{
		fixAllFunctionReturnTypes(&functions[0], functions.size());
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
			DecompileFunction(row, functionsGrid->GetCellValue(row, 2).c_str().AsChar());
		}, 100);

	wxMenuItem* cpyAddr = menu.Append(101, "Copy Address");
	cpyAddr->SetBackgroundColour(foregroundColor);
	cpyAddr->SetTextColour(textColor);
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(functionsGrid->GetCellValue(row, 0)); }, 101);

	PopupMenu(&menu, ScreenToClient(wxGetMousePosition()));
	e.Skip();
}

void MainGui::CloseApp(wxCloseEvent& e)
{
	bytesDisassemblerMenu->Destroy();
	dataViewerMenu->Destroy();
	Destroy();
}
