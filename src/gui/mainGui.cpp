#include "mainGui.h"
#include "../decompiler/decompilationUtils.h"
#include "../decompiler/dataTypes.h"
#include "../disassembler/registers.h"
#include "../disassembler/mnemonics.h"
#include "../decompiler/functions.h"
#include "../decompiler/intrinsics.h"

wxBEGIN_EVENT_TABLE(MainGui, wxFrame)
EVT_CLOSE(MainGui::CloseApp)
EVT_GRID_CELL_RIGHT_CLICK(MainGui::GridRightClickOptions)
wxEND_EVENT_TABLE()

MainGui::MainGui() : wxFrame(nullptr, MainWindowID, "Jesso Decompiler x64", wxPoint(50, 50), wxSize(800, 600))
{
	SetOwnBackgroundColour(backgroundColor);

	statusStaticText = new wxStaticText(this, wxID_ANY, "Status: idle");
	statusStaticText->SetOwnForegroundColour(textColor);
	
	mainSplitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE);
	topSplitter = new wxSplitterWindow(mainSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE);

	disassemblyTextCtrl = new wxStyledTextCtrl(topSplitter, wxID_ANY, wxPoint(0, 0), wxSize(150, 400));
	SetUpStyledTextCtrl(disassemblyTextCtrl);
	disassemblyTextCtrl->Bind(wxEVT_CONTEXT_MENU, [&](wxContextMenuEvent& e) -> void { StyledTextCtrlRightClickOptions(e); });

	decompilationTextCtrl = new wxStyledTextCtrl(topSplitter, wxID_ANY, wxPoint(0, 0), wxSize(150, 400));
	SetUpStyledTextCtrl(decompilationTextCtrl);
	decompilationTextCtrl->Bind(wxEVT_CONTEXT_MENU, [&](wxContextMenuEvent& e) -> void { StyledTextCtrlRightClickOptions(e); });
	decompilationTextCtrl->SetMarginType(0, wxSTC_MARGIN_NUMBER);
	decompilationTextCtrl->SetMarginWidth(0, decompilationTextCtrl->TextWidth(wxSTC_STYLE_LINENUMBER, "99999"));
	decompilationTextCtrl->StyleSetForeground(wxSTC_STYLE_LINENUMBER, darkerTextColor);
	decompilationTextCtrl->StyleSetBackground(wxSTC_STYLE_LINENUMBER, gridColor);
	decompilationTextCtrl->SetMarginBackground(1, backgroundColor);
	decompilationTextCtrl->SetIndentationGuides(wxSTC_IV_LOOKBOTH);
	decompilationTextCtrl->StyleSetForeground(wxSTC_STYLE_INDENTGUIDE, darkerTextColor);
	decompilationTextCtrl->SetEdgeMode(wxSTC_EDGE_LINE);
	decompilationTextCtrl->SetEdgeColumn(0);
	decompilationTextCtrl->SetEdgeColour(darkerTextColor);
	decompilationTextCtrl->SetTabWidth(4);
	decompilationTextCtrl->SetIndent(4);
	decompilationTextCtrl->StyleSetForeground(wxSTC_STYLE_BRACELIGHT, textColor);
	decompilationTextCtrl->StyleSetBackground(wxSTC_STYLE_BRACELIGHT, foregroundColor);
	decompilationTextCtrl->StyleSetForeground(wxSTC_STYLE_BRACEBAD, wxColour(200, 80, 80));
	decompilationTextCtrl->Bind(wxEVT_STC_UPDATEUI, &MainGui::OnDecompilationUpdateUI, this);

	functionsGrid = new wxGrid(mainSplitter, wxID_ANY, wxPoint(0, 0), wxSize(800, 150));
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
	toolMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& ce) -> void { new BytesDisassembler(this, GetPosition()); }, OpenBytesDisassemblerID);

	wxMenuItem* openSectionsViewer = toolMenu->Append(OpenSectionsViewerID, "File sections");
	toolMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& ce) -> void { new SectionsViewer(this, GetPosition(), sections, numOfSections); }, OpenSectionsViewerID);

	wxMenuItem* openDataViewer = toolMenu->Append(OpenDataViewerID, "Data viewer");
	toolMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& ce) -> void { dataViewerMenu->OpenMenu(GetPosition(), imageBase, sections, numOfSections, fileBytes); }, OpenDataViewerID);

	wxMenuItem* openImportsViewer = toolMenu->Append(OpenImportsViewerID, "Imports");
	toolMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& ce) -> void { new ImportsViewer(this, GetPosition(), imports, numOfImports); }, OpenImportsViewerID);

	wxMenu* optionsMenu = new wxMenu();

	wxMenuItem* colors = optionsMenu->Append(OpenColorsMenuID, "Colors");
	optionsMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& ce) -> void { colorsMenu->OpenMenu(GetPosition()); }, OpenColorsMenuID);

	menuBar->Append(fileMenu, "File");
	menuBar->Append(toolMenu, "Tools");
	menuBar->Append(optionsMenu, "Options");
	this->SetMenuBar(menuBar);

	topSplitter->SplitVertically(disassemblyTextCtrl, decompilationTextCtrl, 0);
	topSplitter->SetSashGravity(0.5);
	topSplitter->SetMinimumPaneSize(100);

	mainSplitter->SplitHorizontally(topSplitter, functionsGrid, 150);
	mainSplitter->SetSashGravity(0.5);
	mainSplitter->SetMinimumPaneSize(100);

	vSizer = new wxBoxSizer(wxVERTICAL);
	vSizer->Add(statusStaticText, 0, wxTOP | wxLEFT, 10);
	vSizer->Add(mainSplitter, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

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

			numOfSections = getNumOfSections(filePath.c_str().AsWChar(), is64Bit);
			sections = new FileSection[numOfSections];
			if (!getAllFileSectionHeaders(filePath.c_str().AsWChar(), is64Bit, sections, numOfSections))
			{
				wxMessageBox("Error getting all file sections", "Failed to open file");
				currentFilePath = "";
				delete[] sections;
				return;
			}

			LoadFileBytes();

			numOfImports = getNumOfImports(filePath.c_str().AsWChar(), is64Bit);
			imports = new ImportedFunction[numOfImports];
			if (getAllImports(filePath.c_str().AsWChar(), is64Bit, imports, numOfImports) != numOfImports)
			{
				wxMessageBox("Error getting all imports", "Failed to open file");
				currentFilePath = "";
				delete[] sections;
				delete[] imports;
				return;
			}
			
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

	int answer = wxMessageBox("Do you want to look for function name symbols? This could take some time.", "Get function name symbols", wxYES_NO, this);
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
	if (fileBytes)
	{
		delete[] fileBytes;
	}

	dataViewerMenu->ClearData();
	
	ClearStyledTextCtrl(disassemblyTextCtrl);

	disassembledInstructions.clear();
	disassembledInstructions.shrink_to_fit();

	jmpTableStartAddresses.clear();
	jmpTableStartAddresses.shrink_to_fit();

	indirectTableStartAddresses.clear();
	indirectTableStartAddresses.shrink_to_fit();

	for (int i = 0; i < numOfSections; i++)
	{
		freeJdcStr(&sections[i].name);
	}
	if (sections)
	{
		delete[] sections;
	}

	for (int i = 0; i < numOfImports; i++) 
	{
		freeJdcStr(&imports[i].name);
	}
	if (imports)
	{
		delete[] imports;
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

void MainGui::LoadFileBytes()
{
	if (currentFilePath == "")
	{
		wxMessageBox("No file opened", "Can't load data");
		return;
	}

	int totalSize = 0;
	for (int i = 0; i < numOfSections; i++) 
	{
		totalSize += sections[i].size;
	}

	fileBytes = new unsigned char[totalSize];

	int currentIndex = 0;
	for (int i = 0; i < numOfSections; i++)
	{
		if (!readFileSection(currentFilePath.c_str().AsWChar(), &sections[i], is64Bit, fileBytes + currentIndex, sections[i].size))
		{
			wxMessageBox("Error reading bytes from file section " + wxString(sections[i].name.buffer), "Can't load data");

			delete[] fileBytes;
			return;
		}

		currentIndex += sections[i].size;
	}
}

void MainGui::DisassembleCodeSections()
{
	struct DisassemblerOptions options = { 0 };
	options.is64BitMode = is64Bit;

	for (int i = 0; i < numOfSections; i++)
	{
		if (sections[i].type != CODE_FST) 
		{
			continue;
		}
		
		unsigned char* bytes = new unsigned char[sections[i].size];
		if (!readFileSection(currentFilePath.c_str().AsWChar(), &sections[i], is64Bit, bytes, sections[i].size))
		{
			wxMessageBox("Error reading bytes from file code section", "Can't disassemble");
			delete[] bytes;
			return;
		}

		int instructionNum = 0;

		struct DisassembledInstruction currentInstruction;
		unsigned int currentIndex = 0;
		unsigned char numOfBytes = 0;
		while (disassembleInstruction(&bytes[currentIndex], bytes + sections[i].size - 1, &options, &currentInstruction, &numOfBytes))
		{
			if (numOfBytes == 0)
			{
				break;
			}

			currentInstruction.address = imageBase + sections[i].virtualAddress + currentIndex;
			currentIndex += numOfBytes;

			disassembledInstructions.push_back(currentInstruction);

			if (currentInstruction.opcode == NO_MNEMONIC)
			{
				break;
			}

			instructionNum++;

			instructionNum += HandleJmpTables(bytes, &currentIndex, sections[i]);
		}

		delete[] bytes;
	}
}

int MainGui::HandleJmpTables(unsigned char* bytes, unsigned int* currentIndexRef, FileSection currentCodeSection)
{
	unsigned long long jmpTableStartAddress = getJumpTableAddress(&disassembledInstructions[0], disassembledInstructions.size());
	if (jmpTableStartAddress != 0 &&
		jmpTableStartAddress > currentCodeSection.virtualAddress + imageBase && jmpTableStartAddress < currentCodeSection.virtualAddress + imageBase + currentCodeSection.size)
	{
		jmpTableStartAddresses.push_back(jmpTableStartAddress);
	}

	unsigned long long indirectTableStartAddress = getIndirectTableAddress(&disassembledInstructions[0], disassembledInstructions.size());
	if (indirectTableStartAddress != 0 &&
		indirectTableStartAddress > currentCodeSection.virtualAddress + imageBase && indirectTableStartAddress < currentCodeSection.virtualAddress + imageBase + currentCodeSection.size)
	{
		indirectTableStartAddresses.push_back(indirectTableStartAddress);
	}

	struct DisassembledInstruction dataInstruction;
	dataInstruction.opcode = DATA;
	dataInstruction.operands[0].type = IMMEDIATE;
	dataInstruction.operands[1].type = NO_OPERAND;
	dataInstruction.operands[2].type = NO_OPERAND;
	dataInstruction.operands[3].type = NO_OPERAND;
	dataInstruction.group1Prefix = NO_PREFIX;
	dataInstruction.isInvalid = 0;
	
	int numOfNewInstructions = 0;
	if (CheckForJmpTableStart(currentCodeSection.virtualAddress + imageBase + *currentIndexRef) || doesInstructionDoNothing(&disassembledInstructions[disassembledInstructions.size() - 1]))
	{
		unsigned long long addressInCode = *(unsigned long long*)(bytes + *currentIndexRef);
		unsigned char addressSize = 8;
		if (!is64Bit)
		{
			addressInCode = *(unsigned int*)(bytes + *currentIndexRef);
			addressSize = 4;
		}
		
		while (!CheckForIndirectTableStart(currentCodeSection.virtualAddress + imageBase + *currentIndexRef) && 
			addressInCode > currentCodeSection.virtualAddress + imageBase && addressInCode < currentCodeSection.virtualAddress + currentCodeSection.size + imageBase)
		{
			dataInstruction.operands[0].immediate.value = addressInCode;
			dataInstruction.address = currentCodeSection.virtualAddress + imageBase + *currentIndexRef;
			disassembledInstructions.push_back(dataInstruction);
			*currentIndexRef += addressSize;
			numOfNewInstructions++;

			addressInCode = *(unsigned long long*)(bytes + *currentIndexRef);
			if (!is64Bit)
			{
				addressInCode = *(unsigned int*)(bytes + *currentIndexRef);
			}
		}
	}

	if (CheckForIndirectTableStart(currentCodeSection.virtualAddress + imageBase + *currentIndexRef))
	{
		while (bytes[*currentIndexRef] != 0xCC)
		{
			if (CheckForJmpTableStart(currentCodeSection.virtualAddress + imageBase + *currentIndexRef)) 
			{
				numOfNewInstructions += HandleJmpTables(bytes, currentIndexRef, currentCodeSection);
				break;
			}
			
			dataInstruction.operands[0].immediate.value = bytes[*currentIndexRef];
			dataInstruction.address = currentCodeSection.virtualAddress + imageBase + *currentIndexRef;
			disassembledInstructions.push_back(dataInstruction);
			(*currentIndexRef)++;
			numOfNewInstructions++;
		}
	}

	return numOfNewInstructions;
}

unsigned char MainGui::CheckForJmpTableStart(unsigned long long currentAddress)
{
	for (int i = jmpTableStartAddresses.size() - 1; i >= 0; i--) 
	{
		if (jmpTableStartAddresses[i] == currentAddress) 
		{
			return 1;
		}
	}

	return 0;
}

unsigned char MainGui::CheckForIndirectTableStart(unsigned long long currentAddress)
{
	for (int i = indirectTableStartAddresses.size() - 1; i >= 0; i--)
	{
		if (indirectTableStartAddresses[i] == currentAddress)
		{
			return 1;
		}
	}

	return 0;
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

	struct JdcStr decompilationResult = initializeJdcStr();
	if (decompilationResult.bufferSize == 0)
	{
		wxMessageBox("Error allocating memory for function decompilation", "Can't decompile");
		return;
	}

	if (!decompileFunction(&decompParams, &decompilationResult))
	{
		wxMessageBox(decompilationResult.buffer, "Can't decompile");
		freeJdcStr(&decompilationResult);
		return;
	}

	decompilationTextCtrl->SetReadOnly(false);
	decompilationTextCtrl->SetValue(decompilationResult.buffer);
	freeJdcStr(&decompilationResult);
	ApplySyntaxHighlighting(decompParams.currentFunc);
	decompilationTextCtrl->SetReadOnly(true);
	currentDecompiledFunc = functionIndex;
}

void MainGui::FindAllFunctions() 
{
	decompParams.imports = imports;
	decompParams.numOfImports = numOfImports;

	decompParams.instructions = disassembledInstructions.data();
	decompParams.numOfInstructions = disassembledInstructions.size();

	decompParams.imageBase = imageBase;
	decompParams.sections = sections;
	decompParams.numOfSections = numOfSections;
	decompParams.fileBytes = fileBytes;

	decompParams.is64Bit = is64Bit;
	
	int numOfInstructions = disassembledInstructions.size();
	int instructionIndex = 0;

	int codeSectionIndex = 0;
	for (int i = 0; i < numOfSections; i++) 
	{
		if (sections[i].type == CODE_FST) 
		{
			codeSectionIndex = i;
			break;
		}
	}
	unsigned long long currentSectionEndAddress = imageBase + sections[codeSectionIndex].virtualAddress + sections[codeSectionIndex].size - 1;

	std::vector<unsigned long long> calledAddresses;
	for (int i = 0; i < numOfInstructions; i++) 
	{
		if (disassembledInstructions[i].opcode == CALL_NEAR) 
		{
			decompParams.startInstructionIndex = i;
			unsigned long long address = resolveJmpChain(&decompParams);
			if (findAddressInArr(&calledAddresses[0], 0, calledAddresses.size() - 1, address) == -1)
			{
				calledAddresses.insert(std::lower_bound(calledAddresses.begin(), calledAddresses.end(), address), address); // sorting it
			}
		}
	}

	decompParams.startInstructionIndex = 0;

	struct Function currentFunction;
	memset(&currentFunction, 0, sizeof(struct Function));
	while (instructionIndex < numOfInstructions && findNextFunction(&decompParams, currentSectionEndAddress, &calledAddresses[0], calledAddresses.size(), &currentFunction, &instructionIndex))
	{
		if (disassembledInstructions[instructionIndex].address > currentSectionEndAddress)
		{
			unsigned foundNextCodeSection = 0;
			for (int i = codeSectionIndex + 1; i < numOfSections; i++)
			{
				if (sections[i].type == CODE_FST)
				{
					codeSectionIndex = i;
					foundNextCodeSection = 1;
					break;
				}
			}

			if (foundNextCodeSection)
			{
				currentSectionEndAddress = imageBase + sections[codeSectionIndex].virtualAddress + sections[codeSectionIndex].size - 1;
			}
		}

		currentFunction.name = initializeJdcStr();
		sprintfJdc(&currentFunction.name, 0, "func%llX", disassembledInstructions[currentFunction.firstInstructionIndex].address - imageBase);

		functions.push_back(currentFunction);
		memset(&currentFunction, 0, sizeof(struct Function));

		decompParams.startInstructionIndex = instructionIndex;
	}

	decompParams.functions = &functions[0];
	decompParams.numOfFunctions = functions.size();

	if (functions.size() > 0) 
	{
		fixAllFunctionReturnTypes(&decompParams);
		fixAllFunctionArgs(&decompParams);
	}
}

void MainGui::UpdateDisassemblyTextCtrl()
{
	disassemblyTextCtrl->SetReadOnly(false);
	disassemblyTextCtrl->Freeze();

	int numOfInstructions = disassembledInstructions.size();
	int sectionIndex = -1;

	wxString disassemblyText = "";
	disassemblyText.reserve(numOfInstructions * 50);

	for (int i = 0; i < numOfInstructions; i++) 
	{
		for (int j = sectionIndex + 1; j < numOfSections; j++)
		{
			if (sections[j].type == CODE_FST && disassembledInstructions[i].address >= sections[j].virtualAddress + imageBase)
			{
				sectionIndex = j;
				break;
			}
		}
		
		char addressStr[20] = { 0 };
		sprintf(addressStr, "%llX", disassembledInstructions[i].address);
		wxString addressInfoStr = wxString(addressStr) + wxString(sections[sectionIndex].name.buffer) + "\t";

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
		else if(disassembledInstructions[i].operands[0].type == IMMEDIATE && 
			(isOpcodeJcc(disassembledInstructions[i].opcode) || disassembledInstructions[i].opcode == JMP_SHORT || disassembledInstructions[i].opcode == JMP_NEAR))
		{
			char jmpAddressStr[20] = { 0 };
			sprintf(jmpAddressStr, "%llX", disassembledInstructions[i].address + disassembledInstructions[i].operands[0].immediate.value);

			asmStr += " ; jump to " + wxString(jmpAddressStr);
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

		char addressStr[10];
		sprintf(addressStr, "%llX", disassembledInstructions[functions[i].firstInstructionIndex].address);
		functionsGrid->SetCellValue(i, 0, wxString(addressStr));

		functionsGrid->SetCellValue(i, 1, wxString(callingConventionStrs[functions[i].callingConvention]));

		functionsGrid->SetCellValue(i, 2, wxString(functions[i].name.buffer));
		functionsGrid->SetCellValue(i, 3, std::to_string(functions[i].lastInstructionIndex - functions[i].firstInstructionIndex + 1));
	}

	functionsGrid->Thaw();
}

void MainGui::GetFunctionSymbols() 
{
	functionsGrid->Freeze();

	int numOfFunctions = functions.size();
	for (int i = 0; i < numOfFunctions; i++)
	{
		if (getSymbolByValue(currentFilePath.c_str().AsWChar(), is64Bit, disassembledInstructions[functions[i].firstInstructionIndex].address, &functions[i].name))
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
	const int ID_VIEW_INFO = 101;
	const int ID_EDIT_PROPERTIES = 102;
	const int ID_COPY_ADDRESS = 103;
	const int ID_COPY_NAME = 104;
	const int ID_FIND_BY_ADDR = 105;
	const int ID_FIND_BY_NAME = 106;

	menu.Append(ID_DECOMPILE, "Decompile");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { DecompileFunction(row); }, ID_DECOMPILE);

	menu.Append(ID_VIEW_INFO, "View info");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { new FunctionInfoMenu(this, GetPosition(), &disassembledInstructions[0], &functions[row]); }, ID_VIEW_INFO);

	menu.Append(ID_EDIT_PROPERTIES, "Edit properties");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { new FunctionPropertiesMenu(this, GetPosition(), this, row); }, ID_EDIT_PROPERTIES);

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
				unsigned char foundFunction = 0;
				int numOfFunctions = functions.size();
				for (int i = 0; i < numOfFunctions; i++) 
				{
					if (address >= disassembledInstructions[functions[i].firstInstructionIndex].address && address <= disassembledInstructions[functions[i].lastInstructionIndex].address) 
					{
						functionsGrid->GoToCell(i, 0);
						functionsGrid->SelectRow(i);
						foundFunction = 1;
						break;
					}
				}

				if (!foundFunction)
				{
					wxMessageBox("No function with that address", "Failed to find function");
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
					wxString funcName = wxString(functions[i].name.buffer);
					if (funcName.Contains(txt))
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
	const int ID_GO_TO_ADDR = 104;

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
		findCtrl = ctrl;
		lastFindText = "";

		if (!findDialog)
		{
			findDialog = new wxFindReplaceDialog(this, &findData, "Find");
			findDialog->Bind(wxEVT_FIND, &MainGui::OnFindDialog, this);
			findDialog->Bind(wxEVT_FIND_NEXT, &MainGui::OnFindDialog, this);
			findDialog->Bind(wxEVT_FIND_CLOSE, &MainGui::OnFindDialogClose, this);
		}

		findDialog->Show();
		findDialog->Raise();
	}, ID_FIND);

	if(ctrl == disassemblyTextCtrl)
	{
		menu.Append(ID_GO_TO_ADDR, "Go to address");
		menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
			wxTextEntryDialog dlg(this, "", "Address");
			if (dlg.ShowModal() == wxID_OK)
			{
				wxString txt = dlg.GetValue();
				unsigned long long address = 0;
				if (txt.ToULongLong(&address, 16))
				{
					int index = findInstructionByAddress(&disassembledInstructions[0], 0, disassembledInstructions.size() - 1, address);
					if (index == -1)
					{
						wxMessageBox("Address not found", "Failed to find address");
					}
					else
					{
						disassemblyTextCtrl->GotoLine(index);
						int pos = disassemblyTextCtrl->PositionFromLine(index);
						disassemblyTextCtrl->SetSelection(pos, pos + disassemblyTextCtrl->GetLineLength(index));
					}
				}
				else
				{
					wxMessageBox("Not valid hex number", "Failed to find address");
				}
			}
		}, ID_GO_TO_ADDR);
	}

	PopupMenu(&menu, ScreenToClient(e.GetPosition()));
}

void MainGui::OnDecompilationUpdateUI(wxStyledTextEvent& e)
{
	if (!decompilationTextCtrl)
	{
		return;
	}

	int pos = decompilationTextCtrl->GetCurrentPos();
	int bracePos1 = -1;
	int bracePos2 = -1;

	int ch = decompilationTextCtrl->GetCharAt(pos);
	if (ch == '{' || ch == '}' || ch == '(' || ch == ')' || ch == '[' || ch == ']')
	{
		bracePos1 = pos;
	}

	if (bracePos1 != -1)
	{
		bracePos2 = decompilationTextCtrl->BraceMatch(bracePos1);
	}

	if (bracePos1 != -1 && bracePos2 != -1)
	{
		decompilationTextCtrl->BraceHighlight(bracePos1, bracePos2);
	}
	else if (bracePos1 != -1)
	{
		decompilationTextCtrl->BraceBadLight(bracePos1);
	}
	else
	{
		decompilationTextCtrl->BraceHighlight(-1, -1);
	}
}

void MainGui::OnFindDialog(wxFindDialogEvent& e)
{
	if (!findCtrl)
	{
		return;
	}

	wxString text = e.GetFindString();
	if (text.IsEmpty())
	{
		return;
	}

	int flags = 0;
	if (e.GetFlags() & wxFR_MATCHCASE)
	{
		flags |= wxSTC_FIND_MATCHCASE;
	}
	if (e.GetFlags() & wxFR_WHOLEWORD)
	{
		flags |= wxSTC_FIND_WHOLEWORD;
	}

	if (!text.IsSameAs(lastFindText))
	{
		totalFindResults = CountNumOfResults(findCtrl, text, findCtrl->GetLength(), flags);

		if (totalFindResults == 0)
		{
			wxMessageBox("Text not found", "Failed to find text");
			return;
		}

		lastFindText = text;
	}

	long selStart = findCtrl->GetSelectionStart();
	long selEnd = findCtrl->GetSelectionEnd();
	unsigned char forward = (e.GetFlags() & wxFR_DOWN) != 0;

	int start = forward ? selEnd : 0;
	int end = forward ? findCtrl->GetLength() : selStart;

	int pos = FindInRange(findCtrl, text, start, end, flags, forward);
	if (pos == -1)
	{
		int wrapStart = forward ? 0 : selEnd;
		int wrapEnd = forward ? selStart : findCtrl->GetLength();
		pos = FindInRange(findCtrl, text, wrapStart, wrapEnd, flags, forward);
	}

	findCtrl->GotoPos(pos);
	findCtrl->SetSelection(pos, pos + text.size());
	statusStaticText->SetLabelText("Status: Finding '" + text + "' (" + std::to_string(CountNumOfResults(findCtrl, text, pos, flags) + 1) + "/" + std::to_string(totalFindResults) + ")");
}

int MainGui::FindInRange(wxStyledTextCtrl* ctrl, const wxString& text, int start, int end, int flags, unsigned char forward)
{
	if (forward)
	{
		return ctrl->FindText(start, end, text, flags);
	}

	int lastPos = -1;
	int pos = ctrl->FindText(start, end, text, flags);
	while (pos != -1)
	{
		lastPos = pos;
		pos = ctrl->FindText(pos + 1, end, text, flags);
	}

	return lastPos;
}

int MainGui::CountNumOfResults(wxStyledTextCtrl* ctrl, const wxString& text, int end, int flags)
{
	int result = 0;

	int start = 0;
	int pos = ctrl->FindText(start, end, text, flags);
	while (pos != -1)
	{
		result++;
		start = pos + 1;
		pos = ctrl->FindText(pos + 1, end, text, flags);
	}

	return result;
}

void MainGui::OnFindDialogClose(wxFindDialogEvent& e)
{
	if (findDialog)
	{
		findDialog->Destroy();
		findDialog = nullptr;
	}

	statusStaticText->SetLabelText("Status: idle");
}

void MainGui::CloseApp(wxCloseEvent& e)
{
	if (findDialog)
	{
		findDialog->Destroy();
		findDialog = nullptr;
	}
	
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

	// stack vars
	for (int i = 0; i < function->numOfStackVars; i++) 
	{
		ColorAllStrs(text, function->stackVars[i].name.buffer, ColorsMenu::DecompilationColor::LOCAL_VAR_COLOR, 1);
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

	// intrinsic functions
	for (int i = 0; i < numOfReturningIntrinsicFuncs; i++)
	{
		ColorAllStrs(text, returningIntrinsicFuncs[i].name, ColorsMenu::DecompilationColor::INTRINSIC_COLOR, 0);
	}
	for (int i = 0; i < numOfVoidIntrinsicFuncs; i++)
	{
		ColorAllStrs(text, voidIntrinsicFuncs[i].name, ColorsMenu::DecompilationColor::INTRINSIC_COLOR, 0);
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
	ColorAllStrs(text, "unsigned", ColorsMenu::DecompilationColor::PRIMITIVE_COLOR, 0);
	ColorAllStrs(text, "sizeof", ColorsMenu::DecompilationColor::PRIMITIVE_COLOR, 0);

	// keywords
	const char* keywordStrs[11] = { "if", "else", "for", "while", "do", "break", "continue", "switch", "case", "goto", "return" };
	for (int i = 0; i < 11; i++)
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

	// comments
	start = 0;
	while (start < text.length())
	{
		int pos = text.find("//", start);
		int end = text.find("\n", pos + 1);
		if (pos != wxNOT_FOUND && end != wxNOT_FOUND)
		{
			decompilationTextCtrl->StartStyling(pos);
			decompilationTextCtrl->SetStyling(end - pos + 1, ColorsMenu::DecompilationColor::COMMENT_DECOMP_COLOR);

			start = end + 1;
		}
		else
		{
			break;
		}
	}

	// labels
	start = 0;
	while (start < text.length())
	{
		int pos = text.find("label_", start);
		int end = text.find("\n", pos + 1);
		
		if (pos != wxNOT_FOUND && end != wxNOT_FOUND)
		{
			decompilationTextCtrl->StartStyling(pos);
			decompilationTextCtrl->SetStyling(end - pos - 1, ColorsMenu::DecompilationColor::LABEL_COLOR);

			start = end + 1;
		}
		else
		{
			break;
		}
	}

	// regs/segs that arent variables/arguments
	for (int i = 0; i < numOfRegisters; i++)
	{
		ColorAllStrs(text, registerStrs[i], ColorsMenu::DecompilationColor::ERROR_COLOR, 0);
	}
	for (int i = 0; i < numOfSegments; i++)
	{
		ColorAllStrs(text, segmentStrs[i], ColorsMenu::DecompilationColor::ERROR_COLOR, 0);
	}
	ColorAllStrs(text, "ERROR", ColorsMenu::DecompilationColor::ERROR_COLOR, 0);

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
			if (instruction->operands[i].type == REGISTER)
			{
				wxString regStr = wxString(registerStrs[instruction->operands[i].reg]);
				int loc = asmStr.find(regStr, regStart);
				disassemblyTextCtrl->StartStyling(pos + loc);
				disassemblyTextCtrl->SetStyling(regStr.length(), ColorsMenu::DisassemblyColor::REGISTER_COLOR);
				regStart = loc + regStr.length();
			}
			else if (instruction->operands[i].type == MEM_ADDRESS)
			{
				if (instruction->operands[i].memoryAddress.reg != NO_REG)
				{
					wxString regStr = wxString(registerStrs[instruction->operands[i].memoryAddress.reg]);
					int loc = asmStr.find(regStr, regStart);
					disassemblyTextCtrl->StartStyling(pos + loc);
					disassemblyTextCtrl->SetStyling(regStr.length(), ColorsMenu::DisassemblyColor::REGISTER_COLOR);
					regStart = loc + regStr.length();
				}

				if (instruction->operands[i].memoryAddress.regDisplacement != NO_REG)
				{
					wxString regStr = wxString(registerStrs[instruction->operands[i].memoryAddress.regDisplacement]);
					int loc = asmStr.find(regStr, regStart);
					disassemblyTextCtrl->StartStyling(pos + loc);
					disassemblyTextCtrl->SetStyling(regStr.length(), ColorsMenu::DisassemblyColor::REGISTER_COLOR);
					regStart = loc + regStr.length();
				}

				if (instruction->operands[i].memoryAddress.segment != NO_SEGMENT)
				{
					wxString segStr = wxString(segmentStrs[instruction->operands[i].memoryAddress.segment]) + ":";
					int loc = asmStr.find(segStr, segStart);
					disassemblyTextCtrl->StartStyling(pos + loc);
					disassemblyTextCtrl->SetStyling(segStr.length(), ColorsMenu::DisassemblyColor::SEGMENT_COLOR);
					segStart = loc + segStr.length();
				}

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
			else if (instruction->operands[i].type == SEGMENT)
			{
				wxString segStr = wxString(segmentStrs[instruction->operands[i].segment]);
				int loc = asmStr.find(segStr, segStart);
				disassemblyTextCtrl->StartStyling(pos + loc);
				disassemblyTextCtrl->SetStyling(segStr.length(), ColorsMenu::DisassemblyColor::SEGMENT_COLOR);
				segStart = loc + segStr.length();
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

		// comments
		int commentStart = asmStr.find(";", 0);
		if (commentStart != wxNOT_FOUND)
		{
			disassemblyTextCtrl->StartStyling(pos + commentStart);
			disassemblyTextCtrl->SetStyling(asmStr.length() - commentStart, ColorsMenu::DisassemblyColor::COMMENT_DIS_COLOR);
		}

		pos += asmStr.size();
	}
}

void MainGui::ColorAllStrs(wxString text, const char* str, ColorsMenu::DecompilationColor color, unsigned char forceColor)
{
	if(!str || !strcmp(str, ""))
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
			int end = pos + strlen(str);

			if (forceColor || 
				decompilationTextCtrl->GetStyleAt(pos) == color || // incase there are two strs that are equal except for one having more text at the end
				decompilationTextCtrl->GetStyleAt(pos) == ColorsMenu::DecompilationColor::OPERATOR_COLOR) // only apply color if it hasn't been colored yet
			{
				decompilationTextCtrl->StartStyling(pos);
				decompilationTextCtrl->SetStyling(strlen(str), color);
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

FunctionPropertiesMenu::FunctionPropertiesMenu(wxWindow* parent, wxPoint position, MainGui* main, int funcIndex) : wxFrame(parent, MainWindowID, "Change Function Properties", wxPoint(50, 50), wxSize(600, 600))
{
	Function* function = &(main->functions[funcIndex]);
	
	SetOwnBackgroundColour(backgroundColor);

	functionNameLabel = new wxStaticText(this, wxID_ANY, "Function Name");
	functionNameLabel->SetOwnForegroundColour(textColor);

	functionNameTextCtrl = new wxTextCtrl(this, wxID_ANY, function->name.buffer, wxPoint(0, 0), wxSize(100, 25));
	functionNameTextCtrl->SetOwnBackgroundColour(foregroundColor);
	functionNameTextCtrl->SetOwnForegroundColour(textColor);

	vSizer = new wxBoxSizer(wxVERTICAL);

	vSizer->Add(functionNameLabel, 0, wxEXPAND | wxTOP | wxLEFT, 10);
	vSizer->Add(functionNameTextCtrl, 0, wxEXPAND | wxLEFT | wxBOTTOM, 10);

	varsGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(400, 200));
	varsGrid->SetLabelBackgroundColour(foregroundColor);
	varsGrid->SetLabelTextColour(textColor);
	varsGrid->SetDefaultCellBackgroundColour(gridColor);
	varsGrid->SetDefaultCellTextColour(textColor);
	varsGrid->CreateGrid(0, 2);
	varsGrid->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
	varsGrid->SetScrollRate(0, 10);
	varsGrid->DisableDragRowSize();
	varsGrid->SetColLabelValue(0, "Var type");
	varsGrid->SetColLabelValue(1, "Name");
	varsGrid->HideRowLabels();
	varsGrid->SetColSize(0, 200);
	varsGrid->SetColSize(1, 200);
	varsGrid->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	vSizer->Add(varsGrid, 1, wxEXPAND | wxLEFT | wxRIGHT, 10);

	int rowIndex = 0;

	for (int i = 0; i < function->numOfRegArgs; i++)
	{
		varsGrid->AppendRows(1);
		varsGrid->SetCellValue(rowIndex, 0, "Reg arg");
		varsGrid->SetCellValue(rowIndex, 1, function->regArgs[i].name.buffer);
		varsGrid->SetReadOnly(rowIndex, 0);
		rowIndex++;
	}

	for (int i = 0; i < function->numOfRegVars; i++)
	{
		varsGrid->AppendRows(1);
		varsGrid->SetCellValue(rowIndex, 0, "Reg var");
		varsGrid->SetCellValue(rowIndex, 1, function->regVars[i].name.buffer);
		varsGrid->SetReadOnly(rowIndex, 0);
		rowIndex++;
	}

	for (int i = 0; i < function->numOfStackArgs; i++)
	{
		varsGrid->AppendRows(1);
		varsGrid->SetCellValue(rowIndex, 0, "Stack arg");
		varsGrid->SetCellValue(rowIndex, 1, function->stackArgs[i].name.buffer);
		varsGrid->SetReadOnly(rowIndex, 0);
		rowIndex++;
	}

	for (int i = 0; i < function->numOfStackVars; i++)
	{
		varsGrid->AppendRows(1);
		varsGrid->SetCellValue(rowIndex, 0, "Stack var");
		varsGrid->SetCellValue(rowIndex, 1, function->stackVars[i].name.buffer);
		varsGrid->SetReadOnly(rowIndex, 0);
		rowIndex++;
	}

	for (int i = 0; i < function->numOfReturnedVars; i++)
	{
		varsGrid->AppendRows(1);
		varsGrid->SetCellValue(rowIndex, 0, "Returned var");
		varsGrid->SetCellValue(rowIndex, 1, function->returnedVars[i].name.buffer);
		varsGrid->SetReadOnly(rowIndex, 0);
		rowIndex++;
	}

	SetSizerAndFit(vSizer);

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

	int rowIndex = 0;

	for (int i = 0; i < currentFunction->numOfRegArgs; i++)
	{
		strcpyJdc(&currentFunction->regArgs[i].name, varsGrid->GetCellValue(rowIndex, 1).c_str());
		rowIndex++;
	}

	for (int i = 0; i < currentFunction->numOfRegVars; i++)
	{
		strcpyJdc(&currentFunction->regVars[i].name, varsGrid->GetCellValue(rowIndex, 1).c_str());
		rowIndex++;
	}

	for (int i = 0; i < currentFunction->numOfStackArgs; i++)
	{
		strcpyJdc(&currentFunction->stackArgs[i].name, varsGrid->GetCellValue(rowIndex, 1).c_str());
		rowIndex++;
	}

	for (int i = 0; i < currentFunction->numOfStackVars; i++)
	{
		strcpyJdc(&currentFunction->stackVars[i].name, varsGrid->GetCellValue(rowIndex, 1).c_str());
		rowIndex++;
	}

	for (int i = 0; i < currentFunction->numOfReturnedVars; i++)
	{
		strcpyJdc(&currentFunction->returnedVars[i].name, varsGrid->GetCellValue(rowIndex, 1).c_str());
		rowIndex++;
	}

	// update name in function grid
	mainGui->functionsGrid->SetCellValue(functionIndex, 2, currentFunction->name.buffer);

	// redecompile if it is currently in the decompilation box. this is to refresh it
	if (mainGui->currentDecompiledFunc == functionIndex)
	{
		mainGui->DecompileFunction(functionIndex);
	}

	Destroy();
}
