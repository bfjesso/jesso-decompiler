#include "mainGui.h"
#include "../decompiler/dataTypes.h"
#include "../disassembler/registers.h"
#include "../disassembler/mnemonics.h"
#include "../decompiler/functions.h"

wxBEGIN_EVENT_TABLE(MainGui, wxFrame)
EVT_CLOSE(MainGui::CloseApp)
EVT_GRID_CELL_RIGHT_CLICK(MainGui::GridRightClickOptions)
wxEND_EVENT_TABLE()

MainGui::MainGui() : wxFrame(nullptr, MainWindowID, "Jesso Decompiler x64", wxPoint(50, 50), wxSize(800, 600))
{
	SetOwnBackgroundColour(backgroundColor);

	statusStaticText = new wxStaticText(this, wxID_ANY, "Status: idle");
	statusStaticText->SetOwnForegroundColour(textColor);
	
	disassemblyTextCtrl = new wxStyledTextCtrl(this, wxID_ANY, wxPoint(0, 0), wxSize(400, 300));
	SetUpStyledTextCtrl(disassemblyTextCtrl);
	disassemblyTextCtrl->Bind(wxEVT_CONTEXT_MENU, [&](wxContextMenuEvent& e) -> void { StyledTextCtrlRightClickOptions(e); });

	decompilationTextCtrl = new wxStyledTextCtrl(this, wxID_ANY, wxPoint(0, 0), wxSize(300, 300));
	SetUpStyledTextCtrl(decompilationTextCtrl);
	decompilationTextCtrl->Bind(wxEVT_CONTEXT_MENU, [&](wxContextMenuEvent& e) -> void { StyledTextCtrlRightClickOptions(e); });

	functionsGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(800, 200));
	functionsGrid->SetLabelBackgroundColour(foregroundColor);
	functionsGrid->SetLabelTextColour(textColor);
	functionsGrid->SetDefaultCellBackgroundColour(gridColor);
	functionsGrid->SetDefaultCellTextColour(textColor);
	functionsGrid->CreateGrid(0, 4);
	functionsGrid->EnableGridLines(false);
	functionsGrid->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
	functionsGrid->SetScrollRate(0, 10);
	functionsGrid->SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectRows);
	functionsGrid->SetCellHighlightPenWidth(0);
	functionsGrid->SetCellHighlightROPenWidth(0);
	functionsGrid->DisableDragRowSize();
	functionsGrid->EnableEditing(false);
	functionsGrid->SetColLabelValue(0, "Address");
	functionsGrid->SetColLabelValue(1, "Calling convention");
	functionsGrid->SetColLabelValue(2, "Name");
	functionsGrid->SetColLabelValue(3, "Number of instructions");
	functionsGrid->HideRowLabels();
	functionsGrid->SetColSize(0, 200);
	functionsGrid->SetColSize(1, 200);
	functionsGrid->SetColSize(2, 200);
	functionsGrid->SetColSize(3, 9999);
	functionsGrid->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	menuBar = new wxMenuBar();
	bytesDisassemblerMenu = new BytesDisassembler();
	dataViewerMenu = new DataViewer();
	colorsMenu = new ColorsMenu(disassemblyTextCtrl, decompilationTextCtrl, dataViewerMenu->dataTextCtrl);

	wxMenu* fileMenu = new wxMenu();

	wxMenuItem* openFile = fileMenu->Append(OpenFileID, "Open file");
	fileMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& ce) -> void { OpenFile(); }, OpenFileID);

	wxMenuItem* disassembleFile = fileMenu->Append(DisassembleFileID, "Disassemble file");
	fileMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& ce) -> void { DisassembleFile(); }, DisassembleFileID);

	wxMenuItem* analyzeFile = fileMenu->Append(AnalyzeFileID, "Analyze file");
	fileMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& ce) -> void { AnalyzeFile(); }, AnalyzeFileID);

	wxMenu* toolMenu = new wxMenu();

	wxMenuItem* openBytesDisassembler = toolMenu->Append(OpenBytesDisassemblerID, "Bytes disassembler");
	toolMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& ce) -> void { bytesDisassemblerMenu->OpenMenu(GetPosition()); }, OpenBytesDisassemblerID);

	wxMenuItem* openDataViewer = toolMenu->Append(OpenDataViewerID, "Data viewer");
	toolMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& ce) -> void { dataViewerMenu->OpenMenu(GetPosition(), imageBase, dataSections, numOfDataSections, dataSectionBytes); }, OpenDataViewerID);

	wxMenu* optionsMenu = new wxMenu();

	wxMenuItem* colors = optionsMenu->Append(OpenColorsMenuID, "Colors");
	optionsMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& ce) -> void { colorsMenu->OpenMenu(GetPosition()); }, OpenColorsMenuID);

	menuBar->Append(fileMenu, "File");
	menuBar->Append(toolMenu, "Tools");
	menuBar->Append(optionsMenu, "Options");
	this->SetMenuBar(menuBar);

	row1Sizer = new wxBoxSizer(wxHORIZONTAL);
	row2Sizer = new wxBoxSizer(wxHORIZONTAL);
	vSizer = new wxBoxSizer(wxVERTICAL);

	row1Sizer->Add(disassemblyTextCtrl, 1, wxEXPAND | wxBOTTOM | wxRIGHT | wxLEFT, 10);
	row1Sizer->Add(decompilationTextCtrl, 1, wxEXPAND | wxBOTTOM | wxRIGHT, 10);

	row2Sizer->Add(functionsGrid, 1, wxBOTTOM | wxRIGHT | wxLEFT, 10);

	vSizer->Add(statusStaticText, 0, wxTOP | wxLEFT, 10);
	vSizer->Add(row1Sizer, 1, wxEXPAND);
	vSizer->Add(row2Sizer, 0, wxEXPAND);

	SetSizerAndFit(vSizer);
}

void MainGui::OpenFile()
{
	wxFileDialog openFileDialog(this, "Choose file", "", "", "", wxFD_FILE_MUST_EXIST);

	if (openFileDialog.ShowModal() != wxID_CANCEL)
	{
		ClearData();

		wxString filePath = openFileDialog.GetPath();
		if (!filePath.empty())
		{
			currentFilePath = filePath;

			if (!isFile64Bit(filePath.c_str().AsWChar(), &is64Bit))
			{
				this->SetTitle("Jesso Decompiler x64");
				wxMessageBox("Error determining file architecture", "Failed to open file");
				currentFilePath = "";
				return;
			}

			imageBase = getFileImageBase(filePath.c_str().AsWChar(), is64Bit);
			numOfImports = getAllImports(filePath.c_str().AsWChar(), is64Bit, imports, importsBufferMaxSize);
			
			wxString fileName = openFileDialog.GetPath().Mid(openFileDialog.GetPath().Last('\\') + 1);
			this->SetTitle("Jesso Decompiler x64 - opened file " + fileName);

			int disassembleAnswer = wxMessageBox("Do you want to disassemble the code sections?", "Disassemble code sections", wxYES_NO, this);
			if (disassembleAnswer == wxYES)
			{
				DisassembleFile();
			}
		}
		else
		{
			this->SetTitle("Jesso Decompiler x64");
			wxMessageBox("Error opening the file", "Failed to open file");
			currentFilePath = "";
			return;
		}
	}

	openFileDialog.Close(true);
}

void MainGui::DisassembleFile()
{
	if (currentFilePath == "")
	{
		wxMessageBox("No file opened", "Can't disassemble");
		return;
	}
	if (disassembledInstructions.size() > 0) 
	{
		wxMessageBox("File already disassembled", "Can't disassemble");
		return;
	}

	statusStaticText->SetLabelText("Status: disassembling...");
	statusStaticText->Refresh();
	statusStaticText->Update();

	DisassembleCodeSections();

	statusStaticText->SetLabelText("Status: finished disassembling, updating GUI...");
	statusStaticText->Refresh();
	statusStaticText->Update();

	UpdateDisassemblyTextCtrl();

	statusStaticText->SetLabelText("Status: idle");

	int answer = wxMessageBox("Do you want to analyze the file?", "Analyze file", wxYES_NO, this);
	if (answer == wxYES)
	{
		AnalyzeFile();
	}
}

void MainGui::AnalyzeFile() 
{
	if (currentFilePath == "")
	{
		wxMessageBox("No file opened", "Can't analyze");
		return;
	}
	if (disassembledInstructions.size() == 0) 
	{
		wxMessageBox("File not disassembled", "Can't analyze");
		return;
	}
	if (functions.size() > 0)
	{
		wxMessageBox("File already analyzed", "Can't analyze");
		return;
	}

	statusStaticText->SetLabelText("Status: finding all functions...");
	statusStaticText->Refresh();
	statusStaticText->Update();

	FindAllFunctions();

	statusStaticText->SetLabelText("Status: updating functions grid...");
	statusStaticText->Refresh();
	statusStaticText->Update();

	UpdateFunctionsGrid();

	statusStaticText->SetLabelText("Status: idle");

	int answer = wxMessageBox("Do you want to load bytes from the data section?", "Get data section bytes", wxYES_NO, this);
	if (answer == wxYES)
	{
		LoadDataSectionBytes();
	}

	answer = wxMessageBox("Do you want to look for function name symbols? This could take some time.", "Get function name symbols", wxYES_NO, this);
	if (answer == wxYES)
	{
		statusStaticText->SetLabelText("Status: looking for function name symbols...");
		statusStaticText->Refresh();
		statusStaticText->Update();
		
		GetFunctionSymbols();
	}

	statusStaticText->SetLabelText("Status: idle");
}

void MainGui::ClearData() 
{
	if (dataSectionBytes)
	{
		delete[] dataSectionBytes;
	}
	
	ClearStyledTextCtrl(disassemblyTextCtrl);

	disassembledInstructions.clear();
	disassembledInstructions.shrink_to_fit();

	for (int i = 0; i < numOfImports; i++) 
	{
		freeJdcStr(&imports[i].name);
	}
	
	for (int i = 0; i < functions.size(); i++)
	{
		freeFunction(&functions[i]);
	}

	functions.clear();
	functions.shrink_to_fit();
	ClearStyledTextCtrl(decompilationTextCtrl);
	int rows = functionsGrid->GetNumberRows();
	if (rows > 0)
	{
		functionsGrid->DeleteRows(0, functionsGrid->GetNumberRows());
	}
}

void MainGui::LoadDataSectionBytes()
{
	if (currentFilePath == "")
	{
		wxMessageBox("No file opened", "Can't load data");
		return;
	}

	numOfDataSections = getFileDataSections(currentFilePath.c_str().AsWChar(), is64Bit, dataSections, dataSectionsBufferSize);

	if (numOfDataSections == 0)
	{
		wxMessageBox("Failed to get any data section headers", "Can't get data section header");
		return;
	}

	int totalSize = 0;
	for (int i = 0; i < numOfDataSections; i++) 
	{
		totalSize += dataSections[i].size;
	}

	dataSectionBytes = new unsigned char[totalSize];

	int currentIndex = 0;
	for (int i = 0; i < numOfDataSections; i++)
	{
		if (!readFileSection(currentFilePath.c_str().AsWChar(), &dataSections[i], is64Bit, dataSectionBytes + currentIndex, dataSections[i].size))
		{
			wxMessageBox("Error reading bytes from file data section", "Can't load data");

			delete[] dataSectionBytes;
			return;
		}

		currentIndex += dataSections[i].size;
	}
}

void MainGui::DisassembleCodeSections()
{
	numOfCodeSections = getFileCodeSections(currentFilePath.c_str().AsWChar(), is64Bit, codeSections, 10);

	if (numOfCodeSections == 0)
	{
		wxMessageBox("No code sections found in file", "Can't disassemble");
		return;
	}

	struct DisassemblerOptions options = { 0 };
	options.is64BitMode = is64Bit;

	for (int i = 0; i < numOfCodeSections; i++)
	{
		unsigned char* bytes = new unsigned char[codeSections[i].size];
		if (!readFileSection(currentFilePath.c_str().AsWChar(), &codeSections[i], is64Bit, bytes, codeSections[i].size))
		{
			wxMessageBox("Error reading bytes from file code section", "Can't disassemble");
			delete[] bytes;
			return;
		}

		int instructionNum = 0;

		struct DisassembledInstruction currentInstruction;
		unsigned int currentIndex = 0;
		unsigned char numOfBytes = 0;
		unsigned long long jmpTableAddress = 0;
		while (disassembleInstruction(&bytes[currentIndex], bytes + codeSections[i].size - 1, &options, &currentInstruction, &numOfBytes))
		{
			if (numOfBytes == 0)
			{
				break;
			}

			currentInstruction.address = imageBase + codeSections[i].virtualAddress + currentIndex;
			currentIndex += numOfBytes;

			disassembledInstructions.push_back(currentInstruction);

			if (currentInstruction.opcode == NO_MNEMONIC)
			{
				break;
			}
			else if (codeSections[i].virtualAddress + currentIndex == jmpTableAddress)
			{
				struct DisassembledInstruction dataInstruction;
				dataInstruction.opcode = DATA;
				dataInstruction.operands[0].type = IMMEDIATE;
				dataInstruction.operands[1].type = NO_OPERAND;
				dataInstruction.operands[2].type = NO_OPERAND;
				dataInstruction.operands[3].type = NO_OPERAND;
				dataInstruction.group1Prefix = NO_PREFIX;
				dataInstruction.isInvalid = 0;

				while (bytes[currentIndex] != 0xCC || bytes[currentIndex + 1] != 0xCC)
				{
					dataInstruction.operands[0].immediate.value = bytes[currentIndex];
					dataInstruction.address = imageBase + codeSections[i].virtualAddress + currentIndex;
					disassembledInstructions.push_back(dataInstruction);
					currentIndex++;
					instructionNum++;
				}

				jmpTableAddress = 0;
			}
			else if (jmpTableAddress == 0) 
			{
				jmpTableAddress = getJumpTableAddress(&disassembledInstructions[0], disassembledInstructions.size()); // returns zero if there is none
			}

			instructionNum++;
		}

		delete[] bytes;
	}
}

void MainGui::DecompileFunction(int functionIndex)
{
	if (currentFilePath == "")
	{
		wxMessageBox("No file opened", "Can't decompile");
		return;
	}

	ClearStyledTextCtrl(decompilationTextCtrl);

	decompParams.currentFunc = &functions[functionIndex];
	decompParams.startInstructionIndex = 0;
	decompParams.skipUpperBound = -1;
	decompParams.skipLowerBound = -1;

	struct JdcStr decompiledFunction = initializeJdcStr();
	if (decompiledFunction.bufferSize == 0)
	{
		wxMessageBox("Error allocating memory for function decompilation", "Can't decompile");
		return;
	}

	if (!decompileFunction(decompParams, &decompiledFunction))
	{
		wxMessageBox("Error decompiling function", "Can't decompile");
		freeJdcStr(&decompiledFunction);
		return;
	}

	decompilationTextCtrl->SetReadOnly(false);
	decompilationTextCtrl->SetValue(decompiledFunction.buffer);
	freeJdcStr(&decompiledFunction);
	ApplySyntaxHighlighting(decompParams.currentFunc);
	decompilationTextCtrl->SetReadOnly(true);
	currentDecompiledFunc = functionIndex;
}

void MainGui::FindAllFunctions() 
{
	decompParams.imports = imports;
	decompParams.numOfImports = numOfImports;
	decompParams.startInstructionIndex = 0;

	decompParams.allInstructions = disassembledInstructions.data();
	decompParams.totalNumOfInstructions = disassembledInstructions.size();

	decompParams.imageBase = imageBase;
	decompParams.dataSections = dataSections;
	decompParams.numOfDataSections = numOfDataSections;
	decompParams.dataSectionByte = dataSectionBytes;

	decompParams.is64Bit = is64Bit;
	
	int numOfInstructions = disassembledInstructions.size();
	int instructionIndex = 0;
	int codeSectionIndex = 1;
	unsigned long long nextSectionStartAddress = imageBase + codeSections[codeSectionIndex].virtualAddress;

	struct Function currentFunction;
	memset(&currentFunction, 0, sizeof(struct Function));
	while (instructionIndex < disassembledInstructions.size() && findNextFunction(decompParams, nextSectionStartAddress, &currentFunction, &instructionIndex))
	{
		if (disassembledInstructions[instructionIndex].address >= nextSectionStartAddress)
		{
			codeSectionIndex++;
			nextSectionStartAddress = imageBase + codeSections[codeSectionIndex].virtualAddress;
		}

		currentFunction.name = initializeJdcStr();
		sprintfJdc(&currentFunction.name, 0, "func%llX", currentFunction.instructions[0].address - imageBase);

		functions.push_back(currentFunction);
		memset(&currentFunction, 0, sizeof(struct Function));

		decompParams.startInstructionIndex = instructionIndex;
	}

	if (functions.size() > 0) 
	{
		fixAllFunctionReturnTypes(&functions[0], functions.size(), is64Bit);
		fixAllFunctionArgs(&functions[0], functions.size());
	}

	numOfDataSections = 0;
	if (dataSectionBytes)
	{
		delete[] dataSectionBytes;
	}

	decompParams.functions = &functions[0];
	decompParams.numOfFunctions = functions.size();
}

void MainGui::UpdateDisassemblyTextCtrl()
{
	disassemblyTextCtrl->SetReadOnly(false);
	disassemblyTextCtrl->Freeze();

	int numOfInstructions = disassembledInstructions.size();
	int codeSectionIndex = 0;

	wxString disassemblyText = "";
	disassemblyText.reserve(numOfInstructions * 50);

	for (int i = 0; i < numOfInstructions; i++) 
	{
		if (disassembledInstructions[i].address > codeSections[codeSectionIndex].virtualAddress + codeSections[codeSectionIndex].size + imageBase)
		{
			codeSectionIndex++;
		}
		
		char addressStr[20] = { 0 };
		sprintf(addressStr, "%llX", disassembledInstructions[i].address);
		wxString addressInfoStr = wxString(addressStr) + wxString(codeSections[codeSectionIndex].name) + "\t";

		char buffer[255] = { 0 };
		wxString asmStr = "";
		if (instructionToStr(&disassembledInstructions[i], buffer, 255))
		{
			asmStr = wxString(buffer);
		}

		if (disassembledInstructions[i].isInvalid)
		{
			asmStr += " ; invalid opcode";
		}

		disassemblyText += addressInfoStr + asmStr + "\n";
	}

	disassemblyTextCtrl->SetText(disassemblyText);
	ApplyAsmHighlighting();

	disassemblyTextCtrl->Thaw();
	disassemblyTextCtrl->SetReadOnly(true);
}

void MainGui::UpdateFunctionsGrid()
{
	functionsGrid->Freeze();

	int numOfFunctions = functions.size();
	for (int i = 0; i < numOfFunctions; i++) 
	{
		functionsGrid->AppendRows(1);

		if (functions[i].instructions)
		{
			char addressStr[10];
			sprintf(addressStr, "%llX", functions[i].instructions[0].address);
			functionsGrid->SetCellValue(i, 0, wxString(addressStr));

			functionsGrid->SetCellValue(i, 1, wxString(callingConventionStrs[functions[i].callingConvention]));
		}

		functionsGrid->SetCellValue(i, 2, functions[i].name.buffer);
		functionsGrid->SetCellValue(i, 3, std::to_string(functions[i].numOfInstructions));
	}

	functionsGrid->Thaw();
}

void MainGui::GetFunctionSymbols() 
{
	functionsGrid->Freeze();

	int numOfFunctions = functions.size();
	for (int i = 0; i < numOfFunctions; i++)
	{
		if (getSymbolByValue(currentFilePath.c_str().AsWChar(), is64Bit, functions[i].instructions[0].address, &functions[i].name))
		{
			functionsGrid->SetCellValue(i, 2, functions[i].name.buffer);
		}
	}

	functionsGrid->Thaw();
}

void MainGui::GridRightClickOptions(wxGridEvent& e)
{
	wxMenu menu;

	int row = e.GetRow(); // row right-clicked on

	const int ID_DECOMPILE = 100;
	const int ID_EDIT_PROPERTIES = 101;
	const int ID_COPY_ADDRESS = 102;
	const int ID_COPY_NAME = 103;
	const int ID_FIND_BY_ADDR = 104;
	const int ID_FIND_BY_NAME = 105;

	menu.Append(ID_DECOMPILE, "Decompile");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { DecompileFunction(row); }, ID_DECOMPILE);

	menu.Append(ID_EDIT_PROPERTIES, "Edit properties");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { functionPropertiesMenu = new FunctionPropertiesMenu(GetPosition(), this, row); }, ID_EDIT_PROPERTIES);

	menu.Append(ID_COPY_ADDRESS, "Copy address");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(functionsGrid->GetCellValue(row, 0)); }, ID_COPY_ADDRESS);

	menu.Append(ID_COPY_NAME, "Copy name");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(functionsGrid->GetCellValue(row, 2)); }, ID_COPY_NAME);

	menu.Append(ID_FIND_BY_ADDR, "Find function by address");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { 
		wxTextEntryDialog dlg(this, "", "Address");
		if (dlg.ShowModal() == wxID_OK)
		{
			wxString txt = dlg.GetValue();
			unsigned long long address = 0;
			if (txt.ToULongLong(&address, 16)) 
			{
				int index = findFunctionByAddress(&functions[0], 0, functions.size() - 1, address);
				if (index == -1)
				{
					wxMessageBox("No function with that address", "Failed to find function");
				}
				else
				{
					functionsGrid->GoToCell(index, 0);
					functionsGrid->SelectRow(index);
				}
			}
			else 
			{
				wxMessageBox("Not valid hex number", "Failed to find function");
			}
		}
	}, ID_FIND_BY_ADDR);

	menu.Append(ID_FIND_BY_NAME, "Find function by name");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void {
		wxTextEntryDialog dlg(this, "", "Name");
		if (dlg.ShowModal() == wxID_OK)
		{
			unsigned char funcFound = 0;
			wxString txt = dlg.GetValue();
			if (!txt.IsEmpty())
			{
				int numOfFunctions = functions.size();
				for (int i = 0; i < numOfFunctions; i++) 
				{
					if (strcmp(functions[i].name.buffer, txt.c_str()) == 0) 
					{
						functionsGrid->GoToCell(i, 0);
						functionsGrid->SelectRow(i);
						funcFound = 1;
						break;
					}
				}
			}

			if (!funcFound) 
			{
				wxMessageBox("No function with that name", "Failed to find function");
			}
		}
		}, ID_FIND_BY_NAME);

	PopupMenu(&menu, ScreenToClient(wxGetMousePosition()));
	e.Skip();
}

void MainGui::StyledTextCtrlRightClickOptions(wxContextMenuEvent& e) 
{
	wxMenu menu;

	const int ID_COPY = 100;
	const int ID_SELECT_ALL = 101;
	const int ID_CONVERT_NUMBER = 102;
	const int ID_FIND = 103;

	wxStyledTextCtrl* ctrl = (wxStyledTextCtrl*)(e.GetEventObject());

	long start;
	long end;
	ctrl->GetSelection(&start, &end);
	wxString selection = "";

	if (start != end)
	{
		wxString text = ctrl->GetValue();
		selection = text.substr(start, end - start);

		menu.Append(ID_COPY, "Copy");
		menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) { CopyToClipboard(selection); }, ID_COPY);

		int numColor = ctrl == disassemblyTextCtrl ? ColorsMenu::DisassemblyColor::CONSTANT_COLOR : ColorsMenu::DecompilationColor::NUMBER_COLOR;
		if (ctrl->GetStyleAt(start) == numColor && !IsCharDigit(text[start - 1]) && !IsCharDigit(text[end]))
		{
			long long num = 0;
			unsigned long long unum = 0;
			if (selection.ToLongLong(&num, 10))
			{
				menu.Append(ID_CONVERT_NUMBER, "Convert to hexadecimal");
				menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
					ctrl->SetReadOnly(false);
					char numStr[50] = { 0 };
					sprintf(numStr, "0x%llX", num);
					ctrl->Replace(start, end, numStr);
					ctrl->StartStyling(start);
					ctrl->SetStyling(strlen(numStr), numColor);
					ctrl->SetReadOnly(true);
					}, ID_CONVERT_NUMBER);
			}
			else if (selection.ToLongLong(&num, 16))
			{
				menu.Append(ID_CONVERT_NUMBER, "Convert to decimal");
				menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) { 
					ctrl->SetReadOnly(false);
					wxString numStr = std::to_string(num);
					ctrl->Replace(start, end, numStr);
					ctrl->StartStyling(start);
					ctrl->SetStyling(strlen(numStr), numColor);
					ctrl->SetReadOnly(true);
					}, ID_CONVERT_NUMBER);
			}
			else if (selection.ToULongLong(&unum, 10))
			{
				menu.Append(ID_CONVERT_NUMBER, "Convert to hexadecimal");
				menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
					ctrl->SetReadOnly(false);
					char numStr[50] = { 0 };
					sprintf(numStr, "0x%llX", unum);
					ctrl->Replace(start, end, numStr);
					ctrl->StartStyling(start);
					ctrl->SetStyling(strlen(numStr), numColor);
					ctrl->SetReadOnly(true);
					}, ID_CONVERT_NUMBER);
			}
			else if (selection.ToULongLong(&unum, 16))
			{
				menu.Append(ID_CONVERT_NUMBER, "Convert to decimal");
				menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
					ctrl->SetReadOnly(false);
					wxString numStr = std::to_string(unum);
					ctrl->Replace(start, end, numStr);
					ctrl->StartStyling(start);
					ctrl->SetStyling(strlen(numStr), numColor);
					ctrl->SetReadOnly(true);
					}, ID_CONVERT_NUMBER);
			}
		}
	}

	
	menu.Append(ID_SELECT_ALL, "Select all");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
		ctrl->SetSelection(0, ctrl->GetLastPosition());
		ctrl->SetFocus();
		}, ID_SELECT_ALL);

	menu.Append(ID_FIND, "Find");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
		wxTextEntryDialog dlg(this, "", "Find text");
		if (dlg.ShowModal() == wxID_OK) 
		{
			wxString txt = dlg.GetValue();
			if (!txt.IsEmpty()) 
			{
				int pos = ctrl->FindText(0, ctrl->GetLength(), txt);
				if (pos == -1) 
				{
					wxMessageBox("Text not found", "Failed to find text");
				}
				else 
				{
					ctrl->GotoPos(pos);
					ctrl->SetSelection(pos, pos + txt.size());
				}
			}
		}
		}, ID_FIND);

	PopupMenu(&menu, ScreenToClient(e.GetPosition()));
}

void MainGui::CloseApp(wxCloseEvent& e)
{
	ClearData();
	bytesDisassemblerMenu->Destroy();
	dataViewerMenu->Destroy();
	colorsMenu->Destroy();
	Destroy();
}

char MainGui::IsCharDigit(char c)
{
	return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
}

void MainGui::ApplySyntaxHighlighting(Function* function)
{
	wxString text = decompilationTextCtrl->GetValue();

	decompilationTextCtrl->StartStyling(0);
	decompilationTextCtrl->SetStyling(text.length(), ColorsMenu::DecompilationColor::OPERATOR_COLOR);

	// local vars
	for (int i = 0; i < function->numOfLocalVars; i++) 
	{
		ColorAllStrs(text, function->localVars[i].name.buffer, ColorsMenu::DecompilationColor::LOCAL_VAR_COLOR, 1);
	}

	// reg vars
	for (int i = 0; i < function->numOfRegVars; i++)
	{
		ColorAllStrs(text, function->regVars[i].name.buffer, ColorsMenu::DecompilationColor::LOCAL_VAR_COLOR, 1);
	}

	// returned vars
	for (int i = 0; i < function->numOfReturnedVars; i++)
	{
		ColorAllStrs(text, function->returnedVars[i].name.buffer, ColorsMenu::DecompilationColor::LOCAL_VAR_COLOR, 1);
	}

	// stack args
	for (int i = 0; i < function->numOfStackArgs; i++)
	{
		ColorAllStrs(text, function->stackArgs[i].name.buffer, ColorsMenu::DecompilationColor::ARGUMENT_COLOR, 1);
	}

	// reg args
	for (int i = 0; i < function->numOfRegArgs; i++)
	{
		ColorAllStrs(text, function->regArgs[i].name.buffer, ColorsMenu::DecompilationColor::ARGUMENT_COLOR, 1);
	}

	// functions
	for (int i = 0; i < functions.size(); i++)
	{
		ColorAllStrs(text, functions[i].name.buffer, ColorsMenu::DecompilationColor::FUNCTION_COLOR, 0);
	}

	// imports
	for (int i = 0; i < numOfImports; i++)
	{
		ColorAllStrs(text, imports[i].name.buffer, ColorsMenu::DecompilationColor::IMPORT_COLOR, 0);
	}

	// opcodes
	for (int i = 0; i < numOfImports; i++)
	{
		ColorAllStrs(text, imports[i].name.buffer, ColorsMenu::DecompilationColor::IMPORT_COLOR, 0);
	}

	// calling conventions
	for (int i = 0; i < 4; i++)
	{
		ColorAllStrs(text, callingConventionStrs[i], ColorsMenu::DecompilationColor::PRIMITIVE_COLOR, 0);
	}

	// primitive data types
	for (int i = 0; i < numOfPrimitiveTypes; i++) 
	{
		ColorAllStrs(text, primitiveTypeStrs[i], ColorsMenu::DecompilationColor::PRIMITIVE_COLOR, 0);
	}

	// keywords
	const char* keywordStrs[6] = { "if", "else", "for", "while", "do", "return" };
	for (int i = 0; i < 6; i++)
	{
		ColorAllStrs(text, keywordStrs[i], ColorsMenu::DecompilationColor::KEYWORD_COLOR, 0);
	}

	// strings
	int start = 0;
	while (start < text.length())
	{
		int pos = text.find("\"", start);
		int end = text.find("\"", pos + 1);
		if (pos != wxNOT_FOUND && end != wxNOT_FOUND)
		{
			decompilationTextCtrl->StartStyling(pos);
			decompilationTextCtrl->SetStyling(end - pos + 1, ColorsMenu::DecompilationColor::STRING_COLOR);

			start = end + 1;
		}
		else
		{
			break;
		}
	}

	// numbers
	const char* numberChars[17] = { "0x", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F" };
	for (int i = 0; i < 17; i++)
	{
		ColorAllStrs(text, numberChars[i], ColorsMenu::DecompilationColor::NUMBER_COLOR, 0);
	}
}

void MainGui::ApplyAsmHighlighting()
{
	int numOfInstructions = disassembledInstructions.size();
	int pos = 0;
	wxString disassemblyText = disassemblyTextCtrl->GetValue();

	for (int i = 0; i < numOfInstructions; i++) 
	{
		struct DisassembledInstruction* instruction = &disassembledInstructions[i];
		
		int tabPos = disassemblyText.find('\t', pos);
		wxString addressInfoStr = disassemblyText.substr(pos, tabPos - pos);
		wxString asmStr = disassemblyText.substr(tabPos + 1, disassemblyText.find('\n', tabPos) - (tabPos + 1));
		
		disassemblyTextCtrl->StartStyling(pos);
		disassemblyTextCtrl->SetStyling(addressInfoStr.length(), ColorsMenu::DisassemblyColor::ADDRESS_COLOR);

		pos += addressInfoStr.length() + 1;

		disassemblyTextCtrl->StartStyling(pos);
		disassemblyTextCtrl->SetStyling(asmStr.length(), ColorsMenu::DisassemblyColor::PUNCTUATION_COLOR);

		disassemblyTextCtrl->StartStyling(pos);

		int opcodeLen = strlen(mnemonicStrs[instruction->opcode]) + 1;
		if (instruction->group1Prefix != NO_PREFIX) 
		{
			opcodeLen += strlen(getGroup1PrefixStr(instruction->group1Prefix)) + 1;
		}
		disassemblyTextCtrl->SetStyling(opcodeLen, ColorsMenu::DisassemblyColor::OPCODE_COLOR);

		// operands
		int regStart = 0;
		int segStart = 0;
		int ptrSizeStart = 0;
		for (int i = 0; i < 4; i++)
		{
			wxString regStr = "";
			wxString regStr2 = "";
			if (instruction->operands[i].type == REGISTER)
			{
				regStr = wxString(registerStrs[instruction->operands[i].reg]);
			}
			else if (instruction->operands[i].type == MEM_ADDRESS)
			{
				if (instruction->operands[i].memoryAddress.reg != NO_REG)
				{
					regStr = wxString(registerStrs[instruction->operands[i].memoryAddress.reg]);
				}

				if (instruction->operands[i].memoryAddress.regDisplacement != NO_REG)
				{
					regStr2 = wxString(registerStrs[instruction->operands[i].memoryAddress.regDisplacement]);
				}
			}

			if (!regStr.IsEmpty())
			{
				int loc = asmStr.find(regStr, regStart);
				disassemblyTextCtrl->StartStyling(pos + loc);
				disassemblyTextCtrl->SetStyling(regStr.length(), ColorsMenu::DisassemblyColor::REGISTER_COLOR);
				regStart = loc + regStr.length();
			}

			if (!regStr2.IsEmpty())
			{
				int loc = asmStr.find(regStr2, regStart);
				disassemblyTextCtrl->StartStyling(pos + loc);
				disassemblyTextCtrl->SetStyling(regStr2.length(), ColorsMenu::DisassemblyColor::REGISTER_COLOR);
				regStart = loc + regStr2.length();
			}

			if (instruction->operands[i].type == MEM_ADDRESS)
			{
				// segment
				if (instruction->operands[i].memoryAddress.segment != NO_SEGMENT)
				{
					wxString segStr = wxString(segmentStrs[instruction->operands[i].memoryAddress.segment]) + ":";
					int loc = asmStr.find(segStr, segStart);
					disassemblyTextCtrl->StartStyling(pos + loc);
					disassemblyTextCtrl->SetStyling(segStr.length(), ColorsMenu::DisassemblyColor::SEGMENT_COLOR);
					segStart = loc + segStr.length();
				}

				// ptr size
				int ptrSize = instruction->operands[i].memoryAddress.ptrSize;
				if (ptrSize != 0)
				{
					wxString sizeStr = wxString(getPtrSizeStr(ptrSize));
					int loc = asmStr.find(sizeStr, ptrSizeStart);
					disassemblyTextCtrl->StartStyling(pos + loc);
					disassemblyTextCtrl->SetStyling(sizeStr.length(), ColorsMenu::DisassemblyColor::PTR_SIZE_COLOR);
					ptrSizeStart = loc + sizeStr.length();
				}
			}
		}

		// numbers
		int start = 0;
		while (start < asmStr.length())
		{
			int num = asmStr.find("0x", start);
			if (num != wxNOT_FOUND)
			{
				int end = asmStr.length();
				for (int i = num + 2; i < end; i++)
				{
					if ((asmStr[i] < '0' || asmStr[i] > '9') && (asmStr[i] < 'A' || asmStr[i] > 'F'))
					{
						end = i;
						break;
					}
				}

				disassemblyTextCtrl->StartStyling(pos + num);
				disassemblyTextCtrl->SetStyling(end - num, ColorsMenu::DisassemblyColor::CONSTANT_COLOR);

				start = end + 1;
			}
			else
			{
				break;
			}
		}

		pos += asmStr.size();
	}
}

void MainGui::ColorAllStrs(wxString text, wxString str, ColorsMenu::DecompilationColor color, unsigned char forceColor)
{
	if(str == "")
	{
		return;
	}

	int start = 0;
	int pos = 0;
	while (start < text.length())
	{
		pos = text.find(str, start);
		if (pos != wxNOT_FOUND)
		{
			int end = pos + str.length();

			if (forceColor || decompilationTextCtrl->GetStyleAt(pos) == ColorsMenu::DecompilationColor::OPERATOR_COLOR) // only apply color if it hasn't been colored yet
			{
				decompilationTextCtrl->StartStyling(pos);
				decompilationTextCtrl->SetStyling(str.length(), color);
			}

			start = end;
		}
		else
		{
			break;
		}
	}
}

// Function Properties Menu

wxBEGIN_EVENT_TABLE(FunctionPropertiesMenu, wxFrame)
EVT_CLOSE(FunctionPropertiesMenu::CloseMenu)
wxEND_EVENT_TABLE()

FunctionPropertiesMenu::FunctionPropertiesMenu(wxPoint position, MainGui* main, int funcIndex) : wxFrame(nullptr, MainWindowID, "Change Function Properties", wxPoint(50, 50), wxSize(600, 600))
{
	Function* function = &(main->functions[funcIndex]);
	
	SetOwnBackgroundColour(backgroundColor);

	functionNameLabel = new wxStaticText(this, wxID_ANY, "Function Name");
	functionNameLabel->SetOwnForegroundColour(textColor);

	functionNameTextCtrl = new wxTextCtrl(this, wxID_ANY, function->name.buffer, wxPoint(0, 0), wxSize(100, 25));
	functionNameTextCtrl->SetOwnBackgroundColour(foregroundColor);
	functionNameTextCtrl->SetOwnForegroundColour(textColor);

	vSizer = new wxBoxSizer(wxVERTICAL);

	vSizer->Add(functionNameLabel, 0, wxEXPAND);
	vSizer->Add(functionNameTextCtrl, 0, wxEXPAND);

	if (function->numOfRegArgs > 0)
	{
		wxStaticText* regArgLabel = new wxStaticText(this, wxID_ANY, "Reg Arg Names");
		regArgLabel->SetOwnForegroundColour(textColor);
		vSizer->Add(regArgLabel, 0, wxEXPAND);
		for (int i = 0; i < function->numOfRegArgs; i++)
		{
			wxTextCtrl* regArgTextCtrl = new wxTextCtrl(this, wxID_ANY, function->regArgs[i].name.buffer, wxPoint(0, 0), wxSize(100, 25));
			regArgTextCtrl->SetOwnBackgroundColour(foregroundColor);
			regArgTextCtrl->SetOwnForegroundColour(textColor);

			vSizer->Add(regArgTextCtrl, 0, wxEXPAND);

			regArgNameTextCtrls.push_back(regArgTextCtrl);
		}
	}

	if (function->numOfStackArgs > 0)
	{
		wxStaticText* stackArgLabel = new wxStaticText(this, wxID_ANY, "Stack Arg Names");
		stackArgLabel->SetOwnForegroundColour(textColor);
		vSizer->Add(stackArgLabel, 0, wxEXPAND);
		for (int i = 0; i < function->numOfStackArgs; i++)
		{
			wxTextCtrl* stackArgTextCtrl = new wxTextCtrl(this, wxID_ANY, function->stackArgs[i].name.buffer, wxPoint(0, 0), wxSize(100, 25));
			stackArgTextCtrl->SetOwnBackgroundColour(foregroundColor);
			stackArgTextCtrl->SetOwnForegroundColour(textColor);

			vSizer->Add(stackArgTextCtrl, 0, wxEXPAND);

			stackArgNameTextCtrls.push_back(stackArgTextCtrl);
		}
	}

	if (function->numOfLocalVars > 0)
	{
		wxStaticText* localVarLabel = new wxStaticText(this, wxID_ANY, "Local Var Names");
		localVarLabel->SetOwnForegroundColour(textColor);
		vSizer->Add(localVarLabel, 0, wxEXPAND);
		for (int i = 0; i < function->numOfLocalVars; i++)
		{
			wxTextCtrl* localVarTextCtrl = new wxTextCtrl(this, wxID_ANY, function->localVars[i].name.buffer, wxPoint(0, 0), wxSize(100, 25));
			localVarTextCtrl->SetOwnBackgroundColour(foregroundColor);
			localVarTextCtrl->SetOwnForegroundColour(textColor);

			vSizer->Add(localVarTextCtrl, 0, wxEXPAND);

			localVarNameTextCtrls.push_back(localVarTextCtrl);
		}
	}

	if (function->numOfReturnedVars > 0)
	{
		wxStaticText* retVarLabel = new wxStaticText(this, wxID_ANY, "Returned Var Names");
		retVarLabel->SetOwnForegroundColour(textColor);
		vSizer->Add(retVarLabel, 0, wxEXPAND);
		for (int i = 0; i < function->numOfReturnedVars; i++)
		{
			wxTextCtrl* retVarTextCtrl = new wxTextCtrl(this, wxID_ANY, function->returnedVars[i].name.buffer, wxPoint(0, 0), wxSize(100, 25));
			retVarTextCtrl->SetOwnBackgroundColour(foregroundColor);
			retVarTextCtrl->SetOwnForegroundColour(textColor);

			vSizer->Add(retVarTextCtrl, 0, wxEXPAND);

			retVarNameTextCtrls.push_back(retVarTextCtrl);
		}
	}

	SetSizer(vSizer);

	mainGui = main;
	functionIndex = funcIndex;

	position.x += 10;
	position.y += 10;
	SetPosition(position);
	Show();
	Raise();
}

void FunctionPropertiesMenu::CloseMenu(wxCloseEvent& e)
{
	Function* currentFunction = &(mainGui->functions[functionIndex]);

	strcpyJdc(&currentFunction->name, functionNameTextCtrl->GetValue().c_str());

	for (int i = 0; i < currentFunction->numOfRegArgs; i++)
	{
		strcpyJdc(&currentFunction->regArgs[i].name, regArgNameTextCtrls[i]->GetValue().c_str());
	}

	for (int i = 0; i < currentFunction->numOfStackArgs; i++)
	{
		strcpyJdc(&currentFunction->stackArgs[i].name, stackArgNameTextCtrls[i]->GetValue().c_str());
	}

	for (int i = 0; i < currentFunction->numOfLocalVars; i++)
	{
		strcpyJdc(&currentFunction->localVars[i].name, localVarNameTextCtrls[i]->GetValue().c_str());
	}

	for (int i = 0; i < currentFunction->numOfReturnedVars; i++)
	{
		strcpyJdc(&currentFunction->returnedVars[i].name, retVarNameTextCtrls[i]->GetValue().c_str());
	}

	// update name in function grid
	mainGui->functionsGrid->SetCellValue(functionIndex, 2, currentFunction->name.buffer);

	// redecompile if it is currently in the decompilation box
	if (mainGui->currentDecompiledFunc == functionIndex)
	{
		mainGui->DecompileFunction(functionIndex);
	}

	Destroy();
}
