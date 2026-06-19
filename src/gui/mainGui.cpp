#include "mainGui.h"
#include <wx/numdlg.h>
#include "fileHeadersMenu.h"
#include "calculatorMenu.h"
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

	disassemblyTextCtrl = new JdcTextCtrl(topSplitter, wxSize(150, 400), DISASSEMBLY_CTRL_TYPE, &disassembledInstructions, &functions, &currentDecompiledFunc);
	decompilationTextCtrl = new JdcTextCtrl(topSplitter, wxSize(150, 400), DECOMPILATION_CTRL_TYPE, &disassembledInstructions, &functions, &currentDecompiledFunc);

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
	stringsMenu = new StringsMenu();
	colorsMenu = new ColorsMenu(disassemblyTextCtrl, decompilationTextCtrl, dataViewerMenu->dataTextCtrl);

	wxMenu* fileMenu = new wxMenu();
	AddMenuItem(fileMenu, OpenFileID, "Open file", [&](wxCommandEvent& ce) -> void { OpenFile(); });
	AddMenuItem(fileMenu, DisassembleFileID, "Disassemble file", [&](wxCommandEvent& ce) -> void { DisassembleFile(); });
	AddMenuItem(fileMenu, AnalyzeFileID, "Analyze file", [&](wxCommandEvent& ce) -> void { AnalyzeFile(); });

	wxMenu* toolMenu = new wxMenu();
	AddMenuItem(toolMenu, OpenBytesDisassemblerID, "Bytes disassembler", [&](wxCommandEvent& ce) -> void { new BytesDisassembler(this, GetPosition()); });
	AddMenuItem(toolMenu, OpenSectionsViewerID, "File sections", [&](wxCommandEvent& ce) -> void { new SectionsViewer(this, GetPosition(), sections, numOfSections); });
	AddMenuItem(toolMenu, OpenDataViewerID, "Data viewer", [&](wxCommandEvent& ce) -> void { dataViewerMenu->OpenMenu(GetPosition(), imageBase, sections, numOfSections, fileBytes); });
	AddMenuItem(toolMenu, OpenStringsMenuID, "Strings", [&](wxCommandEvent& ce) -> void { stringsMenu->OpenMenu(GetPosition(), imageBase, sections, numOfSections, fileBytes); });
	AddMenuItem(toolMenu, OpenImportsViewerID, "Imports", [&](wxCommandEvent& ce) -> void { new ImportsViewer(this, GetPosition(), imports, numOfImports); });
	AddMenuItem(toolMenu, OpenFileHeadersMenuID, "File headers", [&](wxCommandEvent& ce) -> void { new FileHeadersMenu(this, GetPosition(), currentFilePath); });
	AddMenuItem(toolMenu, OpenCalculatorMenuID, "Calculator", [&](wxCommandEvent& ce) -> void { new CalculatorMenu(this, GetPosition()); });

	wxMenu* optionsMenu = new wxMenu();
	AddMenuItem(optionsMenu, OpenColorsMenuID, "Colors", [&](wxCommandEvent& ce) -> void { colorsMenu->OpenMenu(GetPosition()); });

	menuBar->Append(fileMenu, "File");
	menuBar->Append(toolMenu, "Tools");
	menuBar->Append(optionsMenu, "Options");
	this->SetMenuBar(menuBar);

	disassemblyTextCtrl->AddAssociatedTextCtrl(decompilationTextCtrl);
	disassemblyTextCtrl->AddAssociatedTextCtrl(dataViewerMenu->dataTextCtrl);
	decompilationTextCtrl->AddAssociatedTextCtrl(disassemblyTextCtrl);

	topSplitter->SplitVertically(disassemblyTextCtrl, decompilationTextCtrl, 0);
	topSplitter->SetSashGravity(0.5);
	topSplitter->SetMinimumPaneSize(100);

	mainSplitter->SplitHorizontally(topSplitter, functionsGrid, 150);
	mainSplitter->SetSashGravity(1);
	mainSplitter->SetMinimumPaneSize(100);

	vSizer = new wxBoxSizer(wxVERTICAL);
	vSizer->Add(statusStaticText, 0, wxTOP | wxLEFT, 10);
	vSizer->Add(mainSplitter, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

	SetSizerAndFit(vSizer);
}

void MainGui::AddMenuItem(wxMenu* menu, int id, const char* name, const std::function<void(wxCommandEvent&)>& function)
{
	wxMenuItem* menuItem = new wxMenuItem(0, id, name);
	menuItem->SetBackgroundColour(foregroundColor);
	menuItem->SetTextColour(textColor);
	menu->Append(menuItem);
	menu->Bind(wxEVT_MENU, function, id);
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

			numOfFileBytes = getNumOfFileBytes(filePath.c_str().AsWChar());
			if (numOfFileBytes == 0)
			{
				wxMessageBox("Error getting number of bytes in file", "Can't load data");
				return;
			}

			wxString fileName = openFileDialog.GetPath().Mid(openFileDialog.GetPath().Last('\\') + 1);

			if (!isFile64Bit(filePath.c_str().AsWChar(), &is64Bit))
			{
				int loadAnyway = wxMessageBox("Error determining file architecture. Do you still want to load the file?", "Failed to open file", wxYES_NO, this);
				if (loadAnyway == wxYES)
				{
					int ask64Bit = wxMessageBox("Do you want to disassemble in 64 bit mode?", "Specify architecture", wxYES_NO, this);
					is64Bit = ask64Bit == wxYES;

					imageBase = 0;
					entryPoint = wxGetNumberFromUser("Specify the entry point as a file offset (image base will be zero):", "Entry point:", "Specify the entry point", 0, 0, numOfFileBytes - 1, this);
				
					fileBytes = new unsigned char[numOfFileBytes];
					if (!readFileBytes(currentFilePath.c_str().AsWChar(), fileBytes, numOfFileBytes))
					{
						wxMessageBox("Error reading bytes from file", "Can't load data");
						currentFilePath = "";
						delete[] fileBytes;
						return;
					}

					numOfSections = 1;
					sections = new FileSection[1];
					sections[0].name = initializeJdcStrWithVal(wxString("." + fileName).c_str());
					sections[0].type = CODE_FST;
					sections[0].isReadOnly = 1;
					sections[0].virtualAddress = 0;
					sections[0].fileOffset = 0;
					sections[0].size = numOfFileBytes;

					numOfImports = 0;

					this->SetTitle("Jesso Decompiler x64 - opened file " + fileName);

					DisassembleFile();
					openFileDialog.Close(true);
					return;
				}
				
				this->SetTitle("Jesso Decompiler x64");
				currentFilePath = "";
				return;
			}

			imageBase = getFileImageBase(filePath.c_str().AsWChar(), is64Bit);
			entryPoint = getFileEntryPoint(filePath.c_str().AsWChar(), is64Bit);

			fileBytes = new unsigned char[numOfFileBytes];
			if (!readFileBytes(currentFilePath.c_str().AsWChar(), fileBytes, numOfFileBytes))
			{
				wxMessageBox("Error reading bytes from file", "Can't load data");
				currentFilePath = "";
				delete[] fileBytes;
				return;
			}

			numOfSections = getNumOfSections(filePath.c_str().AsWChar(), is64Bit);
			sections = new FileSection[numOfSections];
			if (!getAllFileSectionHeaders(filePath.c_str().AsWChar(), is64Bit, sections, numOfSections))
			{
				wxMessageBox("Error getting all file sections", "Failed to open file");
				currentFilePath = "";
				delete[] fileBytes;
				delete[] sections;
				return;
			}

			numOfImports = getNumOfImports(filePath.c_str().AsWChar(), is64Bit);
			imports = new ImportedFunction[numOfImports];
			if (getAllImports(filePath.c_str().AsWChar(), is64Bit, imports, numOfImports) != numOfImports)
			{
				wxMessageBox("Error getting all imports", "Failed to open file");
				currentFilePath = "";
				delete[] fileBytes;
				delete[] sections;
				delete[] imports;
				return;
			}
			
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

unsigned char CompareInstructions(const DisassembledInstruction& a, const DisassembledInstruction& b) 
{
	return a.address < b.address;
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

	// first the instructions that are definitely executed are disassembled, then the other code sections bytes or bytes inbetween instructions are disassembled
	struct DisassemblerOptions options = { 0 };
	options.is64BitMode = is64Bit;
	struct DisassembledInstruction instructionBuffer;
	unsigned long long errorAddress = 0;
	if (!DisassembleTakingJumps(entryPoint + imageBase, &instructionBuffer, &options, &errorAddress))
	{
		wxMessageBox("An error occured while disassembling", "Disassembly not fully completed");
	}

	unsigned long long firstAddress = disassembledInstructions[0].address;
	unsigned long long lastAddress = disassembledInstructions[disassembledInstructions.size() - 1].address + disassembledInstructions[disassembledInstructions.size() - 1].numOfBytes;
	for (int i = 0; i < numOfSections; i++)
	{
		if (sections[i].type == CODE_FST)
		{
			unsigned long long sectionStart = sections[i].virtualAddress + imageBase;
			unsigned long long sectionEnd = sections[i].virtualAddress + sections[i].size + imageBase;
			if (sectionEnd < firstAddress || sectionStart > lastAddress)
			{
				if (!DisassembleBetweenBounds(sectionStart, sectionEnd, &instructionBuffer, &options))
				{
					wxMessageBox("An error occured while disassembling", "Disassembly not fully completed");
				}
			}
			else 
			{
				if (sectionStart < firstAddress && sectionEnd > firstAddress)
				{
					if (!DisassembleBetweenBounds(sectionStart, firstAddress, &instructionBuffer, &options))
					{
						wxMessageBox("An error occured while disassembling", "Disassembly not fully completed");
					}
				}

				if (sectionStart < lastAddress && sectionEnd > lastAddress)
				{
					if (!DisassembleBetweenBounds(lastAddress, sectionEnd, &instructionBuffer, &options))
					{
						wxMessageBox("An error occured while disassembling", "Disassembly not fully completed");
					}
				}
			}
		}
	}

	std::sort(disassembledInstructions.begin(), disassembledInstructions.end(), CompareInstructions);

	int numOfInstructions = disassembledInstructions.size();
	for (int i = 0; i < numOfInstructions - 1; i++)
	{
		unsigned long long startVA = disassembledInstructions[i].address + disassembledInstructions[i].numOfBytes;
		unsigned long long endVA = disassembledInstructions[i + 1].address;
		if (startVA != endVA)
		{
			if (!DisassembleBetweenBounds(startVA, endVA, &instructionBuffer, &options))
			{
				wxMessageBox("An error occured while disassembling", "Disassembly not fully completed");
			}
		}
	}

	std::sort(disassembledInstructions.begin(), disassembledInstructions.end(), CompareInstructions);

	statusStaticText->SetLabelText("Status: finished disassembling, updating GUI...");
	statusStaticText->Refresh();
	statusStaticText->Update();

	UpdateDisassemblyTextCtrl();

	if (errorAddress != 0) 
	{
		int line = findInstructionByAddress(&disassembledInstructions[0], 0, disassembledInstructions.size() - 1, errorAddress);
		disassemblyTextCtrl->HighlightLine(line, RED_INDICATOR, 1);
	}

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

	memset(&decompParams, 0, sizeof(decompParams));

	dataViewerMenu->ClearData();
	stringsMenu->ClearData();
	
	disassemblyTextCtrl->ClearText();
	decompilationTextCtrl->ClearText();

	disassembledInstructions.clear();
	disassembledInstructions.shrink_to_fit();

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
	int rows = functionsGrid->GetNumberRows();
	if (rows > 0)
	{
		functionsGrid->DeleteRows(0, functionsGrid->GetNumberRows());
	}
}

unsigned char MainGui::DisassembleTakingJumps(unsigned long long startVA, struct DisassembledInstruction* instructionBuffer, struct DisassemblerOptions* options, unsigned long long* errorAddress)
{
	struct FileSection* currentSection = 0;
	unsigned long long currentFileOffset = rvaToFileOffset(sections, numOfSections, startVA - imageBase, &currentSection);
	if (!currentSection || currentSection->type != CODE_FST)
	{
		return 1;
	}
	else if (currentFileOffset >= numOfFileBytes || currentSection->fileOffset + currentSection->size > numOfFileBytes)
	{
		return 0;
	}

	unsigned long long currentVirtualAddress = startVA;
	while (currentFileOffset < currentSection->fileOffset + currentSection->size)
	{
		if (!disassembleInstruction(&fileBytes[currentFileOffset], fileBytes + currentSection->fileOffset + currentSection->size - 1, options, instructionBuffer))
		{
			if (errorAddress) { *errorAddress = currentVirtualAddress; }
			return 0;
		}

		instructionBuffer->address = currentVirtualAddress;
		if (findInstructionByAddress(&disassembledInstructions[0], 0, disassembledInstructions.size() - 1, instructionBuffer->address) != -1)
		{
			return 1;
		}

		currentFileOffset += instructionBuffer->numOfBytes;
		currentVirtualAddress += instructionBuffer->numOfBytes;

		// this needs to be sorted here because the jump handling may need to determine a reg's value
		int instructionIndex = findInstructionInsertPoint(&disassembledInstructions[0], 0, disassembledInstructions.size() - 1, instructionBuffer->address);
		disassembledInstructions.insert(disassembledInstructions.begin() + instructionIndex, *instructionBuffer);

		unsigned long long jmpDst = 0;
		unsigned char stop = 0;
		if (checkForControlFlowJump(&disassembledInstructions[0], instructionIndex, &jmpDst, &stop))
		{
			if (jmpDst == 0) 
			{
				continue;
			}

			if (jmpDst != instructionBuffer->address)
			{
				if (stop)
				{
					struct FileSection* section = 0;
					currentFileOffset = rvaToFileOffset(sections, numOfSections, jmpDst - imageBase, &section);
					if (!section || section->type != CODE_FST)
					{
						return 1;
					}

					currentVirtualAddress = jmpDst;
				}
				else if (!DisassembleTakingJumps(jmpDst, instructionBuffer, options, errorAddress))
				{
					return 0;
				}
			}
			else if(stop)
			{
				return 1;
			}
		}
	}

	return 1;
}

unsigned char MainGui::DisassembleBetweenBounds(unsigned long long startVA, unsigned long long endVA, struct DisassembledInstruction* instructionBuffer, struct DisassemblerOptions* options)
{
	struct FileSection* currentSection = 0;
	unsigned long long currentFileOffset = rvaToFileOffset(sections, numOfSections, startVA - imageBase, &currentSection);
	if (!currentSection || currentSection->type != CODE_FST)
	{
		return 1;
	}
	else if (currentFileOffset >= numOfFileBytes)
	{
		return 0;
	}

	unsigned long long endFileOffset = rvaToFileOffset(sections, numOfSections, endVA - imageBase, 0);
	if (endFileOffset > numOfFileBytes || endFileOffset == 0)
	{
		return 0;
	}

	unsigned long long currentVirtualAddress = startVA;
	while (currentFileOffset < endFileOffset)
	{
		if (!disassembleInstruction(&fileBytes[currentFileOffset], fileBytes + endFileOffset - 1, options, instructionBuffer))
		{
			int numOfBytes = instructionBuffer->numOfBytes;
			if (numOfBytes == 0) 
			{
				return 0;
			}

			memset(instructionBuffer, 0, sizeof(struct DisassembledInstruction));
			instructionBuffer->opcode = DATA;
			instructionBuffer->operands[0].type = IMMEDIATE;
			instructionBuffer->operands[0].immediate.size = 1;
			instructionBuffer->numOfBytes = 1;

			for (int i = 0; i < numOfBytes; i++) 
			{
				if (currentFileOffset >= endFileOffset)
				{
					return 1;
				}
				
				instructionBuffer->address = currentVirtualAddress;
				instructionBuffer->operands[0].immediate.value = fileBytes[currentFileOffset];
				
				currentFileOffset++;
				currentVirtualAddress++;
				disassembledInstructions.push_back(*instructionBuffer);
			}
		}
		else 
		{
			instructionBuffer->address = currentVirtualAddress;
			
			currentFileOffset += instructionBuffer->numOfBytes;
			currentVirtualAddress += instructionBuffer->numOfBytes;
			disassembledInstructions.push_back(*instructionBuffer);
		}
	}

	return 1;
}

void MainGui::DecompileFunction(int functionIndex)
{
	if (currentFilePath == "")
	{
		wxMessageBox("No file opened", "Can't decompile");
		return;
	}

	decompilationTextCtrl->ClearText();
	decompilationTextCtrl->ClearIndicators();
	disassemblyTextCtrl->ClearIndicators();

	decompParams.currentFunc = &functions[functionIndex];

	struct JdcStr decompilationResult = initializeJdcStr();
	if (decompilationResult.bufferSize == 0)
	{
		wxMessageBox("Error allocating memory for function decompilation", "Can't decompile");
		return;
	}

	struct JdcStr statusMessage = initializeJdcStr();
	int errorInstructionIndex = 0;
	if (!decompileFunction(&decompParams, &decompilationResult, &statusMessage, &errorInstructionIndex))
	{
		disassemblyTextCtrl->HighlightLine(errorInstructionIndex, RED_INDICATOR, 1);

		wxMessageBox(statusMessage.buffer, "Can't decompile");
		freeJdcStr(&statusMessage);

		int showOutput = wxMessageBox("Do you still want to see the mangled output?", "Show output", wxYES_NO, this);
		if (showOutput == wxNO)
		{
			freeJdcStr(&decompilationResult);
			return;
		}
	}

	currentDecompiledFunc = functionIndex;
	decompilationTextCtrl->SetReadOnly(false);
	decompilationTextCtrl->SetValue(decompilationResult.buffer);
	freeJdcStr(&decompilationResult);
	decompilationTextCtrl->ApplySyntaxHighlighting(imports, numOfImports);
	decompilationTextCtrl->SetReadOnly(true);
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
	decompParams.numOfFileBytes = numOfFileBytes;

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
			unsigned long long address = resolveJmpChain(&decompParams, i);
			if (findAddressInArr(&calledAddresses[0], 0, calledAddresses.size() - 1, address) == -1)
			{
				calledAddresses.insert(std::lower_bound(calledAddresses.begin(), calledAddresses.end(), address), address); // sorting it
			}
		}
	}

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
	}

	decompParams.functions = &functions[0];
	decompParams.numOfFunctions = functions.size();
	if (functions.size() > 0) 
	{
		getAllFunctionReturnTypes(&decompParams);
		fixAllFunctionReturnTypes(&decompParams);

		getAllFunctionConditionsAndArguments(&decompParams);
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

	struct JdcStr instructionStrBuffer = initializeJdcStr();
	int entryPointIndex = 0;
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

		wxString asmStr = "";
		if (instructionToStr(&disassembledInstructions[i], &instructionStrBuffer))
		{
			asmStr = wxString(instructionStrBuffer.buffer);
		}

		if (disassembledInstructions[i].isInvalid)
		{
			asmStr += " ; invalid instruction";
		}
		else if (disassembledInstructions[i].opcode == NO_MNEMONIC)
		{
			asmStr += " ; unrecognized opcode";
		}
		else if (disassembledInstructions[i].address == entryPoint + imageBase) 
		{
			asmStr += " ; entry point";
			entryPointIndex = i;
		}

		unsigned long long dst = getJmpDst(&disassembledInstructions[0], i, i - 0x1000);
		if (dst != 0)
		{
			char dstStr[20] = { 0 };
			sprintf(dstStr, "%llX", dst);
			asmStr += " ; dst: " + wxString(dstStr);
		}

		disassemblyText += addressInfoStr + asmStr + "\n";
	}

	freeJdcStr(&instructionStrBuffer);

	disassemblyTextCtrl->SetText(disassemblyText);
	disassemblyTextCtrl->ApplyAsmHighlighting();

	disassemblyTextCtrl->Thaw();
	disassemblyTextCtrl->SetReadOnly(true);

	disassemblyTextCtrl->HighlightLine(entryPointIndex, YELLOW_INDICATOR, 1);
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
	wxGrid* grid = (wxGrid*)(e.GetEventObject());
	if(grid != functionsGrid)
	{
		return;
	}

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

void MainGui::CloseApp(wxCloseEvent& e)
{
	dataViewerMenu->Destroy();
	stringsMenu->Destroy();
	colorsMenu->Destroy();
	Destroy();
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