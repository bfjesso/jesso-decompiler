#include "mainGui.h"
#include "../decompiler/dataTypes.h"

wxBEGIN_EVENT_TABLE(MainGui, wxFrame)
EVT_CLOSE(MainGui::CloseApp)
EVT_BUTTON(DisassembleFileButtonID, MainGui::DisassembleButton)
EVT_BUTTON(AnalyzeFileButtonID, MainGui::AnalyzeButton)
EVT_GRID_CELL_RIGHT_CLICK(MainGui::GridRightClickOptions)
wxEND_EVENT_TABLE()

MainGui::MainGui() : wxFrame(nullptr, MainWindowID, "Jesso Decompiler x64", wxPoint(50, 50), wxSize(800, 600))
{
	SetOwnBackgroundColour(backgroundColor);

	disassembleFileButton = new wxButton(this, DisassembleFileButtonID, "Disassemble", wxPoint(0, 0), wxSize(100, 25));
	disassembleFileButton->SetOwnBackgroundColour(foregroundColor);
	disassembleFileButton->SetOwnForegroundColour(textColor);

	analyzeFileButton = new wxButton(this, AnalyzeFileButtonID, "Analyze", wxPoint(0, 0), wxSize(75, 25));
	analyzeFileButton->SetOwnBackgroundColour(foregroundColor);
	analyzeFileButton->SetOwnForegroundColour(textColor);

	disassemblyTextCtrl = new wxStyledTextCtrl(this, wxID_ANY, wxPoint(0, 0), wxSize(400, 300));
	SetUpStyledTextCtrl(disassemblyTextCtrl);

	decompilationTextCtrl = new wxStyledTextCtrl(this, wxID_ANY, wxPoint(0, 0), wxSize(300, 300));
	SetUpStyledTextCtrl(decompilationTextCtrl);

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
	colorsMenu = new ColorsMenu(disassemblyTextCtrl, decompilationTextCtrl);

	wxMenu* fileMenu = new wxMenu();

	wxMenuItem* openFile = fileMenu->Append(OpenFileID, "Open file");
	fileMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& ce) -> void { OpenFile(); }, OpenFileID);

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
	row3Sizer = new wxBoxSizer(wxHORIZONTAL);
	vSizer = new wxBoxSizer(wxVERTICAL);

	row1Sizer->Add(disassembleFileButton, 0, wxALL, 10);
	row1Sizer->Add(analyzeFileButton, 0, wxUP | wxBOTTOM | wxRIGHT, 10);

	row2Sizer->Add(disassemblyTextCtrl, 1, wxEXPAND | wxBOTTOM | wxRIGHT | wxLEFT, 10);
	row2Sizer->Add(decompilationTextCtrl, 1, wxEXPAND | wxBOTTOM | wxRIGHT, 10);

	row3Sizer->Add(functionsGrid, 1, wxBOTTOM | wxRIGHT | wxLEFT, 10);

	vSizer->Add(row1Sizer);
	vSizer->Add(row2Sizer, 1, wxEXPAND);
	vSizer->Add(row3Sizer, 0, wxEXPAND);

	SetSizerAndFit(vSizer);
}

void MainGui::OpenFile()
{
	wxFileDialog openFileDialog(this, "Choose file", "", "", "", wxFD_FILE_MUST_EXIST);

	if (openFileDialog.ShowModal() != wxID_CANCEL)
	{
		currentFilePath = "";
		instructionAddresses.clear();
		instructionAddresses.shrink_to_fit();
		disassembledInstructions.clear();
		disassembledInstructions.shrink_to_fit();
		functions.clear();
		functions.shrink_to_fit();
		ClearStyledTextCtrl(disassemblyTextCtrl);
		ClearStyledTextCtrl(decompilationTextCtrl);
		currentDecompiledFunc = -1;

		int rows = functionsGrid->GetNumberRows();
		if (rows > 0)
		{
			functionsGrid->DeleteRows(0, functionsGrid->GetNumberRows());
		}

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
			wxMessageBox(fileName + " has been opened. It can now be disassembled and analyzed.", "Successfully opened the file");
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

void MainGui::DisassembleButton(wxCommandEvent& e) 
{
	if (currentFilePath == "")
	{
		wxMessageBox("No file opened", "Can't disassemble");
		return;
	}

	ClearStyledTextCtrl(disassemblyTextCtrl);

	instructionAddresses.clear();
	instructionAddresses.shrink_to_fit();

	disassembledInstructions.clear();
	disassembledInstructions.shrink_to_fit();
	
	DisassembleCodeSections();
}

void MainGui::AnalyzeButton(wxCommandEvent& e) 
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

	functions.clear();
	functions.shrink_to_fit();
	ClearStyledTextCtrl(decompilationTextCtrl);
	int rows = functionsGrid->GetNumberRows();
	if (rows > 0)
	{
		functionsGrid->DeleteRows(0, functionsGrid->GetNumberRows());
	}
	
	FindAllFunctions();

	functionsGrid->SetLabelTextColour(textColor);

	numOfDataSections = 0;
	if (dataSectionBytes)
	{
		delete[] dataSectionBytes;
	}

	int answer = wxMessageBox("Do you want to load bytes from the data section?", "Get data section bytes", wxYES_NO, this);
	if (answer == wxYES) 
	{
		LoadDataSectionBytes();
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
	
	for (int i = 0; i < numOfDataSections; i++)
	{
		if (!readFileSection(currentFilePath.c_str().AsWChar(), &dataSections[i], is64Bit, dataSectionBytes + (i != 0 ? dataSections[i - 1].size : 0), dataSections[i].size))
		{
			wxMessageBox("Error reading bytes from file data section", "Can't load data");

			delete[] dataSectionBytes;
			return;
		}
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

	unsigned int instructionNum = 0;

	disassemblyTextCtrl->SetReadOnly(false);

	for (int i = 0; i < numOfCodeSections; i++)
	{
		unsigned char* bytes = new unsigned char[codeSections[i].size];
		if (!readFileSection(currentFilePath.c_str().AsWChar(), &codeSections[i], is64Bit, bytes, codeSections[i].size))
		{
			wxMessageBox("Error reading bytes from file code section", "Can't disassemble");

			delete[] bytes;
			return;
		}

		struct DisassembledInstruction currentInstruction;
		unsigned int currentIndex = 0;
		while (disassembleInstruction(&bytes[currentIndex], bytes + codeSections[i].size - 1, &options, &currentInstruction))
		{
			unsigned long long address = imageBase + codeSections[i].virtualAddress + currentIndex;

			char addressStr[20] = { 0 };
			sprintf(addressStr, "%llX", address);

			currentIndex += currentInstruction.numOfBytes;

			int pos = disassemblyTextCtrl->GetLength() - 1;
			wxString addressInfoStr = wxString(addressStr) + wxString(codeSections[i].name) + "\t";
			disassemblyTextCtrl->AppendText(addressInfoStr);
			disassemblyTextCtrl->StartStyling(pos);
			disassemblyTextCtrl->SetStyling(addressInfoStr.length(), ColorsMenu::DisassemblyColor::ADDRESS_COLOR);

			pos += addressInfoStr.length() + 1;

			char buffer[255] = { 0 };
			if (instructionToStr(&currentInstruction, buffer, 255))
			{
				wxString asmStr = wxString(buffer);
				disassemblyTextCtrl->AppendText(asmStr);
				disassemblyTextCtrl->StartStyling(pos);
				disassemblyTextCtrl->SetStyling(asmStr.length(), ColorsMenu::DisassemblyColor::PUNCTUATION_COLOR);

				ApplyAsmHighlighting(pos, asmStr, &currentInstruction);

				instructionAddresses.push_back(address);
				disassembledInstructions.push_back(currentInstruction);
			}

			if(currentInstruction.opcode == NO_MNEMONIC)
			{
				break;
			}

			disassemblyTextCtrl->AppendText("\n");

			instructionNum++;
		}

		delete[] bytes;
	}

	disassemblyTextCtrl->SetReadOnly(true);
}

void MainGui::DecompileFunction(unsigned short functionIndex)
{
	if (currentFilePath == "")
	{
		wxMessageBox("No file opened", "Can't decompile");
		return;
	}

	ClearStyledTextCtrl(decompilationTextCtrl);

	DecompilationParameters params = { 0 };
	params.functions = &functions[0];
	params.numOfFunctions = functions.size();
	params.imports = imports;
	params.numOfImports = numOfImports;
	params.currentFunc = &functions[functionIndex];
	params.startInstructionIndex = 0;
	params.skipUpperBound = -1;
	params.skipLowerBound = -1;

	params.allInstructions = disassembledInstructions.data();
	params.allAddresses = instructionAddresses.data();
	params.totalNumOfInstructions = disassembledInstructions.size();
	
	params.imageBase = imageBase;
	params.dataSections = dataSections;
	params.numOfDataSections = numOfDataSections;
	params.dataSectionByte = dataSectionBytes;

	params.is64Bit = is64Bit;

	struct JdcStr decompiledFunction = { 0 };
	if (!initializeJdcStr(&decompiledFunction, 20000))
	{
		wxMessageBox("Error allocating memory for function decompilation", "Can't decompile");
		return;
	}

	if (!decompileFunction(params, &decompiledFunction))
	{
		wxMessageBox("Error decompiling function", "Can't decompile");
		freeJdcStr(&decompiledFunction);
		return;
	}

	decompilationTextCtrl->SetReadOnly(false);
	decompilationTextCtrl->SetValue(decompiledFunction.buffer);
	ApplySyntaxHighlighting(params.currentFunc);
	decompilationTextCtrl->SetReadOnly(true);
	currentDecompiledFunc = functionIndex;
	freeJdcStr(&decompiledFunction);
}

void MainGui::FindAllFunctions() 
{
	int functionNum = 0;
	int numOfInstructions = disassembledInstructions.size();
	functions.push_back({ 0 });
	int instructionIndex = 0;
	int codeSectionIndex = 1;
	unsigned long long nextSectionStartAddress = imageBase + codeSections[codeSectionIndex].virtualAddress;
	while (instructionIndex < disassembledInstructions.size() && findNextFunction(&disassembledInstructions[instructionIndex], &instructionAddresses[instructionIndex], numOfInstructions, nextSectionStartAddress, &functions[functionNum], &instructionIndex, is64Bit))
	{
		if(instructionAddresses[instructionIndex] >= nextSectionStartAddress)
		{
			codeSectionIndex++;
			nextSectionStartAddress = imageBase + codeSections[codeSectionIndex].virtualAddress;
		}

		numOfInstructions -= functions[functionNum].numOfInstructions;
		functionsGrid->AppendRows(1);

		if (functions[functionNum].addresses)
		{
			char addressStr[10];
			sprintf(addressStr, "%llX", *functions[functionNum].addresses);
			functionsGrid->SetCellValue(functionNum, 0, wxString(addressStr));

			functionsGrid->SetCellValue(functionNum, 1, wxString(callingConventionStrs[functions[functionNum].callingConvention]));

			functions[functionNum].name[0] = 0;
			if (!getSymbolByValue(currentFilePath.c_str().AsWChar(), is64Bit, *functions[functionNum].addresses, functions[functionNum].name))
			{
				sprintf(functions[functionNum].name, "func%llX", (*functions[functionNum].addresses) - imageBase);
			}
		}

		functionsGrid->SetCellValue(functionNum, 2, wxString(functions[functionNum].name));
		functionsGrid->SetCellValue(functionNum, 3, std::to_string(functions[functionNum].numOfInstructions));
		functionNum++;

		functions.push_back({ 0 });
	}

	functions.pop_back(); // remove last empty one

	if (functions.size() > 0) 
	{
		fixAllFunctionReturnTypes(&functions[0], functions.size(), is64Bit);
		getAllFuncReturnVars(&functions[0], functions.size(), &disassembledInstructions[0], &instructionAddresses[0], disassembledInstructions.size(), imports, numOfImports, is64Bit);
	}
}

void MainGui::GridRightClickOptions(wxGridEvent& e)
{
	wxMenu menu;

	int row = e.GetRow(); // row right-clicked on

	const int ID_DECOMPILE = 100;
	const int ID_EDIT_PROPERTIES = 101;
	const int ID_COPY_ADDRESS = 102;

	menu.Append(ID_DECOMPILE, "Decompile");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { DecompileFunction(row); }, ID_DECOMPILE);

	menu.Append(ID_EDIT_PROPERTIES, "Edit properties");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { functionPropertiesMenu = new FunctionPropertiesMenu(GetPosition(), this, row); }, ID_EDIT_PROPERTIES);

	menu.Append(ID_COPY_ADDRESS, "Copy address");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(functionsGrid->GetCellValue(row, 0)); }, ID_COPY_ADDRESS);

	PopupMenu(&menu, ScreenToClient(wxGetMousePosition()));
	e.Skip();
}

void MainGui::StyledTextCtrlRightClickOptions(wxContextMenuEvent& e) 
{
	wxMenu menu;

	const int ID_COPY = 100;
	const int ID_SELECT_ALL = 101;
	const int ID_CONVERT_NUMBER = 102;

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

	PopupMenu(&menu, ScreenToClient(e.GetPosition()));
}

void MainGui::CloseApp(wxCloseEvent& e)
{
	if (dataSectionBytes) 
	{
		delete[] dataSectionBytes;
	}
	
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
		ColorAllStrs(text, function->localVars[i].name, ColorsMenu::DecompilationColor::LOCAL_VAR_COLOR, 1);
	}

	// return vars
	for (int i = 0; i < function->numOfReturnVars; i++)
	{
		ColorAllStrs(text, function->returnVars[i].name, ColorsMenu::DecompilationColor::LOCAL_VAR_COLOR, 1);
	}

	// stack args
	for (int i = 0; i < function->numOfStackArgs; i++)
	{
		ColorAllStrs(text, function->stackArgs[i].name, ColorsMenu::DecompilationColor::ARGUMENT_COLOR, 1);
	}

	// reg args
	for (int i = 0; i < function->numOfRegArgs; i++)
	{
		ColorAllStrs(text, function->regArgs[i].name, ColorsMenu::DecompilationColor::ARGUMENT_COLOR, 1);
	}

	// functions
	for (int i = 0; i < functions.size(); i++)
	{
		ColorAllStrs(text, functions[i].name, ColorsMenu::DecompilationColor::FUNCTION_COLOR, 0);
	}

	// imports
	for (int i = 0; i < numOfImports; i++)
	{
		ColorAllStrs(text, imports[i].name, ColorsMenu::DecompilationColor::IMPORT_COLOR, 0);
	}

	// calling conventions
	for (int i = 0; i < 4; i++)
	{
		ColorAllStrs(text, callingConventionStrs[i], ColorsMenu::DecompilationColor::PRIMITIVE_COLOR, 0);
	}

	// primitive data types
	for (int i = 0; i < 7; i++) 
	{
		ColorAllStrs(text, primitiveTypeStrs[i], ColorsMenu::DecompilationColor::PRIMITIVE_COLOR, 0);
	}

	// keywords
	const char* keywordStrs[5] = { "if", "else", "for", "while", "return" };
	for (int i = 0; i < 5; i++)
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

void MainGui::ApplyAsmHighlighting(int pos, wxString str, DisassembledInstruction* instruction)
{
	disassemblyTextCtrl->StartStyling(pos);
	disassemblyTextCtrl->SetStyling(strlen(mnemonicStrs[instruction->opcode]) + 1, ColorsMenu::DisassemblyColor::OPCODE_COLOR);

	// regs
	int start = 0;
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
			int loc = str.find(regStr, start);
			disassemblyTextCtrl->StartStyling(pos + loc);
			disassemblyTextCtrl->SetStyling(regStr.length(), ColorsMenu::DisassemblyColor::REGISTER_COLOR);
			start = loc + regStr.length();
		}

		if (!regStr2.IsEmpty())
		{
			int loc = str.find(regStr2, start);
			disassemblyTextCtrl->StartStyling(pos + loc);
			disassemblyTextCtrl->SetStyling(regStr2.length(), ColorsMenu::DisassemblyColor::REGISTER_COLOR);
			start = loc + regStr2.length();
		}

		if (instruction->operands[i].type == MEM_ADDRESS)
		{
			// segment
			if (instruction->operands[i].memoryAddress.segment != NO_SEGMENT) 
			{
				wxString segStr = wxString(segmentStrs[instruction->operands[i].memoryAddress.segment]) + ":";
				disassemblyTextCtrl->StartStyling(pos + str.find(segStr));
				disassemblyTextCtrl->SetStyling(segStr.length(), ColorsMenu::DisassemblyColor::SEGMENT_COLOR);
			}
			
			// ptr size
			if (instruction->operands[i].memoryAddress.ptrSize != 0 && instruction->operands[i].memoryAddress.ptrSize <= 10)
			{
				wxString sizeStr = wxString(ptrSizeStrs[instruction->operands[i].memoryAddress.ptrSize / 2]);
				disassemblyTextCtrl->StartStyling(pos + str.find(sizeStr));
				disassemblyTextCtrl->SetStyling(sizeStr.length(), ColorsMenu::DisassemblyColor::PTR_SIZE_COLOR);
			}
		}
	}

	// numbers
	start = 0;
	while (start < str.length())
	{
		int num = str.find("0x", start);
		if (num != wxNOT_FOUND)
		{
			int end = str.length();
			for (int i = num + 2; i < end; i++) 
			{
				if ((str[i] < '0' || str[i] > '9') && (str[i] < 'A' || str[i] > 'F')) 
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

void MainGui::SetUpStyledTextCtrl(wxStyledTextCtrl* ctrl) 
{
	ctrl->SetReadOnly(true);
	ctrl->SetMarginWidth(1, 0);
	ctrl->StyleSetFont(wxSTC_STYLE_DEFAULT, codeFont);
	ctrl->StyleSetBackground(wxSTC_STYLE_DEFAULT, gridColor);
	ctrl->StyleClearAll();
	ctrl->SetSelBackground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
	ctrl->SetSelForeground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
	ctrl->SetCaretForeground(textColor);
	ctrl->SetCaretWidth(2);
	ctrl->SetWrapMode(wxSTC_WRAP_NONE);
	ctrl->SetScrollWidthTracking(true);
	ctrl->SetScrollWidth(1);
	ctrl->SetViewWhiteSpace(wxSTC_WS_INVISIBLE);
	ctrl->Bind(wxEVT_CONTEXT_MENU, [&](wxContextMenuEvent& e) -> void { StyledTextCtrlRightClickOptions(e); });
}

void MainGui::ClearStyledTextCtrl(wxStyledTextCtrl* ctrl)
{
	ctrl->SetReadOnly(false);
	ctrl->SetText("");
	ctrl->SetReadOnly(true);
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

	functionNameTextCtrl = new wxTextCtrl(this, wxID_ANY, function->name, wxPoint(0, 0), wxSize(100, 25));
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
			wxTextCtrl* regArgTextCtrl = new wxTextCtrl(this, wxID_ANY, function->regArgs[i].name, wxPoint(0, 0), wxSize(100, 25));
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
			wxTextCtrl* stackArgTextCtrl = new wxTextCtrl(this, wxID_ANY, function->stackArgs[i].name, wxPoint(0, 0), wxSize(100, 25));
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
			wxTextCtrl* localVarTextCtrl = new wxTextCtrl(this, wxID_ANY, function->localVars[i].name, wxPoint(0, 0), wxSize(100, 25));
			localVarTextCtrl->SetOwnBackgroundColour(foregroundColor);
			localVarTextCtrl->SetOwnForegroundColour(textColor);

			vSizer->Add(localVarTextCtrl, 0, wxEXPAND);

			localVarNameTextCtrls.push_back(localVarTextCtrl);
		}
	}

	if (function->numOfReturnVars > 0)
	{
		wxStaticText* retVarLabel = new wxStaticText(this, wxID_ANY, "Returned Var Names");
		retVarLabel->SetOwnForegroundColour(textColor);
		vSizer->Add(retVarLabel, 0, wxEXPAND);
		for (int i = 0; i < function->numOfReturnVars; i++)
		{
			wxTextCtrl* retVarTextCtrl = new wxTextCtrl(this, wxID_ANY, function->returnVars[i].name, wxPoint(0, 0), wxSize(100, 25));
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

	currentFunction->name[0] = 0;
	strcpy(currentFunction->name, functionNameTextCtrl->GetValue().c_str());

	for (int i = 0; i < currentFunction->numOfRegArgs; i++)
	{
		currentFunction->regArgs[i].name[0] = 0;
		strcpy(currentFunction->regArgs[i].name, regArgNameTextCtrls[i]->GetValue().c_str());
	}

	for (int i = 0; i < currentFunction->numOfStackArgs; i++)
	{
		currentFunction->stackArgs[i].name[0] = 0;
		strcpy(currentFunction->stackArgs[i].name, stackArgNameTextCtrls[i]->GetValue().c_str());
	}

	for (int i = 0; i < currentFunction->numOfLocalVars; i++)
	{
		currentFunction->localVars[i].name[0] = 0;
		strcpy(currentFunction->localVars[i].name, localVarNameTextCtrls[i]->GetValue().c_str());
	}

	for (int i = 0; i < currentFunction->numOfReturnVars; i++)
	{
		currentFunction->returnVars[i].name[0] = 0;
		strcpy(currentFunction->returnVars[i].name, retVarNameTextCtrls[i]->GetValue().c_str());
	}

	// update name in function grid
	mainGui->functionsGrid->SetCellValue(functionIndex, 2, currentFunction->name);

	// redecompile if it is currently in the decompilation box
	if (mainGui->currentDecompiledFunc == functionIndex)
	{
		mainGui->DecompileFunction(functionIndex);
	}

	Destroy();
}
