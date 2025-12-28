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

	menuBar = new wxMenuBar();
	bytesDisassemblerMenu = new BytesDisassembler();
	dataViewerMenu = new DataViewer();
	colorsMenu = new ColorsMenu();

	wxMenu* fileMenu = new wxMenu();

	wxMenuItem* openFile = fileMenu->Append(OpenFileID, "Open file");
	fileMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& ce) -> void { OpenFile(); }, OpenFileID);

	wxMenu* toolMenu = new wxMenu();

	wxMenuItem* openBytesDisassembler = toolMenu->Append(OpenBytesDisassemblerID, "Bytes disassembler");
	toolMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& ce) -> void { bytesDisassemblerMenu->OpenMenu(GetPosition()); }, OpenBytesDisassemblerID);

	wxMenuItem* openDataViewer = toolMenu->Append(OpenDataViewerID, "Data viewer");
	toolMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& ce) -> void { dataViewerMenu->OpenMenu(GetPosition(), imageBase, dataSections, numOfDataSections, dataSectionBytes); }, OpenDataViewerID);

	wxMenu* optionsMenu = new wxMenu();

	wxMenuItem* colors = optionsMenu->Append(OpenColorsMenuID, "Syntax highlighting");
	optionsMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& ce) -> void { colorsMenu->OpenMenu(GetPosition()); }, OpenColorsMenuID);

	menuBar->Append(fileMenu, "File");
	menuBar->Append(toolMenu, "Tools");
	menuBar->Append(optionsMenu, "Options");
	this->SetMenuBar(menuBar);

	disassembleFileButton = new wxButton(this, DisassembleFileButtonID, "Disassemble", wxPoint(0, 0), wxSize(100, 25));
	disassembleFileButton->SetOwnBackgroundColour(foregroundColor);
	disassembleFileButton->SetOwnForegroundColour(textColor);

	analyzeFileButton = new wxButton(this, AnalyzeFileButtonID, "Analyze", wxPoint(0, 0), wxSize(75, 25));
	analyzeFileButton->SetOwnBackgroundColour(foregroundColor);
	analyzeFileButton->SetOwnForegroundColour(textColor);

	disassemblyGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(400, 300));
	disassemblyGrid->SetLabelBackgroundColour(foregroundColor);
	disassemblyGrid->SetLabelTextColour(textColor);
	disassemblyGrid->SetDefaultCellBackgroundColour(gridColor);
	disassemblyGrid->SetDefaultCellTextColour(textColor);

	disassemblyGrid->CreateGrid(0, 4);
	disassemblyGrid->EnableGridLines(false);
	disassemblyGrid->SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectRows);
	disassemblyGrid->SetScrollRate(0, 10);
	disassemblyGrid->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
	disassemblyGrid->DisableDragRowSize();
	disassemblyGrid->EnableEditing(false);
	disassemblyGrid->SetColLabelValue(0, "Index");
	disassemblyGrid->SetColLabelValue(1, "Section");
	disassemblyGrid->SetColLabelValue(2, "Address");
	disassemblyGrid->SetColLabelValue(3, "Assembly");
	disassemblyGrid->HideRowLabels();
	disassemblyGrid->SetColSize(0, 50);
	disassemblyGrid->SetColSize(1, 100);
	disassemblyGrid->SetColSize(2, 100);
	disassemblyGrid->SetColSize(3, 9999);
	disassemblyGrid->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	decompilationTextCtrl = new wxTextCtrl(this, wxID_ANY, "", wxPoint(0, 0), wxSize(300, 300), wxTE_RICH2 | wxTE_READONLY | wxHSCROLL | wxTE_MULTILINE);
	decompilationTextCtrl->SetOwnBackgroundColour(gridColor);
	wxFont codeFont(wxFontInfo(10).FaceName("Cascadia Mono").Bold());
	decompilationTextCtrl->SetFont(codeFont);
	decompilationTextCtrl->Bind(wxEVT_CONTEXT_MENU, [&](wxContextMenuEvent &e) -> void { DecompRightClickOptions(e); });

	functionsGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(800, 200));
	functionsGrid->SetLabelBackgroundColour(foregroundColor);
	functionsGrid->SetLabelTextColour(textColor);
	functionsGrid->SetDefaultCellBackgroundColour(gridColor);
	functionsGrid->SetDefaultCellTextColour(textColor);

	functionsGrid->CreateGrid(0, 4);
	functionsGrid->EnableGridLines(false);
	functionsGrid->SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectRows);
	functionsGrid->SetScrollRate(0, 10);
	functionsGrid->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
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

	row1Sizer = new wxBoxSizer(wxHORIZONTAL);
	row2Sizer = new wxBoxSizer(wxHORIZONTAL);
	row3Sizer = new wxBoxSizer(wxHORIZONTAL);
	vSizer = new wxBoxSizer(wxVERTICAL);

	row1Sizer->Add(disassembleFileButton, 0, wxALL, 10);
	row1Sizer->Add(analyzeFileButton, 0, wxUP | wxBOTTOM | wxRIGHT, 10);

	row2Sizer->Add(disassemblyGrid, 1, wxEXPAND | wxBOTTOM | wxRIGHT | wxLEFT, 10);
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
		decompilationTextCtrl->Clear();
		currentDecompiledFunc = -1;

		int rows = disassemblyGrid->GetNumberRows();
		if (rows > 0)
		{
			disassemblyGrid->DeleteRows(0, disassemblyGrid->GetNumberRows());
		}
		rows = functionsGrid->GetNumberRows();
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

	int rows = disassemblyGrid->GetNumberRows();
	if (rows > 0)
	{
		disassemblyGrid->DeleteRows(0, disassemblyGrid->GetNumberRows());
	}

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
	decompilationTextCtrl->Clear();
	int rows = functionsGrid->GetNumberRows();
	if (rows > 0)
	{
		functionsGrid->DeleteRows(0, functionsGrid->GetNumberRows());
	}
	
	FindAllFunctions();

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
	FileSection sections[10] = { 0 };
	int numOfCodeSections = getFileCodeSections(currentFilePath.c_str().AsWChar(), is64Bit, sections, 10);

	if (numOfCodeSections == 0)
	{
		wxMessageBox("No code sections found in file", "Can't disassemble");
		return;
	}

	struct DisassemblerOptions options = { 0 };
	options.is64BitMode = is64Bit;

	unsigned int instructionNum = 0;

	for (int i = 0; i < numOfCodeSections; i++)
	{
		unsigned char* bytes = new unsigned char[sections[i].size];
		if (!readFileSection(currentFilePath.c_str().AsWChar(), &sections[i], is64Bit, bytes, sections[i].size))
		{
			wxMessageBox("Error reading bytes from file code section", "Can't disassemble");

			delete[] bytes;
			return;
		}

		struct DisassembledInstruction currentInstruction;
		unsigned int currentIndex = 0;
		while (disassembleInstruction(&bytes[currentIndex], bytes + sections[i].size - 1, &options, &currentInstruction))
		{
			unsigned long long address = imageBase + sections[i].virtualAddress + currentIndex;

			char addressStr[20] = { 0 };
			sprintf(addressStr, "%llX", address);

			currentIndex += currentInstruction.numOfBytes;

			disassemblyGrid->AppendRows(1);
			disassemblyGrid->SetCellValue(instructionNum, 0, std::to_string(instructionNum+1));
			disassemblyGrid->SetCellValue(instructionNum, 1, sections[i].name);
			disassemblyGrid->SetCellValue(instructionNum, 2, wxString(addressStr));

			char buffer[255] = { 0 };
			if (instructionToStr(&currentInstruction, buffer, 255))
			{
				disassemblyGrid->SetCellValue(instructionNum, 3, wxString(buffer));

				instructionAddresses.push_back(address);
				disassembledInstructions.push_back(currentInstruction);
			}
			else
			{
				disassemblyGrid->SetCellValue(instructionNum, 3,"ERROR");
				break;
			}

			instructionNum++;
		}

		delete[] bytes;
	}
}

void MainGui::DecompileFunction(unsigned short functionIndex)
{
	if (currentFilePath == "")
	{
		wxMessageBox("No file opened", "Can't decompile");
		return;
	}

	decompilationTextCtrl->Clear();

	DecompilationParameters params = { 0 };
	params.functions = &functions[0];
	params.numOfFunctions = functions.size();
	params.imports = imports;
	params.numOfImports = numOfImports;
	params.currentFunc = &functions[functionIndex];
	params.startInstructionIndex = 0;

	params.allInstructions = disassembledInstructions.data();
	params.allAddresses = instructionAddresses.data();
	params.totalNumOfInstructions = disassembledInstructions.size();
	
	params.imageBase = imageBase;
	params.dataSections = dataSections;
	params.numOfDataSections = numOfDataSections;
	params.dataSectionByte = dataSectionBytes;

	LineOfC* decompiledFunction = new LineOfC[255];
	if (!decompiledFunction) 
	{
		wxMessageBox("Error allocating memory for function decompilation", "Can't decompile");
		return;
	}

	memset(decompiledFunction, 0, 255 * sizeof(LineOfC));

	unsigned short numOfLinesDecompiled = decompileFunction(params, decompiledFunction, 255);
	if (numOfLinesDecompiled == 0)
	{
		wxMessageBox("Error decompiling function", "Can't decompile");
		delete[] decompiledFunction;
		return;
	}

	for (int i = 0; i < numOfLinesDecompiled; i++)
	{
		wxString str = wxString(decompiledFunction[i].line);
		ReplaceEscapeChars(&str);

		for (int j = 0; j < decompiledFunction[i].indents; j++) 
		{
			str = "    " + str;
		}

		decompilationTextCtrl->AppendText(str);
		decompilationTextCtrl->AppendText("\n");
	}

	ApplySyntaxHighlighting(params.currentFunc);

	currentDecompiledFunc = functionIndex;

	delete[] decompiledFunction;
}

void MainGui::FindAllFunctions() 
{
	int functionNum = 0;
	int numOfInstructions = disassembledInstructions.size();
	functions.push_back({ 0 });
	int instructionIndex = 0;
	while (instructionIndex < disassembledInstructions.size() && findNextFunction(&disassembledInstructions[instructionIndex], &instructionAddresses[instructionIndex], numOfInstructions, &functions[functionNum], &instructionIndex))
	{
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
		fixAllFunctionReturnTypes(&functions[0], functions.size());
		getAllFuncReturnVars(&functions[0], functions.size(), &disassembledInstructions[0], &instructionAddresses[0], disassembledInstructions.size(), imports, numOfImports);
	}
}

void MainGui::GridRightClickOptions(wxGridEvent& e)
{
	wxMenu menu;

	int row = e.GetRow(); // row right-clicked on
	wxGrid* grid = (wxGrid*)(e.GetEventObject());

	if (grid == functionsGrid) 
	{
		const int ID_DECOMPILE = 100;
		const int ID_EDIT_PROPERTIES= 101;
		const int ID_COPY_ADDRESS = 102;
		
		menu.Append(ID_DECOMPILE, "Decompile");
		menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { DecompileFunction(row); }, ID_DECOMPILE);

		menu.Append(ID_EDIT_PROPERTIES, "Edit properties");
		menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { functionPropertiesMenu = new FunctionPropertiesMenu(GetPosition(), this, row); }, ID_EDIT_PROPERTIES);

		menu.Append(ID_COPY_ADDRESS, "Copy address");
		menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(functionsGrid->GetCellValue(row, 0)); }, ID_COPY_ADDRESS);
	}
	else if(grid == disassemblyGrid)
	{
		const int ID_COPY_SECTION = 100;
		const int ID_COPY_ADDRESS = 101;
		const int ID_COPY_ASSEMBLY = 102;
		
		menu.Append(ID_COPY_SECTION, "Copy section");
		menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(disassemblyGrid->GetCellValue(row, 1)); }, ID_COPY_SECTION);

		menu.Append(ID_COPY_ADDRESS, "Copy address");
		menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(disassemblyGrid->GetCellValue(row, 2)); }, ID_COPY_ADDRESS);

		menu.Append(ID_COPY_ASSEMBLY, "Copy assembly");
		menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(disassemblyGrid->GetCellValue(row, 3)); }, ID_COPY_ASSEMBLY);	
	}

	PopupMenu(&menu, ScreenToClient(wxGetMousePosition()));
	e.Skip();
}

void MainGui::DecompRightClickOptions(wxContextMenuEvent& e) 
{
	wxMenu menu;

	const int ID_COPY = 100;
	const int ID_SELECT_ALL = 101;
	const int ID_CONVERT_NUMBER = 102;

	long start;
	long end;
	decompilationTextCtrl->GetSelection(&start, &end);
	wxString selection = "";

	if (start != end)
	{
		wxString text = decompilationTextCtrl->GetValue();
		selection = text.substr(start, end - start);

		menu.Append(ID_COPY, "Copy");
		menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) { CopyToClipboard(selection); }, ID_COPY);

		if (!IsCharDigit(text[start - 1]) && !IsCharDigit(text[end]))
		{
			long long num = 0;
			if (selection.ToLongLong(&num, 10))
			{
				menu.Append(ID_CONVERT_NUMBER, "Convert to hexadecimal");
				menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
					char numStr[50] = { 0 };
					sprintf(numStr, "0x%llX", num);

					decompilationTextCtrl->Replace(start, end, numStr);

					wxTextAttr numColor;
					numColor.SetTextColour(colorsMenu->numberColor);
					decompilationTextCtrl->SetStyle(start, start + strlen(numStr), numColor);
					}, ID_CONVERT_NUMBER);
			}
			else if (selection.ToLongLong(&num, 16))
			{
				menu.Append(ID_CONVERT_NUMBER, "Convert to decimal");
				menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) { 
					wxString numStr = std::to_string(num);
					decompilationTextCtrl->Replace(start, end, numStr);

					wxTextAttr numColor;
					numColor.SetTextColour(colorsMenu->numberColor);
					decompilationTextCtrl->SetStyle(start, start + numStr.length(), numColor);
					}, ID_CONVERT_NUMBER);
			}
		}
	}

	
	menu.Append(ID_SELECT_ALL, "Select all");
	menu.Bind(wxEVT_MENU, [this](wxCommandEvent&) {
		decompilationTextCtrl->SetSelection(0, decompilationTextCtrl->GetLastPosition());
		decompilationTextCtrl->SetFocus();
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

void MainGui::ReplaceEscapeChars(wxString* str) 
{
	str->Replace("\a", "\\a");
	str->Replace("\b", "\\b");
	str->Replace("\f", "\\f");
	str->Replace("\n", "\\n");
	str->Replace("\r", "\\r");
	str->Replace("\t", "\\t");
	str->Replace("\v", "\\v");
}

char MainGui::IsCharDigit(char c)
{
	return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
}

void MainGui::ApplySyntaxHighlighting(Function* function)
{
	wxString text = decompilationTextCtrl->GetValue();

	wxTextAttr colorAttr;
	colorAttr.SetTextColour(colorsMenu->operatorColor);
	decompilationTextCtrl->SetStyle(0, text.length() - 1, colorAttr);

	// local vars
	for (int i = 0; i < function->numOfLocalVars; i++) 
	{
		ColorAllStrs(text, function->localVars[i].name, colorsMenu->localVarColor);
	}

	// return vars
	for (int i = 0; i < function->numOfReturnVars; i++)
	{
		ColorAllStrs(text, function->returnVars[i].name, colorsMenu->localVarColor);
	}

	// stack args
	for (int i = 0; i < function->numOfStackArgs; i++)
	{
		ColorAllStrs(text, function->stackArgs[i].name, colorsMenu->argumentColor);
	}

	// reg args
	for (int i = 0; i < function->numOfRegArgs; i++)
	{
		ColorAllStrs(text, function->regArgs[i].name, colorsMenu->argumentColor);
	}

	// functions
	for (int i = 0; i < functions.size(); i++)
	{
		ColorAllStrs(text, functions[i].name, colorsMenu->functionColor);
	}

	// imports
	for (int i = 0; i < numOfImports; i++)
	{
		ColorAllStrs(text, imports[i].name, colorsMenu->importColor);
	}

	// calling conventions
	for (int i = 0; i < 4; i++)
	{
		ColorAllStrs(text, callingConventionStrs[i], colorsMenu->primitiveTypeColor);
	}

	// primitive data types
	for (int i = 0; i < 7; i++) 
	{
		ColorAllStrs(text, primitiveTypeStrs[i], colorsMenu->primitiveTypeColor);
	}

	// keywords
	const char* keywordStrs[5] = { "if", "else", "for", "while", "return" };
	for (int i = 0; i < 5; i++)
	{
		ColorAllStrs(text, keywordStrs[i], colorsMenu->keywordColor);
	}

	// strings
	int start = 0;
	while (start < text.length())
	{
		int pos = text.find("\"", start);
		int end = text.find("\"", pos + 1);
		if (pos != wxNOT_FOUND && end != wxNOT_FOUND)
		{
			colorAttr.SetTextColour(colorsMenu->stringColor);
			decompilationTextCtrl->SetStyle(pos, end + 1, colorAttr);
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
		ColorAllStrs(text, numberChars[i], colorsMenu->numberColor);
	}
}

void MainGui::ColorAllStrs(wxString text, wxString str, wxColour color) 
{
	int start = 0;
	int pos = 0;
	while (start < text.length())
	{
		pos = text.find(str, start);
		if (pos != wxNOT_FOUND)
		{
			int end = pos + str.length();

			wxTextAttr colorAttr;
			decompilationTextCtrl->GetStyle(pos, colorAttr);

			if (colorAttr.GetTextColour() == colorsMenu->operatorColor) // only apply color if it hasn't been colored yet
			{
				colorAttr.SetTextColour(color);
				decompilationTextCtrl->SetStyle(pos, end, colorAttr);
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
