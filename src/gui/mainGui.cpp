#include "mainGui.h"
#include <wx/numdlg.h>
#include "fileHeadersMenu.h"
#include "calculatorMenu.h"
#include "../decompiler/decompilationUtils.h"
#include "../disassembler/mnemonics.h"
#include "../decompiler/functions.h"

wxBEGIN_EVENT_TABLE(MainGui, wxFrame)
EVT_CLOSE(MainGui::CloseApp)
wxEND_EVENT_TABLE()

MainGui::MainGui() : wxFrame(nullptr, MainWindowID, "Jesso Decompiler x64", wxPoint(50, 50), wxSize(800, 600))
{
	this->SetMinSize(wxSize(800, 600));
	
	SetOwnBackgroundColour(backgroundColor);

	colorsMenu = new ColorsMenu();
	dataViewerMenu = new DataViewer(colorsMenu);
	stringsMenu = new StringsMenu();

	statusStaticText = new wxStaticText(this, wxID_ANY, "Status: idle");
	statusStaticText->SetOwnForegroundColour(textColor);

	disassemblyWindow = new DisassemblyWindow(this, wxSize(150, 400), colorsMenu, statusStaticText);

	decompilationWindow = new DecompilationWindow(this, wxSize(150, 400), colorsMenu, statusStaticText);
	decompilationWindow->EnableLineNumbers();
	decompilationWindow->SetAssociatedDisassemblyWindow(disassemblyWindow);

	functionsTextCtrl = new JdcTextCtrl(this, wxSize(800, 150), statusStaticText);
	functionsTextCtrl->EnableLineNumbers();

	disassemblyWindow->SetAdditionalOnUpdateUI([&]() {
		decompilationWindow->ClearIndicators();
		int instructionIndex = disassemblyWindow->GetCurrentLine();
		if (decompilationWindow->currentDecompiledFunc != -1 && showAssociatedDecompiledLines &&
			instructionIndex >= functions[decompilationWindow->currentDecompiledFunc].firstInstructionIndex && instructionIndex <= functions[decompilationWindow->currentDecompiledFunc].lastInstructionIndex)
		{
			for (int i = 0; i < functions[decompilationWindow->currentDecompiledFunc].numOfLines; i++)
			{
				struct AssociatedInstructions* a = &functions[decompilationWindow->currentDecompiledFunc].associatedInstructions[i];
				for (int j = 0; j < a->numOfIndexes; j++)
				{
					if (a->indexes[j] == instructionIndex)
					{
						decompilationWindow->HighlightLine(i, PURPLE_INDICATOR, 1);
						break;
					}
				}
			}

			disassemblyWindow->HighlightLine(instructionIndex, PURPLE_INDICATOR, 0);
		}

		if (showAssociatedFunctions && instructionIndex < disassembledInstructions.size())
		{
			functionsTextCtrl->ClearIndicators();
			int funcIndex = findFunctionByAddressInclusive(&decompParams, 0, functions.size() - 1, disassembledInstructions[instructionIndex].address);
			if (funcIndex != -1) 
			{
				functionsTextCtrl->HighlightLine(funcIndex, PURPLE_INDICATOR, 1);
				disassemblyWindow->HighlightLine(instructionIndex, PURPLE_INDICATOR, 0);
			}
		}

		if (dataViewerMenu->IsShown() && showBytesInDataViewer)
		{
			dataViewerMenu->HighlightInstruction(disassembledInstructions[instructionIndex].address, disassembledInstructions[instructionIndex].numOfBytes);
			disassemblyWindow->HighlightLine(instructionIndex, PURPLE_INDICATOR, 0);
		}
	});

	decompilationWindow->SetAdditionalOnUpdateUI([&]() {
		disassemblyWindow->ClearIndicators();
		int selectedLine = decompilationWindow->GetCurrentLine();
		if (decompilationWindow->currentDecompiledFunc != -1 && showAssociatedInstructions && selectedLine < functions[decompilationWindow->currentDecompiledFunc].associatedInstructionsBufferLen)
		{
			struct AssociatedInstructions* a = &functions[decompilationWindow->currentDecompiledFunc].associatedInstructions[selectedLine];

			for (int i = 0; i < a->numOfIndexes; i++)
			{
				disassemblyWindow->HighlightLine(a->indexes[i], PURPLE_INDICATOR, 1);
			}

			decompilationWindow->HighlightLine(selectedLine, PURPLE_INDICATOR, 0);
		}
	});

	disassemblyWindow->AddRightClickOption("Show associated decompiled lines", 0, &showAssociatedDecompiledLines, [&](wxCommandEvent& e) {
		showAssociatedDecompiledLines = e.IsChecked();
		disassemblyWindow->ClearIndicators();
		decompilationWindow->ClearIndicators();
	});

	disassemblyWindow->AddRightClickOption("Show bytes in data viewer", 0, &showBytesInDataViewer, [&](wxCommandEvent& e) {
		showBytesInDataViewer = e.IsChecked();
		disassemblyWindow->ClearIndicators();
		dataViewerMenu->dataTextCtrl->ClearIndicators();
	});

	disassemblyWindow->AddRightClickOption("Show associated function", 0, &showAssociatedFunctions, [&](wxCommandEvent& e) {
		showAssociatedFunctions = e.IsChecked();
		disassemblyWindow->ClearIndicators();
		functionsTextCtrl->ClearIndicators();
	});

	decompilationWindow->AddRightClickOption("Show associated instructions", 0, &showAssociatedInstructions, [&](wxCommandEvent& e) {
		showAssociatedInstructions = e.IsChecked();
		disassemblyWindow->ClearIndicators();
		decompilationWindow->ClearIndicators();
	});

	functionsTextCtrl->AddRightClickOption("Find function by address", 'G', 0, [&](wxCommandEvent&) {
		wxTextEntryDialog dlg(this, "", "Find address");
		if (dlg.ShowModal() == wxID_OK)
		{
			wxString txt = dlg.GetValue();
			unsigned long long address = 0;
			if (txt.ToULongLong(&address, 16))
			{
				int index = findFunctionByAddressInclusive(&decompParams, 0, functions.size() - 1, address);
				if (index == -1)
				{
					wxMessageBox("Address not found", "Failed to find address");
					return;
				}

				functionsTextCtrl->CenterLine(index);
				return;
			}

			wxMessageBox("Not valid hex number", "Failed to find address");
		}
	});

	functionsTextCtrl->AddRightClickOption("Decompile", 0, 0, [&](wxCommandEvent& e) {
		int selectedLine = functionsTextCtrl->GetCurrentLine();
		if (selectedLine >= 0 && selectedLine < functions.size()) 
		{
			decompilationWindow->DecompileFunction(&decompParams, selectedLine);
		}
	});

	functionsTextCtrl->AddRightClickOption("View info", 0, 0, [&](wxCommandEvent& e) {
		int selectedLine = functionsTextCtrl->GetCurrentLine();
		if (selectedLine >= 0 && selectedLine < functions.size())
		{
			new FunctionInfoMenu(this, GetPosition(), disassembledInstructions.data(), &functions[selectedLine]);
		}
	});

	functionsTextCtrl->AddRightClickOption("Edit properties", 0, 0, [&](wxCommandEvent& e) {
		int selectedLine = functionsTextCtrl->GetCurrentLine();
		if (selectedLine >= 0 && selectedLine < functions.size())
		{
			new FunctionPropertiesMenu(this, GetPosition(), this, selectedLine);
		}
	});

	menuBar = new wxMenuBar();

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

	colorsMenu->AddTextCtrl(decompilationWindow);
	colorsMenu->AddTextCtrl(dataViewerMenu->dataTextCtrl);
	colorsMenu->AddTextCtrl(disassemblyWindow);
	colorsMenu->AddTextCtrl(functionsTextCtrl);

	auiManager.SetManagedWindow(this);
	auiManager.SetFlags(auiManager.GetFlags() ^ wxAUI_MGR_LIVE_RESIZE);
	auiManager.AddPane(statusStaticText, wxAuiPaneInfo()
		.Name("status")
		.Top()
		.DockFixed(true)
		.PaneBorder(false)
		.Floatable(false)
		.Movable(false)
		.Gripper(false)
		.CaptionVisible(false)
		.CloseButton(false));

	auiManager.AddPane(disassemblyWindow, wxAuiPaneInfo()
		.Name("disassembly")
		.Caption("Disassembly")
		.Left()
		.BestSize(300, 400)
		.MinSize(100, 100)
		.CloseButton(false));

	auiManager.AddPane(decompilationWindow, wxAuiPaneInfo()
		.Name("decompilation")
		.Caption("Decompilation")
		.Right()
		.BestSize(300, 400)
		.MinSize(100, 100)
		.CloseButton(false));

	auiManager.AddPane(functionsTextCtrl, wxAuiPaneInfo()
		.Name("functions")
		.Caption("Functions")
		.Bottom()
		.BestSize(800, 150)
		.MinSize(100, 100)
		.CloseButton(false));

	auiManager.Update();
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

					// this is just for formatting in the gui
					if (numOfFileBytes < 0x10) { imageBase = 0x10; }
					else if (numOfFileBytes < 0x1000) { imageBase = 0x1000; }
					else if (numOfFileBytes < 0x100000) { imageBase = 0x100000; }
					else if (numOfFileBytes < 0x10000000) { imageBase = 0x10000000; }
					else if (numOfFileBytes < 0x1000000000) { imageBase = 0x1000000000; }
					else if (numOfFileBytes < 0x100000000000) { imageBase = 0x100000000000; }
					else if (numOfFileBytes < 0x10000000000000) { imageBase = 0x10000000000000; }
					else { imageBase = 0x1000000000000000; }

					while (1)
					{
						wxTextEntryDialog dlg(this, "", "Specify the entry point as a file offset");
						if (dlg.ShowModal() == wxID_OK)
						{
							wxString txt = dlg.GetValue();
							if (!txt.ToULongLong(&entryPoint, 16))
							{
								wxMessageBox("Not a valid hex number", "Failed to set entry point");
							}
							else if(entryPoint >= numOfFileBytes) 
							{
								wxMessageBox("File offset is larger than the file", "Failed to set entry point");
							}
							else 
							{
								break;
							}
						}
						else
						{
							this->SetTitle("Jesso Decompiler x64");
							currentFilePath = "";
							return;
						}
					}
					

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

	disassemblyWindow->Initialize(disassembledInstructions.data(), disassembledInstructions.size(), sections, numOfSections, imageBase, entryPoint, errorAddress);

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

	int getSymbols = wxMessageBox("Do you want to look for function name symbols? This could take some time.", "Get function name symbols", wxYES_NO, this);
	
	statusStaticText->SetLabelText("Status: finished finding functions, updating GUI...");
	statusStaticText->Refresh();
	statusStaticText->Update();

	UpdateFunctionsTextCtrl(getSymbols == wxYES);

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
	
	disassemblyWindow->ClearData();
	decompilationWindow->ClearText();
	functionsTextCtrl->ClearText();

	int numOfInstructions = disassembledInstructions.size();
	for (int i = 0; i < numOfInstructions; i++)
	{
		free(disassembledInstructions[i].operands);
	}

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
		if (findInstructionByAddress(&disassembledInstructions[0], 0, disassembledInstructions.size() - 1, currentVirtualAddress) != -1)
		{
			return 1;
		}

		// this checks if the address overlaps with an existing instruction
		int instructionIndex = findInstructionInsertPoint(&disassembledInstructions[0], 0, disassembledInstructions.size() - 1, currentVirtualAddress);
		if (instructionIndex > 0 && instructionIndex < disassembledInstructions.size() &&
			currentVirtualAddress > disassembledInstructions[instructionIndex - 1].address &&
			currentVirtualAddress < disassembledInstructions[instructionIndex - 1].address + disassembledInstructions[instructionIndex - 1].numOfBytes)
		{
			return 0;
		}

		if (!disassembleInstruction(&fileBytes[currentFileOffset], fileBytes + currentSection->fileOffset + currentSection->size - 1, options, instructionBuffer))
		{
			if (errorAddress) { *errorAddress = currentVirtualAddress; }
			return 0;
		}

		instructionBuffer->address = currentVirtualAddress;

		currentFileOffset += instructionBuffer->numOfBytes;
		currentVirtualAddress += instructionBuffer->numOfBytes;

		// this needs to be sorted here because the jump handling may need to determine a reg's value
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
			instructionBuffer->numOfBytes = 1;

			for (int i = 0; i < numOfBytes; i++) 
			{
				if (currentFileOffset >= endFileOffset)
				{
					return 1;
				}
				
				instructionBuffer->address = currentVirtualAddress;
				instructionBuffer->operands = (struct Operand*)calloc(1, sizeof(struct Operand));
				instructionBuffer->operands[0].immediate.value = fileBytes[currentFileOffset];
				instructionBuffer->operands[0].type = IMMEDIATE;
				instructionBuffer->operands[0].immediate.size = 1;
				instructionBuffer->numOfOperands = 1;
				
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

	decompParams.currentFunc = 0;
}

void MainGui::UpdateFunctionsTextCtrl(unsigned char getSymbols)
{
	functionsTextCtrl->SetReadOnly(false);
	functionsTextCtrl->Freeze();

	wxString functionsStr = "";

	struct JdcStr functionHeaderBuffer = initializeJdcStr();
	int numOfFunctions = functions.size();
	for (int i = 0; i < numOfFunctions; i++) 
	{
		struct Function* function = &functions[i];
		if (getSymbols)
		{
			getSymbolByValue(currentFilePath.c_str().AsWChar(), is64Bit, disassembledInstructions[function->firstInstructionIndex].address, &function->name);
		}
		
		if (!generateFunctionHeader(function, &functionHeaderBuffer)) 
		{
			return;
		}

		sprintfJdc(&functionHeaderBuffer, 1, "; // address: 0x%llX; num of instructions: %d", disassembledInstructions[function->firstInstructionIndex].address, function->lastInstructionIndex - function->firstInstructionIndex + 1);
		functionsStr += wxString(functionHeaderBuffer.buffer);

		if (i != numOfFunctions - 1) 
		{
			functionsStr += "\n";
		}
	}

	functionsTextCtrl->SetText(functionsStr);
	functionsTextCtrl->ApplyFunctionsHighlighting(colorsMenu->decompColors);
	functionsTextCtrl->Thaw();
	functionsTextCtrl->SetReadOnly(true);
}

void MainGui::CloseApp(wxCloseEvent& e)
{
	auiManager.UnInit();
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
	//mainGui->functionsGrid->SetCellValue(functionIndex, 2, currentFunction->name.buffer);

	// redecompile if it is currently in the decompilation box. this is to refresh it
	//if (mainGui->decompilationWindow->currentDecompiledFunc == functionIndex)
	//{
	//	mainGui->DecompileFunction(functionIndex);
	//}

	Destroy();
}